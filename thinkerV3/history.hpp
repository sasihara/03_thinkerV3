#pragma once

#include <vector>
#include "TFHandler.hpp"
#include "Node.hpp"

#define	MAX_NUM_TRANSAC_HIST	5
#define FORMAT_VERSION			200

typedef struct _HISTORY_DATA {
	DISKCOLORS diskcolor;
	DISKCOLORS board[64];
	double probability[DN_OUTPUT_SIZE];
	float value;
} HistoryData;

typedef struct _HISTORY_DATA_LIST {
	GameId gameId;
	std::vector<HistoryData> historyData;
} HistoryDataList;

class History {
public:
	int add(GameId _gameId, DISKCOLORS diskcolor, DISKCOLORS *_board, std::vector<Scores> scores);
	int setValue(GameId _gameId, DISKCOLORS diskcolor, float _value);
private:
	int init();
	int outputFile(GameId _gameId, DISKCOLORS diskcolor);
	std::vector<HistoryDataList> historyDataList;
	GameId gameId;
	bool isGameIdValid = false;
	// ���v�ǉ� �g�����U�N�V����ID
};

