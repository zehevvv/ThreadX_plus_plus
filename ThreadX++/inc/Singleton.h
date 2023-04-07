#pragma once

#include "Types.h"

template <typename T> class Singleton
{
public:
	/// @brief Return instance of the singleton class, if it not allocated yet, create it.
	static T* Instance()
	{
		if (m_instance == NULL)
			m_instance = new T();

		return m_instance;
	}

protected:
	/// C'tor.
	Singleton()
	{
	}

	static T* m_instance;
};

template <typename T>
T* Singleton<T>::m_instance = NULL;
