#include <stdio.h>
#include "logging.h"
#include "history.hpp"
#include "thinkV1.hpp"

#define FORMAT_VERSION_SUPPORTED	200

int main(int argc, char** argv)
{
	int ret;

	// 改行出力
	printf("\n");

	// 入力チェック
	if (argc < 2) {
		printf("historyAnalizer.exe [ヒストリファイル名]\n\n");
		return -1;
	}

	// 入力ファイルのオープン
	FILE* fIn;
	if (fopen_s(&fIn, argv[1], "rb") != 0) {
		printf("ヒストリファイル %s が見つかりません.\n\n", argv[1]);
		return -2;
	}
	else {
		printf("ログファイル名: %s\n", argv[1]);
	}

	// ログファイル名
	char logFileName[1024];

	// ログ分析&出力
	Logging logging;
	HistoryData historyData;

	try {
		// 出力ファイルのオープン
		// 出力ファイル名の生成
		// ファイル名の長さを取得
		size_t len = strlen(argv[1]);

		// 拡張子チェック
		if (strcmp(&argv[1][len - 4], ".bhs") != 0) {
			throw -3;
		}

		// 拡張子の置き換えたものを新ファイル名とする
		errno_t errno_t_ret;
		errno_t_ret = strcpy_s(logFileName, sizeof(logFileName), argv[1]);
		if (errno_t_ret != 0) throw -4;

		errno_t_ret = strcpy_s(&logFileName[len - 4], 5, ".txt");
		if (errno_t_ret != 0) throw -5;

		// ログファイル名の画面出力
		printf("出力ファイル名: %s\n", logFileName);

		// ログファイルのオープン
		logging.init(9, logFileName);
	}
	catch (int ret) {
		printf("\n[ERROR] %sのオープンに失敗.\n\n", logFileName);

		fclose(fIn);
		return ret;
	}

	try {
		// ファイル読み込み
		int c = 0;

		int sum = 0;

		size_t count;
		unsigned _int16 formatVersion;

		count = fread_s(&formatVersion, sizeof(formatVersion), sizeof(formatVersion), 1, fIn);

		if (formatVersion != FORMAT_VERSION_SUPPORTED) {
			printf("\n[ERROR] サポートされてないフォーマットのファイルです(バージョン = %d)\n", formatVersion);
			throw - 1;
		}

		while (feof(fIn) == 0) {
			//logging.logout("%d(0x%X)バイト目から%dバイト読み込み", sum, sum, sizeof(historyData.diskcolor));
			count = fread_s(&historyData.diskcolor, sizeof(historyData.diskcolor), sizeof(historyData.diskcolor), 1, fIn);
			if (count < 1) break;
			sum += sizeof(historyData.diskcolor);

			//logging.logout("%d(0x%X)バイト目から%dバイト読み込み", sum, sum, sizeof(historyData.board));
			count = fread_s(historyData.board, sizeof(historyData.board), sizeof(historyData.board), 1, fIn);
			if (count < 1) break;
			sum += sizeof(historyData.board);

			//logging.logout("%d(0x%X)バイト目から%dバイト読み込み", sum, sum, sizeof(historyData.probability));
			count = fread_s(historyData.probability, sizeof(historyData.probability), sizeof(historyData.probability), 1, fIn);
			if (count < 1) throw -7;
			sum += sizeof(historyData.probability);

			//logging.logout("%d(0x%X)バイト目から%dバイト読み込み", sum, sum, sizeof(historyData.value));
			count = fread_s(&historyData.value, sizeof(historyData.value), sizeof(historyData.value), 1, fIn);
			if (count < 1) throw -8;
			sum += sizeof(historyData.value);

			// board内容の出力
			c = c + 1;
			logging.logout("●No. %d", c);
			logging.logprintf("プレイヤーの石の色: %s\n\n", historyData.diskcolor == DISKCOLORS::COLOR_BLACK ? "●" : historyData.diskcolor == DISKCOLORS::COLOR_WHITE ? "○" : "");

			ret = logoutBoard(logging, historyData.board);

			// probalibityの出力
			logging.logprintf("\n*** Probability ***\n");
			for (size_t x = 0; x < 8; x++) {
				for (size_t y = 0; y < 8; y++) {
					if (historyData.board[y * 8 + x] == DISKCOLORS::COLOR_BLACK) {
						logging.logprintf("●", historyData.probability[y * 8 + x]);
					}
					else if (historyData.board[y * 8 + x] == DISKCOLORS::COLOR_WHITE) {
						logging.logprintf("○", historyData.probability[y * 8 + x]);
					}
					else {
						logging.logprintf("%2.0f", historyData.probability[y * 8 + x] * 100);
					}
					logging.logprintf(",");
				}

				logging.logprintf("\n");
			}
			logging.logprintf("\n");

			logging.logprintf("(生データ)\n");
			for (size_t i = 0; i < 65; i++) {
				logging.logprintf("%.02f", historyData.probability[i]);
				if (i % 8 == 7) {
					logging.logprintf("\n");
				}
				else {
					logging.logprintf(", ");
				}
			}
			logging.logprintf("\n\n");

			// value値の出力
			logging.logprintf("*** Value ***\n");
			logging.logprintf("%f\n\n", historyData.value);
		}

		// ファイルのクローズ
		fclose(fIn);
		logging.end();

		// 画面出力
		printf("出力終了.\n");

		return 0;
	}
	catch (int ret) {
		// 画面出力
		printf("\n[ERROR] ファイル読み込みで例外が発生。例外No. = %d.\n", ret);

		// ログ出力
		logging.logout("[ERROR] ファイル読み込みで例外が発生。例外No. = %d.", ret);

		// ファイルのクローズ
		fclose(fIn);
		logging.end();
		return ret;
	}
}