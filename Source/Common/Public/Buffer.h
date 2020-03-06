#ifndef	__COMMON_PUBLIC_BUFFER_H__
#define __COMMON_PUBLIC_BUFFER_H__

#include <NoCopyAssign.h>
#include <FuAssert.h>
#include <Device.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <vector_types.h>
#include <memory>
#include <vector>

namespace fu {
///	\class Buffer
///	\brief an allocated memory range on a device
///	\tparam	the device this buffer has stored data
template <typename ElementType, Device Dev>
class BufferObj { };
///	\typedef Buffer
///	\brief a reference counted BufferObj
///	Please use this instead of directly BufferObjs
template <typename ElementType, Device Dev>
using Buffer = std::shared_ptr<BufferObj<ElementType, Dev>>;
///	\class Buffer
///	\brief CPU specialization
template <typename ElementType>
class BufferObj<ElementType, Device::CPU>
{
private:
	/// Construction and creation
	///	buffers can only be created via factory method
	struct Constructor { };
public:
	NoCopyAssignMove(BufferObj);
	///	\typedef element_t	
	///	\brief the buffer's element type
	using element_t = typename ElementType;
	///	\typedef device
	///	\brief the buffer's debive
	static constexpr Device dev = Device::CPU;
	///	Construction
	///	\brief construction only through factory method
	BufferObj(Constructor, size_t elementCount)
		: m_Data(std::make_shared<buffer_t>(elementCount)), m_Count(elementCount)
	{ }
	///	\brief create a buffer from the specified element count
	///	\param	elementCount	the number of elements that this buffer will contain
	///	\return a reference counted buffer object
	static Buffer<ElementType, Device::CPU> Create(size_t elementCount)
	{
		return std::make_shared<BufferObj<ElementType, Device::CPU>>(Constructor{}, elementCount);
	}
	///	\brief get the number of elements in the buffer
	///	\return the number of elements in the buffer
	size_t Count() const
	{
		return m_Count;
	}
	///	\brief get the byte size of the buffer
	///	\return the size of the buffer in bytes
	size_t ByteSize() const
	{
		return m_Count * sizeof(ElementType);
	}
	///	\brief get the  stride of the buffer
	///	\return the buffer's stride in bytes (i.e. the byte size of the element type)
	size_t Stride() const
	{
		return sizeof(ElementType);
	}
	///	\brief get the start address of the buffer
	///	\return the buffer's data start address
	const ElementType* Data() const
	{
		return m_Data->data();
	}
	///	\brief get the start address of the buffer
	///	\return the buffer's data start address
	ElementType* Data()
	{
		return m_Data->data();
	}
private:
	///	\typedef buffer_t
	///	\brief the underlying data buffer type,. which is just an std::vector
	using buffer_t = std::vector<ElementType>;
	///	\typedef data_t
	///	\brief the raw data type (not) :P. a shared pointer to the buffer vector
	///	so we don't have to implement custom deleters
	using data_t = std::shared_ptr<buffer_t>;
	///	the buffer's data
	data_t 	m_Data;
	///	the buffer's count
	size_t 	m_Count;
};	///	!class BufferObj
///	\typedef BufferCPU 
///	\brief a cpu buffer alias
///	\tparam	TlementType	the type of the buffer's elements
template <typename ElementType>
using BufferCPU = Buffer<ElementType, Device::CPU>;
///	\brief create a cpu buffer
///	\param	count	the element count of the buffer to create
template <typename ElementType>
static BufferCPU<ElementType> CreateBufferCPU(size_t count)
{
	using buf_obj_t = BufferObj<ElementType, Device::CPU>;
	return buf_obj_t::Create(count);
}
///	\class BufferObj
///	\brief GPU specialization
template <typename ElementType>
class BufferObj<ElementType, Device::GPU>
{
private:
	/// construction available only through factory methods
	struct Constructor { };
public:
	NoCopyAssignMove(BufferObj);
	/// Construction
	///	\param	Constructor	to deny construction
	///	\param	count	the element count of the buffer
	BufferObj(Constructor, size_t count)
		: m_Count(count)
	{
		cudaError_t res = cudaMalloc((void**)&m_Data, count * sizeof(ElementType));
		DebugAssertMsg(res == CUDA_SUCCESS, "Failed to allocate Buffer on the GPU.");
	}
	///	Destruction
	///	\brief Deallocates GPU memory
	~BufferObj()
	{
		cudaError_t res = cudaFree((void*)m_Data);
		DebugAssertMsg(res == CUDA_SUCCESS, "Failed to deallocate Buffer on the GPU.");
	}
	///	\brief create a buffer
	///	\param	count the element count of the buffer that will be created
	///	\return a gpu buffer with the specified element count
	static Buffer<ElementType, Device::GPU> Create(size_t count)
	{
		return std::make_shared<BufferObj<ElementType, Device::GPU>>(Constructor{}, count);
	}
	///	Access & Modification
	/// Access
	///	\brief get the buffer's element count
	///	\return the buffer's element count
	size_t Count() const
	{
		return m_Count;
	}
	///	\brief get the buffer's byte size
	///	\return the buffer's byte size
	size_t ByteSize() const
	{
		return m_Count * sizeof(ElementType);
	}
	///	\brief get the stride of the buffer
	///	\return the byte size of the buffer's element type
	size_t Stride() const
	{
		return sizeof(ElementType);
	}
	///	\brief get the buffer's data start address
	///	\return a pointer to the buffer's data
	ElementType* Data()
	{
		return m_Data;
	}
	///	\brief get the buffer's data start address
	///	\return a pointer to the buffer's data
	const ElementType* Data() const
	{
		return m_Data;
	}
private:
	ElementType*	m_Data;
	size_t			m_Count;
};	///	!class BufferObj
///	\typedef BufferGPU
///	\brief a buffer that holds gpu memory
template <typename ElementType>
using BufferGPU = Buffer<ElementType, Device::GPU>;
///	\brief create a gpu buffer
///	\param count	the element count of the buffer
template <typename ElementType>
BufferGPU<ElementType> CreateBufferGPU(size_t count)
{
	using buf_obj_t = BufferObj<ElementType, Device::GPU>;
	return buf_obj_t::Create(count);
}
///=============
///	Buffer Copy
///=============
/// Copy CPU -> CPU
///	\brief copy a source cpu buffer to another destination cpu buffer
///	\param	dest	the destination buffer
///	\param	src		tje spirce buffer
///	\return true if the copy was successful, false otherwise
template <typename ElementType>
static void BufferCopy(BufferCPU<ElementType>& dest, const BufferCPU<ElementType>& src)
{
	/// check that the buffers have the same size
	size_t srcsz = src->ByteSize();
	size_t destsz = dest->ByteSize();
	DebugAssertMsg(destsz == srcsz, "Failed to copy buffers. The buffers do not have the same size");
	memcpy(dest->Data(), src->Data(), srcsz);
}
/// Copy CPU -> GPU
///	\brief copy a source cpu buffer to a destination gpu buffer
///	\param	src		the source cpu bufer to copy
///	\param	dest	the destination gpu buffer
///	\return true if the copy was successful, false otherwise
template <typename ElementType>
static void BufferCopy(BufferGPU<ElementType>& dest, const BufferCPU<ElementType>& src)
{
	/// check that the buffers have the same size
	size_t srcsz = src->ByteSize();
	size_t destsz = dest->ByteSize();
	DebugAssertMsg(destsz == srcsz, "Failed to copy buffers. The buffers do not have the same size");
	cudaError_t res = cudaMemcpy((void*)dest->Data(), (void*)src->Data(), srcsz, cudaMemcpyHostToDevice);
	DebugAssert(res == CUDA_SUCCESS);
}
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_BUFFER_H__