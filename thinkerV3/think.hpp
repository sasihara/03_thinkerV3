#pragma once
#include "externalThinkerMessages.hpp"
#include "main.hpp"
#include "TFHandler.hpp"

#define MODELINFO	"(Unknown Model)"

#define SP_TEMPERATURE 1.0
//#define SP_TEMPERATURE 4.0

typedef int Action;

typedef struct _POLICY {
	int x;
	int y;
	float policy;
} Policy;

class Thinker {
public:
	int init(double _spTemperature);
	int think(int turn, DISKCOLORS* board, int *place, GameId gameId);
	char* getModelInfo();
	~Thinker();

private:
	boolean isInitialized = false;
	Model model;
	char modelInfo[1024];
	double spTemperature = SP_TEMPERATURE;
};