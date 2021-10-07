#ifndef ANIMATOR_H
#define ANIMATOR_H

struct AnimateInstance
{
	float elapsed;
	float blendFactor;
	unsigned currAnimateIndex;
	unsigned nextAnimateIndex;

};

class Animator
{
public: 
	Animator();
	~Animator();
	bool isActive() const;
	bool isBlending() const;
private:
	std::string currName;
	bool active;
	bool paused;
	bool blending;
	float speed;
	unsigned currAnimateIndex;
	unsigned nextAnimateIndex;
};
#endif
