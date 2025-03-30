#include <stdio.h>
#include <time.h>
#include <direct.h>
#include "history.hpp"

// Global
extern Logging logging;

int History::init()
{
	for (size_t i = 0; i < historyDataList.size(); i++) {
		historyDataList[i].historyData.clear();
	}

	historyDataList.clear();

	return 0;
}

int History::add(GameId _gameId, DISKCOLORS _diskcolor, DISKCOLORS* _board, std::vector<Score> scores)
{
	logging.logout("History::add()�J�n. isGameIdValid = %d, �ۑ��ς�gameId.pid = %d, �w��gameId.pid = %d.",
		isGameIdValid,
		gameId.pid,
		_gameId.pid
		);

	// HistoryData�̏���
	HistoryData historyData;

	// diskcolor�̃Z�b�g
	historyData.diskcolor = _diskcolor;

	// board���e�̃Z�b�g
	memmove_s(&historyData.board, sizeof(historyData.board), _board, sizeof(historyData.board));

	// probability�̃Z�b�g
	// �x�N�^�^scores�Ɋi�[�����probability�l��_probability�z��֊i�[����
	double probability[DN_OUTPUT_SIZE];
	memset(probability, 0, sizeof(probability));

	for (size_t i = 0; i < scores.size(); i++) {
		int idx = scores[i].x * 8 + scores[i].y;

		if (idx < DN_OUTPUT_SIZE) {
			probability[idx] = scores[i].probability;
		}
	}

	memmove_s(&historyData.probability, sizeof(historyData.probability), probability, sizeof(probability));

	// value�l�̏�����
	historyData.value = 0.0;

	// �Y����gameId�̃q�X�g�������ɑ��݂��邩�H
	bool isHistoryListExist = false;
	size_t index = 0;

	for (index = 0; index < historyDataList.size(); index++) {
		// ���Ɏw�肳�ꂽgameId�̃q�X�g���f�[�^���X�g�����݂���ꍇ
		if (historyDataList[index].gameId == _gameId && historyDataList[index].diskcolor == _diskcolor) {
			isHistoryListExist = true;
			break;
		}
	}

	// �q�X�g���f�[�^��ۑ�����
	// �q�X�g���f�[�^���X�g��������Ȃ������ꍇ�͐V��������Ă����Ɋi�[
	if (isHistoryListExist == false) {
		// ���łɋK�萔�ȏ�̃g�����U�N�V�������̃q�X�g���f�[�^���ۑ�����Ă���ΐ擪�̊w�K�f�[�^������
		if (historyDataList.size() >= MAX_NUM_TRANSAC_HIST) {
			// historyDataList�̐擪���폜
			historyDataList.begin()->historyData.clear();
			historyDataList.erase(historyDataList.begin());
		}
			
		// �V�����g�����U�N�V�����̃q�X�g���f�[�^��ۑ�����
		HistoryDataList newHistoryDataList;

		newHistoryDataList.gameId = _gameId;
		newHistoryDataList.diskcolor = _diskcolor;
		newHistoryDataList.historyData.push_back(historyData);

		historyDataList.push_back(newHistoryDataList);
	}
	else {
		historyDataList[index].historyData.push_back(historyData);
	}

	logging.logout("�q�X�g���f�[�^��ۑ����܂���.");

	return 0;
}

