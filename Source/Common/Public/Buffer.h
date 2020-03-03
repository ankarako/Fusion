#ifndef	__COMMON_PUBLIC_BUFFER_H__
#define __COMMON_PUBLIC_BUFFER_H__

#include <Device.h>
#include <memory>

namespace fu {
///	\class Buffer
///	\brief an allocated memory range on a device
///	\tparam	the device this buffer has stored data
template <typename ElementType, Device Dev>
class BufferObj { };
///	\typedef Buffer
///	\brief a reference counted BufferObj
template <typename ElementType, Device Dev>
using Buffer = std::shared_ptr<BufferObj<ElementType, Dev>>;
namespace detail {
	///	\struct BufferObjDeleter
	///	\brief a deleter for buffer objects
	template <typename ElementType, Device Dev = Device::Unknown>
	struct BufferObjDeleter { };
	///	\struct BufferObjDeleter
	///	\brief CPU specialization
	template <typename ElementType>
	struct BufferObjDeleter<ElementType, Device::CPU>
	{
		void operator()(BufferObj<ElementType, Device::CPU>* ptr) const
		{
			delete[] ptr->m_Data;
		}
	};	///	!struct BufferObjDeleter
	///	\struct BufferObjDeleter
	///	\brief GPU specialization
	template <typename ElementType>
	struct BufferObjDeleter<ElementType, Device::GPU>
	{
		void operator()(ElementType* ptr) const
		{
			/// TODO:
		}
	};	///	!struct BufferObjDeleter
}	///	!namespace detail
///	\class Buffer
///	\brief CPU specialization
template <typename ElementType>
class BufferObj<ElementType, Device::CPU>
{
private:
	/// Construction and creation
	///	buffers can only be created via factory method
	struct Constructor { };
	///	friend deleter class
	friend detail::BufferObjDeleter<ElementType, Device::CPU>;
public:
	///	\typedef element_t	
	///	\brief the buffer's element type
	using element_t = typename ElementType;
	///	\typedef buffer_t
	///	\brief the type of this buffer
	using buffer_t = Buffer<ElementType, Device::CPU>;
	///	\typedef device
	///	\brief the buffer's debive
	static constexpr Device dev = Device::CPU;
	///	Construction
	///	\brief construction only through factory method
	BufferObj(Constructor, size_t elementCount)
		: m_Data(new ElementType[elementCount]), m_ElementCount(elementCount)
	{ }
	///	\brief construction only through factory method
	BufferObj(Constructor, ElementType* data, size_t elementCount)
		: m_Data(data), m_ElementCount(elementCount)
	{ }
	///	\brief create a buffer from the specified element count
	///	\param	elementCount	the number of elements that this buffer will contain
	///	\return a reference counted buffer object
	static buffer_t Create(size_t elementCount)
	{
		return std::make_shared<BufferObj<ElementType, Device::CPU>>(
			new BufferObj(Constructor{}, elementCount), 
			detail::BufferObjDeleter<ElementType, Device::CPU>()
		);
	}
	///	\brief create a buffer from the specified pointer and element count
	///	\param	data			the starting address of the data
	///	\param	elementCount	the number of elements in the data
	///	\return a reference counted buffer object
	static buffer_t Create(ElementType* data, size_t elementCount)
	{
		return std::make_shared<BufferObj<ElementType, Device::CPU>>(
			new BufferObj(Constructor{}, data, elementCount),
			detail::BufferObjDeleter<ElementType, Device::CPU>()
			);
	}
	///	\brief get the number of elements in the buffer
	///	\return the number of elements in the buffer
	size_t ElementCount() const
	{
		return m_ElementCount;
	}
	///	\brief get the byte size of the buffer
	///	\return the size of the buffer in bytes
	size_t ByteSize() const
	{
		return m_ElementCount * sizeof(ElementType);
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
		return m_Data;
	}
	///	\brief get the start address of the buffer
	///	\return the buffer's data start address
	ElementType* Data()
	{
		return m_Data;
	}
private:
	ElementType* 	m_Data;
	size_t 			m_ElementCount;
};	///	!class BufferObj
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_BUFFER_H__