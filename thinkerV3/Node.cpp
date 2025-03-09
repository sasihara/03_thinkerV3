#include "TFHandler.hpp"
//#include "State.hpp"
#include "common.h"
#include "Node.hpp"
#include "logging.h"

// Global
extern Logging logging;

//----------------------------------------------------------------------------------------
Node::Node(Model *_model, State *_state, double _p)
{
	int ret;

	model = _model;
	p = _p;

	ret = _state->copyTo(&state);
	if (ret < 0) return;

	w = 0.0;
	n = 0;
	child_nodes.clear();

	initialized = true;
}

Node::~Node()
{
	size_t n = child_nodes.size();

	for (size_t i = 0; i < n; i++) {
		delete child_nodes[i].node;
	}
}

int Node::evaluate(float *result)
{
	int ret;
	bool isGaveOver;
	float value = 0.0;

	// �Ֆʂ̃��O�o��
	//state.logout(logging);

	ret = state.IsGameOver(&isGaveOver);
	if (ret < 0) return -1;

	if (isGaveOver == true) {
		GAMERESULT gameResult;
		
		ret = state.getGameResult(&gameResult);
		if (ret < 0) return -2;

		if (gameResult == GAMERESULT_ERROR) return -1;
		else if (gameResult == GAMERESULT_LOSE) value = -1;

		LOGOUT(LOGLEVEL_TRACE, "�Q�[������ = %s, value = %.6f", gameResult == GAMERESULT_WIN ? "����" : gameResult == GAMERESULT_LOSE ? "����" : "��������", value);

		w += value;
		n += 1;
		*result = value;
		LOGOUT(LOGLEVEL_TRACE, "w <= %.6f, n <= %d, result = %.6f", w, n, *result);
	}
	else if (child_nodes.size() == 0) {

		// Call predict()
		float policies[DN_OUTPUT_SIZE];
		
		LOGOUT(LOGLEVEL_TRACE, "�q�m�[�h��=0�Ȃ̂�NN�ŔՖʂ�]�����܂�.");

		ret = predict(model, state, policies, &value);

		LOGOUT(LOGLEVEL_TRACE, "predict()�߂�l = %d, value = %.6f", ret, value);

		if (ret < 0) {
			return -3;
		}

		// Update w, n
		w += (double)value;
		n += 1;

		 //Update child_nodes
		child_nodes.clear();

		LOGOUT(LOGLEVEL_TRACE, "�e���@��ɂ��Đ΂�u������̔Ֆʂ�child_nodes�ɒǉ����܂�.");

		// �e���@��ɂ��čX�V��̔Ֆʂ�child_nodes�ɒǉ�
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				// �u����ꏊ��T��
				int flag;

				if ((flag = state.check(x, y, state.currentPlayer)) > 0) {

					LOGOUT(LOGLEVEL_TRACE, "���@�肪������܂���. x = %d, y = %d.", x, y);

					//State* next;
					State next;
					//try {
					//	next = new State();
					//}
					//catch (...) {
					//	return -4;
					//}

					// �V���ȔՖʂ��R�s�[���Đ�������
					//ret = next->init(state.board, state.opponent);
					ret = next.init(state.board, state.opponent);
					if (ret < 0) return -5;

					// �R�s�[�����Ֆʏ�Ő΂�u���Ď��̔Ֆʂ𐶐�����
					//ret = next->turnDisk(x, y, state.currentPlayer, flag);
					ret = next.turnDisk(x, y, state.currentPlayer, flag);
					if (ret < 0) return -6;

					// �V�����Ֆʂ�������Node�C���X�^���X���쐬
					Node* child_node;
					
					try {
						//child_node = new Node(model, next, policies[x * 8 + y]);
						child_node = new Node(model, &next, policies[x * 8 + y]);
					}
					catch(...){
						return -5;
					}

					// Node�C���X�^���X��child_nodes�ɒǉ�
					ChildNode childNode;
					childNode.x = x;
					childNode.y = y;
					childNode.node = child_node;
					child_nodes.push_back(childNode);

					LOGOUT(LOGLEVEL_TRACE, "child_nodes�ɒǉ����܂���.");
				}
			}
		}

		*result = value;
		LOGOUT(LOGLEVEL_TRACE, "w <= %.6f, n <= %d, result = %.6f", w, n, *result);
	}
	else {
		LOGOUT(LOGLEVEL_TRACE, "�q�m�[�h��>0�Ȃ̂ŃA�[�N�]���l�̍ł������q�m�[�h��I�����܂�.");

		// Call evaluate()
		ChildNode* next_child_node;

		ret = get_next_child_node(&next_child_node);
		if (ret < 0) return -7;

		ret = next_child_node->node->evaluate(&value);
		if (ret < 0) {
			LOGOUT(LOGLEVEL_TRACE, "[ERROR] evaluate()�ŃG���[�����Bret = %d", ret);
			return -8;
		}

		// Update w, n
		w += value;
		n += 1;
		*result = value;

		LOGOUT(LOGLEVEL_TRACE, "w <= %.6f, n <= %d, result = %.6f", w, n, *result);
	}

	return 0;
}

