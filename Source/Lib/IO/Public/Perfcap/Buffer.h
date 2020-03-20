#ifndef	__IO_PUBLIC_PERFCAP_BUFFER_H__
#define __IO_PUBLIC_PERFCAP_BUFFER_H__

#include <stddef.h>
#include <cstdlib>

namespace fu {
namespace io {
namespace utils {
	
template<class T> 
class Buffer
{
private:
	T* m_Buffer;
	int m_Size;
	int m_AllocatedSize;

public:

	Buffer() : m_Size(0), m_AllocatedSize(0), m_Buffer(NULL)
	{

	}

	Buffer(const Buffer<T>& other) : m_Buffer(NULL), m_AllocatedSize(0), m_Size(0)
	{
		resize(other.getSize());
		memcpy(m_Buffer,other.getBuffer(),m_Size*sizeof(T));
	}

	Buffer(int size) : m_Buffer(NULL),m_Size(0),m_AllocatedSize(0)
	{
		resize(size);
	}

	Buffer<T>& operator=(const Buffer<T>& other)
	{
		if(other.m_Buffer != this->m_Buffer) {
			resize(other.getSize());
			memcpy(m_Buffer,other.getBuffer(),m_Size*sizeof(T));
		}
		return *this;
	}

	~Buffer()
	{
		free();
	}

	int getSize() const {
		return m_Size;
	}

	int getSizeInBytes() const {
		return m_Size * sizeof(T);
	}

	inline T* getBuffer()
	{
		return m_Buffer;
	}

	inline const T* getBuffer() const
	{
		return m_Buffer;
	}

	inline T& operator[](int pos)
	{
		return m_Buffer[pos];
	}

	inline const T& operator[](int pos) const
	{
		return m_Buffer[pos];
	}

	void extract(const Buffer<T>& inbuffer,int startIndex,int size)
	{
		if(startIndex+size <= inbuffer.getSize()) {
			resize(size);
			memcpy(m_Buffer,inbuffer.getBuffer()+startIndex,size*sizeof(T));
		}
	}

	void append(const T* buf, int size)
	{
		int oldsize = m_Size;
		resize(oldsize+size);
		memcpy(m_Buffer+oldsize,buf,size*sizeof(T));
	}

	void append(const T& value)
	{
		int oldsize = m_Size;
		resize(oldsize+1);
		memcpy(m_Buffer+oldsize,&value,sizeof(T));
	}

	void append(const Buffer<T>& buf)
	{
		int oldsize = m_Size;
		resize(oldsize+buf.getSize());
		memcpy(m_Buffer+oldsize,buf.getBuffer(),buf.getSizeInBytes());
	}

	void copyTo(T* outbuffer) const
	{
		memcpy(outbuffer,m_Buffer,getSizeInBytes());
	}

	void copyFrom(const T* inbuffer,int size)
	{
		if(size <= m_Size) {
			memcpy(m_Buffer,inbuffer,size*sizeof(T));
		}
	}

	void expand(int size)
	{
		resize(m_Size + size);
	}

	bool expandInBytes(int size)
	{
		return resizeInBytes(m_Size*sizeof(T)+size);
	}

	void reserve(int size)
	{
		if((m_Buffer == NULL) && (size>0)) {
			m_Buffer = (T*)malloc(size*sizeof(T));
			m_AllocatedSize = size;			
		}
		else if(size > m_AllocatedSize) {
			m_Buffer = (T*)realloc(m_Buffer,size*sizeof(T));
			m_AllocatedSize = size;
		}
	}

	void resize(int size)
	{
		if((m_Buffer == NULL) && (size>0)) {
			m_Buffer = (T*)malloc(size*sizeof(T));
			m_AllocatedSize = size;
			m_Size = size;
		}
		else if(size > m_AllocatedSize) {
			m_Buffer = (T*)realloc(m_Buffer,size*sizeof(T));
			m_AllocatedSize = size;
			m_Size = size;
		}
		else
			m_Size = size;
	}

	bool resizeInBytes(int size)
	{
		if(size % sizeof(T) == 0) {
			resize(size/sizeof(T));
			return true;
		}
		else
			return false;
	}

	void set(const T& value)
	{
		for(int i=0;i<m_Size;i++)
			m_Buffer[i] = value;
	}

	void set(const T* inbuffer,int size)
	{
		resize(size);
		if(size>0) {
			copyFrom(inbuffer,size);
		}
	}

	void free()
	{
		if(m_Buffer != NULL)
			::free(m_Buffer);
		m_Buffer = NULL;
		m_Size = 0;
		m_AllocatedSize = 0;
	}

	template <class f>
	void serialize(f& buf) const {
		buf << m_Size;
		for (int i = 0; i < m_Size; i++)
			buf << m_Buffer[i];
	}

	template <class f>
	void parse(f& buf) {
		int size;
		buf >> size;
		resize(size);
		for (int i = 0; i < size; i++)
			buf >> m_Buffer[i];
	}
};
}	///	!namepace utils
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_BUFFER_H__