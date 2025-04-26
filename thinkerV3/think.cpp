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
	int ret = 0;

	// State�I�u�W�F�N�g�̐���
	State* state;

	try {
		state = new State;
	}
	catch (...) {
		return -2;
	}

	state->init(board, CURRENTPLAYER(turn));

	// pv_mcts_action�I�u�W�F�N�g�̐���
	Pv_mcts_action *next_action;
	
	try {
		next_action = new Pv_mcts_action(&model, spTemperature);
	}
	catch (...) {
		return -3;
	}

	// ���̎�𓾂�
	Action action;
	ret = next_action->run(state, &action, gameId);

	if (ret < 0) return -4;

	// Set the best place to "place"
	*place = (int)action;

	// Delete next_action
	delete(next_action);

	// Delete state
	delete state;

	return 0;
}

char* Thinker::getModelInfo()
{
	return modelInfo;
}