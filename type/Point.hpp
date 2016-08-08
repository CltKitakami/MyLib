#ifndef _POINT_HPP_
#define _POINT_HPP_

template <typename T>
class Point
{
public:
	T x, y;

	Point(T x = 0, T y = 0) : x(x), y(y) {}
	Point(const Point &p) : x(p.x), y(p.y) {}

	Point operator + (const Point &p) const
	{ return Point(x + p.x, y + p.y); }

	Point operator - (const Point &p) const
	{ return Point(x - p.x, y - p.y); }

	Point operator * (T value) const
	{ return Point(x * value, y * value); }

	Point operator * (float value) const
	{ return Point((T)((float)x * value), (T)((float)y * value)); }

	Point operator * (double value) const
	{ return Point((T)((double)x * value), (T)((double)y * value)); }

	Point operator / (T value) const
	{ return Point(x / value, y / value); }

	Point operator / (float value) const
	{ return Point((T)((float)x / value), (T)((float)y / value)); }

	Point operator / (double value) const
	{ return Point((T)((double)x / value), (T)((double)y / value)); }

	Point & operator += (const Point &p)
	{ x += p.x, y += p.y; return *this; }

	Point & operator -= (const Point &p)
	{ x -= p.x, y -= p.y; return *this; }

	Point & operator *= (T value)
	{ x *= value, y *= value; return *this; }

	Point & operator *= (float value)
	{
		x = (T)((float)x * value);
		y = (T)((float)y * value);
		return *this;
	}

	Point & operator *= (double value)
	{
		x = (T)((double)x * value);
		y = (T)((double)y * value);
		return *this;
	}

	Point & operator /= (T value)
	{ x /= value, y /= value; return *this; }

	Point & operator /= (float value)
	{
		x = (T)((float)x / value);
		y = (T)((float)y / value);
		return *this;
	}

	Point & operator /= (double value)
	{
		x = (T)((double)x / value);
		y = (T)((double)y / value);
		return *this;
	}

	bool operator == (const Point &p) const
	{ return x == p.x && y == p.y; }

	bool operator != (const Point &p) const
	{ return x != p.x || y != p.y; }
};

typedef Point<uint32_t> PointU;
typedef Point<int32_t> PointI;
typedef Point<float> PointF;
typedef Point<double> PointD;

#endif
