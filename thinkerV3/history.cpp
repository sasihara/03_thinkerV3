#include <stdio.h>
#include <time.h>
#include <direct.h>
#include "history.hpp"

// Global
extern Logging logging;

int History::init()
{
	for (size_t i = 0; i < MAX_NUM_HIST_DATA_SET; i++) {
		for (size_t j = 0; j < MAX_NUM_HISTORY_DATA; j++) {
			historyDataSets[i].historyData[j].isValid = false;
		}
		historyDataSets[i].isValid = false;
	}

	histDataSetSeqId = 0;

	return 0;
}

int History::add(GameId _gameId, DISKCOLORS _diskcolor, DISKCOLORS* _board, std::vector<Score> scores)
{
	logging.logout("History::add()開始. isGameIdValid = %d, 保存済みgameId.pid = %d, 指定gameId.pid = %d.",
		isGameIdValid,
		gameId.pid,
		_gameId.pid
		);

	// HistoryDataの準備
	HistoryData *historyData;

	// 該当のgameIdのヒストリが既に存在するか？
	bool isHistoryListExist = false;
	size_t index = 0;

	for (index = 0; index < MAX_NUM_HIST_DATA_SET; index++) {
		// 既に指定されたgameIdのヒストリデータリストが存在する場合
		if (historyDataSets[index].isValid == true && 
			historyDataSets[index].gameId == _gameId && 
			historyDataSets[index].diskcolor == _diskcolor) {
			
			isHistoryListExist = true;
			break;
		}
	}

	// ヒストリデータを保存する
	// ヒストリデータリストが見つからなかった場合は新しく作ってそこに格納
	if (isHistoryListExist == false) {
		// すでに規定数以上のトランザクション数のヒストリデータが保存されていれば先頭の学習データを消す
		// 次の格納場所を初期化
		for (size_t i = 0; i < MAX_NUM_HISTORY_DATA; i++) {
			historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].historyData[i].isValid = false;
		}

		// isValidフラグ更新
		historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].isValid = true;

		// 新しいトランザクションのヒストリデータを保存する
		historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].gameId = _gameId;
		historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].diskcolor = _diskcolor;
		historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].numHistoryData = 1;

		historyData = &historyDataSets[histDataSetSeqId % MAX_NUM_HIST_DATA_SET].historyData[0];

		// ヒストリデータセットのシーケンス番号を1つ進める
		histDataSetSeqId++;
	}
	else {
		historyData = &historyDataSets[index].historyData[historyDataSets[index].numHistoryData];
		historyDataSets[index].numHistoryData++;
	}

	// diskcolorのセット
	historyData->diskcolor = _diskcolor;

	// board内容のセット
	memmove_s(&historyData->board, sizeof(historyData->board), _board, sizeof(historyData->board));

	// probabilityのセット
	// ベクタ型scoresに格納されるprobability値を_probability配列へ格納する
	memset(historyData->probability, 0, sizeof(historyData->probability));

	for (size_t i = 0; i < scores.size(); i++) {
		int idx = scores[i].x * 8 + scores[i].y;

		if (idx < DN_OUTPUT_SIZE) {
			historyData->probability[idx] = scores[i].probability;
		}
	}

	// value値の初期化
	historyData->value = 0.0;

	// isValidフラグをセット
	historyData->isValid = true;

	logging.logout("ヒストリデータを保存しました.");

	return 0;
}

