#include "MyEngine.h"
#include "EngineImage.h"
#include "VulkanImage.h"


EngineImage::EngineImage(VulkanDevice* theDevice, std::string filename)
{
	mVulkanImage = new VulkanImage(theDevice,"Resources/jpeg_bad.jpg");
}





EngineImage::EngineImage(VulkanImage* theImage) :
mVulkanImage(theImage)
{}





EngineImage::~EngineImage()
{
	delete mVulkanImage;
}