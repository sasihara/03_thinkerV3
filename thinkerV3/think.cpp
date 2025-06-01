// Othello Thinker Version 3.00
// Copyright (C) 2023 T.Sashihara
// This program goes with Othello for Windows Ver 3.00

#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "main.hpp"
#include "think.hpp"
#include "TFHandler.hpp"
#include "State.hpp"
#include "Pv_mcts.action.hpp"
#include "thinkV1.hpp"
#include "history.hpp"

extern History history;
extern Logging logging;

int Thinker::init(double _spTemperature)
{
	int ret;

	// ���C�����������������Ɏ���
	// ���f���̃��[�h
	ret = load_model(&model);

	if (ret < 0) {
		return -1;
	}

	// ���f�����̎擾
	FILE* f;
	if (fopen_s(&f, "04_OthelloDeepModel\\MODELINFO.txt", "r") == 0) {
		fscanf_s(f, "%s", modelInfo, (unsigned int)sizeof(modelInfo));
		fclose(f);
	}
	else {
		strcpy_s(modelInfo, sizeof(modelInfo), MODELINFO);
	}

	// ���x�p�����[�^���Z�b�g
	spTemperature = _spTemperature;

	// ���^�[��
	isInitialized = true;

	return 0;
}

Thinker::~Thinker()
{
	if (isInitialized == true) {
		free_model(&model);
	}
}

//
//	Function Name: think
//	Summary: Set the search depth and then call findBestPlaceForCurrentPlayer to find the best place for the current player.
//	
//	In:
//		No parameters.
//
//	Return:
//		Position for the disk. If the position is (x, y), the result value is x * 10 + y.
//
int Thinker::think(int turn, DISKCOLORS *board, int *place, GameId gameId)
{
	ThinkerV1 thinkerV1;
	int ret = 0;

	// Decide thinking mode.
	int numSpaceLeft;
	numSpaceLeft = CountDisk(DISKCOLORS::COLOR_NONE, board);
	LOGOUT(LOGLEVEL_TRACE, "�c��ΐ� = %d.", numSpaceLeft);

	if (numSpaceLeft <= NUM_FOR_GAMESTATE_END) {
		// thinkerV1(=���S�ǂ݃��[�h)�Ŏv�l
		// thinkerV1�̏�����
		ret = thinkerV1.SetParams(turn, board);
		if (ret < 0) return -1;

		ret = thinkerV1.think();
		if (ret < 0) return -2;

		*place = ret;

		// Add to History
		DISKCOLORS boardToHistory[64];
		memmove_s(boardToHistory, sizeof(boardToHistory), board, sizeof(DISKCOLORS) * 64);

		if (ret < 0) return -1;

		std::vector<Score> scoresList;
		Score score;
		score.x = *place / 10;
		score.y = *place % 10;
		score.n = 0;
		score.probability = 1.0;
		try {
			scoresList.push_back(score);
		}
		catch (...) {
			return -2;
		}

		ret = history.add(gameId, CURRENTPLAYER(turn), boardToHistory, scoresList);
		if (ret < 0) return -3;
	}
	else {
		// Deep Learning�x�[�X�Ŏv�l
		// State�I�u�W�F�N�g�̐���
		State* state;

		try {
			state = new State;
		}
		catch (...) {
			return -4;
		}

		state->init(board, CURRENTPLAYER(turn));

		// pv_mcts_action�I�u�W�F�N�g�̐���
		Pv_mcts_action* next_action;

		try {
			next_action = new Pv_mcts_action(&model, spTemperature);
		}
		catch (...) {
			return -5;
		}

		// ���̎�𓾂�
		Action action;
		ret = next_action->run(state, &action, gameId);

		if (ret < 0) return -6;

		// Set the best place to "place"
		*place = (int)action;

		// Delete next_action
		delete(next_action);

		// Delete state
		delete state;
	}

	return 0;
}

char* Thinker::getModelInfo()
{
	return modelInfo;
}

int Thinker::CountDisk(DISKCOLORS color, DISKCOLORS _board[64])
{
	int c = 0, i;

	for (i = 0; i < 64; i++) {
		if (_board[i] == color) c++;
	}
	return c;
}
