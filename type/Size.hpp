#ifndef _SIZE_HPP_
#define _SIZE_HPP_

template <typename T = int>
class Size
{
public:
	T width, height;

	Size() : width(0), height(0) {}
	Size(T width, T height) : width(width), height(height) {}
	Size(const Size &size) : width(size.width), height(size.height) {}

	T getArea() const { return width * height; }

	Size operator + (const Size &s) const
	{ return Size(width + s.width, height + s.height); }

	Size operator - (const Size &s) const
	{ return Size(width - s.width, height - s.height); }

	Size operator * (T value) const
	{ return Size(width * value, height * value); }

	Size operator * (float value) const
	{ return Size((T)((float)width * value), (T)((float)height * value)); }

	Size operator * (double value) const
	{ return Size((T)((double)width * value), (T)((double)height * value)); }

	Size operator / (T value) const
	{ return Size(width / value, height / value); }

	Size operator / (float value) const
	{ return Size((T)((float)width / value), (T)((float)height / value)); }

	Size operator / (double value) const
	{ return Size((T)((double)width / value), (T)((double)height / value)); }

	Size & operator += (const Size &s) { width += s.width, height += s.height; return *this; }

	Size & operator -= (const Size &s) { width -= s.width, height -= s.height; return *this; }

	Size & operator *= (T value)
	{ width *= value, height *= value; return *this; }

	Size & operator *= (float value)
	{
		width = (T)((float)width * value);
		height = (T)((float)height * value);
		return *this;
	}

	Size & operator *= (double value)
	{
		width = (T)((double)width * value);
		height = (T)((double)height * value);
		return *this;
	}

	Size & operator /= (T value)
	{ width /= value, height /= value; return *this; }

	Size & operator /= (float value)
	{
		width = (T)((float)width / value);
		height = (T)((float)height / value);
		return *this;
	}

	Size & operator /= (double value)
	{
		width = (T)((double)width / value);
		height = (T)((double)height / value);
		return *this;
	}

	bool operator == (const Size &s) const
	{ return width == s.width && height == s.height; }

	bool operator != (const Size &s) const
	{ return width != s.width || height != s.height; }
};

typedef Size<uint32_t> SizeU;
typedef Size<int32_t> SizeI;
typedef Size<float> SizeF;
typedef Size<double> SizeD;

#endif