int Node::get_next_child_node(ChildNode** next_child_node)
{
	int ret;
	int t;
	double max_pucb_value = -DBL_MAX;

	LOGOUT(LOGLEVEL_TRACE, "get_next_child_node()�J�n.");

	// ���s�񐔂�0�̎q�m�[�h��T��
	for (size_t i = 0; i < child_nodes.size(); i++) {
		if (child_nodes[i].node->n == 0) {
			LOGOUT(LOGLEVEL_TRACE, "���s��=0�̃m�[�h�����������̂ŁA���̃m�[�h��D�悵�ĕԂ��܂��Bi = %d, (x, y) = (%d, %d)", i, child_nodes[i].x, child_nodes[i].y);
			*next_child_node = &child_nodes[i];
			return 0;
		}
	}
	
	// �p�����[�^t(���v�l)���v�Z����
	ret = sum_child_nodes(&t);
	if (ret < 0) return -1;

	for (size_t i = 0; i < child_nodes.size(); i++) {
		double pucb_value_1st = child_nodes[i].node->n > 0 ? child_nodes[i].node->w / child_nodes[i].node->n : 0.0;
		double pucb_value_2nd = C_PUCT * child_nodes[i].node->p * sqrt((double)t) / (1 + child_nodes[i].node->n);
		double pucb_value = pucb_value_1st + pucb_value_2nd;

		LOGOUT(LOGLEVEL_TRACE, "n = %d, w = %.6f, p = %.6f, t = %d, C_PUCT = %.6f, w / n = %f, C_PUCT * p * sqrt(t) / (1 + n) = %.6f",
			child_nodes[i].node->n,
			child_nodes[i].node->w,
			child_nodes[i].node->p,
			t,
			C_PUCT,
			pucb_value_1st,
			pucb_value_2nd
		);

		LOGOUT(LOGLEVEL_TRACE, "child_nodes No.%d: max = %.6g, x = %d, y = %d, pucb_value = %.6f", i, max_pucb_value, child_nodes[i].x, child_nodes[i].y, pucb_value);

		if (pucb_value > max_pucb_value) {
			LOGOUT(LOGLEVEL_TRACE, "�ő�l���X�V");
			max_pucb_value = pucb_value;
			*next_child_node = &child_nodes[i];
		}
	}

	LOGOUT(LOGLEVEL_TRACE, "get_next_child_node()�I��. �I�����ꂽ��: x = %d, y = %d.", (ChildNode*)(*next_child_node)->x, (ChildNode*)(*next_child_node)->y);
	return 0;
}

int Node::sum_child_nodes(int* result)
{
	size_t i, num;

	// child_nodes�̒�����̏ꍇ�̓G���[��Ԃ�
	num = child_nodes.size();
	if (num <= 0) return -1;

	// child_nodes�Ɋ܂܂��node��n�̑��a�����߂�
	*result = 0;

	for (i = 0; i < num; i++) {
		*result += child_nodes[i].node->n;
	}

	return 0;
}

int Node::nodes_to_scores(std::vector<Scores>* numOfTrial)
{
	for (int i = 0; i < child_nodes.size(); i++) {
		Scores score;
		score.x = child_nodes[i].x;
		score.y = child_nodes[i].y;
		score.n = child_nodes[i].node->n;

		numOfTrial->push_back(score);
	}

	return 0;
}

