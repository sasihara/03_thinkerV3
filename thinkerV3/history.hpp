#pragma once

#include <vector>
#include "TFHandler.hpp"
#include "Node.hpp"

#define	MAX_NUM_HIST_DATA_SET	10
#define MAX_NUM_HISTORY_DATA	60
#define FORMAT_VERSION			200

typedef struct _HISTORY_DATA {		// �q�X�g���f�[�^(1�蕪)
	bool isValid = false;
	DISKCOLORS diskcolor;
	DISKCOLORS board[64];
	double probability[DN_OUTPUT_SIZE];
	float value;
} HistoryData;

typedef struct _HISTORY_DATA_SET {		// �q�X�g���f�[�^�Z�b�g(���E���ǂ��炩��1�������̃q�X�g���f�[�^)
	bool isValid = false;
	GameId gameId;
	DISKCOLORS diskcolor;
	size_t numHistoryData = 0;		// historyData�Ɋi�[�����f�[�^��
	HistoryData historyData[MAX_NUM_HISTORY_DATA];
} HistoryDataSet;

class History {
public:
	int init();
	int add(GameId _gameId, DISKCOLORS _diskcolor, DISKCOLORS *_board, std::vector<Score> scores);
	int finish(GameId _gameId, DISKCOLORS _diskcolor, float _value);
private:
	int outputFile(GameId _gameId, DISKCOLORS diskcolor);
	HistoryDataSet historyDataSets[MAX_NUM_HIST_DATA_SET];
	int histDataSetSeqId = 0;			// �q�X�g���f�[�^�Z�b�g�̒ʂ��ԍ��A�i�[�ꏊ����Ɏg�p

	GameId gameId;
	bool isGameIdValid = false;
	// ���v�ǉ� �g�����U�N�V����ID
};

