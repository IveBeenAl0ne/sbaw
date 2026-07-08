#pragma once
#include "TileEntityRenderer.h"
class BannerModel;

class BannerRenderer : public TileEntityRenderer
{
public:
	static BannerRenderer *instance;

private:
	static ResourceLocation BANNER_LOCATION;
	BannerModel *bannerModel;

	void renderModelColoured(int baseColor, float alpha, bool useCompiled, float brightness = 1.0f);

public:
	BannerRenderer();
	virtual void init(TileEntityRenderDispatcher *dispatcher) override;
	virtual void render(shared_ptr<TileEntity> banner, double x, double y, double z, float a, bool setColor, float alpha = 1.0f, bool useCompiled = true);

	void renderBannerForGui(float x, float y, float scaleX, float scaleY, float alpha, int baseColor);
	void renderBannerForHand(float alpha, int baseColor);
};
