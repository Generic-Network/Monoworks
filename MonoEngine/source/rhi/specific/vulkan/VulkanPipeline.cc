#include <mwpch.hh>
#include "VulkanPipeline.hh"

#include <rhi/agnostic/Pipeline.hh>

#include <rhi/specific/vulkan/VulkanContext.hh>

namespace Monoworks::RHI 
{


	CVulkanGraphicsPipeline::CVulkanGraphicsPipeline( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

		if ( !( pInfo->Flags & MW_PIPELINE_CREATION_FLAGS_DEFFERED_INITIALIZATION_BIT ) )
		{
			Init( pInfo );
		}
	}

	CVulkanGraphicsPipeline::~CVulkanGraphicsPipeline() NOEXCEPT
	{
		MW_PROFILE_FUNC;
		Shutdown();
	}

	void CVulkanGraphicsPipeline::Init( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT
	{
		MW_PROFILE_FUNC;
		Invalidate( pInfo );
	}

	void CVulkanGraphicsPipeline::Shutdown()
	{
		MW_PROFILE_FUNC; 
	}

	static VkFormat ShaderDataTypeToVulkanFormat( EShaderDataType type )
	{
		switch ( type )
		{
		case MW_SHADER_DATA_TYPE_FLOAT:  return VK_FORMAT_R32_SFLOAT;
		case MW_SHADER_DATA_TYPE_FLOAT_2: return VK_FORMAT_R32G32_SFLOAT;
		case MW_SHADER_DATA_TYPE_FLOAT_3: return VK_FORMAT_R32G32B32_SFLOAT;
		case MW_SHADER_DATA_TYPE_FLOAT_4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case MW_SHADER_DATA_TYPE_INT:    return VK_FORMAT_R32_SINT;
		case MW_SHADER_DATA_TYPE_INT_2:   return VK_FORMAT_R32G32_SINT;
		case MW_SHADER_DATA_TYPE_INT_3:   return VK_FORMAT_R32G32B32_SINT;
		case MW_SHADER_DATA_TYPE_INT_4:   return VK_FORMAT_R32G32B32A32_SINT;
		case MW_SHADER_DATA_TYPE_BOOL:   return VK_FORMAT_R8_UINT;
		default: return VK_FORMAT_UNDEFINED;
		}
	}

	static VkPrimitiveTopology ToVulkanPrimitiveTopology( EPrimitiveTopology topology )
	{
		switch ( topology )
		{
		case MW_PRIMITIVE_TOPOLOGY_POINT_LIST:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case MW_PRIMITIVE_TOPOLOGY_LINE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case MW_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		default:
			MW_ERROR("Invalid API usage: passed invalid EPrimitiveTopology Enum");
			return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		}
	}


	void CVulkanGraphicsPipeline::Invalidate( const SVulkanPipelineCreationInfo* pInfo ) NOEXCEPT
	{
		MW_PROFILE_FUNC;

		struct alignas( 16 ) PushConstantData
		{
			Vector4 Color;
		};

		// TODO: Do descriptor sets & shader reflection 

		VkPushConstantRange range{};
		range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		range.offset = 0;
		range.size = sizeof( PushConstantData );

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &range;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkShaderModule> modules;

		for ( const auto& object : pInfo->ShaderObjects )
		{
			VkShaderModule module;

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = object.Code.Size;
			createInfo.pCode = ( u32* )object.Code.pCode;

			MW_VK_CHECK( vkCreateShaderModule( *CVulkanContext::GetDevice()->GetDevice(), &createInfo, nullptr, &module ), "Failed to create Shader Module" );

			modules.push_back( module );

			VkPipelineShaderStageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			info.module = module;
			info.pName = object.pEntrypoint;
			auto getShaderStage = [&]( EShaderStage stage )
				{
					switch ( stage )
					{
					case MW_SHADER_STAGE_VERTEX:
						return VK_SHADER_STAGE_VERTEX_BIT;
					case MW_SHADER_STAGE_FRAGMENT:
						return VK_SHADER_STAGE_FRAGMENT_BIT;
					case MW_SHADER_STAGE_TESSELATION_CONTROL:
					{
						if ( !( pInfo->Flags & MW_PIPELINE_CREATION_FLAGS_TESSELATION_CONTROL_SHADER_BIT ) )
						{
							MW_ERROR( "Invalid API usage: Pass Tesselation Control Shader without MW_PIPELINE_CREATION_FLAGS_TESSELATION_CONTROL_SHADER_BIT set." );
							break;
						}
						return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
					}
					case MW_SHADER_STAGE_TESSELATION_EVALUATION:
					{
						if ( !( pInfo->Flags & MW_PIPELINE_CREATION_FLAGS_TESSELATION_EVALULATION_SHADER_BIT ) )
						{
							MW_ERROR( "Invalid API usage: Pass Tesselation Evaluation Shader without MW_PIPELINE_CREATION_FLAGS_TESSELATION_EVALULATION_SHADER_BIT set." );
							break;
						}
						return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
					}
					case MW_SHADER_STAGE_GEOMETRY:
					{
						if ( !( pInfo->Flags & MW_PIPELINE_CREATION_FLAGS_GEOMETRY_SHADER_BIT ) )
						{
							MW_ERROR( "Invalid API usage: Pass Geometry Shader without MW_PIPELINE_CREATION_FLAGS_GEOMETRY_SHADER_BIT set." );
							break;
						}
						return VK_SHADER_STAGE_GEOMETRY_BIT;
					}
					case MW_SHADER_STAGE_COMPUTE:
					{
						MW_ERROR( "Invalid API usage: Pass Compute Shader to Graphics Pipeline" );
						break;
					}
					default:
					{
						return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
					}
					};
				};

			info.stage = getShaderStage(object.ShaderStage);

			shaderStages.push_back( info );
		}

		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.stride = m_VertexLayout.GetStride();
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescs;
		uint32_t location = 0;
		for ( const auto& element : m_VertexLayout.GetElements() )
		{
			if ( element.Type == MW_SHADER_DATA_TYPE_MAT_3 || element.Type == MW_SHADER_DATA_TYPE_MAT_4 )
			{
				uint32_t count = ( element.Type == MW_SHADER_DATA_TYPE_MAT_4 ) ? 4 : 3;
				for ( uint32_t i = 0; i < count; i++ )
				{
					VkVertexInputAttributeDescription attr{};
					attr.binding = 0;
					attr.location = location++;
					attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
					attr.offset = element.Offset + ( i * 16 );
					attributeDescs.push_back( attr );
				}
			}
			else
			{
				VkVertexInputAttributeDescription attr{};
				attr.binding = 0;
				attr.location = location++;
				attr.format = ShaderDataTypeToVulkanFormat( element.Type );
				attr.offset = element.Offset;
				attributeDescs.push_back( attr );
			}

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
			vertexInputInfo.vertexAttributeDescriptionCount = ( u32 )attributeDescs.size();
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

			VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
			pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
			pipelineRenderingCreateInfo.colorAttachmentCount = ( u32 )pInfo->ColorFormats.size();
			pipelineRenderingCreateInfo.pColorAttachmentFormats = ( VkFormat* )pInfo->ColorFormats.data();
			pipelineRenderingCreateInfo.depthAttachmentFormat = ( VkFormat )pInfo->DepthAttachmentFormat;
			pipelineRenderingCreateInfo.stencilAttachmentFormat = ( VkFormat )pInfo->StencilAttachmentFormat;

			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyCreateInfo{};
			pipelineInputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineInputAssemblyCreateInfo.topology = ToVulkanPrimitiveTopology( pInfo->Topology );

			VkPipelineViewportStateCreateInfo pipelineViewportCreateInfo{};
			pipelineViewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportCreateInfo.scissorCount = pInfo->ScissorCount;
			pipelineViewportCreateInfo.viewportCount = pInfo->ViewportCount;
			pipelineViewportCreateInfo.pScissors = nullptr; // dynamic in cmd buffer
			pipelineViewportCreateInfo.pViewports = nullptr; // dynamic in cmd buffer too

			// TODO: Add tesselation 

			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
			graphicsPipelineCreateInfo.stageCount = ( u32 )shaderStages.size();
			graphicsPipelineCreateInfo.pStages = shaderStages.data();
			graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
			graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyCreateInfo;
			graphicsPipelineCreateInfo.pViewportState = &pipelineViewportCreateInfo;

		}



	}

}
