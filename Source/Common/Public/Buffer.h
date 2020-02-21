#ifndef	__COMMON_PUBLIC_BUFFER_H__
#define __COMMON_PUBLIC_BUFFER_H__

#include <vector>

namespace fu {
///	\enum BufferStorageProc
///	\brief the device memory that the buffer lives
enum class BufferStorageProc
{
	CPU,
	GPU,
};
///	\struct BufferStorage
///	\brief a simple memory range on a device specified by start address and byte size
struct BufferStorage
{
	///	allocation
	template <BufferStorageProc Proc>
	void Allocate(size_t byteSize) { }

	template <>
	void Allocate<BufferStorageProc::CPU>(size_t sz)
	{
		Data = malloc(sz);
		if (Data != nullptr)
			Size = sz;
	}

	template <>
	void Allocate<BufferStorageProc::GPU>(size_t sz)
	{

	}
	///	deallocation
	template <BufferStorageProc Proc>
	void Deallocate() { }

	template <>
	void Deallocate<BufferStorageProc::CPU>()
	{
		if (Data != nullptr)
		{
			free(Data);
			Size = 0;
		}
	}

	template <>
	void Deallocate<BufferStorageProc::GPU>()
	{

	}
	void* 	Data = nullptr;
	size_t 	Size = 0;
};
///	\struct Dims
///	\brief the dimensions of the buffer
struct Dims
{
	int Width{ 0 };
	int Height{ 0 };
	int Depth{ 0 };
	/// Construction
	Dims(int w, int h, int d)
		: Width(w), Height(h), Depth(d)
	{ }
};
///	\class Buffer
///	\brief a buffer of memory that lives on a device
///	\tparam	ElmtType	the buffer's data type
///	\tparam	BufferStorageProc	the device that the buffer's data live on
template <typename ElmtType, BufferStorageProc Proc>
class Buffer
{
private:
	/// for disallowing public construction
	struct Constructor { };
public:
	/// Construction
	///	\brief default constructor
	///	does nothind
	Buffer() = default;
	///	\brief disallow public construction
	///	\note construction only through create method
	Buffer(Constructor, Dims dims) 
		: m_Dims(dims)
	{ 
		size_t bytes = dims.Width * dims.Height * dims.Depth * sizeof(ElmtType);
		m_Storage.Allocate<Proc>(bytes);
	}
	///	destruction
	~Buffer()
	{
		//m_Storage.Deallocate<Proc>();
	}
	///	\brief create a buffer from given dimensions
	///	\param	dims	the dimensions of the buffer
	///	\return a new buffer of the given dimensions
	static Buffer Create(Dims dims)
	{
		Buffer buf(Constructor{}, dims);
		return buf;
	}
	///	\brief create a buffer from specified dimensions and data
	///	\param	dims	the buffer's dimensions
	///	\param	data	the data to copy into the buffer
	static Buffer Create(Dims dims, ElmtType const * const Data)
	{
		
		
	}
	///	\brief check if the buffer is valid
	///	(i.e. for nullptr)
	///	\return true if the buffer is allocated
	bool IsValid() const
	{
		return m_Storage.Data != nullptr;
	}
	///	\brief check if the buffer is emp
	///	\return true if the buffer is empty, false otherwise
	bool Empty() const
	{
		return m_Storage.Size != 0;
	}
	///	\brief get the byte size of the buffer
	///	\return the byte size of the buffer
	size_t GetByteSize() const 
	{ 
		return m_Storage.Size; 
	}
	///	\brief get the dimensions of the buffer
	///	\return the dimensions of the buffer
	Dims GetDims() const 
	{ 
		return m_Dims; 
	}
	///	\brief get a pointer at the start address of the data
	///	\return the memory address of the first element in the data
	const ElmtType* Data() const
	{
		return (ElmtType*)m_Storage.Data;
	}
	///	\brief get a pointer at the start address of the data
	///	\return the memory address of the first element in the data
	ElmtType* Data()
	{
		return (ElmtType*)m_Storage.Data;
	}
	///	\brief upload the buffer's data to another device buffer
	///	\param	outBuffer the buffer to copy this buffer's data
	template <BufferStorageProc OtherProc>
	void Upload(Buffer<ElmtType, OtherProc>& outBuffer) 
	{ 
	
	}
	///	\brief download the buffer's data from another device's buffer
	///	\param	outBuffer the buffer to copy this buffer's data
	template <BufferStorageProc OtherProc>
	void Download(Buffer<ElmtType, OtherProc>& otherStorage) 
	{

	}
private:
	BufferStorage m_Storage;
	Dims m_Dims;
};
}	///	!namespace fu
#endif	///	__COMMON_PUBLIC_BUFFER_H__