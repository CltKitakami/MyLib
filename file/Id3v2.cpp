#include "Id3v2.hpp"


const int32_t Id3v2::Frame::ids[FRAME_IDS] =
{
	FRAME_ID_TO_INT("AENC"),
	FRAME_ID_TO_INT("APIC"),
	FRAME_ID_TO_INT("COMM"),
	FRAME_ID_TO_INT("COMR"),
	FRAME_ID_TO_INT("ENCR"),
	FRAME_ID_TO_INT("EQUA"),
	FRAME_ID_TO_INT("ETCO"),
	FRAME_ID_TO_INT("GEOB"),
	FRAME_ID_TO_INT("GRID"),
	FRAME_ID_TO_INT("IPLS"),
	FRAME_ID_TO_INT("LINK"),
	FRAME_ID_TO_INT("MCDI"),
	FRAME_ID_TO_INT("MLLT"),
	FRAME_ID_TO_INT("OWNE"),
	FRAME_ID_TO_INT("PRIV"),
	FRAME_ID_TO_INT("PCNT"),
	FRAME_ID_TO_INT("POPM"),
	FRAME_ID_TO_INT("POSS"),
	FRAME_ID_TO_INT("RBUF"),
	FRAME_ID_TO_INT("RVAD"),
	FRAME_ID_TO_INT("RVRB"),
	FRAME_ID_TO_INT("SYLT"),
	FRAME_ID_TO_INT("SYTC"),
	FRAME_ID_TO_INT("TALB"),
	FRAME_ID_TO_INT("TBPM"),
	FRAME_ID_TO_INT("TCOM"),
	FRAME_ID_TO_INT("TCON"),
	FRAME_ID_TO_INT("TCOP"),
	FRAME_ID_TO_INT("TDAT"),
	FRAME_ID_TO_INT("TDLY"),
	FRAME_ID_TO_INT("TENC"),
	FRAME_ID_TO_INT("TEXT"),
	FRAME_ID_TO_INT("TFLT"),
	FRAME_ID_TO_INT("TIME"),
	FRAME_ID_TO_INT("TIT1"),
	FRAME_ID_TO_INT("TIT2"),
	FRAME_ID_TO_INT("TIT3"),
	FRAME_ID_TO_INT("TKEY"),
	FRAME_ID_TO_INT("TLAN"),
	FRAME_ID_TO_INT("TLEN"),
	FRAME_ID_TO_INT("TMED"),
	FRAME_ID_TO_INT("TOAL"),
	FRAME_ID_TO_INT("TOFN"),
	FRAME_ID_TO_INT("TOLY"),
	FRAME_ID_TO_INT("TOPE"),
	FRAME_ID_TO_INT("TORY"),
	FRAME_ID_TO_INT("TOWN"),
	FRAME_ID_TO_INT("TPE1"),
	FRAME_ID_TO_INT("TPE2"),
	FRAME_ID_TO_INT("TPE3"),
	FRAME_ID_TO_INT("TPE4"),
	FRAME_ID_TO_INT("TPOS"),
	FRAME_ID_TO_INT("TPUB"),
	FRAME_ID_TO_INT("TRCK"),
	FRAME_ID_TO_INT("TRDA"),
	FRAME_ID_TO_INT("TRSN"),
	FRAME_ID_TO_INT("TRSO"),
	FRAME_ID_TO_INT("TSIZ"),
	FRAME_ID_TO_INT("TSRC"),
	FRAME_ID_TO_INT("TSSE"),
	FRAME_ID_TO_INT("TYER"),
	FRAME_ID_TO_INT("TXXX"),
	FRAME_ID_TO_INT("UFID"),
	FRAME_ID_TO_INT("USER"),
	FRAME_ID_TO_INT("USLT"),
	FRAME_ID_TO_INT("WCOM"),
	FRAME_ID_TO_INT("WCOP"),
	FRAME_ID_TO_INT("WOAF"),
	FRAME_ID_TO_INT("WOAR"),
	FRAME_ID_TO_INT("WOAS"),
	FRAME_ID_TO_INT("WORS"),
	FRAME_ID_TO_INT("WPAY"),
	FRAME_ID_TO_INT("WPUB"),
	FRAME_ID_TO_INT("WXXX")
};


Id3v2::Header::Info Id3v2::Header::read(File &rf)
{
	Info info;
	char header[11];
	rf.read(header, 10);

	if (check(header) == false)
	{
		LOG.d("ID3 not found.");
		return info;
	}

	info.majorVersion = header[3];
	info.minorVersion = header[4];
	LOG.d(Log::format("ID3v2.%d.%d",
		(int)info.majorVersion, (int)info.minorVersion));
	LOG.d(Log::format("flag = 0x%x", getFlag(header)));

	if (getFlag(header) == 0)
		info.size = getSize(header);

	return info;
}


void Id3v2::Header::write(File &wf, const Id3v2::Header::Info &info)
{
	wf.write("ID3", 3);
	wf.write(info.majorVersion);
	wf.write(info.minorVersion);
	wf.write(0x00);
	wf.write((info.size >> 21) & 0x7f);
	wf.write((info.size >> 14) & 0x7f);
	wf.write((info.size >> 7) & 0x7f);
	wf.write(info.size & 0x7f);
}


