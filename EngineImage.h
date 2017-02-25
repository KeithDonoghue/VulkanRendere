#ifndef VUKAN_IMAGE_HDR
#define VUKAN_IMAGE_HDR 1

#include <string>
#include <memory>

class VulkanImage;
class VulkanDevice;

class EngineImage {
public:
	EngineImage(){}
	EngineImage(VulkanDevice *,std::string);
	EngineImage(VulkanImage*);
	
    void setImage(std::shared_ptr<VulkanImage> theImage){
		mVulkanImage = theImage;
	}
	VulkanImage * getVulkanImage(){
		return mVulkanImage.get();
	}
private:
	std::shared_ptr<VulkanImage> mVulkanImage;
};

#endif // VUKAN_IMAGE_HDR
