#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "racing.cpp"
#include "frogger.cpp"
#include "tetris.cpp"
#include "asteroids.cpp"
#include "snake.cpp"

#include <unordered_map>


constexpr int32_t nPatch = 8;
//int state = 1;

class menuobject
{
public:
	menuobject()
	{
		sName = "root";
	}

	menuobject(const std::string& name)
	{
		sName = name;
	}

	menuobject& SetTable(int nColumns, int nRows)
	{
		vCellTable = { nColumns, nRows };	return *this;
	}

	menuobject& SetID(int32_t id)
	{
		nID = id; return *this;
	}

	int32_t GetID()
	{
		return nID;
	}

	std::string& GetName()
	{
		return sName;
	}

	olc::vi2d getCurPosition(int32_t pos) {
		return positions[pos];
	}

	menuobject& Enable(bool b)
	{
		bEnabled = b; return *this;
	}

	bool Enabled()
	{
		return bEnabled;
	}

	void ResetIndex() {
		ind = 0;
		//cout << "*" << ind << "*" << endl;
	}

	// For now, cells are simply one line strings
	olc::vi2d GetSize()
	{
		return { int32_t(sName.size()), 1 };
	}

	olc::vi2d& GetCursorPosition()
	{
		return vCursorPos;
	}

	menuobject& operator[](const std::string& name)
	{
		if (itemPointer.count(name) == 0)
		{
			itemPointer[name] = items.size();
			items.push_back(menuobject(name));
		}

		return items[itemPointer[name]];
	}

	void Build()
	{
		// Recursively build all children, so they can determine their size, use
		// that size to indicate cell sizes if this object contains more than 
		// one item
		for (auto& m : items)
		{
			m.Build();

			vCellSize.x = std::max(m.GetSize().x, vCellSize.x);
			vCellSize.y = std::max(m.GetSize().y, vCellSize.y);
		}

		vCellSize.x += 5;
		vCellSize.y *= 10;

		// Adjust size of this object (in patches) if it were rendered as a panel
		vSizeInPatches.x = vCellTable.x * vCellSize.x + (vCellTable.x - 1) * vCellPadding.x + 2;
		vSizeInPatches.y = vCellTable.y * vCellSize.y + (vCellTable.y - 1) * vCellPadding.y + 2;
	}

	void Build2() //replace
	{
		// Recursively build all children, so they can determine their size, use
		// that size to indicate cell sizes if this object contains more than 
		// one item
		for (auto& m : items)
		{
			////if (m.HasChildren())
			//{
			m.Build2();
			//}

			// Longest child name determines cell width
			vCellSize.x = std::max(m.GetSize().x, vCellSize.x) + 1;
			vCellSize.y = std::max(m.GetSize().y, vCellSize.y);
		}

		// Adjust size of this object (in patches) if it were rendered as a panel
		vSizeInPatches.x = vCellTable.x * vCellSize.x + (vCellTable.x - 1) * vCellPadding.x + 2;
		vSizeInPatches.y = vCellTable.y * vCellSize.y + (vCellTable.y - 1) * vCellPadding.y + 2;

		// Calculate how many rows this item has to hold
		nTotalRows = (items.size() / vCellTable.x) + (((items.size() % vCellTable.x) > 0) ? 1 : 0);
	}

	void DrawSelf(olc::PixelGameEngine& pge, olc::Sprite* sprGFX, olc::Sprite* games, olc::vi2d vScreenOffset)
	{
		// === Draw Panel
		
		// Record current pixel mode user is using
		olc::Pixel::Mode currentPixelMode = pge.GetPixelMode();
		pge.SetPixelMode(olc::Pixel::MASK);

		// Draw Panel & Border
		olc::vi2d vPatchPos = { 0,0 };
		for (vPatchPos.x = 0; vPatchPos.x < vSizeInPatches.x; vPatchPos.x++)
		{
			for (vPatchPos.y = 0; vPatchPos.y < vSizeInPatches.y; vPatchPos.y++)
			{
				// Determine position in screen space
				olc::vi2d vScreenLocation = vPatchPos * nPatch + vScreenOffset; //1

				// Calculate which patch is needed
				olc::vi2d vSourcePatch = { 0, 0 };
				if (vPatchPos.x > 0) vSourcePatch.x = 1;
				if (vPatchPos.x == vSizeInPatches.x - 1) vSourcePatch.x = 2;
				if (vPatchPos.y > 0) vSourcePatch.y = 1;
				if (vPatchPos.y == vSizeInPatches.y - 1) vSourcePatch.y = 2;

				// Draw Actual Patch
				pge.DrawPartialSprite(vScreenLocation, sprGFX, vSourcePatch * nPatch, vPatchSize);
			}
		}

		// === Draw Panel Contents
		olc::vi2d vCell = { 0,0 };
		vPatchPos = { 1,1 };

		// Draw Visible Items
		for (int32_t i = 0; i < 5; i++)
		{
			// Cell location
			vCell.x = i % vCellTable.x;
			vCell.y = (i / vCellTable.x) + 1;

			// Patch location (including border offset and padding)
			vPatchPos.x = i / 3 == 0 ? 47 + (i * 109) + ((72 - (items[i].sName.size() * 8)) / 2) : 101 + ((i - 3) * 109) + ((72 - (items[i].sName.size() * 8)) / 2); //vCell.x * (vCellSize.x + vCellPadding.x) + ((vCellSize.x - items[nTopLeftItem + i].GetSize().x) / 2) + 1; //((vCellSize.x - items[nTopLeftItem + i].sName.size()) / 2);//
			vPatchPos.y = (vCell.y * (vCellSize.y + vCellPadding.y) - 1) * nPatch + vScreenOffset.y;

			// Actual screen location in pixels
			olc::vi2d vScreenLocation4 = vPatchPos; //4
			positions.push_back(vScreenLocation4);

			// Display Item Header
			pge.DrawString(vScreenLocation4, items[i].sName, items[i].bEnabled ? olc::WHITE : olc::DARK_GREY);
		}

		for (int32_t i = 0; i < 5; i++) {
			olc::vi2d vScreenLocation5 = { i / 3 == 0 ? 47 + (i * 109) : 101 + ((i - 3) * 109), positions[i].y - 56 }; //5

			pge.DrawPartialSprite(vScreenLocation5, games, olc::vi2d((i * 2) + 1, 0) * 72, olc::vi2d(72, 48));
		}

		// Calculate cursor position in screen space in case system draws it
		//cout << ind << endl;
		vCursorPos.x = positions[ind].x - (nPatch * 2);
		vCursorPos.y = positions[ind].y;

		switch (ind) {
		case 0:
			pge.DrawPartialSprite({ 47, positions[ind].y - 56 }, games, olc::vi2d((ind * 2), 0) * 72, olc::vi2d(72, 48));
			break;
		case 1:
			pge.DrawPartialSprite({ 156, positions[ind].y - 56 }, games, olc::vi2d((ind * 2), 0) * 72, olc::vi2d(72, 48));
			break;
		case 2:
			pge.DrawPartialSprite({ 265, positions[ind].y - 56 }, games, olc::vi2d((ind * 2), 0) * 72, olc::vi2d(72, 48));
			break;
		case 3:
			pge.DrawPartialSprite({ 101, positions[ind].y - 56 }, games, olc::vi2d((ind * 2), 0) * 72, olc::vi2d(72, 48));
			break;
		case 4:
			pge.DrawPartialSprite({ 210, positions[ind].y - 56 }, games, olc::vi2d((ind * 2), 0) * 72, olc::vi2d(72, 48));
			break;
		default:
			break;
		}
	}

