#include <float.h>
#include "common.h"
#include "Pv_mcts.action.hpp"
#include "TFHandler.hpp"
#include "Node.hpp"
#include "logging.h"
#include "history.hpp"

// Global
extern Logging logging;
extern History history;

Pv_mcts_action::Pv_mcts_action(Model* _model, Temperature _temperature)
{
	model = _model;
	temperature = _temperature;
}

int Pv_mcts_action::run(State* state, Action* action, GameId gameId)
{
	int ret;
	std::vector<Score> scores;

	// �Ֆʂ̃��O�o��
	state->logout(logging);

	// pv_mcts_scores()
	ret = pv_mcts_scores(state, &scores);

	// �߂�l�̃`�F�b�N
	if (ret < 0) {
		return -1;
	}

	// �w�K�f�[�^�̕ۑ�
	ret = history.add(gameId, state->currentPlayer, state->board, scores);
	if (ret < 0) {
		return -2;
	}

	// np.random.choice
	if (scores.size() > 0) {
		// ���@�肪���݂���ꍇ
		ret = ranom_choice(state, scores, action);

		// �߂�l�̃`�F�b�N
		if (ret < 0) {
			return -3;
		}
	}
	else {
		// ���@�肪���݂��Ȃ������ꍇ
		// �p�X������x=8, y=0���Z�b�g
		*action = 80;
	}

	// ���O�o��
	logging.logout("���݂̔Ֆʂ͈ȉ��̒ʂ�ł�.");
	state->logout(logging);

	// ���^�[��
	return 0;
}

int Pv_mcts_action::pv_mcts_scores(State* state, std::vector<Score>* scores)
{
	int ret;

	LOGOUT(LOGLEVEL_INFO, "pv_mcts_scores()�J�n.");

	Node* root_node;
	
	try {
		root_node = new Node(model, state, 0.0);
	}
	catch (...) {
		return -1;
	}

	try {
		for (int i = 0; i < PV_EVALUATE_COUNT; i++) {
			LOGOUT(LOGLEVEL_TRACE, "%d��ڂ̕]�����J�n.", i + 1);

			float value;
			ret = root_node->evaluate(&value);
			if (ret < 0) throw -2;
		}

		//scores = nodes_to_scores(root_node.child_nodes)
		LOGOUT(LOGLEVEL_TRACE, "�e���@��̉񐔂̃��X�g�����J�n.");
		ret = root_node->nodes_to_scores(scores);
		if (ret < 0) throw -3;
	}
	catch (int ret) {
		delete root_node;
		return ret;
	}

	// root_node�Ǝq�m�[�h�̃��������
	delete root_node;

	// ���@��L���̃`�F�b�N
	// ���@�肪�����ꍇ��scores���X�g�̃T�C�Y��0�ƂȂ�
	if (scores->size() > 0) {
		//	if temperature == 0:
		LOGOUT(LOGLEVEL_TRACE, "���x���l�����e���@��̑I���m����ݒ�B���x=%.1f.", temperature);
		if (temperature <= DBL_EPSILON) {
			LOGOUT(LOGLEVEL_TRACE, "���x=0�Ȃ̂ŁA�ő�X�R�A�̍��@��̑I���m��=1�Ƃ��܂�.", temperature);

			// n���ő�ƂȂ�m�[�h��������
			int max_n = INT_MIN;
			bool isFound = false;
			size_t max_i = 0;

			//action = np.argmax(scores)
			//	scores = np.zeros(len(scores))
			//	scores[action] = 1
			for (size_t i = 0; i < scores->size(); i++) {
				// ���̎��_�łƂ肠�������ׂĂ̗v�f��probability��0���Z�b�g���Ă���
				scores->at(i).probability = 0.0;

				if (scores->at(i).n >= max_n || isFound == false) {
					max_i = i;
					max_n = scores->at(i).n;
					isFound = true;
				}
			}

			// �ő�ƂȂ����m�[�h��probability��1�ɃZ�b�g
			if (isFound == true) {
				scores->at(max_i).probability = 1.0;
			}
			else {
				// ���̏�������������邱�Ƃ͖������O�̂���
				return -4;
			}
		}
		else {
			LOGOUT(LOGLEVEL_TRACE, "���x��0�Ȃ̂ŁA�{���c�}�����z�ɏ]���Ċe���@��̑I���m�����Z�b�g���܂�.", temperature);

			//scores = boltzman(scores, temperature)
			ret = bolzman(scores, temperature);
			if (ret < 0) {
				return -5;
			}
		}
	}

	LOGOUT(LOGLEVEL_INFO, "pv_mcts_scores()�I��.");

	//	return scores
	return 0;
}

