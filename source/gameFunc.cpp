#include "gameFunc.hpp"

#if DEBUG_MODE
void Debug(Game &game)
{
  consoleClear();
  DrawText("mode: " + std::to_string(game.mode), 1, 1);
  DrawText("save: " + std::to_string(game.saveData[game.mode]), 1, 2);
  DrawText("hiScore: " + std::to_string(game.hiScore), 1, 3);
  while (true)
  {
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_A) break;
  }
}
#endif

void DrawText(const std::string text, const int x, const int y)
{
	printf("\x1b[%d;%dH%s", y, x, text.c_str());
}

void Display(Game &game)
{
	consoleClear();
	DrawText("Score   : " + std::to_string(game.score), 2, 2);
	DrawText("HiScore : " + std::to_string(game.hiScore), 2, 3);
	DrawText("Level   : " + std::to_string(game.level), 2, 4);
	DrawText("Mode    : " + std::to_string(game.mode), 2, 5);
	DrawText("Press START to Game exit.", 2, 7);
}

void ModeSelect(Game &game)
{
	consoleClear();
	DrawText("Select difficulty level.", 1, 1);
	const int option = 6;
	std::string text[] = {"Easy", "Normal", "Hard", "Very Hard", "Expert", "Impossible"};
	std::string arrow[option];
	for (int i = 0; i < option; i++)
		arrow[i] = "   ";
	if (game.mode < 0) game.mode = option - 1;
	if (game.mode > option - 1) game.mode = 0;
	arrow[game.mode] = "-> ";
	for (int i = 0; i < option; i++)
		DrawText(arrow[i] + text[i], 1, 3 + i);
}

bool IsHit(C2D_Sprite *target, C2D_Sprite *sprite)
{
	float relativeMaxY = target->params.pos.y + target->params.pos.h / 2;
	float relativeMinY = target->params.pos.y - target->params.pos.h / 2;
	float relativeMaxX = target->params.pos.x + target->params.pos.w / 2;
	float relativeMinX = target->params.pos.x - target->params.pos.w / 2;

	if (relativeMaxX >= sprite->params.pos.x && relativeMinX <= sprite->params.pos.x)
	{
		if (relativeMaxY >= sprite->params.pos.y && relativeMinY <= sprite->params.pos.y)
		{
			return true;
		}
	}
	return false;
}

void LevelUp(Game &game)
{
	if (game.score < game.level * 100) return;
	game.level++;
	game.fallingSpeed = (game.level * 0.5) + BASE_FALLING_SPEED;
}

int GetBomCount(Game &game)
{
  const int bomTable[] = {1, 2, 3, 4, 5, 10};
  return bomTable[game.mode];
}

void GameReset(Game &game, Flag &flag)
{
  game.score = 0;
  game.level = 1;
  game.fallingSpeed = 2;

  for (int i = 0; i < 2; i++)
    flag.fukuroFlag[i] = false;
  for (int i = 0; i < 3; i++)
    flag.coinFlag[i] = false;
  for (int i = 0; i < GetBomCount(game); i++)
    flag.bomFlag[i] = false;
}

bool GameOver()
{
	DrawText("Game Over!", 2, 9);
	DrawText("Press A to Retry.", 2, 10);
	DrawText("Press START to Game exit.", 2, 11);
	while (true)
	{
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) return false;
		if (kDown & KEY_START) return true;
	}
	return true;
}

void HiScoreUpdated(Game &game)
{
	if (game.score > game.hiScore)
    game.hiScore = game.score;
}

void SaveLoad(Game &game)
{
	FILE *file;
	file = fopen(SAVE_FILE_PATH, "rb");
  if (file)
  {
    fread(game.saveData, sizeof(int), SAVE_DATA_COUNT, file);
    game.hiScore = game.saveData[game.mode];
    fclose(file);
  } else {
    FILE *newFile;
    newFile = fopen(SAVE_FILE_PATH, "wb");
    for (int i = 0; i < SAVE_DATA_COUNT; i++)
      game.saveData[i] = 0;
    fwrite(game.saveData, sizeof(int), SAVE_DATA_COUNT, newFile);
    fclose(newFile);
  }
}

void SaveWrite(Game &game)
{
  FILE *file;
  file = fopen(SAVE_FILE_PATH, "rb");
  fread(game.saveData, sizeof(int), SAVE_DATA_COUNT, file);
  if (! file || game.hiScore <= game.saveData[game.mode])
  {
    fclose(file);
    return;
  }
  fclose(file);

  file = fopen(SAVE_FILE_PATH, "wb");
  if (game.hiScore > game.saveData[game.mode])
  {
    game.saveData[game.mode] = game.hiScore;
    fwrite(game.saveData, sizeof(int), SAVE_DATA_COUNT, file);
  }
  fclose(file);
}