	void DrawSelf2(olc::PixelGameEngine& pge, olc::Sprite* sprGFX, olc::vi2d vScreenOffset) //replace
	{
		// === Draw Panel

		// Record current pixel mode user is using
		olc::Pixel::Mode currentPixelMode = pge.GetPixelMode();
		pge.SetPixelMode(olc::Pixel::MASK);

		// Draw Panel & Border
		olc::vi2d vPatchPos = { 0,0 };
		for (vPatchPos.x = 0; vPatchPos.x < vSizeInPatches.x; vPatchPos.x++)
		{
			for (vPatchPos.y = 0; vPatchPos.y < vSizeInPatches.y; vPatchPos.y++)
			{
				// Determine position in screen space
				olc::vi2d vScreenLocation = vPatchPos * nPatch + vScreenOffset;

				// Calculate which patch is needed
				olc::vi2d vSourcePatch = { 0, 0 };
				if (vPatchPos.x > 0) vSourcePatch.x = 1;
				if (vPatchPos.x == vSizeInPatches.x - 1) vSourcePatch.x = 2;
				if (vPatchPos.y > 0) vSourcePatch.y = 1;
				if (vPatchPos.y == vSizeInPatches.y - 1) vSourcePatch.y = 2;

				// Draw Actual Patch
				pge.DrawPartialSprite(vScreenLocation, sprGFX, vSourcePatch * nPatch, vPatchSize);
			}
		}

		// === Draw Panel Contents
		olc::vi2d vCell = { 0,0 };
		vPatchPos = { 1,1 };

		// Work out visible items
		int32_t nTopLeftItem = nTopVisibleRow * vCellTable.x;
		int32_t nBottomRightItem = vCellTable.y * vCellTable.x + nTopLeftItem;

		// Clamp to size of child item vector
		nBottomRightItem = std::min(int32_t(items.size()), nBottomRightItem);
		int32_t nVisibleItems = nBottomRightItem - nTopLeftItem;

		// Draw Scroll Markers (if required)
		if (nTopVisibleRow > 0)
		{
			vPatchPos = { vSizeInPatches.x - 2, 0 };
			olc::vi2d vScreenLocation = vPatchPos * nPatch + vScreenOffset;
			olc::vi2d vSourcePatch = { 3, 0 };
			pge.DrawPartialSprite(vScreenLocation, sprGFX, vSourcePatch * nPatch, vPatchSize);
		}

		if ((nTotalRows - nTopVisibleRow) > vCellTable.y)
		{
			vPatchPos = { vSizeInPatches.x - 2, vSizeInPatches.y - 1 };
			olc::vi2d vScreenLocation = vPatchPos * nPatch + vScreenOffset;
			olc::vi2d vSourcePatch = { 3, 2 };
			pge.DrawPartialSprite(vScreenLocation, sprGFX, vSourcePatch * nPatch, vPatchSize);
		}

		// Draw Visible Items
		for (int32_t i = 0; i < nVisibleItems; i++)
		{
			// Cell location
			vCell.x = i % vCellTable.x;
			vCell.y = i / vCellTable.x;

			// Patch location (including border offset and padding)
			vPatchPos.x = vCell.x * (vCellSize.x + vCellPadding.x) + 2;
			vPatchPos.y = vCell.y * (vCellSize.y + vCellPadding.y) + 1;

			// Actual screen location in pixels
			olc::vi2d vScreenLocation = vPatchPos * nPatch + vScreenOffset;

			// Display Item Header
			pge.DrawString(vScreenLocation, items[nTopLeftItem + i].sName, items[nTopLeftItem + i].bEnabled ? olc::WHITE : olc::DARK_GREY);
		}

		// Calculate cursor position in screen space in case system draws it
		vCursorPos.x = (0 * (vCellSize.x + vCellPadding.x)) * nPatch + vScreenOffset.x - nPatch + 8;
		vCursorPos.y = ((ind - nTopVisibleRow) * (vCellSize.y + vCellPadding.y)) * nPatch + vScreenOffset.y + nPatch;

	}

	void OnUp()
	{
		if (ind > 0) {
			ind--;
		}
	}

	void OnDown()
	{
		if (ind < items.size() - 1) {
			ind++;
		}
	}

	void OnLeft()
	{
		if (ind > 0) {
			ind--;
		}
	}

	void OnRight()
	{
		if (ind < items.size() - 1) {
			ind++;
		}
	}

	menuobject* OnConfirm()
	{
		return this;
	}

	menuobject* GetSelectedItem()
	{
		return &items[ind];
	}


protected:
	int32_t nID = -1;
	olc::vi2d vCellTable = { 1, 0 };
	std::unordered_map<std::string, size_t> itemPointer;
	std::vector<menuobject> items;
	std::vector<olc::vi2d> positions;
	olc::vi2d vSizeInPatches = { 0, 0 };
	olc::vi2d vCellSize = { 0, 0 };
	olc::vi2d vCellPadding = { 2, 0 };
	//olc::vi2d vCellCursor = { 0, 0 };
	//int32_t nCursorItem = 0;
	int32_t ind = 0;
	const olc::vi2d vPatchSize = { nPatch, nPatch };
	std::string sName;
	olc::vi2d vCursorPos = { 0, 0 };
	bool bEnabled = true;
	int32_t nTopVisibleRow = 0; //change
	int32_t nTotalRows = 0; //change
};

class menumanager
{
public:
	menumanager() { }

	void Open(menuobject* mo) { Close(); mo->ResetIndex(); panels.push_back(mo); }
	void Close() { panels.clear(); }

	void OnUp() { if (!panels.empty()) panels.back()->OnUp(); } //--> acts only on pause menu
	void OnDown() { if (!panels.empty()) panels.back()->OnDown(); } //--> acts only on pause menu

	void OnLeft() { if (!panels.empty()) panels.back()->OnLeft(); }
	void OnRight() { if (!panels.empty()) panels.back()->OnRight(); }
	void OnBack() { if (!panels.empty()) panels.pop_back(); }

	menuobject* OnConfirm()
	{
		if (panels.empty()) return nullptr;

		menuobject* next = panels.back()->OnConfirm();
		if (next == panels.back())
		{
			if (panels.back()->GetSelectedItem()->Enabled())
				return panels.back()->GetSelectedItem();
		}
		else
		{
			if (next->Enabled())
				panels.push_back(next);
		}

		return nullptr;
	}

	void Draw(olc::PixelGameEngine& pge, olc::Sprite* sprGFX, olc::Sprite* games, olc::vi2d vScreenOffset, int menu)
	{
		if (panels.empty()) return;

		// Draw Visible Menu System
		if (menu == 0) {
			for (auto& p : panels)
			{
				p->DrawSelf(pge, sprGFX, games, vScreenOffset);
				vScreenOffset += {10, 10};
			}
		}
		else {
			for (auto& p : panels)
			{
				p->DrawSelf2(pge, sprGFX, vScreenOffset);
				vScreenOffset += {10, 10};
			}
		}

		// Draw Cursor
		olc::Pixel::Mode currentPixelMode = pge.GetPixelMode();
		pge.SetPixelMode(olc::Pixel::ALPHA);
		pge.DrawPartialSprite(panels.back()->GetCursorPosition(), sprGFX, olc::vi2d(4, 0) * nPatch, { nPatch * 2, nPatch * 2 });

		//std::cout << panels.back()->GetCursorPosition().y;

		pge.SetPixelMode(currentPixelMode);
	}

private:
	std::list<menuobject*> panels;
};

// Override base class with your custom functionality
class olcRetroPopUpMenu : public olc::PixelGameEngine
{
public:
	olcRetroPopUpMenu()
	{
		sAppName = "Retro Pop-Up Menu";
	}

	olc::Sprite* sprGFX = nullptr;
	olc::Sprite* games = nullptr;

	olc::Sprite* spriteTruck = nullptr;
	olc::Sprite* spriteLog = nullptr;
	olc::Sprite* spritePavement = nullptr;
	olc::Sprite* spriteCar1 = nullptr;
	olc::Sprite* spriteCar2 = nullptr;
	olc::Sprite* spriteWall = nullptr;
	olc::Sprite* spriteHome = nullptr;

	olc::Sprite* spriteFrog = nullptr;
	olc::Sprite* spriteFrog2 = nullptr;
	olc::Sprite* spriteFrog3 = nullptr;
	olc::Sprite* spriteFrog4 = nullptr;

	olc::Decal* decalFrog = nullptr;
	olc::Decal* decalFrog2 = nullptr;
	olc::Decal* decalFrog3 = nullptr;
	olc::Decal* decalFrog4 = nullptr;

	struct Tetromino {
		string name;
		wstring shape;
		olc::Pixel color;
	};

	Tetromino tetrominoes[7];

	menuobject mo;
	menumanager mm;

	menuobject mg; //pause menu object

