#include "GameTimer.h"

GameTimer::GameTimer()
:mSecondsPerCount(0.0f),
mDeltaTime(-1.0f),
mBaseTime(0),
mPausedTime(0),
mPrevTime(0),
mCurrentTime(0),
mStopped(false)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	mSecondsPerCount = 1.0f / (double)countsPerSecond;
}

void GameTimer::Tick()
{

}