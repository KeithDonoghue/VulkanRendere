#include "ForwardRender.h"
#include "MyEngine.h"
#include "EngineImage.h"
#include "RenderInstance.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"

#include "VulkanBuffer.h"



ForwardRender::ForwardRender(MyEngine * theEngine)
{
	mRenderTarget = new EngineImage;
	mRenderEngine = theEngine;
}





ForwardRender::~ForwardRender()
{
	for (auto it : mColourImages)
		delete it;

	for (auto it : mDepthImages)
		delete it;

	for (auto it : mRenderPasses)
		delete it;

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



void  ForwardRender::CreateInitialData()
{

	mImage = mRenderEngine->CreateEngineImage("Resources/jpeg_bad.jpg");

	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/vert.spv"));
	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/frag.spv"));
	mShaders.push_back(mRenderEngine->CreateShaderModule("Resources/red.spv"));


	mPipeline = mRenderEngine-> CreatePipeline(*mRenderPasses[0], mShaders[0], mShaders[1]);
	mPipeline2 = mRenderEngine->CreatePipeline(*mRenderPasses[0], mShaders[0], mShaders[2]);
	mRenderInstance = mRenderEngine->CreateRenderInstance(mPipeline, mImage);
	mRenderInstance2 = mRenderEngine->CreateRenderInstance(mPipeline2, mImage);

	CreateVertexBuffer();
	CreateVertexIndexBuffer();
	CreateIndexBuffer();
		
	mIndexbuffer->DoTheImportThing("Resources/cube.dae");


	VertexDraw	theDraw(6, 1, 0, 0, mDrawbuffer);
	IndexDraw	theIndexDraw(36, 2, 0, 0, 0, mIndexDrawbuffer, mIndexbuffer);

	mRenderInstance->SetDraw(theDraw);
	mRenderInstance2->SetDraw(theDraw);

	mRenderInstance->SetDraw(theIndexDraw);
	mRenderInstance2->SetDraw(theIndexDraw);
	
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










void ForwardRender::TakeInput(unsigned int keyPress)
{
	EngineLog("key: ", keyPress);
	if (keyPress == 65)
		mRenderInstance->ChangeWorldPosition(1.0f, 0.0f, 0.0f);

	if (keyPress == 66)
		mRenderInstance->ChangeWorldPosition(-1.0f, 0.0f, 0.0f);

	if (keyPress == 67)
		mRenderInstance->ChangeWorldPosition(0.0f, 0.0f, 1.0f);

	if (keyPress == 68)
		mRenderInstance->ChangeWorldPosition(0.0f, 0.0f, -1.0f);

	if (keyPress == 37)
		mRenderInstance->ChangeWorldPosition(0.0f, -1.0f, 0.0f);

	if (keyPress == 39)
		mRenderInstance->ChangeWorldPosition(0.0f, 1.0f, 0.0f);
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





void ForwardRender::DoRender()
{
	SetUpTargets();
	//		mVulkanDevice->DoRendering();
	mRenderTarget->getVulkanImage()->ClearImage(1.0f);
	mRenderTarget->getVulkanImage()->BlitFullImage(*mImage->getVulkanImage());

	mRenderInstance->Draw(*mCurrentRenderPass);

	mRenderEngine->SetImage(*mRenderTarget);
}