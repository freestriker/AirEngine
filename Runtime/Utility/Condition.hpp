#pragma once
#include "ExportMacro.hpp"
#include "ContructorMacro.hpp"

namespace AirEngine
{
    namespace Runtime
    {
        namespace Utility
        {
            template<class TMutex, class TConditionVariable>
            class AIR_ENGINE_API Condition final
            {
			private:
				TMutex _mutex;
				bool _ready;
				TConditionVariable _conditionVariable;
			public:
				Condition();
				~Condition() = default;
				NO_COPY_MOVE(Condition)
				void Wait();
				void Awake();
				void Reset();
				bool IsReady();
			};
			template<class TMutex, class TConditionVariable>
			inline Condition<TMutex, TConditionVariable>::Condition()
				: _mutex()
				, _ready(false)
				, _conditionVariable()
			{
			}
			template<class TMutex, class TConditionVariable>
			inline void Condition<TMutex, TConditionVariable>::Wait()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_conditionVariable.wait(lock, [this] { return this->_ready; });
			}
			template<class TMutex, class TConditionVariable>
			inline void Condition<TMutex, TConditionVariable>::Awake()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_ready = true;
				_conditionVariable.notify_all();
			}
			template<class TMutex, class TConditionVariable>
			inline void Condition<TMutex, TConditionVariable>::Reset()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_ready = false;
			}
			template<class TMutex, class TConditionVariable>
			inline bool Condition<TMutex, TConditionVariable>::IsReady()
			{
				return _ready;
			}
		}
    }
}
