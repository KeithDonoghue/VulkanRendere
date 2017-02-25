#include "EngineImage.h"
#include "VulkanImage.h"


EngineImage::EngineImage(VulkanDevice* theDevice, std::string filename)
{
	mVulkanImage = std::make_shared<VulkanImage>(theDevice,"Resources/jpeg_bad.jpg");
}





EngineImage::EngineImage(VulkanImage* theImage) :
mVulkanImage(theImage)
{}
