#include <mwpch.hh>

#include <renderer/StaticRenderer.hh>
#include "VulkanRenderManager.hh"


namespace Monoworks::RHI 
{
	SVulkanFrameData			   CVulkanRenderManager::m_RootFrameData[MFIF];
	std::vector<SVulkanWorkerData> CVulkanRenderManager::m_VulkanWorkerRenderData;

	void CVulkanRenderManager::Init() NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};

	void CVulkanRenderManager::Shutdown() NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};

	void CVulkanRenderManager::BeginRootCommandBuffer( u32 frameIndex ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};

	void CVulkanRenderManager::EndRootCommandBuffer( u32 frameIndex ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};

	void CVulkanRenderManager::BeginWorkerCommandBuffers( u32 frameIndex ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};

	void CVulkanRenderManager::EndWorkerCommandBuffers( u32 frameIndex ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

	};


}
