#include "EngineImage.h"
#include "VulkanImage.h"

EngineImage::EngineImage(VulkanDevice* theDevice, std::string filename)
{
	mVulkanImage = new VulkanImage(theDevice, "Resources/jpeg_bad.jpg");
	/*
	mVulkanImage = new VulkanImage(this, 400, 400, ImageType::VULKAN_IMAGE_COLOR_RGBA8);

	mVulkanImage->ClearImage(1.0f);
	mVulkanImage->LoadDataToImage("Resources/jpeg_bad.jpg");
	*/
	
}






EngineImage::~EngineImage()
{
	delete mVulkanImage;
}