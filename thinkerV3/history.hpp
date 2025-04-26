#pragma once

#include <vector>
#include "TFHandler.hpp"
#include "Node.hpp"

#define	MAX_NUM_HIST_DATA_SET	10
#define MAX_NUM_HISTORY_DATA	60
#define FORMAT_VERSION			200

typedef struct _HISTORY_DATA {		// ヒストリデータ(1手分)
	bool isValid = false;
	DISKCOLORS diskcolor;
	DISKCOLORS board[64];
	double probability[DN_OUTPUT_SIZE];
	float value;
} HistoryData;

typedef struct _HISTORY_DATA_SET {		// ヒストリデータセット(黒・白どちらかの1試合分のヒストリデータ)
	bool isValid = false;
	GameId gameId;
	DISKCOLORS diskcolor;
	size_t numHistoryData = 0;		// historyDataに格納されるデータ数
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
	int histDataSetSeqId = 0;			// ヒストリデータセットの通し番号、格納場所決定に使用

	GameId gameId;
	bool isGameIdValid = false;
	// ★要追加 トランザクションID
};