int History::finish(GameId _gameId, DISKCOLORS _diskcolor, float _value)
{
	logging.logout("History::setValue() start.");

	for (size_t i = 0; i < MAX_NUM_HIST_DATA_SET; i++) {
		if (historyDataSets[i].isValid == true && 
			historyDataSets[i].gameId == _gameId && 
			historyDataSets[i].diskcolor == _diskcolor) {
			
			logging.logout("value = %dをセットします.", _value);

			// 現在保存されているヒストリ情報にvalue値をセットする
			for (size_t j = 0; j < historyDataSets[i].numHistoryData; j++) {
				if (historyDataSets[i].historyData[j].isValid == true && 
					historyDataSets[i].historyData[j].diskcolor == _diskcolor) {
					
					historyDataSets[i].historyData[j].value = _value;
				}
			}

			// ヒストリ情報をファイルに出力する
			logging.logout("学習データをファイルに出力します.");

			int ret;
			ret = outputFile(_gameId, _diskcolor);

			if (ret < 0) {
				return -1;
			}

			// データを削除する
			for (size_t j = 0; j < MAX_NUM_HISTORY_DATA; j++)
				historyDataSets[i].historyData[j].isValid = false;

			historyDataSets[i].isValid = false;

			logging.logout("History::setValue() finish.");
			return 0;
		}
	}

	// 指定されたgameIdのヒストリデータが保存されてない場合
	logging.logout("[WARNING] gameId = %d の学習データが無いので値のセットおよびファイル出力は行いません.", _gameId);
	logging.logout("History::setValue() finish.");
	return -2;
}

int History::outputFile(GameId _gameId, DISKCOLORS _diskcolor)
{
	logging.logout("History::outputFile() start.");

	for (size_t i = 0; i < MAX_NUM_HIST_DATA_SET; i++) {
		if (historyDataSets[i].isValid == true && 
			historyDataSets[i].gameId == _gameId && 
			historyDataSets[i].diskcolor == _diskcolor) {
			
			// ヒストリデータの有無を確認する
			if (historyDataSets[i].numHistoryData <= 0) {
				return -1;
			}

			// 学習データ格納用フォルダを作成する
			if (_mkdir("history") != 0) {
				if(errno != EEXIST) return -2;
			}

			// ファイル名を決定する
			// ファイル名は"年月日時分秒石の色.hst"
			time_t currentTime, ret;
			struct tm localTime;
			char fileName[256];

			ret = time(&currentTime);
			if (ret < 0) return -3;

			errno_t err;
			err = localtime_s(&localTime, &currentTime);

			if (err) {
				return -4;
			}

			sprintf_s(fileName, "history\\%04d%02d%02d%02d%02d%02d%c_%d.bhs",
				localTime.tm_year + 1900,
				localTime.tm_mon + 1,
				localTime.tm_mday,
				localTime.tm_hour,
				localTime.tm_min,
				localTime.tm_sec,
				_diskcolor == DISKCOLORS::COLOR_BLACK ? 'B' : 'W',
				_gameId.pid
			);

			// ファイルをオープンする
			FILE* f;
			if (fopen_s(&f, fileName, "wb") != 0) {
				return -5;
			}

			// ファイルを書き込む
			unsigned _int16  formatVersion = FORMAT_VERSION;

			// フォーマットバージョン
			fwrite(&formatVersion, sizeof(formatVersion), 1, f);

			for (size_t j = 0; j < historyDataSets[i].numHistoryData; j++) {
				if (historyDataSets[i].historyData[j].diskcolor == _diskcolor) {
					// 下で出力されるポリシー値・value値がどちらの石の色に対する値なのかを出力
					fwrite(&historyDataSets[i].historyData[j].diskcolor, sizeof(historyDataSets[i].historyData[j].diskcolor), 1, f);

					// 盤面の書き込み
					fwrite(historyDataSets[i].historyData[j].board, sizeof(historyDataSets[i].historyData[j].board), 1, f);

					// ポリシー値の書き込み
					fwrite(historyDataSets[i].historyData[j].probability, sizeof(historyDataSets[i].historyData[j].probability), 1, f);

					// value値の書き込み
					fwrite(&historyDataSets[i].historyData[j].value, sizeof(historyDataSets[i].historyData[j].value), 1, f);
				}
			}

			// ファイルをクローズする
			fclose(f);
			return 0;
		}
	}

	// 指定されたgameIdのヒストリデータが保存されてない場合
	logging.logout("[WARNING] gameId = %d の学習データが無いのでファイル出力は行いません.", _gameId);
	logging.logout("History::outputFile() finish.");
	return -6;
}
