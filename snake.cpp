#include "olcPixelGameEngine.h"
#include "vector"

using namespace std;

struct SnakeSegment
{
	olc::vi2d vLastPos;
	olc::vi2d vCurrentPos;

	SnakeSegment(const olc::vi2d& currpos)
	{
		vCurrentPos = currpos;
	}
	SnakeSegment(const SnakeSegment& snakeSeg)
		:vLastPos(snakeSeg.vLastPos), vCurrentPos(snakeSeg.vCurrentPos)
	{
	}
};

class OneLoneCoder_Snake
{
public:
	std::vector<SnakeSegment>* SnakeBody = new std::vector<SnakeSegment>();
	std::vector<SnakeSegment>& snakeBodyRef = *SnakeBody;
	olc::vi2d vSnakeHeadDir;
	olc::vi2d RightDir;
	olc::vi2d LeftDir;
	int* GridArray;
	olc::Sprite* sprTile;
	olc::vi2d vBlockSize = { 16,16 };
	olc::vi2d vTextPos = { 400, 20 };
	olc::vi2d vFoodPos;
	olc::Pixel cFoodColour;
	int CurrentScore = 0;
	bool isGameover = false;
	float currentTime = 0.0f;

	OneLoneCoder_Snake()
	{
		ResetSnake();
	}

	~OneLoneCoder_Snake()
	{
		delete SnakeBody;
	}

	void ResetSnake()
	{
		SnakeBody->clear();
		SnakeBody->emplace_back(olc::vi2d{ 12,7 });//head
		SnakeBody->emplace_back(olc::vi2d{ 11,7 });//body
		SnakeBody->emplace_back(olc::vi2d{ 10,7 });//tail
		ChangeDirection({ 1,0 });
	}

	void DrawSnakeSegments(olc::PixelGameEngine* gameInstance, const olc::vi2d& vBlockSize)
	{
		for (unsigned int i = 0; i < snakeBodyRef.size(); i++)
		{
			gameInstance->FillRect(snakeBodyRef[i].vCurrentPos * vBlockSize, vBlockSize, olc::RED);
		}
	}

	bool DetectContact(const olc::vi2d& point1, const olc::vi2d& point2, const olc::vi2d& vBlockSize)
	{
		return (point1.x >= point2.x && point1.y >= point2.y && point1.x < (point2.x + vBlockSize.x) && point1.y < (point2.y + vBlockSize.y));
	}

	void DetectSelfContact(bool& isGameOver, const olc::vi2d& vBlockSize)
	{
		for (int i = 0; i < snakeBodyRef.size(); i++)
		{
			if (i == 0)//the head should not detect itself
			{
				continue;
			}
			if (DetectContact(GetSnakeHead().vCurrentPos * vBlockSize, snakeBodyRef[i].vCurrentPos * vBlockSize, vBlockSize))
			{
				isGameOver = true;
			}
		}
	}

	void AddSnakeSegment(const olc::vi2d& vBlockSize)
	{
		const SnakeSegment lastSegment = GetCurrentSnakeTail();
		const olc::vi2d vNextSegmentPos = (lastSegment.vLastPos * vBlockSize) - (vBlockSize * vSnakeHeadDir);
		SnakeBody->emplace_back(vNextSegmentPos);
	}

	void MoveSnakeSegments()
	{
		for (int i = 0; i < snakeBodyRef.size(); i++)
		{
			if (i == 0)//the head only needs to change it's lastpos
			{
				snakeBodyRef[i].vLastPos = snakeBodyRef[i].vCurrentPos;
			}
			else//the head has to move at least one step forward to move the rest of the body 
			{
				if (i == 1)
				{
					snakeBodyRef[i].vLastPos = snakeBodyRef[i].vCurrentPos;
					snakeBodyRef[i].vCurrentPos = GetSnakeHead().vLastPos;
				}
				else
				{
					snakeBodyRef[i].vLastPos = snakeBodyRef[i].vCurrentPos;
					snakeBodyRef[i].vCurrentPos = snakeBodyRef[i - 1].vLastPos;
				}
			}

		}
		GetSnakeHead().vCurrentPos += vSnakeHeadDir;
	}

	void ChangeDirection(const olc::vi2d& dir)
	{
		vSnakeHeadDir = dir;
		SetPossibleDirection();
	}

	void SetPossibleDirection()
	{
		if (vSnakeHeadDir.x == 1 && vSnakeHeadDir.y == 0)//west
		{
			LeftDir = { 0, -1 };
			RightDir = { 0, 1 };
		}
		else if (vSnakeHeadDir.x == 0 && vSnakeHeadDir.y == 1)//south
		{
			LeftDir = { 1, 0 };
			RightDir = { -1, 0 };
		}
		else if (vSnakeHeadDir.x == -1 && vSnakeHeadDir.y == 0)//east
		{
			LeftDir = { 0, 1 };
			RightDir = { 0, -1 };
		}
		else if (vSnakeHeadDir.x == 0 && vSnakeHeadDir.y == -1)//north
		{
			LeftDir = { -1, 0 };
			RightDir = { 1, 0 };
		}
	}

	SnakeSegment& GetSnakeHead()
	{
		return snakeBodyRef[0];
	}

	int GetCurrentSnakeSize()
	{
		return SnakeBody->size();
	}

	SnakeSegment& GetCurrentSnakeTail()
	{
		return snakeBodyRef[SnakeBody->size() - 1];
	}

	olc::vi2d GetRightDir()
	{
		return RightDir;
	}

	olc::vi2d GetLeftDir()
	{
		return LeftDir;
	}
};