#ifndef VULKAN_BUFFER_HPP 
#define VULKAN_BUFFER_HPP 1

#include "ApiUsageHeader.h"
#include "VulkanMemoryManager.h"

#include <memory>

typedef enum class BufferType 
{
	BUFFER_TYPE_INDEX = 0,
	BUFFER_TYPE_VERTEX = 1,
	BUFFER_TYPE_STAGING = 2,
	BUFFER_TYPE_RANGE_SIZE = BUFFER_TYPE_STAGING - BUFFER_TYPE_INDEX + 1
}BufferType;


class VulkanDevice;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice&, BufferType, size_t, bool isStaging = false);
	~VulkanBuffer();

	VkBuffer        GetVkBuffer(){ return m_TheVulkanBuffer; }
	VkBuffer *      GetVkBufferAddr(){ return &m_TheVulkanBuffer; }
	void            BindMemory(bool mappable = true);
	void            LoadBufferData(const void *, uint32_t size);
	void            CopyFullBuffer(VulkanBuffer&);

	VkBufferUsageFlagBits   getUsage();
	VkBufferUsageFlagBits   getUsage(BufferType);
	VkBufferCreateInfo      getSharedCreateInfo();


	static std::shared_ptr<VulkanBuffer> SetUpVertexBuffer(VulkanDevice&);
	static std::shared_ptr<VulkanBuffer> SetUpVertexIndexBuffer(VulkanDevice&);
	static std::shared_ptr<VulkanBuffer> SetUpIndexBuffer(VulkanDevice&);

private:
	VulkanDevice &      mDevice;
	VkBufferCreateInfo  mBufferCreateInfo;
	uint64_t            mSize;
	BufferType          mType;

	bool TempMemoryBound;

	bool mIsStaging;
	std::shared_ptr<VulkanBuffer> mStagingBuffer;

	VkBuffer m_TheVulkanBuffer;
	allocation mBlock;


	uint32_t mNumPrimitives;
	bool mHostVisible;

// Private Methods.
	std::shared_ptr<VulkanBuffer> CreateStagingBuffer(size_t);


};
#endif // VULKAN_BUFFER_HPP 
