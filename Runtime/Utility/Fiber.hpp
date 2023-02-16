#pragma once
#include "boost/fiber/all.hpp"

namespace AirEngine
{
    namespace Runtime
    {
        namespace Utility
        {
            namespace Fiber = boost::fibers;
            namespace ThisFiber = boost::this_fiber;
        }
    }
}