int History::finish(GameId _gameId, DISKCOLORS _diskcolor, float _value)
{
	logging.logout("History::setValue() start.");

	for (size_t i = 0; i < historyDataList.size(); i++) {
		if (historyDataList[i].gameId == _gameId && historyDataList[i].diskcolor == _diskcolor) {
			logging.logout("value = %d���Z�b�g���܂�.", _value);

			// ���ݕۑ�����Ă���q�X�g������value�l���Z�b�g����
			for (size_t j = 0; j < historyDataList[i].historyData.size(); j++) {
				if (historyDataList[i].historyData[j].diskcolor == _diskcolor) {
					historyDataList[i].historyData[j].value = _value;
				}
			}

			// �q�X�g�������t�@�C���ɏo�͂���
			logging.logout("�w�K�f�[�^���t�@�C���ɏo�͂��܂�.");

			int ret;
			ret = outputFile(_gameId, _diskcolor);

			if (ret < 0) {
				return -1;
			}

			// �f�[�^���폜����
			historyDataList[i].historyData.clear();
			historyDataList.erase(historyDataList.begin() + i);

			logging.logout("History::setValue() finish.");
			return 0;
		}
	}

	// �w�肳�ꂽgameId�̃q�X�g���f�[�^���ۑ�����ĂȂ��ꍇ
	logging.logout("[WARNING] gameId = %d �̊w�K�f�[�^�������̂Œl�̃Z�b�g����уt�@�C���o�͍͂s���܂���.", _gameId);
	logging.logout("History::setValue() finish.");
	return -2;
}

int History::outputFile(GameId _gameId, DISKCOLORS _diskcolor)
{
	logging.logout("History::outputFile() start.");

	for (size_t i = 0; i < historyDataList.size(); i++) {
		if (historyDataList[i].gameId == _gameId && historyDataList[i].diskcolor == _diskcolor) {
			// �q�X�g���f�[�^�̗L�����m�F����
			if (historyDataList[i].historyData.size() <= 0) {
				return -1;
			}

			// �w�K�f�[�^�i�[�p�t�H���_���쐬����
			if (_mkdir("history") != 0) {
				if(errno != EEXIST) return -2;
			}

			// �t�@�C���������肷��
			// �t�@�C������"�N���������b�΂̐F.hst"
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

			sprintf_s(fileName, "history\\%04d%02d%02d%02d%02d%02d%c.bhs",
				localTime.tm_year + 1900,
				localTime.tm_mon + 1,
				localTime.tm_mday,
				localTime.tm_hour,
				localTime.tm_min,
				localTime.tm_sec,
				_diskcolor == DISKCOLORS::COLOR_BLACK ? 'B' : 'W'
			);

			// �t�@�C�����I�[�v������
			FILE* f;
			if (fopen_s(&f, fileName, "wb") != 0) {
				return -5;
			}

			// �t�@�C������������
			unsigned _int16  formatVersion = FORMAT_VERSION;

			// �t�H�[�}�b�g�o�[�W����
			fwrite(&formatVersion, sizeof(formatVersion), 1, f);

			for (size_t j = 0; j < historyDataList[i].historyData.size(); j++) {
				if (historyDataList[i].historyData[j].diskcolor == _diskcolor) {
					// ���ŏo�͂����|���V�[�l�Evalue�l���ǂ���̐΂̐F�ɑ΂���l�Ȃ̂����o��
					fwrite(&historyDataList[i].historyData[j].diskcolor, sizeof(historyDataList[i].historyData[j].diskcolor), 1, f);

					// �Ֆʂ̏�������
					fwrite(historyDataList[i].historyData[j].board, sizeof(historyDataList[i].historyData[j].board), 1, f);

					// �|���V�[�l�̏�������
					fwrite(historyDataList[i].historyData[j].probability, sizeof(historyDataList[i].historyData[j].probability), 1, f);

					// value�l�̏�������
					fwrite(&historyDataList[i].historyData[j].value, sizeof(historyDataList[i].historyData[j].value), 1, f);
				}
			}

			// �t�@�C�����N���[�Y����
			fclose(f);
			return 0;
		}
	}

	// �w�肳�ꂽgameId�̃q�X�g���f�[�^���ۑ�����ĂȂ��ꍇ
	logging.logout("[WARNING] gameId = %d �̊w�K�f�[�^�������̂Ńt�@�C���o�͍͂s���܂���.", _gameId);
	logging.logout("History::outputFile() finish.");
	return -6;
}
