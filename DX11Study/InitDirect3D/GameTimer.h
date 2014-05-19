class GameTimer
{
public:
	GameTimer();
	float GameTime() const;		// In seconds.
	float DeltaTime() const;	// In seconds.
	void Reset();	// Call before message loop.
	void Start();	// Call when unpaused.
	void Stop();	// Call when paused.
	void Tick();	// Call every frame.

private:
	double mSecondsPerCount;
	double mDeltaTime;
	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64	mStopTime;
	__int64 mPrevTime;
	__int64 mCurrentTime;
	bool mStopped;
};