#include "FileStream.h"
#include "Log.h"

bool LoadEmptyFileStream(FileStream& stream)
{
	stream.length = 0;

	stream.data = new u8[16];
	if (!stream.data)
	{
		Log("WARNING: Memory allocation failure in file");
		return false;
	}
	stream.capacity = 16;

	return true;
}

bool LoadFileStream(FileStream& stream, const std::string& path)
{
	FILE* file = nullptr;
	fopen_s(&file, path.c_str(), "rb");
	if (!file)
	{
		Log("WARNING: Couldn't open file %s", path.c_str());
		return false;
	}

	fseek(file, 0, SEEK_END);
	stream.length = (u32)ftell(file);
	if (!stream.length)
	{
		Log("WARNING: Empty file % s", path.c_str());
		return false;
	}
	rewind(file);

	stream.data = new u8[stream.length];
	if (!stream.data)
	{
		Log("WARNING: Memory allocation failure in file %s", path.c_str());
		fclose(file);
		return false;
	}
	stream.capacity = stream.length;

	fread_s(stream.data, stream.length, sizeof(u8), stream.length, file);
	fgetc(file);

	if (!feof(file))
		Log("WARNING: Unread data in file %s", path.c_str());

	fclose(file);
	return true;
}

bool SaveFileStream(const FileStream& stream, const std::string& path)
{
	FILE* file = nullptr;
	fopen_s(&file, path.c_str(), "wb");
	if (!file)
	{
		Log("WARNING: Couldn't open file %s", path.c_str());
		return false;
	}

	fwrite(stream.data, sizeof(u8), stream.length, file);

	fclose(file);
	return true;
}

void ReleaseFileStream(FileStream& stream)
{
	delete[] stream.data;

	stream.data = nullptr;
	stream.length = 0;
}

void FileStreamBufferRead(const FileStream& stream, u32 offset, u8* buffer, u32 length)
{
	memcpy(buffer, stream.data + offset, length);
}

void FileStreamBufferReadUpdate(const FileStream& stream, u32& offset, u8* buffer, u32 length)
{
	FileStreamBufferRead(stream, offset, buffer, length);
	offset += length;
}

void FileStreamExpand(FileStream& stream)
{
	stream.capacity *= 2;
	u8* data = new u8[stream.capacity];
	memcpy(data, stream.data, stream.length);

	delete[] stream.data;
	stream.data = data;
}

void FileStreamBufferWriteBack(FileStream& stream, const u8* buffer, u32 length)
{
	while (stream.length + length >= stream.capacity)
		FileStreamExpand(stream);

	memcpy(stream.data + stream.length, buffer, length);
	stream.length += length;
}

u8* FileStreamGetDataPtr(const FileStream& stream, u32 offset)
{
	return stream.data + offset;
}