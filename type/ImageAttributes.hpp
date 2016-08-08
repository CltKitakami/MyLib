#ifndef _IMAGEATTRIBUTES_HPP_
#define _IMAGEATTRIBUTES_HPP_

#include <cstdint>
#include "Point.hpp"
#include "Size.hpp"
#include "Rect.hpp"


class ImageAttributes
{
public:
	ImageAttributes() :
		offsetX(0), offsetY(0), width(0), height(0),
		lineStep(0), channel(3), buffer(nullptr) {}

	void assign(char *buffer) { this->buffer = buffer; }

	void assign(char *buffer, uint8_t channel, uint32_t lineStep)
	{
		this->buffer = buffer;
		this->channel = channel;
		this->lineStep = lineStep;
	}

	char * getBuffer() { return this->buffer; }
	const char * getBuffer() const { return this->buffer; }

	char * getRow(uint32_t row)
	{ return &buffer[lineStep * (row + offsetY) + offsetX * channel]; }

	const char * getRow(uint32_t row) const
	{ return &buffer[lineStep * (row + offsetY) + offsetX * channel]; }

	char * getPosition(uint32_t x, uint32_t y)
	{ return &buffer[lineStep * (y + offsetY) + (x + offsetX) * channel]; }

	char * getPosition(const PointU &position)
	{ return getPosition(position.x, position.y); }

	const char * getPosition(uint32_t x, uint32_t y) const
	{ return &buffer[lineStep * (y + offsetY) + (x + offsetX) * channel]; }

	const char * getPosition(const PointU &position) const
	{ return getPosition(position.x, position.y); }

	PointU getOffset() const { return PointU(offsetX, offsetY); }

	void setOffset(const PointU &offset)
	{ setOffset(offset.x, offset.y); }

	void setOffset(uint32_t x, uint32_t y)
	{ this->offsetX = x, this->offsetY = y; }

	uint32_t getOffsetX() const { return this->offsetX; }
	void setOffsetX(uint32_t x) { this->offsetX = x; }

	uint32_t getOffsetY() const { return this->offsetY; }
	void setOffsetY(uint32_t y) { this->offsetY = y; }

	SizeU getSize() const { return SizeU(width, height); }
	void setSize(const SizeU &size) { setSize(size.width, size.height); }

	void setSize(uint32_t width, uint32_t height)
	{ this->width = width, this->height = height; }

	uint32_t getWidth() const { return this->width; }
	void setWidth(uint32_t width) { this->width = width; }

	uint32_t getHeight() const { return this->height; }
	void setHeight(uint32_t height) { this->height = height; }

	RectU getRect() const
	{ return RectU(offsetX, offsetY, width, height); }

	void setRect(const RectU &rect)
	{ setRect(rect.x, rect.y, rect.width, rect.height); }

	void setRect(uint32_t offsetX, uint32_t offsetY,
		uint32_t width, uint32_t height)
	{
		this->offsetX = offsetX;
		this->offsetY = offsetY;
		this->width = width;
		this->height = height;
	}

	uint8_t getChannel() const { return channel; }
	void setChannel(uint8_t channel) { this->channel = channel; }

	uint32_t getLineStep() const { return lineStep; }
	void setLineStep(uint32_t lineStep) { this->lineStep = lineStep; }

private:
	uint32_t offsetX, offsetY, width, height, lineStep;
	uint8_t channel;
	char *buffer;
};

#endif