const char * Id3v2::Frame::readText(File &rf, int32_t size)
{
	if (this->text.length() != 0)
		this->text.clear();

	rf.read(this->text, size);

	return this->text.data();
}


int Id3v2::Frame::seekTwoBytes(File &rf, char first, char second, int maxRead)
{
	int cnt = maxRead;
	bool isFind = false;

	while (--cnt >= 0)
	{
		if ((char)rf.read() == first)
		{
			char c = (char)rf.read();

			if (c == second)
			{
				isFind = true;
				break;
			}
			else if (c == first)
				rf.goToOffset(-1);
		}
	}

	if (isFind == true)
	{
		rf.goToOffset(-2);
		return maxRead - cnt - 1;
	}
	else
		return -1;
}


Id3v2::Frame::PictureFormat Id3v2::Frame::readImageFormat(File &rf)
{
	char format[12] = { '\0' };
	char *start = format;
	const char *end = format + 11;

	*start = (char)rf.read();

	while (*start != '\0' && start != end)
	{
		++start;
		*start = (char)rf.read();
	}

	LOG.d(Log::format("image format = %s", format));

	if (Convert::isInRange(format[6], 'A', 'Z') == true)
		format[6] = char((int)format[6] - (int)'A' + (int)'a');

	switch (format[6])
	{
		case 'j': return JPEG;
		case 'p': return PNG;
		case 'b': return BMP;
		default: return UNKNOWN_FORMAT;
	}
}


void Id3v2::Frame::readImage(File &rf)
{
	this->picture.format = readImageFormat(rf);
	this->picture.type = readImageType(rf);

	if (picture.type == FRONT_COVER)
	{
		const int typeByte = 1;
		int mimeTypeBytes = 0;
		int bytes = -1;

		if (picture.format == JPEG)
		{
			bytes = seekTwoBytes(rf, '\xff', '\xd8', 10);
			mimeTypeBytes = (int)sizeof("image/jpeg") - 1;
		}
		else if (picture.format == PNG)
		{
			bytes = seekTwoBytes(rf, '\x89', 'P', 10);
			mimeTypeBytes = (int)sizeof("image/png") - 1;
		}
		else if (picture.format == BMP)
		{
			bytes = seekTwoBytes(rf, 'B', 'M', 10);
			mimeTypeBytes = (int)sizeof("image/x-ms-bmp") - 1;	// TODO: check
		}

		LOG << "bytes = " << bytes;

		if (bytes != -1)
		{
			picture.size = this->size - typeByte - mimeTypeBytes - bytes;
			this->readText(rf, picture.size);
		}
		else
		{
			LOG.e(TRACE, "picture error.");
			LOG.e(TRACE, Log::format("format = %d", (int)picture.format));
			LOG.e(TRACE, Log::format("type = %d", (int)picture.type));
		}
	}
	else
	{
		// TODO:
		LOG.e(TRACE, "other picture type.");
	}
}


Id3v2::Frame Id3v2::Frame::read(File &rf)
{
	char header[11];
	rf.read(header, 10);

	Frame frame;
	frame.id = getId(header);

	if (frame.id != -1)
	{
		frame.size = getSize(header);
		frame.flag = getFlag(header);
		frame.readEncode(rf);	// size - 1;

		LOG << "size = " << frame.size << '\n';
		LOG << "flag = " << frame.flag << '\n';
		LOG << "encode = " << (int)frame.encode << '\n';

		if (isText(frame.id) == true)
		{
			frame.readText(rf, frame.size - 1);
			LOG << "text = " << frame.getText() << '\n';;
		}
		else
		{
			frame.readImage(rf);
		}
	}
	else
	{
		LOG.e("The frame id is not valid");
		rf.goToOffset(-10);
	}

	return frame;
}


void Id3v2::Frame::write(File &wf, const Id3v2::Frame &frame)
{
	writeIntBigEndian(wf, frame.id);
	writeIntBigEndian(wf, frame.size);
	wf.write(0x00);
	wf.write(0x00);
	wf.write(static_cast<char>(frame.encode));

	if (isText(frame.id) == true)
		wf.write(frame.getText(), frame.size - 1);
	else
	{
		if (frame.picture.format != UNKNOWN_FORMAT &&
			frame.picture.type == FRONT_COVER)
		{
			const char *mimeTypeStr = nullptr;
			const char *mimeTypes[] = {
				"image/jpeg\0",
				"image/png\0",
				"image/x-ms-bmp\0"
			};

			switch (frame.picture.format)
			{
				case JPEG:
					mimeTypeStr = mimeTypes[0];
					break;
				case PNG:
					mimeTypeStr = mimeTypes[1];
					break;
				case BMP:
					mimeTypeStr = mimeTypes[2];
					break;
				default:
					mimeTypeStr = mimeTypes[0];
					break;
			}

			size_t mimeTypeBytes = strlen(mimeTypeStr) + 1;
			LOG << "write " << mimeTypeStr << '\n';

			wf.write(mimeTypeStr, (File::SizeType)mimeTypeBytes);
			wf.write("\x03\x00", 2);
			wf.write(frame.getText(), frame.picture.size);

			const char padding[64] = { '\0' };
			wf.write(padding, 64);
		}
	}
}
