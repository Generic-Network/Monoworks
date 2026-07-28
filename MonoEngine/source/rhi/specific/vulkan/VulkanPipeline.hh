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
		std::vector<SColorBlendAttachmentState> ColorBlendAttachments;
		std::vector<EDynamicState> DynamicStates = { MW_DYNAMIC_STATE_VIEWPORT_COUNT, MW_DYNAMIC_STATE_VIEWPORT_COUNT };
		EPipelineFlags Flags;
		EImageFormat DepthAttachmentFormat;
		EImageFormat StencilAttachmentFormat;
		
		u32 ViewportCount = 1;
		u32 ScissorCount = 1;
		ECompareOp CompareOp = MW_COMPARE_OP_LESS;
		ECullMode CullMode = MW_CULL_MODE_BACK;
		EPrimitiveTopology Topology = MW_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		EPolygonMode PolygonMode = MW_POLYGON_MODE_FILL;
		
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
		std::vector<VkPipelineColorBlendAttachmentState> m_ColorAttachmentStates; 
		std::vector<VkDynamicState> m_DynamicStates;

		CVertexLayout m_VertexLayout;
		VkPipelineLayout m_VulkanPipelineLayout;
		VkPipeline m_VulkanPipeline;
	};
}
