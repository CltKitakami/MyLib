#ifndef _SHA512_HPP_
#define _SHA512_HPP_

#include "HashFunctionInterface.hpp"


class Sha512 : public HashFunctionInterface<128, 16>
{
public:
	Sha512() { this->reset(); }

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

	uint64_t digest[8];
	uint64_t lengthHigh;

	static const uint64_t k512[80];
};

#endif
