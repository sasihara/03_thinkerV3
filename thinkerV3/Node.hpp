#pragma once
#include <vector>
#include "State.hpp"

#define	C_PUCT	1.0

class Node;

typedef	double	Temperature;

typedef struct _CHILDNODES {
	int x, y;
	Node* node;
} ChildNode;

typedef struct _SCORES {
	int x, y;
	int n;
	double probability;
} Scores;

class Node {
public:
	Model *model;
	int n;

	Node(Model *_model, State *_state, double _p);
	~Node();
	int evaluate(float *result);
	int get_next_child_node(ChildNode** next_child_node);
	int sum_child_nodes(int* result);
	int nodes_to_scores(std::vector<Scores> *scores);
private:
	bool initialized = false;
	State state;
	double p;
	double w;
	std::vector<ChildNode> child_nodes;
};
