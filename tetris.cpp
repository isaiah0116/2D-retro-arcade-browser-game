#include "olcPixelGameEngine.h"
#include <string>
using namespace std;

class OneLoneCoder_Tetris {
public:
    int iFieldWidth = 10;
    int iFieldHeight = 20;
    int iSize = 16;
    int iDrawOffsetX = 0;
    int iDrawOffsetY = 0;

    unsigned char* pField = nullptr;

    int iCurrentPeice = 0;
    int iNextPeice = 0;
    int iCurrentRotation = 0;
    int iCurrentX = iFieldWidth / 2;        // Start the piece roughly in the middle of the playing field
    int iCurrentY = 0;

    int iSpeed = 15;
    int iSpeedCounter = 0;
    bool bForceDown = false;

    int iScore = 0;
    int iLinesCleared = 0;
    int iLevel = 1;

    float tickRate = 1.0f / 30;
    float fcurrentTicks = 0.0f;

    vector<int> vLines;

    bool bIsDebug = false;
    bool isGameOver = false;

    OneLoneCoder_Tetris() {}
    ~OneLoneCoder_Tetris() {}

    void createTetris() {

        // Initialize the playing field
        pField = new unsigned char[iFieldWidth * iFieldHeight];
        for (int x = 0; x < iFieldWidth; x++) {
            for (int y = 0; y < iFieldHeight; y++) {
                pField[y * iFieldWidth + x] = (x == 0 || x == iFieldWidth - 1 || y == iFieldHeight - 1) ? 11 : 0;
            }
        }

        iDrawOffsetX = 2;
        iDrawOffsetY = -5;


        iNextPeice = std::rand() % 7;

        SpawnRandomPeice();
    }

    /**
     * Perform game logic here
     */

    void SpawnRandomPeice() {
        iCurrentPeice = iNextPeice;
        iNextPeice = std::rand() % 7;
        iCurrentRotation = rand() % 3;
        iCurrentX = iFieldWidth / 2;        // Start the piece roughly in the middle of the playing field
        iCurrentY = 0;
    }

    int Rotate(int posX, int posY, int r) {
        switch (r % 4) {
        case 0:         // 0 degress
            return posY * 4 + posX;
        case 1:         // 90 degrees
            return 12 + posY - (posX * 4);
        case 2:         // 180 degrees
            return 15 - (posY * 4) - posX;
        case 3:         // 270 degrees
            return 3 - posY + (posX * 4);
        }
        return 0;
    }
};