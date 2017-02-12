#ifndef VULKAN_BUFFER_HPP 
#define VULKAN_BUFFER_HPP 1

#include "ApiUsageHeader.h"
#include "VulkanMemoryManager.h"

#include <memory>


class VulkanDevice;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice&, VkBufferCreateInfo, bool);
	~VulkanBuffer();
	VkBuffer GetVkBuffer(){ return m_TheVulkanBuffer; }
	VkBuffer * GetVkBufferAddr(){ return &m_TheVulkanBuffer; }
	void BindMemory(bool mappable = true);
	void LoadBufferData(const void *, uint32_t size);
	void CopyFullBuffer(VulkanBuffer&);
	void DoTheImportThing(const std::string& pFile);


	static std::shared_ptr<VulkanBuffer> CreateStagingBuffer(VulkanDevice&, size_t);
	static std::shared_ptr<VulkanBuffer> CreateVertexBuffer(VulkanDevice&, size_t);
	static std::shared_ptr<VulkanBuffer> CreateIndexBuffer(VulkanDevice&, size_t);
	static std::shared_ptr<VulkanBuffer> SetUpVertexBuffer(VulkanDevice&);
	static std::shared_ptr<VulkanBuffer> SetUpVertexIndexBuffer(VulkanDevice&);
	static std::shared_ptr<VulkanBuffer> SetUpIndexBuffer(VulkanDevice&);

private:
	bool TempMemoryBound;

	bool mIsStaging;
	std::shared_ptr<VulkanBuffer> mStagingBuffer;

	VkBuffer m_TheVulkanBuffer;
	VulkanDevice & mDevice;
	allocation mBlock;

	uint64_t mSize;

	uint32_t mNumPrimitives;
	bool mHostVisible;
};
#endif // VULKAN_BUFFER_HPP 
