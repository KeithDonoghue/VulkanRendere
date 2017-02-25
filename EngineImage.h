#ifndef VUKAN_IMAGE_HDR
#define VUKAN_IMAGE_HDR 1

#include <string>

class VulkanImage;
class VulkanDevice;

class EngineImage {
public:
	EngineImage(){}
	EngineImage(VulkanDevice *,std::string);
	EngineImage(VulkanImage*);
	~EngineImage();
	void setImage(VulkanImage* theImage){
		mVulkanImage = theImage;
	}

	VulkanImage * getVulkanImage(){
		return mVulkanImage;
	}
private:
	VulkanImage * mVulkanImage;
};

#endif // VUKAN_IMAGE_HDR
