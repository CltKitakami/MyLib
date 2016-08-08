#ifndef _SHA1_HPP_
#define _SHA1_HPP_

#include "HashFunctionInterface.hpp"


class Sha1 : public HashFunctionInterface<64, 8>
{
public:
	Sha1() { this->reset(); }

	virtual void reset() override;

	virtual void transform(const uint8_t *data, size_t length) override;

	virtual void finalize() override;

	virtual std::string toString() override { return this->toHexString(); }

	std::string toHexString();
	std::string toBase64String();

	static std::string toHexString(const uint8_t *data, size_t length);
	static std::string toHexString(const char *data, size_t length);
	static std::string toHexString(const std::string &data);

	static std::string toBase64String(const uint8_t *data, size_t length);
	static std::string toBase64String(const char *data, size_t length);
	static std::string toBase64String(const std::string &data);

private:
	virtual void transformBlock(const uint8_t *block) override;
	virtual void addLengthMessage() override;

	uint32_t digest[5];
};

#endif
