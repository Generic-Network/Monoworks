#pragma once
#include <common/Base.hh>

#include <rhi/agnostic/VertexBuffer.hh>
#include <rhi/agnostic/GraphicsPipeline.hh>

#include <volk/volk.h>

namespace Monoworks::RHI 
{

	class CVulkanGraphicsPipeline : public IGraphicsPipeline
	{
	public:
		CVulkanGraphicsPipeline( const SPipelineCreationInfo* pInfo ) NOEXCEPT;
		~CVulkanGraphicsPipeline() NOEXCEPT;

		void Init( const SPipelineCreationInfo* pInfo ) NOEXCEPT override;
		void Shutdown() override;

		void Invalidate( const SPipelineCreationInfo* pInfo ) NOEXCEPT override;
	
	private:
		std::vector<VkPipelineColorBlendAttachmentState> m_ColorAttachmentStates; 
		std::vector<VkDynamicState> m_DynamicStates;

		CVertexLayout m_VertexLayout;
		VkPipelineLayout m_VulkanPipelineLayout;
		VkPipeline m_VulkanPipeline;
	};
}
