#ifndef __APPLICATION_PUBLIC_ACTIVATABLE_H__
#define __APPLICATION_PUBLIC_ACTIVATABLE_H__

#include <rxcpp/rx.hpp>

namespace app {
///	\class Activatable
///	\brief simple Activate/deactivate interface
class Activatable
{
public:
	using unit_t = void*;

	bool IsActive() const
	{
		return m_Active;
	}

	virtual void Activate()
	{
		if (!m_Active)
		{
			m_Active = true;
			m_OnActivatedSubject.get_subscriber().on_next(nullptr);
		}
	}

	virtual void Deactivate()
	{
		if (m_Active)
		{
			m_Active = false;
			m_OnDeactivatedSubject.get_subscriber().on_next(nullptr);
		}
	}

	rxcpp::observable<unit_t> OnActivated()
	{
		return m_OnActivatedSubject.get_observable().as_dynamic();
	}
	rxcpp::observable<unit_t> OnDeactivated()
	{
		return m_OnDeactivatedSubject.get_observable().as_dynamic();
	}

	virtual ~Activatable() = default;
private:
	rxcpp::subjects::subject<unit_t> m_OnActivatedSubject;
	rxcpp::subjects::subject<unit_t> m_OnDeactivatedSubject;
	bool	m_Active{ false };
};	///	!class Activatable
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_ACTIVATABLE_H__