#ifndef MAIN_HPP
#define MAIN_HPP

#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <iostream>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define BASE_FALLING_SPEED 2
#define MAX_BOM 10
#define MAX (SCREEN_WIDTH - 24)
#define MIN 24
#define RAND_POS (rand() % (MAX - MIN + 1) + MIN)
#define SAVE_DATA_COUNT 5
#define SAVE_FILE_PATH "MikoMoneyGame.sav"
#define DEBUG_MODE 0

struct Game
{
	int playerSpeed = 3;
	int fallingSpeed = 2;
	int score = 0;
	int hiScore = 0;
	int level = 1;
	int mode = 1;
  int saveData[SAVE_DATA_COUNT] = {0,0,0,0,0};
};

struct Flag
{
	bool coinFlag[3] = {false, false, false};
	bool fukuroFlag[2] = {false, false};
	bool bomFlag[MAX_BOM];
};

#if DEBUG_MODE
void Debug(Game &game);
#endif
void DrawText(const std::string text, const int x, const int y);
void Display(Game &game);
bool IsHit(C2D_Sprite *target, C2D_Sprite *sprite);
void LevelUp(Game &game);
int GetBomCount(Game &game);
void GameReset(Game &game, Flag &flag);
bool GameOver();
void HiScoreUpdated(Game &game);
void ModeSelect(Game &game);
void SaveLoad(Game &game);
void SaveWrite(Game &game);

#endif
