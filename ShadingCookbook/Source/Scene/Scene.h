#pragma once

class Scene
{
public:
	Scene() : ShouldAnimate(true) {}
	virtual void InitScene() = 0;
	virtual void Update(float t) = 0;
	virtual void Render() = 0;

	virtual void Resize(int x, int y) = 0;

	void Animate(bool value)
	{
		ShouldAnimate = value;
	}

	bool IsAnimating()
	{
		return ShouldAnimate;
	}

protected:
	bool ShouldAnimate;
};