#ifndef	__IO_PUBLIC_PERFCAP_BUFFERREF_H__
#define __IO_PUBLIC_PERFCAP_BUFFERREF_H__

#include <stddef.h>
#include <cstdlib>
#include <Perfcap/Buffer.h>

namespace fu {
namespace io {
namespace utils {

template<class T> 
class BufferRef
{
private:
	const T* m_Buffer;
	int m_Size;	

public:

	BufferRef() : m_Size(0), m_Buffer(NULL)
	{

	}

	BufferRef(const Buffer<T>& buf)
	{
		set(buf);
	}

	BufferRef(const T* buffer, int size)
	{
		set(buffer,size);
	}

	void set(const T* buffer, int size)
	{
		m_Buffer = buffer;
		m_Size = size;
	}

	T& operator[](int index)
	{
		return m_Buffer[index];
	}

	const T& operator[](int index) const
	{
		return m_Buffer[index];
	}

	void set(const Buffer<T>& buf)
	{
		m_Buffer = buf.getBuffer();
		m_Size = buf.getSize();
	}

	/*T* getBuffer()
	{
		return m_Buffer;
	}*/

	const T* getBuffer() const
	{
		return m_Buffer;
	}

	int getSize() const {
		return m_Size;
	}

	int getSizeInBytes() const {
		return m_Size * sizeof(T);
	}

	template <class f>
	void serialize(f& buf) const {
		buf << m_Size;
		for (int i = 0; i < m_Size; i++)
			buf << m_Buffer[i];
	}
};
}	///	!namespace utils
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_BUFFERREF_H__