#pragma once

class IDrawable
{
public:
	virtual ~IDrawable() {}
	IDrawable() {}
	virtual void Render() = 0;
};