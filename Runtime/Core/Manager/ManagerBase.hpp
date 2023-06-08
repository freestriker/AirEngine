#pragma once

#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <string>
#include <vector>
#include "../../Utility/Initializer.hpp"
#include <iostream>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class MainManager;
				class AIR_ENGINE_API ManagerBase
				{
					friend class MainManager;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase() = default;
					NO_COPY_MOVE(ManagerBase)

					virtual Utility::InitializerWrapper OnGetInitializer()
					{
						return {
							0, 0,
							[this]()->void {
								std::cout << _name << "'s initializer.\n";
							}
						};
					}
					virtual std::vector<Utility::InitializerWrapper> OnGetInternalInitializers()
					{
						return {
							{
								0, 0,
								[this]()->void {
									std::cout << _name << "'s internal initializer.\n";
								}
							}
						};
					}
					virtual std::vector<Utility::InitializerWrapper> OnGetInternalFinalizers()
					{
						return {
							{
								0, 0,
								[this]()->void {
									std::cout << _name << "'s internal finalizer.\n";
								}
							}
						};
					}
					virtual Utility::InitializerWrapper OnGetFinalizer()
					{
						return {
							0, 0,
							[this]()->void {
								std::cout << _name << "'s finalizer.\n";
							}
						};
					}
				public:
					const std::string& Name() const
					{
						return _name;
					}
				};
			}
		}
	}
}