	OneLoneCoder_FormulaOLC* race;
	OneLoneCoder_Frogger* frog;
	OneLoneCoder_Tetris* tetris;
	OneLoneCoder_Asteroids* asteroids;
	OneLoneCoder_Snake* snake;

public:
	void drawRace(float fElapsedTime) {
		// Handle control input
		int nCarDirection = 0;

		if (GetKey(olc::Key::UP).bHeld)
			race->fSpeed += 2.0f * fElapsedTime;
		else
			race->fSpeed -= 1.0f * fElapsedTime;

		// Car Curvature is accumulated left/right input, but inversely proportional to speed
		// i.e. it is harder to turn at high speed
		if (GetKey(olc::Key::LEFT).bHeld)
		{
			race->fPlayerCurvature -= 0.8f * fElapsedTime * (1.0f - race->fSpeed / 2.0f);
			nCarDirection = -1;
		}

		if (GetKey(olc::Key::RIGHT).bHeld)
		{
			race->fPlayerCurvature += 0.8f * fElapsedTime * (1.0f - race->fSpeed / 2.0f);
			nCarDirection = +1;
		}

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			gameState = false;
			mm.Open(&mg["pause"]);
			menu = 1;
		}

		race->updateRace(fElapsedTime);

		// Draw Sky - light blue and dark blue
		for (int y = 0; y < ScreenHeight() / 2; y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				Draw(x, y, y < ScreenHeight() / 4 ? olc::BLUE : olc::DARK_BLUE);
			}
		}

		// Draw Scenery - our hills are a rectified sine wave, where the phase is adjusted by the
		// accumulated track curvature
		for (int x = 0; x < ScreenWidth(); x++)
		{
			int nHillHeight = (int)(fabs(sinf(x * 0.005f + race->fTrackCurvature) * 32.0f));
			for (int y = (ScreenHeight() / 2) - nHillHeight; y < ScreenHeight() / 2; y++) {
				Draw(x, y, olc::DARK_YELLOW);
			}
		}


		// Draw Track - Each row is split into grass, clip-board and track
		for (int y = 0; y < ScreenHeight() / 2; y++) {
			for (int x = 0; x < ScreenWidth(); x++)
			{
				// Perspective is used to modify the width of the track row segments
				float fPerspective = (float)y / (ScreenHeight() / 2.0f);
				float fRoadWidth = 0.1f + fPerspective * 0.8f; // Min 10% Max 90%
				float fClipWidth = fRoadWidth * 0.15f;
				fRoadWidth *= 0.5f;	// Halve it as track is symmetrical around center of track, but offset...

				// ...depending on where the middle point is, which is defined by the current
				// track curvature.
				float fMiddlePoint = 0.5f + race->fCurvature * powf((1.0f - fPerspective), 3);

				// Work out segment boundaries
				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

				int nRow = ScreenHeight() / 2 + y;

				// Using periodic oscillatory functions to give lines, where the phase is controlled
				// by the distance around the track. These take some fine tuning to give the right "feel"
				olc::Pixel nGrassColour = sinf(20.0f * powf(1.0f - fPerspective, 3) + race->fDistance * 0.1f) > 0.0f ? olc::GREEN : olc::DARK_GREEN;
				olc::Pixel nClipColour = sinf(80.0f * powf(1.0f - fPerspective, 2) + race->fDistance) > 0.0f ? olc::RED : olc::WHITE;

				// Start finish straight changes the road colour to inform the player lap is reset
				olc::Pixel nRoadColour = (race->nTrackSection - 1) == 0 ? olc::WHITE : olc::GREY;

				// Draw the row segments
				if (x >= 0 && x < nLeftGrass) {
					Draw(x, nRow, nGrassColour);
				}
				if (x >= nLeftGrass && x < nLeftClip) {
					Draw(x, nRow, nClipColour);
				}
				if (x >= nLeftClip && x < nRightClip) {
					Draw(x, nRow, nRoadColour);
				}
				if (x >= nRightClip && x < nRightGrass) {
					Draw(x, nRow, nClipColour);
				}
				if (x >= nRightGrass && x < ScreenWidth()) {
					Draw(x, nRow, nGrassColour);
				}
			}
		}
		// Draw Car - car position on road is proportional to difference between
		// current accumulated track curvature, and current accumulated player curvature
		// i.e. if they are similar, the car will be in the middle of the track
		race->fCarPos = race->fPlayerCurvature - race->fTrackCurvature;
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * race->fCarPos) / 2.0) - 7; // Offset for sprite

		// Draw a car that represents what the player is doing. Apologies for the quality
		// of the sprite... :-(
		switch (nCarDirection)
		{
		case 0:
			/*
					DrawString(nCarPos, 80, "   ||####||   ");
					DrawString(nCarPos, 181, "      ##      ");
					DrawString(nCarPos, 182, "     ####     ");
					DrawString(nCarPos, 183, "     ####     ");
					DrawString(nCarPos, 184, "|||  ####  |||");
					DrawString(nCarPos, 185, "|||########|||");
					DrawString(nCarPos, 186, "|||  ####  |||");
					*/
			FillRect(nCarPos - 1, 181, 2 * 2, 1 * 2, olc::BLACK);
			FillRect(nCarPos + 3, 181, 4 * 2, 1 * 2, olc::RED);
			FillRect(nCarPos + 11, 181, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 5, 183, 2 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 3, 185, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 3, 187, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 3, 189, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos - 1, 191, 8 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 3, 193, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos - 7, 189, 3 * 2, 3 * 2, olc::BLACK);
			FillRect(nCarPos + 15, 189, 3 * 2, 3 * 2, olc::BLACK);

			break;

		case +1:
			/*
			DrawString(nCarPos, 80,  "      //####//");
			DrawString(nCarPos, 181, "         ##   ");
			DrawString(nCarPos, 182, "       ####   ");
			DrawString(nCarPos, 183, "      ####    ");
			DrawString(nCarPos, 184, "///  ####//// ");
			DrawString(nCarPos, 185, "//#######///0 ");
			DrawString(nCarPos, 186, "/// #### //// "); 
			*/

			FillRect(nCarPos + 5, 181, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 9, 181, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 17, 181, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 11, 183, 2 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 7, 185, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 5, 187, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos - 7, 189, 3 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 3, 189, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 11, 189, 4 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 7, 191, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 3, 191, 7 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 11, 191, 3 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 17, 191, 1 * 2, 1 * 2, olc::WHITE);

			FillRect(nCarPos - 7, 193, 3 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 1, 193, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 11, 193, 4 * 2, 1 * 2, olc::BLACK);

			break;

		case -1:
			/*
			DrawString(nCarPos, 80,  "\\####\\      ");
			DrawString(nCarPos, 181, "   ##         ");
			DrawString(nCarPos, 182, "   ####       ");
			DrawString(nCarPos, 183, "    ####      ");
			DrawString(nCarPos, 184, " \\\\####  \\\");
			DrawString(nCarPos, 185, " 0\\\#######\\");
			DrawString(nCarPos, 186, " \\\\ #### \\\");
			*/
			FillRect(nCarPos - 7, 181, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 3, 181, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 5, 181, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 1, 183, 2 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos - 1, 185, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 1, 187, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos - 5, 189, 4 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 3, 189, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 15, 189, 3 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 5, 191, 1 * 2, 1 * 2, olc::WHITE);

			FillRect(nCarPos - 3, 191, 3 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 3, 191, 7 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 17, 191, 2 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos - 5, 193, 4 * 2, 1 * 2, olc::BLACK);

			FillRect(nCarPos + 5, 193, 4 * 2, 1 * 2, olc::RED);

			FillRect(nCarPos + 15, 193, 3 * 2, 1 * 2, olc::BLACK);

			break;
		}

		// Draw Stats
		DrawString(0, 0, "Distance: " + to_string(race->fDistance));
		DrawString(0, 16, "Target Curvature: " + to_string(race->fCurvature));
		DrawString(0, 32, "Player Curvature: " + to_string(race->fPlayerCurvature));
		DrawString(0, 48, "Player Speed    : " + to_string(race->fSpeed));
		DrawString(0, 64, "Track Curvature : " + to_string(race->fTrackCurvature));
		

		auto disp_time = [](float t) // Little lambda to turn floating point seconds into minutes:seconds:millis string
		{
			int nMinutes = t / 60.0f;
			int nSeconds = t - (nMinutes * 60.0f);
			int nMilliSeconds = (t - (float)nSeconds) * 1000.0f;
			return to_string(nMinutes) + "." + to_string(nSeconds) + ":" + to_string(nMilliSeconds);
		};

		// Display current laptime
		DrawString(224, 0, to_string(race->fCurrentLapTime));

		// Display last 5 lap times
		
		int j = 16;
		for (auto l : race->listLapTimes)
		{
			DrawString(224, j, to_string(l));
			j += 16;
		}
		
	}

	void drawFrogger(float fElapsedTime) {
		frog->updateFrogger(fElapsedTime);

		if (GetKey(olc::Key::UP).bPressed || frog->upPressed == true)
		{
			frog->upPressed = true;
			if (frog->track < 0) {
				frog->track = 0;
			}

			if (frog->track < 5) {
				if (frog->track == 4) {
					if (frog->fFrogY - floor(frog->fFrogY) < 0.5) {
						frog->fFrogY = (float)(floor(frog->fFrogY));
					}
					else {
						frog->fFrogY = (float)(ceil(frog->fFrogY));
					}
				}
				else {
					frog->fFrogY -= 0.2;
				}
				frog->position = { frog->fFrogX * frog->nCellSize, frog->fFrogY * frog->nCellSize };
				DrawPartialDecal(frog->position, frog->size, decalFrog, (frog->animationData).sourcePos, (frog->animationData).sourceSize);
				frog->track++;
			}
			else {
				frog->upPressed = false;
				frog->track = -1;
			}
		}
		else if (GetKey(olc::Key::RIGHT).bPressed || frog->rightPressed == true)
		{
			frog->rightPressed = true;
			if (frog->track < 0) {
				frog->track = 0;
			}

			if (frog->track < 5) {
				if (frog->track == 4) {
					if (frog->fFrogX - ceil(frog->fFrogX) < 0.5) {
						frog->fFrogX = (float)(ceil(frog->fFrogX));
					}
					else {
						frog->fFrogX = (float)(floor(frog->fFrogX));
					}
				}
				else {
					frog->fFrogX += 0.2;
				}
				frog->position = { frog->fFrogX * frog->nCellSize, frog->fFrogY * frog->nCellSize };
				DrawPartialDecal(frog->position, frog->size, decalFrog2, (frog->animationData).sourcePos, (frog->animationData).sourceSize);
				frog->track++;
			}
			else {
				frog->rightPressed = false;
				frog->track = -2;
			}
		}
		else if (GetKey(olc::Key::LEFT).bPressed || frog->leftPressed == true)
		{
			frog->leftPressed = true;
			if (frog->track < 0) {
				frog->track = 0;
			}

			if (frog->track < 5) {
				if (frog->track == 4) {
					if (frog->fFrogX - floor(frog->fFrogX) < 0.5) {
						frog->fFrogX = (float)(floor(frog->fFrogX));
					}
					else {
						frog->fFrogX = (float)(ceil(frog->fFrogX));
					}
				}
				else {
					frog->fFrogX -= 0.2;
				}
				frog->position = { frog->fFrogX * frog->nCellSize, frog->fFrogY * frog->nCellSize };
				DrawPartialDecal(frog->position, frog->size, decalFrog3, (frog->animationData).sourcePos, (frog->animationData).sourceSize);
				frog->track++;
			}
			else {
				frog->leftPressed = false;
				frog->track = -3;
			}
		}
		else if (GetKey(olc::Key::DOWN).bPressed || frog->backPressed == true)
		{
			frog->backPressed = true;
			if (frog->track < 0) {
				frog->track = 0;
			}

			if (frog->track < 5) {
				if (frog->track == 4) {
					if (frog->fFrogY - ceil(frog->fFrogY) < 0.5) {
						frog->fFrogY = (float)(ceil(frog->fFrogY));
					}
					else {
						frog->fFrogY = (float)(floor(frog->fFrogY));
					}
				}
				else {
					frog->fFrogY += 0.2;
				}
				frog->position = { frog->fFrogX * frog->nCellSize, frog->fFrogY * frog->nCellSize };
				DrawPartialDecal(frog->position, frog->size, decalFrog4, (frog->animationData).sourcePos, (frog->animationData).sourceSize);
				frog->track++;
			}
			else {
				frog->backPressed = false;
				frog->track = -4;
			}
		}

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			gameState = false;
			mm.Open(&mg["pause"]);
			menu = 1;
			//DrawPartialSprite(frog->position, spriteFrog, { 0,0 }, { 16,16 });
			//DrawPartialDecal(frog->position, frog->size, decalFrog, { 0,0 }, { 16,16 });
		}

		// Draw Lanes
		int x = -1, y = 0;
		for (auto lane : frog->vecLanes)
		{
			// Find lane offset start
			int nStartPos = (int)(frog->fTimeSinceStart * lane.first) % 64; //128
			int nCellOffset = (int)((float)frog->nCellSize * frog->fTimeSinceStart * lane.first) % frog->nCellSize;
			if (nStartPos < 0)	nStartPos = 64 - (abs(nStartPos) % 64);

			for (int i = 0; i < frog->nLaneWidth; i++)
			{
				// Draw Graphics
				char graphic = lane.second[(nStartPos + i) % 64];
				//Fill((x + i)*frog->nCellSize - nCellOffset, y*frog->nCellSize, (x + i + 1)*frog->nCellSize - nCellOffset, (y + 1)*frog->nCellSize, graphic);



				switch (graphic) // Graphics always make code verbose and less clear
				{
				case 'a':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteTruck, 0, 0, 24, 24);	break; // Bus 1
				case 's':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteTruck, 24, 0, 24, 24);	break; // Bus 2
				case 'd':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteTruck, 48, 0, 24, 24);	break; // Bus 3
				case 'f':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteTruck, 72, 0, 24, 24);	break; // Bus 4

				case 'j':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLUE); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteLog, 0, 0, 24, 24);		break; // Log Start
				case 'l':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLUE); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteLog, 24, 0, 24, 24);		break; // Log Middle
				case 'k':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLUE); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteLog, 48, 0, 24, 24);	break; // Log End

				case 'z': 	DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteCar1, 0, 0, 24, 24);	break; // Car1 Back
				case 'x':	DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteCar1, 24, 0, 24, 24);	break; // Car1 Front

				case 't': 	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteCar2, 0, 0, 24, 24);	break; // Car2 Back
				case 'y': 	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLACK); DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteCar2, 24, 0, 24, 24);	break; // Car2 Front

				case 'w': 	DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spriteWall, 0, 0, 24, 24);	break; // Wall
				case 'h':	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLUE); DrawPartialSprite(frog->winPos1.x, frog->winPos1.y, spriteHome, 0, 0, 24, 24); DrawPartialSprite(frog->winPos2.x, frog->winPos2.y, spriteHome, 0, 0, 24, 24); DrawPartialSprite(frog->winPos3.x, frog->winPos3.y, spriteHome, 0, 0, 24, 24);	break; // Home
				case ',': 	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, 24, 24, olc::BLUE);	break; // Water
				case 'p': 	DrawPartialSprite((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, spritePavement, 0, 0, 24, 24); break; // Pavement
				case '.': 	FillRect((x + i) * frog->nCellSize - nCellOffset, y * frog->nCellSize, (x + i + 1) * frog->nCellSize - nCellOffset, (y + 1) * frog->nCellSize, olc::BLACK);	break; // Road
				}
				//FillRect(nCarPos + 5, 183, 2 * 2, 1 * 2, olc::RED);

				// Fill Danger buffer
				for (int j = (x + i) * frog->nCellSize - nCellOffset; j < (x + i + 1) * frog->nCellSize - nCellOffset; j++)
					for (int k = y * frog->nCellSize; k < (y + 1) * frog->nCellSize; k++)
						if (j >= 0 && j < ScreenWidth() && k >= 0 && k < ScreenHeight())
							frog->bufDanger[k * ScreenWidth() + j] =
							!(graphic == '.' ||
								graphic == 'j' || graphic == 'k' || graphic == 'l' ||
								graphic == 'p' ||
								graphic == 'h');
			}
			y++;
		}

		switch (frog->track) {
		case 0:
			DrawPartialDecal(frog->position, frog->size, decalFrog, { 0,0 }, { 16,16 });
			break;
		case -1:
			DrawPartialDecal(frog->position, frog->size, decalFrog, { 0,0 }, { 16,16 });
			break;
		case -2:
			DrawPartialDecal(frog->position, frog->size, decalFrog2, { 0,0 }, { 16,16 });
			break;
		case -3:
			DrawPartialDecal(frog->position, frog->size, decalFrog3, { 0,0 }, { 16,16 });
			break;
		case -4:
			DrawPartialDecal(frog->position, frog->size, decalFrog4, { 0,0 }, { 16,16 });
			break;
		}
	}

	void drawTetris(float fElapsedTime) {
		Clear(olc::BLACK);

		tetris->fcurrentTicks += fElapsedTime;

		if (tetris->fcurrentTicks >= tetris->tickRate) {
			// This is an update
			//FixedUpdate();
			tetris->fcurrentTicks = 0.0f;
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		tetris->iSpeedCounter++;
		tetris->bForceDown = (tetris->iSpeedCounter == tetris->iSpeed);

		tetris->iCurrentX -= (GetKey(olc::LEFT).bPressed &&
			DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation, tetris->iCurrentX - 1, tetris->iCurrentY)) ? 1 : 0;

		tetris->iCurrentX += (GetKey(olc::RIGHT).bPressed &&
			DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation, tetris->iCurrentX + 1, tetris->iCurrentY)) ? 1 : 0;

		tetris->iCurrentY += (GetKey(olc::DOWN).bHeld &&
			DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation, tetris->iCurrentX, tetris->iCurrentY + 1)) ? 1 : 0;

		tetris->iCurrentRotation += (GetKey(olc::Z).bPressed &&
			DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation + 1, tetris->iCurrentX, tetris->iCurrentY)) ? 1 : 0;

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			gameState = false;
			mm.Open(&mg["pause"]);
			menu = 1;
		}

		if (tetris->bForceDown) {
			tetris->iSpeedCounter = 0;
			UpdatePositionsAndCheckForCollisios();
		}

		for (int x = 0; x < tetris->iFieldWidth; x++) {
			for (int y = 0; y < tetris->iFieldHeight; y++) {

				if (tetris->pField[y * tetris->iFieldWidth + x] != 0 && tetris->pField[y * tetris->iFieldWidth + x] != 11 &&
					tetris->pField[y * tetris->iFieldWidth + x] != 12) {
					FillRect((x + tetris->iDrawOffsetX) * tetris->iSize, (y + tetris->iDrawOffsetY) * tetris->iSize, tetris->iSize, tetris->iSize,
						tetrominoes[tetris->pField[y * tetris->iFieldWidth + x] -
						1].color);       // Dont forget, we add +1 when we "lock" a tetrominoe in place
				}
				else {
					switch (tetris->pField[y * tetris->iFieldWidth + x]) {
					case 0:
						break;
					case 11:
						FillRect((x + tetris->iDrawOffsetX) * tetris->iSize, (y + tetris->iDrawOffsetY) * tetris->iSize, tetris->iSize, tetris->iSize,
							olc::DARK_RED);
						break;
					case 12:
						FillRect((x + tetris->iDrawOffsetX) * tetris->iSize, (y + tetris->iDrawOffsetY) * tetris->iSize, tetris->iSize, tetris->iSize,
							olc::GREEN);
						break;
					default:
						FillRect((x + tetris->iDrawOffsetX) * tetris->iSize, (y + tetris->iDrawOffsetY) * tetris->iSize, tetris->iSize, tetris->iSize,
							olc::Pixel(215, 83, 162));
						break;
					}
				}

				if (tetris->bIsDebug) {
					DrawString((x + tetris->iDrawOffsetX) * tetris->iSize, (y + tetris->iDrawOffsetY) * tetris->iSize,
						std::to_string(tetris->pField[y * tetris->iFieldWidth +
							x]));// Dont forget, we added +1 when we "lock" a tetrominoe in place
				}

			}
		}

		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				if (tetrominoes[tetris->iCurrentPeice].shape[tetris->Rotate(px, py, tetris->iCurrentRotation)] != L'.') {
					// yeah, this is a piece
					FillRect((tetris->iCurrentX + tetris->iDrawOffsetX + px) * tetris->iSize, (tetris->iCurrentY + tetris->iDrawOffsetY + py) * tetris->iSize, tetris->iSize,
						tetris->iSize,
						tetrominoes[tetris->iCurrentPeice].color);
					if (tetris->bIsDebug)
						DrawString((tetris->iCurrentX + tetris->iDrawOffsetX + px) * tetris->iSize, (tetris->iCurrentY + tetris->iDrawOffsetY + py) * tetris->iSize,
							std::to_string(tetris->iCurrentPeice));
				}
			}
		}

		if (!tetris->vLines.empty()) {
			switch (tetris->vLines.size()) {
			case 1:
				tetris->iScore += 10 * tetris->iLevel;
				break;
			case 2:
				tetris->iScore += 30 * tetris->iLevel;
				break;
			case 3:
				tetris->iScore += 60 * tetris->iLevel;
				break;
			case 4:
				tetris->iScore += 100 * tetris->iLevel;
				break;
			}

			//TODO: Fix this
			std::this_thread::sleep_for(std::chrono::milliseconds(250));

			for (auto& v : tetris->vLines) {
				tetris->iLinesCleared++;
				if (tetris->iLinesCleared % 10 == 0) {
					if (tetris->iLevel + 1 <= 90) {
						tetris->iLevel++;
					}
				}
				for (int px = 1; px < tetris->iFieldWidth - 1; px++) {
					for (int py = v; py > 0; py--) {
						tetris->pField[py * tetris->iFieldWidth + px] = tetris->pField[(py - 1) * tetris->iFieldWidth + px];
						tetris->pField[px] = 0;
					}
				}
			}

			tetris->vLines.clear();
		}

		DrawUI();

		if (tetris->isGameOver) {
			delete tetris;
			tetris = NULL;
		}
	}

	void DrawPreview(int iposX, int iposY) {
		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				if (tetrominoes[tetris->iNextPeice].shape[tetris->Rotate(px, py, 0)] != L'.') {
					FillRect((iposX + tetris->iDrawOffsetX + px) * tetris->iSize, (iposY + 2 + py) * tetris->iSize, tetris->iSize, tetris->iSize,
						tetrominoes[tetris->iNextPeice].color);
					if (tetris->bIsDebug)
						DrawString((iposX + tetris->iDrawOffsetX + px) * tetris->iSize, (iposY + 2 + py) * tetris->iSize,
							std::to_string(tetris->iNextPeice));
				}
			}
		}
	}

	void DrawUI() {
		DrawString(tetris->iFieldWidth + 230 + tetris->iDrawOffsetX, tetris->iSize + tetris->iSize, "NEXT TETROMINO");
		DrawPreview(tetris->iFieldWidth + tetris->iDrawOffsetX, 2);

		DrawString(tetris->iFieldWidth + 230 + tetris->iDrawOffsetX, tetris->iSize + tetris->iSize * 3 + 88,
			"Score: " + std::to_string(tetris->iScore));
		DrawString(tetris->iFieldWidth + 230 + tetris->iDrawOffsetX, tetris->iSize + tetris->iSize * 3 + 100,
			"Cleared Lines: " + std::to_string(tetris->iLinesCleared));
		DrawString(tetris->iFieldWidth + 230 + tetris->iDrawOffsetX, tetris->iSize + tetris->iSize * 3 + 112,
			"Speed: " + std::to_string(tetris->iSpeed));
		DrawString(tetris->iFieldWidth + 230 + tetris->iDrawOffsetX, tetris->iSize + tetris->iSize * 3 + 124,
			"Level: " + std::to_string(tetris->iLevel));
	}

	void UpdatePositionsAndCheckForCollisios() {
		if (DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation, tetris->iCurrentX, tetris->iCurrentY + 1)) {
			tetris->iCurrentY++;
		}
		else {
			// Lock the current piece in place
			for (int px = 0; px < 4; px++) {
				for (int py = 0; py < 4; py++) {
					if (tetrominoes[tetris->iCurrentPeice].shape[tetris->Rotate(px, py, tetris->iCurrentRotation)] != L'.') {
						tetris->pField[(tetris->iCurrentY + py) * tetris->iFieldWidth + (tetris->iCurrentX + px)] = tetris->iCurrentPeice + 1;
					}
				}
			}

			// Check if we careted any horizontal lines
			for (int py = 0; py < 4; py++) {
				if (tetris->iCurrentY + py < tetris->iFieldHeight - 1) {
					bool line = true;       // We assume there is going to be a line
					// Some small optimization: only check last 4 lines where tetrmomine was fixed at

					for (int px = 1; px < tetris->iFieldWidth - 1; px++) {
						line &= (tetris->pField[(tetris->iCurrentY + py) * tetris->iFieldWidth + px]) != 0;
					}

					if (line) {
						for (int px = 1; px < tetris->iFieldWidth - 1; px++) {
							tetris->pField[(tetris->iCurrentY + py) * tetris->iFieldWidth + px] = 12;
						}
						tetris->vLines.push_back(tetris->iCurrentY + py);
					}
				}
			}

			// Generate a new peice
			tetris->SpawnRandomPeice();

			// If new piece doent fit, Game Over
			tetris->isGameOver = !DoesPieceFit(tetris->iCurrentPeice, tetris->iCurrentRotation, tetris->iCurrentX, tetris->iCurrentY);
		}
	}

	bool DoesPieceFit(int iTetrmonino, int iRotation, int iposX, int iposY) {

		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				int pi = tetris->Rotate(px, py, iRotation);

				int fieldIndex = (iposY + py) * tetris->iFieldWidth + (iposX + px);

				// make sure we are inside the field
				if (iposX + px >= 0 && iposX + px < tetris->iFieldWidth) {
					if (iposY + py >= 0 && iposY + py < tetris->iFieldHeight) {
						// we are inside the playing field

						// We are hitting something
						if (tetrominoes[iTetrmonino].shape[pi] != L'.' && tetris->pField[fieldIndex] != 0) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	struct Palette {
		olc::Pixel background, ship, asteroid, projectile;
	};

	Palette palettes[1] = {
		{ olc::BLACK, olc::WHITE, olc::WHITE, olc::WHITE } // contrast
	};

	Palette* palette;

	// Generate a pseudo-random number using a permutation of Lehmer's Algorithm
	uint32_t pseudo_random(uint64_t seed) {
		return (seed * 0xda942042e4dd58b5) >> 32;
	}

	void WrapPosition(olc::vf2d& v) {
		if (v.y > ScreenHeight())
			v.y -= ScreenHeight();
		else if (v.y < 0)
			v.y += ScreenHeight();

		if (v.x > ScreenWidth())
			v.x -= ScreenWidth();
		else if (v.x < 0)
			v.x += ScreenWidth();
	}

	void ProcessInputs() {
		/// Rotation is done with A and D keys, can be 0, -1 and 1
		asteroids->ship->transform.rotation += (GetKey(olc::RIGHT).bHeld - GetKey(olc::LEFT).bHeld) * asteroids->deltaTime * asteroids->ship->stats.rotationSpeed;

		// Forward direction is up vector (cuz the ship is facing up by default) rotated by ship's rotation
		olc::vf2d forward = { 0, 1 };
		RotateVector(forward, olc::vf2d(0, 0), asteroids->ship->transform.rotation);
		
		asteroids->ship->transform.velocity += forward * asteroids->ship->stats.movementSpeed * (GetKey(olc::DOWN).bHeld - GetKey(olc::UP).bHeld) * asteroids->deltaTime;

		if (GetKey(olc::Key::SPACE).bPressed) {
			if (asteroids->gameStart) {
				SummonProjectile(asteroids->ship->transform.position, -forward * asteroids->ship->stats.projectileSpeed);
			}
		}

		// Accounts for drag now
		asteroids->ship->transform.position += asteroids->ship->transform.velocity * asteroids->deltaTime;

		// Wrap the ship, doesn't apply to anything else
		WrapPosition(asteroids->ship->transform.position);
	}

	void ProcessRocks() {
		for (int i = 0; i < asteroids->rocks.size(); ++i) {
			Rock& rock = asteroids->rocks[i];
			if (rock.size == Rock::Size::NONE) continue;
			rock.transform.position += rock.transform.velocity * asteroids->deltaTime;
			WrapPosition(rock.transform.position);
			rock.transform.rotation += rock.transform.velocity.mag() / rock.transform.radius * asteroids->deltaTime;
		}
	}

	void ProcessProjectiles() {
		for (int i = 0; i < PROJECTILE_POOL_SIZE; ++i) {
			Projectile& self = asteroids->projectiles[i];
			self.transform.position += self.transform.velocity * asteroids->deltaTime;
		}
	}

	void ProcessCollisions() {
		// Doesn't handle ship collisions yet
		for (int i = 0; i < asteroids->rocks.size(); i++) {
			Rock& rock = asteroids->rocks[i];
			for (int j = 0; j < PROJECTILE_POOL_SIZE; j++) {
				Projectile& projectile = asteroids->projectiles[j];

				if (projectile.transform && rock.transform && (bool)(rock.size) && projectile.transform.radius != 0) {
					//   custom operator ^                 ^  ordinary boolean  ^

					// Kill both rock and projectile
					DestroyAsteroid(i);
					projectile.transform.radius = 0;
					asteroids->score++;
				}
			}
		}
	}

	void DrawShip() {
		/* Ship vertices layout

			   ^ direction
			   |
			   a
			  /|\
			 / | \
			/  |  \
		   /__/-\__\
		  //   d   \\
		  ^         ^
		  b         c

		*/

		olc::vf2d a, b, c, d, direction = { 0, 1 };

		olc::vf2d center = asteroids->ship->transform.position;
		Transform& transform = asteroids->ship->transform;
		olc::vf2d& dimension = asteroids->ship->dimensions;

		// It is a bit unordinary, but it has it's own proportions
		a = { center.x, center.y - (float)asteroids->ship->dimensions.y / 2 };
		d = { center.x, center.y + (float)asteroids->ship->dimensions.y / 3 };
		b = { center.x - (float)asteroids->ship->dimensions.x / 2, center.y + (float)asteroids->ship->dimensions.y / 2 };
		c = { center.x + (float)asteroids->ship->dimensions.x / 2, center.y + (float)asteroids->ship->dimensions.y / 2 };

		// Rotate all the triangle points
		olc::vf2d* points[4] = { &a, &b, &c, &d };
		for (int i = 0; i < 4; i++)
			RotateVector(*points[i], center, transform.rotation);

		RotateVector(direction, olc::vf2d{ 0, 0 }, transform.rotation /* rad */);

		// No need for spaghetti ifs, draw all of them
		// Too many draw calls is the exact amount of draw calls needed
		// KISS

		/* The resulting grid of fake ships
			x - real
			o - fake

			o -- o -- o
			:    :    :
			o -- x -- o
			:    :    :
			o -- o -- o
		*/

		olc::vi2d offsets[9] = {
			olc::vi2d { 0, 0 },
			olc::vi2d { 0, ScreenHeight() },
			olc::vi2d { ScreenWidth(), 0 },
			olc::vi2d { 0, -ScreenHeight() },
			olc::vi2d { -ScreenWidth(), 0 },
			olc::vi2d { ScreenWidth(), ScreenHeight() },
			olc::vi2d { ScreenWidth(), -ScreenHeight() },
			olc::vi2d { -ScreenWidth(), ScreenHeight() },
			olc::vi2d { -ScreenWidth(), -ScreenHeight() },
		};
		// Might as well unroll this
	//#pragma unroll (9)
		for (int i = 0; i < 9; i++) {
			olc::vi2d offset = offsets[i];

			DrawLine(a + offset, b + offset, palette->ship);
			DrawLine(b + offset, d + offset, palette->ship);
			DrawLine(d + offset, c + offset, palette->ship);
			DrawLine(c + offset, a + offset, palette->ship);
		}
		//ShipCollide(center);
	}

	void DrawAsteroids() {
		// Drawing is done in steps and every iteration we rotate the current vector by this 
		// to achieve rotation

		for (int i = 0; i < asteroids->rocks.size(); ++i) {
			Rock& rock = asteroids->rocks[i];
			if (rock.size == Rock::Size::NONE) continue;

			float step = 6.28319 / BIG_ROCK_STEPS; // rad 
			// The loop starts with previous so it is our initial position
			olc::vf2d start = { 0, (float)
				// using the same formula as per-vertex
				// but times BIG_ROCK_STEPS instead of ii to ensure
				// the last vertex locking on the first
				(pseudo_random(((uint64_t)(&rock)) * BIG_ROCK_STEPS)
					% ((uint64_t)rock.transform.radius / 3)
					+ rock.transform.radius * ROCK_LEAST_RADIUS_COEFFICIENT) };

			olc::vf2d previous = start;
			RotateVector(previous, { 0, 0 }, rock.transform.rotation);
			olc::vf2d current;

			// Move thru all the vertices one by one connecting them
			for (int ii = 1; ii <= BIG_ROCK_STEPS; ii++) {
				/// A vector based on the current position
				current = { 0, (float)
					// when using a pointer random will be specific for each rock
					// and by using ii we ensure that it is also vertex specific
					// maximum drawable radius is larger than it's collision radius
					// to provide better player experience
					(pseudo_random(((uint64_t)(&rock)) * ii)
						% ((uint64_t)rock.transform.radius / 3)
						+ rock.transform.radius * ROCK_LEAST_RADIUS_COEFFICIENT) };

				RotateVector(current, { 0, 0 }, step * ii + rock.transform.rotation);
				DrawLine(
					current + rock.transform.position,
					previous + rock.transform.position,
					olc::YELLOW
				);
				previous = current;
			}
		}
	}

	void DrawProjectiles() {
		for (int i = 0; i < PROJECTILE_POOL_SIZE; ++i) {
			Projectile& self = asteroids->projectiles[i];
			if (self.transform.radius != 0)
				DrawCircle(self.transform.position, self.transform.radius, palette->projectile);
		}
	}

	void RotateVector(olc::vf2d& target, olc::vf2d around, float angle) {
		float cosangle = cos(angle);
		float sinangle = sin(angle);

		// moving point to the origin
		target -= around;

		olc::vf2d rotated = { target.x * cosangle - target.y * sinangle, target.x * sinangle + target.y * cosangle };

		// shift back to origin
		rotated += around;

		target = rotated;
	}

	void ShipCollide() {
		// Debug direction and collision radius

		bool isColliding = false;

		for (int ii = 0; ii < asteroids->rocks.size(); ++ii)
			isColliding |= asteroids->rocks[ii].transform && asteroids->ship->transform && (bool)(asteroids->rocks[ii].size);
		//                                             ^ custom operator ^ ordinary boolean AND

		//DrawCircle(center, asteroids->ship->transform.radius, isColliding ? olc::RED : olc::GREEN);

		if (isColliding) {
			delete asteroids;
			asteroids = NULL;
		}
	}

	void SummonProjectile(olc::vf2d position, olc::vf2d velocity) {
		Projectile* bullet = &asteroids->projectiles[(++asteroids->projectileStackCounter) % PROJECTILE_POOL_SIZE];

		bullet->transform.position = position;
		bullet->transform.radius = PROJECTILE_RADIUS;
		bullet->transform.velocity = velocity;
	}

	void DestroyAsteroid(size_t i) {
		Rock& rock = asteroids->rocks[i];
		rock.size = (Rock::Size)((char)(rock.size) - 1);

		if ((bool)rock.size) {
			RotateVector(rock.transform.velocity, { 0, 0 }, fmod((float)pseudo_random((uint64_t)&rock), 2 * ROCK_TILT_ANGLE) + ROCK_TILT_ANGLE);
			rock.transform.radius /= 2;
			rock.transform.velocity *= fmod((float)pseudo_random((uint64_t)&rock), ROCK_SPEED_COEFFICIENT / 2) + ROCK_SPEED_COEFFICIENT / 2;
			rock = Rock(rock);
			asteroids->rocks.push_back(rock);
			RotateVector(rock.transform.velocity, { 0, 0 }, (-fmod((float)pseudo_random((uint64_t)&rock - (uint64_t)&rock.transform), 2 * ROCK_TILT_ANGLE)) + ROCK_TILT_ANGLE);
		}
		asteroids->rock_counter++;
	}

	void createAsteroid() {
		palette = &palettes[0];

		asteroids->rocks.clear();
		for (int i = 0; i < (asteroids->rock_counter / 7) + 1; i++) {
			asteroids->rocks.push_back(Rock({ {(float)(rand() % ScreenWidth()) , (float)(rand() % 101)},  {0, 20}, BIG_ROCK_RADIUS }, Rock::Size::BIG));
		}

		//ship.transform.position = asteroids->ScreenCenter(2);
		asteroids->ship->dimensions = { 7, 10 };

		// Choose the highest of two for the collision radius
		asteroids->ship->transform.radius = asteroids->ship->dimensions.x < asteroids->ship->dimensions.y ? asteroids->ship->dimensions.x : asteroids->ship->dimensions.y;

		asteroids->ship->stats.rotationSpeed = 5;
		asteroids->ship->stats.movementSpeed = 100;
		asteroids->ship->stats.projectileSpeed = 300;
	}

	void updateAsteroid(float deltaTime) {
		asteroids->deltaTime = deltaTime;

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			gameState = false;
			asteroids->gameStart = false;
			mm.Open(&mg["pause"]);
			menu = 1;
		}

		Clear(palette->background);
		ProcessInputs();
		ProcessProjectiles();
		ProcessRocks();
		ProcessCollisions();
		DrawShip();
		DrawAsteroids();
		DrawProjectiles();

		if (menu != 1) {
			asteroids->gameStart = true;
		}

		DrawString(0, 0, "Score: " + to_string(asteroids->score));

		bool end = true;

		for (int i = 0; i < asteroids->rocks.size(); i++) {
			Rock& rock = asteroids->rocks[i];

			if (rock.size != Rock::Size::NONE) {
				end = false;
			}
		}

		if (end) {
			createAsteroid();
		}

		ShipCollide();
	}

	void ResetLevel()
	{
		for (int y = 0; y < 15; y++)
		{
			for (int x = 0; x < 24; x++)
			{
				if (x == 0 || y == 0 || x == 23 || y == 14)
				{
					snake->GridArray[y * 24 + x] = 5;
				}
				else
				{
					snake->GridArray[y * 24 + x] = 0;
				}
			}
		}

		if (snake == nullptr)
		{
			snake = new OneLoneCoder_Snake();
		}
		else
		{
			snake->ResetSnake();
		}
		snake->CurrentScore = 0;
		snake->cFoodColour = olc::YELLOW;
		snake->vFoodPos = { 18,8 };

	}

	void DrawLevel()
	{
		for (int y = 0; y < 15; y++)
		{
			for (int x = 0; x < 24; x++)
			{
				olc::vi2d currentGridCoord = { x,y };
				if (snake->DetectContact(currentGridCoord * snake->vBlockSize, snake->GetSnakeHead().vCurrentPos * snake->vBlockSize, snake->vBlockSize))
				{
					snake->GridArray[y * 24 + x] = 10;
				}
				if (snake->DetectContact(currentGridCoord * snake->vBlockSize, snake->GetCurrentSnakeTail().vCurrentPos * snake->vBlockSize, snake->vBlockSize))
				{
					snake->GridArray[y * 24 + x] = 0;
				}

				if (snake->GridArray[y * 24 + x] == 0)
				{
					DrawSprite(olc::vi2d(x, y) * snake->vBlockSize, snake->sprTile);
				}
			}
		}
	}

	void DetectWallHits()
	{
		if (snake->GetSnakeHead().vCurrentPos.y > 14)
			snake->GetSnakeHead().vCurrentPos.y -= 14;
		else if (snake->GetSnakeHead().vCurrentPos.y < 0)
			snake->GetSnakeHead().vCurrentPos.y += 14;

		if (snake->GetSnakeHead().vCurrentPos.x > 23)
			snake->GetSnakeHead().vCurrentPos.x -= 23;
		else if (snake->GetSnakeHead().vCurrentPos.x < 0)
			snake->GetSnakeHead().vCurrentPos.x += 23;
	}

	void DrawStuff()
	{
		std::string ScoreUI = "Score: " + std::to_string(snake->CurrentScore);
		std::string SegmentNum = "Number of Segments: " + std::to_string(snake->GetCurrentSnakeSize());

		DrawString(0, 0, ScoreUI);
		DrawString(0, 8, SegmentNum);
	}

	void GameLoopController(const float& fElapsedTime)
	{
		if (!snake->isGameover)
		{
			if (GetKey(olc::RIGHT).bPressed)
			{
				snake->ChangeDirection(snake->GetRightDir());
			}
			if (GetKey(olc::LEFT).bPressed)
			{
				snake->ChangeDirection(snake->GetLeftDir());
			}

			if (snake->currentTime <= 0.2f)
			{
				snake->currentTime += fElapsedTime;
			}
			else
			{
				snake->currentTime = 0.0f;
				snake->MoveSnakeSegments();
				if (snake->DetectContact(snake->GetSnakeHead().vCurrentPos * snake->vBlockSize, snake->vFoodPos * snake->vBlockSize, snake->vBlockSize))
				{
					AddToScore();
					snake->AddSnakeSegment(snake->vBlockSize);
					RepositionFood();
				}
				snake->DetectSelfContact(snake->isGameover, snake->vBlockSize);
			}
		}
	}

	void AddToScore()
	{
		snake->CurrentScore += 1;
	}

	void RepositionFood()
	{
		olc::vi2d vNewRandomPos = { rand() % 24, rand() % 15 };

		while (snake->GridArray[vNewRandomPos.y * 24 + vNewRandomPos.x] == 10 || snake->GridArray[vNewRandomPos.y * 24 + vNewRandomPos.x] == 5)
		{
			vNewRandomPos = { rand() % 25, rand() % 15 };
		}

		snake->vFoodPos = vNewRandomPos;
	}
	void DrawFood()
	{
		FillRect(snake->vFoodPos * snake->vBlockSize, snake->vBlockSize, snake->cFoodColour);
	}


	void createSnake()
	{
		// Called once at the start, so create things here
		snake->GridArray = new int[24 * 15];
		ResetLevel();
		snake->sprTile = new olc::Sprite("tut_tile.png");
	}

	void updateSnake(float fElapsedTime)
	{
		// called once per frame
		Clear(olc::BLACK);
		DrawLevel();

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			gameState = false;
			mm.Open(&mg["pause"]);
			menu = 1;
		}

		DetectWallHits();

		GameLoopController(fElapsedTime);

		DrawFood();
		DrawStuff();
		snake->DrawSnakeSegments(this, snake->vBlockSize);
		if (snake->isGameover)
		{
			ResetLevel();
			snake->isGameover = false;
		}
	}

	bool OnUserCreate() override
	{
		sprGFX = new olc::Sprite("./assets/main/RetroMenu.png");
		games = new olc::Sprite("./assets/main/game_frames_sheet.png");

		spriteTruck = new olc::Sprite("./assets/frogger/truck.png");
		spriteLog = new olc::Sprite("./assets/frogger/log.png");
		spritePavement = new olc::Sprite("./assets/frogger/pavement.png");;
		spriteWall = new olc::Sprite("./assets/frogger/grass.png");
		spriteCar1 = new olc::Sprite("./assets/frogger/car1.png");;
		spriteCar2 = new olc::Sprite("./assets/frogger/car2.png");
		spriteHome = new olc::Sprite("./assets/frogger/win.png");

		spriteFrog = new olc::Sprite("./assets/frogger/frog.png");
		spriteFrog2 = new olc::Sprite("./assets/frogger/frog_right.png");
		spriteFrog3 = new olc::Sprite("./assets/frogger/frog_left.png");
		spriteFrog4 = new olc::Sprite("./assets/frogger/frog_back.png");
		decalFrog = new olc::Decal(spriteFrog);
		decalFrog2 = new olc::Decal(spriteFrog2);
		decalFrog3 = new olc::Decal(spriteFrog3);
		decalFrog4 = new olc::Decal(spriteFrog4);

		tetrominoes[0].name = "Straight";
		tetrominoes[0].color = olc::DARK_CYAN;
		tetrominoes[0].shape = L"..X...X...X...X.";

		tetrominoes[1].name = "Square";
		tetrominoes[1].color = olc::DARK_YELLOW;
		tetrominoes[1].shape = L".....XX..XX.....";

		tetrominoes[2].name = "T";
		tetrominoes[2].color = olc::DARK_MAGENTA;
		tetrominoes[2].shape = L"..X..XX...X.....";

		tetrominoes[3].name = "L";
		tetrominoes[3].color = olc::Pixel(252, 129, 1);
		tetrominoes[3].shape = L".....X...X...XX.";

		tetrominoes[4].name = "Reverse L";
		tetrominoes[4].color = olc::Pixel(5, 18, 255);
		tetrominoes[4].shape = L"......X...X..XX.";

		tetrominoes[5].name = "Skew";
		tetrominoes[5].color = olc::Pixel(51, 253, 1);
		tetrominoes[5].shape = L"..X..XX..X......";

		tetrominoes[6].name = "Reverse Skew";
		tetrominoes[6].color = olc::Pixel(251, 25, 0);
		tetrominoes[6].shape = L".X...XX...X.....";

		mo["main"].SetTable(3, 2);
		mo["main"]["Racing"].SetID(101);
		mo["main"]["Frogger"].SetID(102);
		mo["main"]["Tetris"].SetID(103);
		mo["main"]["Asteroids"].SetID(104);
		mo["main"]["Snake"].SetID(105);

		mg["pause"].SetTable(1, 3);
		mg["pause"]["Resume"];
		mg["pause"]["Restart"];
		mg["pause"]["Quit"];

		mo.Build();

		mg.Build2();

		mm.Open(&mo["main"]);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		title = "Retro Arcade";
		menuobject* command = nullptr;
		olc::vi2d dims;
		
		if (menu == 0) {
			dims = { 0, 50 };
		}
		else {
			dims = { (ScreenWidth() / 2) - 45, (ScreenHeight() / 2) - 20 };
		}

		if (GetKey(olc::Key::UP).bPressed) {
			if (menu == 1) {
				mm.OnUp();
			}
		}
		if (GetKey(olc::Key::DOWN).bPressed) {
			if (menu == 1) {
				mm.OnDown();
			}
		}
		if (GetKey(olc::Key::LEFT).bPressed) {
			if (menu == 0) {
				mm.OnLeft();
			}
		}
		if (GetKey(olc::Key::RIGHT).bPressed) {
			if (menu == 0) {
				mm.OnRight();
			}
		}
		if (GetKey(olc::Key::SPACE).bPressed) command = mm.OnConfirm();

		if (command != nullptr)
		{
			if (menu == 0) { //reset cursor position for all after menu seleciton (?)
				sLastAction = command->GetName();
				gameState = true;
			}
			else { 
				if (command->GetName() == "Resume") {
					gameState = true;
					menu = -1;
				}
				else if (command->GetName() == "Restart") {
					//set all values back to original in game on restart
					if (sLastAction == "Racing") {
						delete race;
						race = NULL;
					}
					else if (sLastAction == "Frogger") {
						delete frog;
						frog = NULL;
					}
					else if (sLastAction == "Tetris") {
						delete tetris;
						tetris = NULL;
					}
					else if (sLastAction == "Asteroids") {
						delete asteroids;
						asteroids = NULL;
					}
					else if (sLastAction == "Snake") {
						delete snake;
						snake = NULL;
					}
					gameState = true;
					menu = -1;
				}
				else if (command->GetName() == "Quit") {
					if (sLastAction == "Racing") {
						delete race;
						race = NULL;
					}
					else if (sLastAction == "Frogger") {
						delete frog;
						frog = NULL;
					}
					else if (sLastAction == "Tetris") {
						delete tetris;
						tetris = NULL;
					}
					else if (sLastAction == "Asteroids") {
						delete asteroids;
						asteroids = NULL;
					}
					else if (sLastAction == "Snake") {
						delete snake;
						snake = NULL;
					}
					gameState = false;
					mm.Open(&mo["main"]);
					menu = 0;
				}
				else {
					gameState = true;
				}
			}
		}

		if (menu == 0) {
			Clear(olc::BLACK);

			DrawString((ScreenWidth() / 2) - 95, 20, title, olc::WHITE, 2);
	
			mm.Draw(*this, sprGFX, games, dims, menu);
		}
		else {
			mm.Draw(*this, sprGFX, games, dims, menu);
		}

		if (gameState) {
			mm.Close();
			if (sLastAction == "Racing") {
				if (!race) {
					race = new OneLoneCoder_FormulaOLC();
					race->createRace();
				}
				drawRace(fElapsedTime);
			}
			else if (sLastAction == "Frogger") {
				if (!frog) {
					frog = new OneLoneCoder_Frogger();
					frog->createFrogger();
				}
				drawFrogger(fElapsedTime);
			}
			else if (sLastAction == "Tetris") {
				if (!tetris) {
					tetris = new OneLoneCoder_Tetris();
					tetris->createTetris();
				}
				drawTetris(fElapsedTime);
			}
			else if (sLastAction == "Asteroids") {
				if (!asteroids) {
					asteroids = new OneLoneCoder_Asteroids();
					createAsteroid();
				}
				updateAsteroid(fElapsedTime);
			}
			else if (sLastAction == "Snake") {
				if (!snake) {
					snake = new OneLoneCoder_Snake();
					createSnake();
				}
				updateSnake(fElapsedTime);
			}
		}

		return true;
	}

	std::string title;
	std::string sLastAction;

	bool gameState = false;
	int menu = 0;
};

int main()
{
	srand((int)time(0));
	olcRetroPopUpMenu demo;
	if (demo.Construct(384, 240, 4, 4))
		demo.Start();

	return 0;
}