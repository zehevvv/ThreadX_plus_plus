#pragma once

#include "Types.h"

template <typename T> class Singleton{
public:
	static T* Instance();

protected:
	Singleton();
	static T* m_instance;
};

template <typename T>
Singleton<T>::Singleton()
{
}

template <typename T>
T* Singleton<T>::Instance()
{
	if (m_instance == nullptr)
		m_instance = new T();

	return m_instance;
}

template <typename T>
T* Singleton<T>::m_instance = NULL;
