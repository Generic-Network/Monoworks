#include <mwpch.hh>

#include <rhi/specific/vulkan/VulkanRenderManager.hh>
#include <rhi/specific/vulkan/VulkanContext.hh>
#include <rhi/specific/vulkan/VulkanPresenter.hh>

#include <renderer/StaticRenderer.hh>
#include <core/Application.hh>

#include "VulkanRenderer.hh"

namespace Monoworks::RHI 
{
    void CVulkanRenderer::Init() NOEXCEPT
    {
        MW_PROFILE_FUNC;
        MW_INFO("Initialize CVulkanRenderer");
        CVulkanRenderManager::Init();
    } 

    void CVulkanRenderer::Shutdown() NOEXCEPT 
    {
        MW_PROFILE_FUNC;
        CVulkanRenderManager::Shutdown();
        MW_INFO( "Shutdown CVulkanRenderer" );
    }

    void CVulkanRenderer::BeginRendering() NOEXCEPT
    {
        MW_PROFILE_FUNC;
        u32* imageIndex = CStaticRenderer::GetCurrentImageIndexPtr();
        u32 frameIndex =  CStaticRenderer::GetCurrentFrameIndex();

		auto presenter = CVulkanContext::GetPresenter();

		if ( CApplication::GetCreateInfos()->UseSDL && CApplication::GetCreateInfos()->UseSwapchain )
		{
			SVulkanSDLPresentationAcquisitionInfo acquisitionInfo{};
            acquisitionInfo.pDevice = CVulkanContext::GetDevice()->GetDevice();
            acquisitionInfo.pPhysDevice = CVulkanContext::GetDevice()->GetPhysicalDevice();
            acquisitionInfo.pVulkanDevice = CVulkanContext::GetDevice();
            acquisitionInfo.pImageAvailableSemaphore = CVulkanRenderManager::GetImageAvailableSemaphore( frameIndex );
            acquisitionInfo.pInFlightFence = CVulkanRenderManager::GetInFlightFence( frameIndex );

			presenter->Acquire( &acquisitionInfo );
		}


        CVulkanRenderManager::BeginRootCommandBuffer( frameIndex );
        CVulkanRenderManager::BeginWorkerCommandBuffers( frameIndex );

    };

    void CVulkanRenderer::EndRendering() NOEXCEPT
    {
        MW_PROFILE_FUNC;
        u32 frameIndex = CStaticRenderer::GetCurrentFrameIndex();

        CVulkanRenderManager::EndWorkerCommandBuffers( frameIndex );
        CVulkanRenderManager::EndRootCommandBuffer( frameIndex );

        CVulkanRenderManager::SubmitRootCommandBuffer( frameIndex );

        auto presenter = CVulkanContext::GetPresenter();

        if ( CApplication::GetCreateInfos()->UseSDL && CApplication::GetCreateInfos()->UseSwapchain )
        {
            SVulkanSDLPresentationPresentInfo presentInfo{};
            presentInfo.pDevice = CVulkanContext::GetDevice()->GetDevice();
            presentInfo.pPhysDevice = CVulkanContext::GetDevice()->GetPhysicalDevice();
            presentInfo.pImageIndex = CStaticRenderer::GetCurrentFrameIndexPtr();
            presentInfo.pPresentQueue = CVulkanContext::GetDevice()->GetPresentQueue();
            presentInfo.pRenderFinishedSemaphore = CVulkanRenderManager::GetRenderFinishedSemaphore( frameIndex );
            presentInfo.pVulkanDevice = CVulkanContext::GetDevice();

            presenter->Present( &presentInfo );
        }
        
    };
}