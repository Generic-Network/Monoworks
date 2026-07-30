#include <mwpch.hh>

#include <rhi/specific/vulkan/VulkanRenderManager.hh>
#include <rhi/specific/vulkan/VulkanContext.hh>
#include <rhi/specific/vulkan/VulkanPresenter.hh>
#include <rhi/specific/vulkan/VulkanTexture.hh>

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

			*imageIndex = presenter->Acquire( &acquisitionInfo );
		}


        CVulkanRenderManager::BeginRootCommandBuffer( frameIndex );
        CVulkanRenderManager::BeginWorkerCommandBuffers( frameIndex );

    };

    void CVulkanRenderer::EndRendering() NOEXCEPT
    {
        MW_PROFILE_FUNC;
        u32 frameIndex = CStaticRenderer::GetCurrentFrameIndex();
        auto presenter = CVulkanContext::GetPresenter();

        CVulkanRenderManager::EndWorkerCommandBuffers( frameIndex );

        if ( CApplication::GetCreateInfos()->UseSDL && CApplication::GetCreateInfos()->UseSwapchain )
        {
            const auto imageIndex = CStaticRenderer::GetCurrentImageIndex();
            auto& swapchainImages = presenter->GetSwapchainImages();
            MW_ASSERT( imageIndex < swapchainImages.size(), "Invalid swapchain image index" );

            auto& swapchainImage = swapchainImages[imageIndex];
            if ( swapchainImage->Layout != MW_IMAGE_LAYOUT_PRESENT_SRC_KHR )
            {
                auto sourceStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                if ( swapchainImage->Layout == MW_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
                {
                    sourceStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                }
                else if ( swapchainImage->Layout != MW_IMAGE_LAYOUT_UNDEFINED )
                {
                    sourceStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                }

                auto vulkanTexture = swapchainImage.As<CVulkanTexture2D>();
                TransitionImageLayout2(
                    *CVulkanRenderManager::GetRootCommandBuffer( frameIndex ),
                    *vulkanTexture->GetImage(),
                    ( VkImageLayout )swapchainImage->Layout,
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    sourceStageMask,
                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT );

                swapchainImage->Layout = MW_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                swapchainImage->PipelineFlags = MW_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            }
        }

        CVulkanRenderManager::EndWorkerCommandBuffers( frameIndex );
        CVulkanRenderManager::EndRootCommandBuffer( frameIndex );

        CVulkanRenderManager::SubmitRootCommandBuffer( frameIndex );

        if ( CApplication::GetCreateInfos()->UseSDL && CApplication::GetCreateInfos()->UseSwapchain )
        {
            CVulkanContext::GetUploader()->Begin();
            SVulkanSDLPresentationTransitionPresentInfo renderInfo{};
            renderInfo.pCmdBuffer = CVulkanContext::GetUploader()->GetCommandBuffer();
            renderInfo.ImageIndex = CStaticRenderer::GetCurrentImageIndex();

            presenter->TransitionPresent( &renderInfo );
            CVulkanContext::GetUploader()->End();

            SVulkanSDLPresentationPresentInfo presentInfo{};
            presentInfo.pDevice = CVulkanContext::GetDevice()->GetDevice();
            presentInfo.pPhysDevice = CVulkanContext::GetDevice()->GetPhysicalDevice();
            presentInfo.pImageIndex = CStaticRenderer::GetCurrentImageIndexPtr();
            presentInfo.pPresentQueue = CVulkanContext::GetDevice()->GetPresentQueue();
            presentInfo.pRenderFinishedSemaphore = CVulkanRenderManager::GetRenderFinishedSemaphore( frameIndex );
            presentInfo.pVulkanDevice = CVulkanContext::GetDevice();

            presenter->Present( &presentInfo );
        }
        
    };
}