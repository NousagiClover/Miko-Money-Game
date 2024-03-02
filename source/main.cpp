#include "gameFunc.hpp"

Game game;
Flag flag;

int main() {
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C2D_SpriteSheet spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	C2D_Sprite bg;
	C2D_Sprite miko;
	C2D_Sprite coin[3];
	C2D_Sprite fukuro[2];
	C2D_Sprite bom[MAX_BOM];

	// 背景
	C2D_SpriteFromSheet(&bg, spriteSheet, 0);
	C2D_SpriteSetCenter(&bg, 0.5, 0.5);
	C2D_SpriteSetPos(&bg, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	for (int i = 0; i < MAX_BOM; i++)
		flag.bomFlag[i] = false;

	// coin
	for (int i = 0; i < 3; i++)
	{
		C2D_SpriteFromSheet(&coin[i], spriteSheet, 2);
		C2D_SpriteSetCenter(&coin[i], 0.5, 0.5);
	}

	// fukuro
	for (int i = 0; i < 2; i++)
	{
		C2D_SpriteFromSheet(&fukuro[i], spriteSheet, 3);
		C2D_SpriteSetCenter(&fukuro[i], 0.5, 0.5);
	}

	// bom
	for (int i = 0; i < MAX_BOM; i++)
	{
		C2D_SpriteFromSheet(&bom[i], spriteSheet, 4);
		C2D_SpriteSetCenter(&bom[i], 0.5, 0.5);
		C2D_SpriteSetPos(&bom[i], -100, -100);
	}

	// miko
	C2D_SpriteFromSheet(&miko, spriteSheet, 1);
	C2D_SpriteSetCenter(&miko, 0.5, 0.5);
	C2D_SpriteSetPos(&miko, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 32);

	ModeSelect(game);
	while (true)
	{
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_DUP)
		{
			game.mode--;
			ModeSelect(game);
		}
		if (kDown & KEY_DDOWN)
		{
			game.mode++;
			ModeSelect(game);
		}
		if (kDown & KEY_A) break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	SaveLoad(game);
	Display(game);

	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		if (kDown & KEY_START)
			break;

		// miko 移動
		if (kHeld & KEY_LEFT)
			C2D_SpriteMove(&miko, -game.playerSpeed, 0);
		if (kHeld & KEY_RIGHT)
			C2D_SpriteMove(&miko, game.playerSpeed, 0);
		if (miko.params.pos.x < 32) miko.params.pos.x = 32;
		if (miko.params.pos.x > SCREEN_WIDTH - 32) miko.params.pos.x = SCREEN_WIDTH - 32;

		// coin
		for (int i = 0; i < 3; i++)
		{
			if (! flag.coinFlag[i])
			{
				C2D_SpriteSetPos(&coin[i], RAND_POS, 0);
				flag.coinFlag[i] = true;
			}
			C2D_SpriteMove(&coin[i], 0, game.fallingSpeed);
			if (IsHit(&coin[i], &miko))
			{
				game.score++;
				HiScoreUpdated(game);
				flag.coinFlag[i] = false;
				Display(game);
				LevelUp(game);
			}
			if (coin[i].params.pos.y > SCREEN_HEIGHT) flag.coinFlag[i] = false;
		}

		// fukuro
		for (int i = 0; i < 2; i++)
		{
			if (! flag.fukuroFlag[i])
			{
				C2D_SpriteSetPos(&fukuro[i], RAND_POS, 0);
				flag.fukuroFlag[i] = true;
			}
			C2D_SpriteMove(&fukuro[i], 0, game.fallingSpeed);
			if (IsHit(&fukuro[i], &miko))
			{
				game.score += 3;
				HiScoreUpdated(game);
				flag.fukuroFlag[i] = false;
				Display(game);
				LevelUp(game);
			}
			if (fukuro[i].params.pos.y > SCREEN_HEIGHT) flag.fukuroFlag[i] = false;
		}

		// bom
		for (int i = 0; i < GetBomCount(game); i++)
		{
			if (! flag.bomFlag[i])
			{
				C2D_SpriteSetPos(&bom[i], RAND_POS, 0);
				flag.bomFlag[i] = true;
			}
			C2D_SpriteMove(&bom[i], 0, game.fallingSpeed);
			if (IsHit(&bom[i], &miko))
			{
				if (GameOver()) goto exitGame;
				GameReset(game, flag);
				Display(game);
			}
			if (bom[i].params.pos.y > SCREEN_HEIGHT) flag.bomFlag[i] = false;
		}

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0, 0.0, 0.0, 1.0));
		C2D_SceneBegin(top);
		C2D_DrawSprite(&bg);
		for (int i = 0; i < 3; i++)
			C2D_DrawSprite(&coin[i]);
		for (int i = 0; i < 2; i++)
			C2D_DrawSprite(&fukuro[i]);
		for (int i = 0; i < GetBomCount(game); i++)
			C2D_DrawSprite(&bom[i]);
		C2D_DrawSprite(&miko);
		C3D_FrameEnd(0);
	}

	exitGame:
	SaveWrite(game);
	C2D_SpriteSheetFree(spriteSheet);
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
