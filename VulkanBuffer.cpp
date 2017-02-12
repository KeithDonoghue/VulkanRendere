#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.h"
#include "CommandBuffer.h"
#include "CommandPool.h"


#include <algorithm>
#include <memory>


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags





VulkanBuffer::VulkanBuffer(VulkanDevice& theDevice, VkBufferCreateInfo theBufferCreateInfo, bool isStaging):
mDevice(theDevice),
TempMemoryBound(false),
mNumPrimitives(0),
mIsStaging(isStaging)
{
	mSize = theBufferCreateInfo.size;

	VkResult result = vkCreateBuffer(mDevice.getVkDevice(), &theBufferCreateInfo, nullptr, &m_TheVulkanBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create buffer");
	}
}





VulkanBuffer::~VulkanBuffer()
{	
	mDevice.GetMemManager().FreeAllocation(mBlock);

	vkDestroyBuffer(mDevice.getVkDevice(), m_TheVulkanBuffer, nullptr);
}





void VulkanBuffer::LoadBufferData(const void * data, uint32_t size)
{
	BindMemory(mIsStaging);

	if (mIsStaging)
	{
		void * mappedPointer;
		VkResult result = vkMapMemory(mDevice.getVkDevice(), mBlock.mPointer, mBlock.mOffset, mBlock.mSize, 0, &mappedPointer);
		if (result != VK_SUCCESS)
		{
			EngineLog("Failed To Map memory");
		}

		memcpy(mappedPointer, data, std::min<int>(size, mBlock.mSize));
		vkUnmapMemory(mDevice.getVkDevice(), mBlock.mPointer);
	}
	else
	{
		if (mStagingBuffer == nullptr)
			mStagingBuffer = CreateStagingBuffer(mDevice, size);

		mStagingBuffer->LoadBufferData(data, size);

		CopyFullBuffer(*mStagingBuffer);
	}
}





void VulkanBuffer::BindMemory(bool mappable)
{
	if (TempMemoryBound)
		return;

	VkMemoryRequirements memRequirements;

	vkGetBufferMemoryRequirements(mDevice.getVkDevice(), m_TheVulkanBuffer, &memRequirements);

	mBlock = mDevice.GetMemManager().GetAllocation(memRequirements, mappable);

	VkResult result = vkBindBufferMemory(mDevice.getVkDevice(), m_TheVulkanBuffer, mBlock.mPointer, mBlock.mOffset);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed To bind Buffer memory");
	}
	else
	{
		TempMemoryBound = true;
	}

}





std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateStagingBuffer(VulkanDevice& theDevice, size_t size)
{

	VkBufferCreateInfo theBufferCreateInfo = {};
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = nullptr;
	theBufferCreateInfo.size = size;
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = nullptr;
	
	return std::make_shared<VulkanBuffer>(theDevice, theBufferCreateInfo, true);
}





std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateVertexBuffer(VulkanDevice& theDevice, size_t size)
{

	VkBufferCreateInfo theBufferCreateInfo = {};
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = nullptr;
	theBufferCreateInfo.size = size;
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = nullptr;

	return std::make_shared<VulkanBuffer>(theDevice, theBufferCreateInfo, false);
}





std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateIndexBuffer(VulkanDevice& theDevice, size_t size)
{

	VkBufferCreateInfo theBufferCreateInfo = {};
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = nullptr;
	theBufferCreateInfo.size = size;
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = nullptr;

	return std::make_shared<VulkanBuffer>(theDevice, theBufferCreateInfo, false);
}





std::shared_ptr<VulkanBuffer> VulkanBuffer::SetUpVertexBuffer(VulkanDevice& theDevice)
{
	const float vb[6][5] = {
		/*      position             texcoord */
		{ -0.5f, -1.0f, 0.25f, 1.0f, 0.0f },
		{ 0.5f, -1.0f, 0.25f, 1.0f, 1.0f },
		{ 0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
		{ 0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
		{ -0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
		{ -0.5f, -1.0f, 0.25f, 0.0f, 0.0f },
	};

	std::shared_ptr<VulkanBuffer> theDrawBuffer = VulkanBuffer::CreateVertexBuffer(theDevice, sizeof(vb));
	theDrawBuffer->LoadBufferData(vb, sizeof(vb));
	return theDrawBuffer;
}





std::shared_ptr<VulkanBuffer> VulkanBuffer::SetUpVertexIndexBuffer(VulkanDevice& theDevice)
{
	const float vb[8][6] = {
		/*      position             texcoord */
		{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }
	};

	std::shared_ptr<VulkanBuffer> theDrawBuffer = VulkanBuffer::CreateVertexBuffer(theDevice, sizeof(vb));
	theDrawBuffer->LoadBufferData(vb, sizeof(vb));
	return theDrawBuffer;
}





std::shared_ptr<VulkanBuffer> VulkanBuffer::SetUpIndexBuffer(VulkanDevice& theDevice)
{
	const uint32_t indices[36] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		0, 3, 7, 0, 7, 4,
		0, 4, 5, 0, 5, 1, 
		1, 5, 6, 1, 6, 2, 
		2, 7, 6, 2, 3, 7
	};

	std::shared_ptr<VulkanBuffer> theIndexBuffer = VulkanBuffer::CreateIndexBuffer(theDevice, sizeof(indices));
	theIndexBuffer->LoadBufferData(indices, sizeof(indices));
	return theIndexBuffer;
}




void VulkanBuffer::CopyFullBuffer(VulkanBuffer& src)
{

	CommandBuffer * currentCommandBuffer = mDevice.GetCommandPool().GetCurrentCommandBuffer();

	VkBufferCopy copyRange = {};
	copyRange.dstOffset = 0;
	copyRange.srcOffset = 0;
	copyRange.size = std::min(mSize, src.mSize);
	
	vkCmdCopyBuffer(currentCommandBuffer->GetVkCommandBuffer(), src.GetVkBuffer(), GetVkBuffer(), 1, &copyRange);
}






void VulkanBuffer::DoTheImportThing(const std::string& pFile)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	EngineLog("HasAnimations: ", scene->HasAnimations());
	EngineLog("HasCameras: ", scene->HasCameras());
	EngineLog("HasLights: ", scene->HasLights());
	EngineLog("HasMaterials ", scene->HasMaterials());
	EngineLog("HasMeshes: ", scene->HasMeshes());
	EngineLog("HasTextures: ", scene->HasTextures());
}


