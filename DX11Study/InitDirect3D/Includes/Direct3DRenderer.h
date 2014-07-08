#pragma once
#include <d3d11.h>
#include <string>
#include "GameTimer.h"
#include "Shader.h"
#include "DeferredShader.h"
#include "DeferredBuffers.h"
#include "FullScreenQuad.h"
#include "LightShader.h"
#include "ReflectionShader.h"
#include "RenderToTexture.h"
#include "ProjectiveTextureShader.h"
#include "ViewPoint.h"
#include "DepthShader.h"
#include "ShadowMapShader.h"
#include "Light.h"

using std::wstring;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SCREEN_NEAR = 1.0f;
const float SCREEN_DEPTH = 1000.0f;

class Direct3DRenderer
{
public:
	Direct3DRenderer(float width, float height, wstring title, bool fullScreen = false);
	~Direct3DRenderer();

	bool initD3D(HWND hWnd);
	void resizeBackBuffer(UINT width, UINT height);
	void beginScene();
	void endScene();
	void changeFillMode(D3D11_FILL_MODE fillMode);
	void switchFillMode();
	void render(RenderParameters& renderParameters);
	void renderBuffer(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	void setShaderResource(int numViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
	ID3D11Device* getDevice() const;
	void setViewport(float width, float height, float topLeftX = 0.0f, float topLeftY = 0.0f, float minDepth = 0.0f, float maxDepth = 1.0f);
	void setClearColor(int r, int g, int b);
	void renderToDeferredBuffers(RenderParameters& renderParameters);
	void renderToTexture(RenderParameters& renderParameters);
	void renderQuad(RenderParameters& renderParameters);
	void renderLight(RenderParameters& renderParameters);
	void renderReflection(RenderParameters& renderParameters);
	void renderProjectiveTexture(RenderParameters& renderParameters);
	void renderDepth(RenderParameters& renderParameters);
	void renderShadowMap(RenderParameters& renderParameters);
	void resetRenderTarget();
	void resetShaderResources();
	void enableOnZTest(bool on);
	void enableAlphaBlend(bool enable);
	void setSamplerState(UINT startSlot, UINT numSamplers, ESamplerType samplerType);
	void setSamplerStates(UINT startSlot, UINT numSamplers, ID3D11SamplerState* const* samplers);
	//private:
	// D3D11 stuffs.
	float mScreenWidth;
	float mScreenHeight;
	wstring mMainWinCaption;
	bool mAppPaused;
	bool mFullScreen;
	bool mEnable4xMsaa;
	bool mResizing;
	bool mMinimized;
	bool mMaximized;
	D3D11_FILL_MODE mFillMode;
	UINT m4xMsaaQuality;
	HWND mMainHWnd;
	HINSTANCE mHInstance;
	GameTimer mTimer;
	Shader* mShader;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;
	ID3D11RasterizerState* mSolidState;
	ID3D11RasterizerState* mWireframeState;
	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDisableDepthStencilState;
	ID3D11SamplerState* mSamplerStatePoint;
	ID3D11SamplerState* mSamplerStateLinear;
	ID3D11SamplerState* mSamplerStateLinearClamp;
	ID3D11SamplerState* mSamplerStateAnisotropic;
	ID3D11BlendState* mAlphaBlendState;
	float mClearColor[3];
	DeferredBuffers* mDeferredBuffers;
	DeferredShader* mDeferredShader;
	FullScreenQuad* mFullScreenQuad;
	LightShader* mLightShader;
	ReflectionShader* mReflectionShader;
	RenderToTexture* mRenderToTexture;
	RenderToTexture* mDepthTexture;
	DepthShader* mDepthShader;
	ShadowMapShader* mShadowMapShader;
	ProjectiveTextureShader* mProjectiveTextureShader;
	ID3D11ShaderResourceView* mProjectiveTexture;
	ViewPoint mViewPoint;
	Light mLight;
};
