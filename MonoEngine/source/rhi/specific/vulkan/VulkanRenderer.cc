#include <common/Base.hh>

#include "VulkanRenderer.hh"

namespace Monoworks::RHI 
{
    void CVulkanRenderer::Init() NOEXCEPT
    {
        MW_PROFILE_FUNC;
        MW_INFO("Initialize CVulkanRenderer");
    } 

    void CVulkanRenderer::Shutdown() NOEXCEPT 
    {
        MW_PROFILE_FUNC;
        MW_INFO("Shutdown CVulkanRenderer");
    }
}