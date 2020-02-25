#ifndef	__FUSIO_PUBLIC_CORE_GPUCONTEXT_H__
#define __FUSIO_PUBLIC_CORE_GPUCONTEXT_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <cuda.h>

namespace fu {
namespace fusion {
class GPUContext : public app::Initializable, public app::Destroyable
{
public:
	GPUContext();
	void Init() override;
	void Destroy() override;
	void Acquire();
	void Release();
private:
	/// Context
};	///	!class GPUContext
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSIO_PUBLIC_CORE_GPUCONTEXT_H__