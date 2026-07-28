#pragma once
#include <common/Base.hh>

namespace Monoworks::RHI 
{
    class IGraphicsAPI 
    {
    public:
        virtual ~IGraphicsAPI() = default;

        virtual void Init() NOEXCEPT = 0;
        virtual void Shutdown() NOEXCEPT = 0;
    };

}
