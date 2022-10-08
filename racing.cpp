#include <iostream>
#include <string>
using namespace std;

#include "olcPixelGameEngine.h"


class OneLoneCoder_FormulaOLC
{
public:
	float fDistance = 0.0f;			// Distance car has travelled around track
	float fCurvature = 0.0f;		// Current track curvature, lerped between track sections
	float fTrackCurvature = 0.0f;	// Accumulation of track curvature
	float fTrackDistance = 0.0f;	// Total distance of track

	float fCarPos = 0.0f;			// Current car position
	float fPlayerCurvature = 0.0f;			// Accumulation of player curvature
	float fSpeed = 0.0f;			// Current player speed

	vector<pair<float, float>> vecTrack; // Track sections, sharpness of bend, length of section

	list<float> listLapTimes;		// List of previous lap times
	float fCurrentLapTime;			// Current lap time
	int nTrackSection;

	// Called by olcConsoleGameEngine
	
	OneLoneCoder_FormulaOLC() {}
	~OneLoneCoder_FormulaOLC() {}

	void createRace()
	{
		// Define track
		vecTrack.push_back(make_pair(0.0f, 10.0f));		// Short section for start/finish line
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.0f, 400.0f));
		vecTrack.push_back(make_pair(-1.0f, 100.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(-1.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(0.2f, 500.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));

		// Calculate total track distance, so we can set lap times
		for (auto t : vecTrack)
			fTrackDistance += t.second;

		listLapTimes = { 0,0,0,0,0 };
		fCurrentLapTime = 0.0f;
	}

	// Called by olcConsoleGameEngine
	void updateRace(float fElapsedTime)
	{
		// If car curvature is too different to track curvature, slow down
		// as car has gone off track
		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.6f)
			fSpeed -= 5.0f * fElapsedTime;

		// Clamp Speed
		if (fSpeed < 0.0f)	fSpeed = 0.0f;
		if (fSpeed > 1.0f)	fSpeed = 1.0f;

		// Move car along track according to car speed
		fDistance += (70.0f * fSpeed) * fElapsedTime;

		// Get Point on track
		float fOffset = 0;
		nTrackSection = 0;

		// Lap Timing and counting
		fCurrentLapTime += fElapsedTime;
		if (fDistance >= fTrackDistance)
		{
			fDistance -= fTrackDistance;
			listLapTimes.push_front(fCurrentLapTime);
			listLapTimes.pop_back();
			fCurrentLapTime = 0.0f;
		}

		// Find position on track (could optimise)
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
		{
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		// Interpolate towards target track curvature
		float fTargetCurvature = vecTrack[nTrackSection - 1].first;
		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;

		// Accumulate player curvature
		fCurvature += fTrackCurveDiff;

		// Accumulate track curvature
		fTrackCurvature += (fCurvature)*fElapsedTime * fSpeed;
	}
};