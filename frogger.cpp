#include <iostream>
#include <string>
using namespace std;

#include "SpriteAnimation.h"

class OneLoneCoder_Frogger
{
public:
	OneLoneCoder_Frogger() {}
	~OneLoneCoder_Frogger() {}

	// Called by olcConsoleGameEngine
	void createFrogger()
	{
		bufDanger = new bool[384 * 240];
		memset(bufDanger, 0, 384 * 240 * sizeof(bool));

		vecLanes =
		{
			{0.0f,  "wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // 64 elements per lane
			{-3.0f, ",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"},
			{3.0f,  ",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"},
			{2.0f,  ",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"},
			{0.0f,  "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"},
			{ 3.0f, "....asdf.......asdf....asdf..........asdf........asdf....asdf..."},
			{-3.0f, ".....ty..ty....ty....ty.....ty........ty..ty.ty......ty.......ty"},
			{ 4.0f, "..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."},
			{-2.0f,  "..ty.....ty.......ty.....ty......ty..ty.ty.......ty....ty......."},
			{0.0f,  "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"}
		};

		// You'll need these sprites!

		spriteAnimation.SetParams(0.125f, 64, 16, 4, 1, 4);

	}

	// Called by olcConsoleGameEngine
	void updateFrogger(float fElapsedTime)
	{
		fTimeSinceStart += fElapsedTime;
		animationData = spriteAnimation.GetInfo(fElapsedTime);

		// Frog is moved by platforms
		if (fFrogY <= 3) {
			fFrogX -= fElapsedTime * vecLanes[(int)/*floor/ceil?*/(fFrogY)].first;
			position.x = fFrogX * nCellSize;
		}

		// Collision detection - check four corners of frog against danger buffer
		bool tl = bufDanger[(int)(fFrogY * nCellSize + 1) * 384 + (int)(fFrogX * nCellSize + 1)];
		bool tr = bufDanger[(int)(fFrogY * nCellSize + 1) * 384 + (int)((fFrogX + 1) * nCellSize - 1)];
		bool bl = bufDanger[(int)((fFrogY + 1) * nCellSize - 1) * 384 + (int)(fFrogX * nCellSize + 1)];
		bool br = bufDanger[(int)((fFrogY + 1) * nCellSize - 1) * 384 + (int)((fFrogX + 1) * nCellSize - 1)];

		if ((tl || tr || bl || br) || fFrogY == 0)
		{
			// Frogs been hit :-(
			if (fFrogY == 0 && fFrogX * nCellSize >= 48 && fFrogX * nCellSize <= 96) {
				winPos1 = { 60,0 };
				winCond++;
			}
			if (fFrogY == 0 && fFrogX * nCellSize >= 168 && fFrogX * nCellSize <= 216) {
				winPos2 = { 180,0 };
				winCond++;
			}
			if (fFrogY == 0 && fFrogX * nCellSize >= 288 && fFrogX * nCellSize <= 336) {
				winPos3 = { 300,0 };
				winCond++;
			}

			fFrogX = 8.0f;
			fFrogY = 9.0f;
			position = { fFrogX * nCellSize, fFrogY * nCellSize };
			upPressed = rightPressed = leftPressed = backPressed = false;
			track = 0;

			if (winCond == 3) {
				winPos1 = winPos2 = winPos3 = { 1000, 1000 };
				winCond = 0;
			}
		}
	}

	vector<pair<float, string>> vecLanes;

	float fTimeSinceStart = 0.0f;
	float fFrogX = 8.0f;
	float fFrogY = 9.0f;
	int nLaneWidth = 18;
	int nCellSize = 24;
	olc::vf2d position = { fFrogX * nCellSize, fFrogY * nCellSize };
	olc::vf2d size = { 24.0f, 24.0f };
	//olc::vf2d sourcePos = { 0.0f, 0.0f };
	//olc::vf2d sourceSize = { 8.0f, 8.0f };
	int track = 0;
	int winCond = 0;
	bool upPressed, rightPressed, leftPressed, backPressed = false;
	bool* bufDanger = nullptr;

	olc::vf2d winPos1 = { 1000, 1000 };
	olc::vf2d winPos2 = { 1000, 1000 };
	olc::vf2d winPos3 = { 1000, 1000 };

	SpriteAnimation spriteAnimation;
	AnimationData animationData;
};