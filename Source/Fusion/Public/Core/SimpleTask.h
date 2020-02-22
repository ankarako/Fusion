#ifndef __FUSION_PUBLIC_CORE_SIMPLETASK_H__
#define __FUSION_PUBLIC_CORE_SIMPLETASK_H__

#include <taskflow/taskflow.hpp>

namespace fu {
namespace fusion {
struct SimpleTask
{
	tf::Taskflow	Flow;
	tf::Task		Task{ Flow.placeholder() };
};
}	///	!namespace
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_SIMPLETASK_H__