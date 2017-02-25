#ifndef FORWARD_RENDER_HPP
#define FORWARD_RENDER_HPP 1
#include "ShaderModule.h"

#include <vector>
#include <memory>

class MyEngine;
class VulkanImage;
class VulkanBuffer;
class VulkanRenderPass;
class VulkanPipeline;
class RenderInstance;
class EngineImage;

class ForwardRender {
public:
	ForwardRender(MyEngine *);
	~ForwardRender();
	
	void CreateRenderTargets(int, int, uint32_t);
	void CreateInitialData();
	void CreateVertexBuffer();
	void CreateVertexIndexBuffer();
	void CreateIndexBuffer();
	void TakeInput(unsigned int);

	void SetUpTargets();
	void DoRender();

private:
	MyEngine * mRenderEngine;

	std::vector<VulkanImage*> mDepthImages;
	std::vector<VulkanImage*> mColourImages;
	std::vector<VulkanRenderPass*> mRenderPasses;

	EngineImage * mRenderTarget;
	VulkanRenderPass * mCurrentRenderPass;

	std::shared_ptr<VulkanBuffer> mDrawbuffer;
	std::shared_ptr<VulkanBuffer> mIndexDrawbuffer;
	std::shared_ptr<VulkanBuffer> mIndexbuffer;

	std::vector<std::shared_ptr<ShaderModule>> mShaders;
	EngineImage * mImage;

	std::shared_ptr<VulkanPipeline> mPipeline, mPipeline2;
	std::shared_ptr<RenderInstance> mRenderInstance, mRenderInstance2;
};

#endif // FORWARD_RENDER_HPP