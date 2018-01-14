#ifndef __H_ANIMATION__ 
#define __H_ANIMATION__ 

#include <vector>
#include "globals.h"

struct SDL_Rect;

class Animation
{
public:

	Animation()
	{}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), frames(anim.frames)
	{}

	SDL_Rect& GetCurrentFrame()
	{
		float lastFrame = (float)frames.size();

		currentFrame += speed;
		if (currentFrame >= lastFrame)
		{
			currentFrame = (loop) ? 0.0f : MAX(lastFrame - 1.0f, 0.0f);
			loops++;
		}

		return frames[(int)currentFrame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		currentFrame = 0.0f;
	}

public:

	bool loop = true;
	float speed = 1.0f;
	std::vector<SDL_Rect> frames;

private:

	float currentFrame = 0.0f;
	int loops = 0;

};

#endif /* __H_ANIMATION__ */
