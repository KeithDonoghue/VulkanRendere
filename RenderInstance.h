#ifndef RENDER_INSTANCE_HPP
#define RENDER_INSTANCE_HPP

#include "ApiUsageHeader.h"

#include <memory>


#include <glm/mat4x4.hpp> // glm::mat4
 
class VulkanImage;
class VulkanRenderPass;
class VulkanPipeline;
class VulkanDevice;
class VulkanBuffer;


typedef struct VertexDraw{
	uint32_t mVertexCount;
	uint32_t mInstanceCount;
	uint32_t mVertexOffset;
	uint32_t mInstanceOffset;
	std::shared_ptr<VulkanBuffer> mDrawBuffer;

	VertexDraw(){}

	VertexDraw(uint32_t numVertices, 
		uint32_t numInstances, 
		uint32_t vertexOffset, 
		uint32_t instanceOffset, 
		std::shared_ptr<VulkanBuffer> drawBuffer) :
		mVertexCount(numVertices),
		mInstanceCount(numInstances),
		mVertexOffset(vertexOffset),
		mInstanceOffset(instanceOffset),
		mDrawBuffer(drawBuffer){}

}VertexDraw;





typedef struct IndexDraw{
	uint32_t mIndexCount;
	uint32_t mInstanceCount;
	uint32_t mIndexOffset;
	uint32_t mInstanceOffset;
	uint32_t mVertexOffset;
	std::shared_ptr<VulkanBuffer> mDrawBuffer;
	std::shared_ptr<VulkanBuffer> mIndexBuffer;

	IndexDraw(){}

	IndexDraw(uint32_t numIndices,
		uint32_t numInstances,
		uint32_t indexOffset,
		uint32_t instanceOffset,
		uint32_t vertexOffset,
		std::shared_ptr<VulkanBuffer> vertexBuffer,
		std::shared_ptr<VulkanBuffer> indexBuffer) :
		mIndexCount(numIndices),
		mInstanceCount(numInstances),
		mIndexOffset(indexOffset),
		mInstanceOffset(instanceOffset),
		mVertexOffset(vertexOffset),
		mDrawBuffer(vertexBuffer),
		mIndexBuffer(indexBuffer){}
}IndexDraw;





class RenderInstance{
public:
	RenderInstance(VulkanDevice&, std::shared_ptr<VulkanPipeline>, VulkanImage*);
	~RenderInstance();
	void Draw(VulkanRenderPass&);
	void SetDrawBuffer(std::shared_ptr<VulkanBuffer>, uint32_t);
	void SetDraw(VertexDraw);
	void SetDraw(IndexDraw);
	void setSampleImage(VulkanImage * newImage){ mImage = newImage; }

	void ChangeWorldPosition(float deltaZ, float deltaX, float deltaR){
		mZPosition += deltaZ;
		mXPosition += deltaX;
		mRotation += deltaR;
		posDirty = true;
	}

	void UpdateMVP();
	void UpdateRelModelMat();
private:
	VulkanImage * mImage;
	std::shared_ptr<VulkanPipeline> mPipeline;
	VulkanDevice & mDevice;
	VkSampler mSampler;

	std::shared_ptr<VulkanBuffer> mDrawBuffer;
	uint32_t mNumPrimitivesToRender;

	VertexDraw mVertexDraw;
	IndexDraw mIndexDraw;

	glm::mat4 MVP;
	float mZPosition;
	float mXPosition;
	float mRotation;
	bool posDirty;
};
#endif // RENDER_INSTANCE_HPP