#ifndef _ALLE5_FILE_STREAM_H
#define _ALLE5_FILE_STREAM_H

#include "defs.h"

struct FileStream
{
	u8* data;
	u32 length;
	u32 capacity;
};

// Load an empty file stream (this MUST be released with "ReleaseFileStream")
bool LoadEmptyFileStream(FileStream& stream);
// Load a file into a file stream (this MUST be released with "ReleaseFileStream")
bool LoadFileStream(FileStream& stream, const std::string& path);
// Save a file stream to a file
bool SaveFileStream(const FileStream& stream, const std::string& path);
// Free file data of a file stream
void ReleaseFileStream(FileStream& stream);

// Get the data of a file stream in a given offset
template<class T>
T FileStreamRead(const FileStream& stream, u32 offset)
{
	return *((T*)&stream.data[offset]);
}

// Get the data of a file stream in a given offset and update the offset to the end of the data
template<class T>
T FileStreamReadUpdate(const FileStream& stream, u32& offset)
{
	T output = FileStreamRead<T>(stream, offset);
	offset += sizeof(T);
	
	return output;
}

// Copy the data from the file stream to a buffer
void FileStreamBufferRead(const FileStream& stream, u32 offset, u8* buffer, u32 length);

// Copy the data from the file stream to a buffer and update the offset to the end of the data
void FileStreamBufferReadUpdate(const FileStream& stream, u32& offset, u8* buffer, u32 length);

// Duplicate the size of the file stream
void FileStreamExpand(FileStream& stream);

// Put data at the back of the file stream
template<class T>
void FileStreamPutBack(FileStream& stream, const T& value)
{
	while (stream.length + sizeof(T) >= stream.capacity)
		FileStreamExpand(stream);

	u8* valuePtr = (u8*)&value;
	for (u32 i = 0; i < sizeof(T); ++i)
		stream.data[stream.length + i] = valuePtr[i];

	stream.length += sizeof(T);
}

// Write a buffer of data at the back of the file stream
void FileStreamBufferWriteBack(FileStream& stream, const u8* buffer, u32 length);

// Replace a set of bytes from the file stream at the offset
template<class T>
bool FileStreamReplace(FileStream& stream, const u32 offset, const T& value)
{
	if (offset + sizeof(T) >= stream.capacity)
		return 0;

	u8* valuePtr = (u8*)&value;
	for (u32 i = 0; i < sizeof(T); ++i)
		stream.data[offset + i] = valuePtr[i];

	return 1;
}

// Get a pointer to an offset of the data stored in the file stream
u8* FileStreamGetDataPtr(const FileStream& stream, u32 offset);

#endif // _ALLE5_FILE_STREAM_H
