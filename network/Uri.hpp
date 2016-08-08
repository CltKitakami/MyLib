#ifndef _URI_HPP_
#define _URI_HPP_

#include <cstdint>
#include <string>


class Uri
{
public:
	typedef enum Type { HTTP, HTTPS, UNKNOWN } Type;

	Uri() : type(UNKNOWN) {}
	Uri(const std::string &uri);
	Uri(const Uri &uri) = default;
	Uri(Uri &&uri) = default;

	Uri & operator = (const Uri &uri) = default;
	Uri & operator = (Uri &&uri) = default;
	Uri & operator = (const std::string &uri);

	Type getType() const { return this->type; }

	uint16_t getPort() const { return this->port; }
	void setPort(uint16_t port) { this->port = port; }

	std::string getHostName() const { return this->hostName; }
	void setHostName(const std::string &n) { this->hostName = n; }

	std::string getHostPath() const { return this->hostPath; }
	void setHostPath(const std::string &p) { this->hostPath = p; }

	void set(const std::string &uri);

	int parseUrl(const std::string &uri);
	void parseUrn(const std::string &uri, int urlEnd);

	static std::string encode(const std::string &str);
	static std::string decode(const std::string &str);


private:
	size_t parseScheme(const std::string &uri);

	Type type;
	uint16_t port;
	std::string hostName;
	std::string hostPath;
};

#endif