int Pv_mcts_action::bolzman(std::vector<Score>* scores, Temperature temperature)
{
	LOGOUT(LOGLEVEL_INFO, "bolzman()�J�n. temperature=%.6f", temperature);

	double sum = 0.0;

	LOGOUT(LOGLEVEL_INFO, "bolzman()�J�n.");
	LOGOUT(LOGLEVEL_TRACE, "----- �񐔁��{���c�}�����z�l�ϊ� -----");
	//xs = [x * *(1 / temperature) for x in xs]
	for (size_t i = 0; i < scores->size(); i++) {
		scores->at(i).probability = pow((double)scores->at(i).n, 1.0 / temperature);
		LOGOUT(LOGLEVEL_TRACE, "%d: (x =%d, y = %d) %.6f => %.6f", i + 1, scores->at(i).x, scores->at(i).y, (double)scores->at(i).n, scores->at(i).probability);

		sum += scores->at(i).probability;
	}

	//return[x / sum(xs) for x in xs]
	LOGOUT(LOGLEVEL_TRACE, "----- �{���c�}�����z�l�����v1�̒l�ɐ��K�� -----");
	for (size_t i = 0; i < scores->size(); i++) {
		scores->at(i).probability = scores->at(i).probability / sum;
		LOGOUT(LOGLEVEL_TRACE, "%d: (x =%d, y = %d) => %.6f", i + 1, scores->at(i).x, scores->at(i).y, scores->at(i).probability);
	}

	LOGOUT(LOGLEVEL_INFO, "bolzman()�I��.");
	return 0;
}

//
//	�֐���: ranom_choice
//	�T�v: �w�肵���X�R�A�ɔ�Ⴕ���m�����z�ɏ]���Ď��̈���I������
//	
//	����:
//		state	���݂̃X�e�[�g(�ՖʁE���̐΂̐F)
//		scores	�I������m�����z
//
//	�o��:
//		action	�I���������̎�
//
//	�������@:
//		�e���@�育�Ƃɗ����𐶐����A���̒l�ɃX�R�A�l(=�m�����z)���悶��B���̏悶�����ʂ̒��ōő�̍��@���I������B
// 
int Pv_mcts_action::ranom_choice(State *state, std::vector<Score> scores, Action* action)
{
	LOGOUT(LOGLEVEL_INFO, "ranom_choice()�J�n.");

	// 0�`1�܂ł̗����𐶐�����
	double randomValue = (double)rand() / (double)RAND_MAX;

	LOGOUT(LOGLEVEL_TRACE, "�����l = %.6f", randomValue);

	// �m�����z�̒l��擪������Z�������l�𒴂�����O�̒l��I������
	double sum = 0.0;
	size_t x = 0, y = 0;

	for (size_t i = 0; i < scores.size(); i++) {
		sum += scores[i].probability;

		// ���v�l�������l�ȏ�ƂȂ����ꍇ�͂��̒l��I��
		// �m�����z�̍��v�l�͕K���ŏI�I�ɗ����l�̍ő�l��1.0�ƂȂ�̂ŁA�K���ȉ���if�������s�����
		if (sum >= randomValue - DBL_EPSILON) {
			x = scores[i].x;
			y = scores[i].y;

			LOGOUT(LOGLEVEL_TRACE, "%d�Ԗڂ̎��I���B", i + 1);

			*action = (int)x * 10 + (int)y;

			LOGOUT(LOGLEVEL_INFO, "ranom_choice()�I��. �ł����x = %d, y = %d �ł�.", x, y);
			return 0;
		}
	}

	// �m�����z�̍��v�l��1.0�����Ŗ{���͂��蓾�Ȃ��B���̏ꍇ�̓G���[�Ƃ��ĕԂ��B
	LOGOUT(LOGLEVEL_TRACE, "[ERROR] ���͂��ꂽ�m�����z�̐ϕ��l��1��菬�����ł��B�ϕ��l = %f�B", sum);

	LOGOUT(LOGLEVEL_INFO, "ranom_choice()�I��.");
	return -1;
}