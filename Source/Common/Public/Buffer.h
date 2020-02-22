#ifndef	__COMMON_PUBLIC_BUFFER_H__
#define __COMMON_PUBLIC_BUFFER_H__

#include <NoCopyAssign.h>
#include <Dims.h>
#include <memory>
#include <vector>

namespace fu {
///	\enum BufferStorageProc
///	\brief the device memory that the buffer lives
enum class BufferStorageDevice
{
	CPU,
	GPU,
	Invalid
};
///	\class BufferObject
///	\brief a buffer of memory that lives on a device
///	\warning: please do not use these objects. use factory methods instead
///	\tparam	ElmtType	the buffer's data type
///	\tparam	BufferStorageProc	the device that the buffer's data live on
template <typename DataType, BufferStorageDevice Proc>
class BufferObject { };
///	\typedef Buffer
///	\brief a reference counted buffer object
///	\note:	please use this object instead of the actual BufferObjects;
template <typename DataType, BufferStorageDevice Proc>
using Buffer = std::shared_ptr<BufferObject<DataType, Proc>>;
///	\brief factory method for creating buffers
///	\class Buffer
///	\brief CPU specializtion
template <typename DataType>
class BufferObject<DataType, BufferStorageDevice::CPU>
{
private:
	NoCopyAssignMove(BufferObject);
	///	make construction no possible
	struct ctor { };
	///	\typedef DataBuffer
	using DataStorage = std::vector<DataType>;
	///	\typedef Data
	///	\brief the pointer type of the data
	using DataPtr = std::shared_ptr<DataStorage>;
public:
	static constexpr BufferStorageDevice Device = BufferStorageDevice::CPU;
	///	Construction
	///	\brief default constructor
	///	initializes data
	BufferObject()
		: m_Data(nullptr), m_ByteSize(0)
	{ }
	///	\brief construction from dimensions
	///	\note:	normal construction only possible from factory methods
	BufferObject(ctor, Dims inDims)
		: m_Data(std::make_shared<std::vector<DataType>>(inDims.Width * inDims.Height * inDims.Depth))
		, m_Dims(inDims)
	{ }
	///	\brief create a data buffer from specified dimensions
	static Buffer<DataType, Device> Create(Dims dims)
	{
		return std::make_shared<BufferObject<DataType, Device>>(ctor{}, dims);
	}
	///	Access
	///	\brief get the number of elements in the buffer
	///	\return the total number of elements in the buffer
	size_t Count() const
	{
		return m_Dims.Width * m_Dims.Height * m_Dims.Depth;
	}
	///	\brief get the dimensions of the buffer
	///	\return the dimensions of the buffer
	Dims Dimensions() const
	{
		return m_Dims;
	}
	///	\brief get the byte size of the buffer
	///	\return the byte size of the buffer
	size_t ByteSize() const
	{
		return m_Dims.Width * m_Dims.Height * m_Dims.Depth * sizeof(DataType);
	}
	///	\brief get a pointer to the start address of the data
	///	\return a pointer to the data
	const DataType* Data() const
	{
		return m_Data->data();
	}
	/// Modification
	///	\brief get a pointer to the start address of the data
	///	\return a pointer to the data
	DataType* const Data()
	{
		return m_Data->data();
	}
private:
	DataPtr	m_Data;
	Dims	m_Dims;	
};	///	!class Buffer
///	\brief factory method
///	create buffer objects using this method
template <typename DataType, BufferStorageDevice Proc>
static Buffer<DataType, Proc> CreateBuffer(Dims dims) 
{ 
	using bufobj = BufferObject<DataType, Proc>;
	return bufobj::Create(dims);
}
///	\brief buffer factory cpu specialization
///	Use this to create buffers, the owner of the buffer is the one who used this
template <typename DataType>
static Buffer<DataType, BufferStorageDevice::CPU> CreateBuffer(Dims dims)
{
	using bufobj = BufferObject<DataType, BufferStorageDevice::CPU>;
	return bufobj::Create(dims);
}
}	///	!namespace fu
#endif	///	__COMMON_PUBLIC_BUFFER_H__