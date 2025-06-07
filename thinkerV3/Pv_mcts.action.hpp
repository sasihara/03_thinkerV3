#pragma once
#include "TFHandler.hpp"
#include "State.hpp"
#include "Node.hpp"
//#include "history.hpp"

#define PV_EVALUATE_COUNT 100

typedef double	Temperature;
typedef int		Action;

class Pv_mcts_action {
public:
	Pv_mcts_action(Model* _model, Temperature _temperature);
	int run(State *state, Action* action, GameId gameId);
	int pv_mcts_scores(State *state, std::vector<Score>* scores);
	int ranom_choice(State *state, std::vector<Score> scores, Action *action);

private:
	Model *model;
	Temperature temperature;
	//History history;

	int bolzman(std::vector<Score>* scores, Temperature temperature);
};

bool compare(Score a, Score b);
