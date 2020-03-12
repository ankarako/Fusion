#ifndef	__COMMON_PUBLIC_READFILEBYTES_H__
#define __COMMON_PUBLIC_READFILEBYTES_H__

#include <fstream>
#include <string>
#include <vector>

namespace fu {
///	\brief load a file and get its content
///	\param	filepath	the path to the file to read
///	\return a vector with the file's contents in binary format
static std::vector<unsigned char> ReadFileBytes(const std::string& filepath)
{
	std::ifstream fd(filepath, std::ios::binary);
	std::vector<unsigned char> byte_buffer;

	if (fd.is_open())
	{
		fd.seekg(0, std::ios::end);
		size_t bytesize = fd.tellg();
		fd.seekg(0, std::ios::beg);
		byte_buffer.resize(bytesize);
		if (fd.read((unsigned char*)byte_buffer.data(), bytesize))
			return byte_buffer;
	}
	throw std::runtime_error("Failed read file. Could not read binary file stream: " + filepath);
}
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_READFILEBYTES_H__