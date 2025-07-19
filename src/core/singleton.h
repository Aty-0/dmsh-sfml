#pragma once

namespace dmsh::core
{
	template <typename T>
	class Singleton
	{
	public:
		Singleton() { }
		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		~Singleton() { }

		Singleton& operator=(const Singleton&) = delete;
		Singleton& operator=(Singleton&&) = delete;

		inline static T* getInstance()
		{
			static auto instance = T();
			return &instance;
		}

	};
}