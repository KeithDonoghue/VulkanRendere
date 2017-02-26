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
	void CreatePipelines();
	void CreateInitialData();
	void CreateVertexBuffer();
	void CreateVertexIndexBuffer();
	void CreateIndexBuffer();
	void TakeInput(unsigned int);
	void DoTheImportThing(const std::string& pFile);

	void SetUpTargets();
	void DoRender();


private:
	MyEngine * mRenderEngine;

	std::vector<std::shared_ptr<VulkanImage>> mDepthImages;
	std::vector<std::shared_ptr<VulkanImage>> mColourImages;
	std::vector<std::shared_ptr<VulkanRenderPass>> mRenderPasses;

	std::shared_ptr<EngineImage> mRenderTarget;
	std::shared_ptr<VulkanRenderPass> mCurrentRenderPass;

	std::shared_ptr<VulkanBuffer> mDrawbuffer;
	std::shared_ptr<VulkanBuffer> mIndexDrawbuffer;
	std::shared_ptr<VulkanBuffer> mIndexbuffer;

	std::vector<std::shared_ptr<ShaderModule>> mShaders;
	EngineImage * mImage;

	std::shared_ptr<VulkanPipeline> mPipeline, mPipeline2;
	std::shared_ptr<RenderInstance> mRenderInstance, mRenderInstance2;

	std::vector<std::shared_ptr<RenderInstance>> mDrawQueue;
};

#endif // FORWARD_RENDER_HPP