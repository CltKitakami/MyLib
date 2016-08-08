#ifndef _RECT_HPP_
#define _RECT_HPP_

#include "Point.hpp"
#include "Size.hpp"


template <typename T = int>
class Rect
{
public:
	T x, y, width, height;

	Rect() : x(0), y(0), width(0), height(0) {}

	Rect(T x, T y, T width, T height) :
		x(x), y(y), width(width), height(height) {}

	Rect(const Rect &rect) :
		x(rect.x), y(rect.y),
		width(rect.width), height(rect.height) {}

	Rect(const Point<T> &topLeft, const Size<T> &size) :
		x(topLeft.x), y(topLeft.y),
		width(size.width), height(size.height) {}

	Rect(const Point<T> &topLeft, const Point<T> &bottomRight) :
		x(topLeft.x), y(topLeft.y),
		width(bottomRight.x - topLeft.x),
		height(bottomRight.y - topLeft.y) {}

	Point<T> topLeft() const { return Point<T>(x, y); }
	Point<T> topRight() const { return Point<T>(x + width, y); }
	Point<T> bottomLeft() const { return Point<T>(x, y + height); }
	Point<T> bottomRight() const { return Point<T>(x + width, y + height); }

	void setSize(const Size<T> &size)
	{ width = size.width, height = size.height; }

	Size<T> getSize() const { return Size<T>(width, height); }

	T getArea() const { return width * height; }

	Point<T> getCenter() const
	{ return Point<T>(x + width / 2, y + height / 2); }

	void moveTo(const Point<T> &topLeft) { x = topLeft.x, y = topLeft.y; }

	void moveCenterTo(const Point<T> &center)
	{
		Size<T> half = getSize() / 2;
		x = center.x - half.width;
		y = center.y - half.height;
	}

	bool contains(const Point<T> &p) const
	{
		return x <= p.x && y <= p.y &&
			x + width >= p.x && y + height >= p.y;
	}

	bool operator == (const Rect &r) const
	{
		return x == r.x && y == r.y &&
			width == r.width && height == r.height;
	}

	bool operator != (const Rect &r) const
	{ return *this == r ? true : false; }
};

typedef Rect<uint32_t> RectU;
typedef Rect<int32_t> RectI;
typedef Rect<float> RectF;
typedef Rect<double> RectD;

#endif
