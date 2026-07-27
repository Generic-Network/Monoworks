#pragma once
#include <common/Base.hh>

#include <rhi/agnostic/VertexBuffer.hh>
#include <rhi/agnostic/Pipeline.hh>

#include <volk/volk.h>

namespace Monoworks::RHI 
{
	struct SVulkanPipelineCreationInfo
	{
		std::vector<SShaderObject> ShaderObjects;
		std::vector<EImageFormat> ColorFormats;
		EImageFormat DepthAttachmentFormat;
		EImageFormat StencilAttachmentFormat;
		u32 ViewportCount = 1;
		u32 ScissorCount = 1; 
		EPipelineFlags Flags;

		ECullMode CullMode;
		EPrimitiveTopology Topology;

		
	};

	class CVulkanGraphicsPipeline
	{
	public:
		CVulkanGraphicsPipeline( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT;
		~CVulkanGraphicsPipeline() NOEXCEPT;

		void Init( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT;
		void Shutdown();

		void Invalidate( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT;
	
	private:
		
		CVertexLayout m_VertexLayout;
		VkPipelineLayout m_VulkanPipelineLayout;
		VkPipeline m_VulkanPipeline;
	};
}
