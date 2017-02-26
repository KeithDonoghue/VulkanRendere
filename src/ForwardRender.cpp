#include "ForwardRender.h"
#include "MyEngine.h"
#include "EngineImage.h"
#include "RenderInstance.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"



#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective




ForwardRender::ForwardRender(MyEngine * theEngine)
{
	mZPosition = 0.0f;
	mXPosition = 0.0f;
	mRotation = 0.0f;
	posDirty = true;
	mRenderTarget = std::make_shared<EngineImage>();
	mRenderEngine = theEngine;
}





ForwardRender::~ForwardRender()
{
	delete mImage;
}





void  ForwardRender::CreateRenderTargets(int width, int height, uint32_t numImages)
{
	for (uint32_t i = 0; i < numImages; i++)
	{
		mDepthImages.emplace_back(mRenderEngine->CreateVulkanImage(width, height, ImageType::VULKAN_IMAGE_DEPTH));
		mColourImages.emplace_back(mRenderEngine->CreateVulkanImage(width, height, ImageType::VULKAN_IMAGE_COLOR_RGBA8));
		mRenderPasses.emplace_back(mRenderEngine->CreateVulkanRenderPass(*mDepthImages[i], *mColourImages[i]));
	}
}





void ForwardRender::CreatePipelines()
{
	mPipeline = mRenderEngine->CreatePipeline(*mRenderPasses[0], mShaders[0], mShaders[2]);
	mPipeline->Init();

	mPipeline2 = mRenderEngine->CreatePipeline(mPipeline, *mRenderPasses[0], mShaders[0], mShaders[1]);

	VkGraphicsPipelineCreateInfo & createinfo = mPipeline2->getCreateInfo();
	VkPipelineRasterizationStateCreateInfo tempRSCreateInfo = *createinfo.pRasterizationState;
	tempRSCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	createinfo.pRasterizationState = &tempRSCreateInfo;
	mPipeline2->Init();

	mPipeline3 = mRenderEngine->CreatePipeline(mPipeline2, *mRenderPasses[0], mShaders[0], mShaders[1]);
	mPipeline3->Init();

}





void  ForwardRender::CreateInitialData()
{

	mImage = mRenderEngine->CreateEngineImage("Resources/jpeg_bad.jpg");

	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/vert.spv"));
	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/frag.spv"));
	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/red.spv"));

	CreatePipelines();

	CreateVertexBuffer();
	CreateVertexIndexBuffer();
	CreateIndexBuffer();

	VertexDraw	theDraw(6, 1, 0, 0, mDrawbuffer);
	IndexDraw	theIndexDraw(36, 2, 0, 0, 0, mIndexDrawbuffer, mIndexbuffer);

	std::shared_ptr<RenderInstance> renderInstance = mRenderEngine->CreateRenderInstance(mPipeline, mImage);
	
	renderInstance->SetDraw(theDraw);
	renderInstance->SetDraw(theIndexDraw);
	mDrawQueue.push_back(renderInstance);


	renderInstance = mRenderEngine->CreateRenderInstance(mPipeline2, mImage);
	renderInstance->SetDraw(theDraw);
	renderInstance->SetDraw(theIndexDraw);
	mDrawQueue.push_back(renderInstance);


	renderInstance = mRenderEngine->CreateRenderInstance(mPipeline3, mImage);
	renderInstance->SetDraw(theDraw);
	renderInstance->SetDraw(theIndexDraw);
	mSecondDrawQueue.push_back(renderInstance);


	DoTheImportThing("Resources/cube.dae");
}




void  ForwardRender::CreateVertexBuffer()
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

	mDrawbuffer = mRenderEngine->CreateVulkanBuffer(BufferType::BUFFER_TYPE_VERTEX, sizeof(vb));
	mDrawbuffer->LoadBufferData(vb, sizeof(vb));
}





void ForwardRender::CreateVertexIndexBuffer()
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

	mIndexDrawbuffer = mRenderEngine->CreateVulkanBuffer(BufferType::BUFFER_TYPE_VERTEX, sizeof(vb));
	mIndexDrawbuffer->LoadBufferData(vb, sizeof(vb));
}




void ForwardRender::CreateIndexBuffer()
{
	const uint32_t indices[36] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		0, 3, 7, 0, 7, 4,
		0, 4, 5, 0, 5, 1,
		1, 5, 6, 1, 6, 2,
		2, 7, 6, 2, 3, 7
	};
	mIndexbuffer = mRenderEngine->CreateVulkanBuffer(BufferType::BUFFER_TYPE_INDEX, sizeof(indices));
	mIndexbuffer->LoadBufferData(indices, sizeof(indices));
}





void ForwardRender::DoTheImportThing(const std::string& pFile)
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





void ForwardRender::TakeInput(unsigned int keyPress)
{
	EngineLog("key: ", keyPress);

	for (auto it : mDrawQueue)
	{
		if (keyPress == 65)
			it->ChangeWorldPosition(1.0f, 0.0f, 0.0f);

		if (keyPress == 66)
			it->ChangeWorldPosition(-1.0f, 0.0f, 0.0f);

		if (keyPress == 67)
			it->ChangeWorldPosition(0.0f, 0.0f, 1.0f);

		if (keyPress == 68)
			it->ChangeWorldPosition(0.0f, 0.0f, -1.0f);

		if (keyPress == 37)
			it->ChangeWorldPosition(0.0f, -1.0f, 0.0f);

		if (keyPress == 39)
			it->ChangeWorldPosition(0.0f, 1.0f, 0.0f);
	}

	if (keyPress == 38)
		ChangeCameraPosition(0.0f, -1.0f, 0.0f);

	if (keyPress == 40)
		ChangeCameraPosition(0.0f, 1.0f, 0.0f);
}





void ForwardRender::SetUpTargets()
{
	static uint32_t nextRenderTarget = 0;

	mRenderTarget->setImage(mColourImages[nextRenderTarget]);
	mCurrentRenderPass = mRenderPasses[nextRenderTarget];
	nextRenderTarget++;

	if (nextRenderTarget == 4)
		nextRenderTarget = 0;
}




void ForwardRender::UpdateViewProj()
{
	
	if (posDirty)
	{
		glm::mat4 Projection = glm::perspective(glm::radians(8.0f), 4.0f / 4.0f, 0.1f, 105.f);
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(mXPosition, 0.0f, mZPosition));
		for (auto it : mDrawQueue)
		{
			it->setView(View);
			it->setProj(Projection);
		}	
		posDirty = false;
	}
	
}






void ForwardRender::DoRender()
{
	UpdateViewProj();
	SetUpTargets();

	mRenderTarget->getVulkanImage()->ClearImage(1.0f);
	mRenderTarget->getVulkanImage()->BlitFullImage(*mImage->getVulkanImage());

	mCurrentRenderPass->Begin();

	for (auto it : mDrawQueue)
		it->Draw(*mCurrentRenderPass);

	mCurrentRenderPass->End();

	for (auto it : mSecondDrawQueue)
	{
		it->setSampleImage(mRenderTarget->getVulkanImage());
	}

	SetUpTargets();

	mRenderTarget->getVulkanImage()->ClearImage(1.0f);
	//mRenderTarget->getVulkanImage()->BlitFullImage(*mImage->getVulkanImage());

	mCurrentRenderPass->Begin();

	for (auto it : mSecondDrawQueue)
	{
		it->Draw(*mCurrentRenderPass);
	}
	
	mCurrentRenderPass->End();

	mRenderEngine->SetImage(*mRenderTarget);
}