#ifndef _ID3V2_HPP_
#define _ID3V2_HPP_

#include <cstdint>

#include "common/Log.hpp"
#include "common/Convert.hpp"
#include "file/File.hpp"


#define FRAME_IDS (74)
#define FRAME_ID_TO_INT(str) \
	((str[0] << 24) | (str[1] << 16) |\
	(str[2] << 8) | str[3])


class Id3v2
{
public:

	static int charToSynchsafeInt(const char *array, int offset)
	{
		const char *start = array + offset;
		return (int)(
			((start[0] & 0x7f) << 21) +
			((start[1] & 0x7f) << 14) +
			((start[2] & 0x7f) << 7) +
			(start[3] & 0x7f));
	}

	static int charToInt(const char *array, int offset)
	{
		const char *start = array + offset;
		return (int)(
			((uint8_t)start[0] << 24) + ((uint8_t)start[1] << 16) +
			((uint8_t)start[2] << 8) + (uint8_t)start[3]);
	}


	class Header
	{
	public:
		typedef struct Info
		{
			Info() : majorVersion(3), minorVersion(0),
				size(-1) {}

			char majorVersion, minorVersion;
			int size;
		} Info;

		static int check(const char *header)
		{
			return header[0] == 'I' && header[1] == 'D' && header[2] == '3';
		}

		static int16_t getFlag(const char *header)
		{ return (int16_t)header[5]; }

		static int getSize(const char *header)
		{ return Id3v2::charToSynchsafeInt(header, 6); }


		static Info read(File &rf);
		static void write(File &wf, const Info &info);
	};


	class Frame
	{
	public:
		typedef enum Encode
		{
			ISO88591,
			UTF16,
			UTF16BE,
			UTF8
		} Encode;

		typedef enum PictureFormat
		{
			UNKNOWN_FORMAT = -1, JPEG = 0, PNG, BMP
		} PictureFormat;

		typedef enum PictureType
		{
			FRONT_COVER = 3
		} PictureType;

		typedef struct Picture
		{
			Picture() :
				format(UNKNOWN_FORMAT),
				type(FRONT_COVER), size(0) {}

			PictureFormat format;
			PictureType type;
			int size;
		} Picture;

		Frame() :
			id(0), size(0), flag(0),
			encode(ISO88591) {}
		virtual ~Frame() {}

		static int getId(const char *header)
		{
			LOG << "${" << std::string(header, 4) << "}\n";
			int id = FRAME_ID_TO_INT(header);
			return isIdValid(id) == true ? id : -1;
		}

		static int getSize(const char *header)
		{ return Id3v2::charToInt(header, 4); }

		static int16_t getFlag(const char *header)
		{ return (int16_t)header[8]; }

		Encode readEncode(File &rf)
		{
			this->encode = (Encode)rf.read();
			return this->encode;
		}

		const char * getText() const { return this->text.data(); }
		const char * readText(File &rf, int size);

		static int seekTwoBytes(File &rf, char first, char second, int maxRead);

		static PictureFormat readImageFormat(File &rf);
		static PictureType readImageType(File &rf)
		{ return (PictureType)rf.read(); }


		void readImage(File &rf);

		static bool isText(int id)
		{
			if (id != FRAME_ID_TO_INT("APIC"))
				return true;
			else
				return false;
		}

		static Frame read(File &rf);
		static void write(File &wf, const Frame &frame);

		static void writeIntBigEndian(File &wf, int value)
		{
			wf.write((char)((value >> 24) & 0xff));
			wf.write((char)((value >> 16) & 0xff));
			wf.write((char)((value >> 8) & 0xff));
			wf.write((char)(value & 0xff));
		}


		int id;
		int size;
		int16_t flag;
		Encode encode;
		std::string text;
		Picture picture;

	private:
		static bool isIdValid(int id)
		{
			for (int i = FRAME_IDS - 1; i >= 0; --i)
				if (ids[i] == id)
					return true;
			return false;
		}

		static const int ids[FRAME_IDS];
	};
};

#endif
