#pragma once

#include <Core/CoreHeader.h>

#include <RHI/RHIResource.h>

namespace z {
class Scene;
class RenderScene;

class DX12Viewport;
class DX12PipelineState;
class DX12VertexBuffer;
class DX12IndexBuffer;
class DX12ConstantBuffer;
class DX12DepthStencil;

class Viewport {
public:
	Viewport(uint32_t width, uint32_t height);
	virtual ~Viewport();

	void Resize(int width, int height);
	void Tick();
	void Render();

	void DrawTex();

	
private:
	Scene* mScene;
	RenderScene* mRenderScene;

	RefCountPtr<RHIViewport> mRHIViewport;
	RefCountPtr<RHIVertexBuffer> vb;
	RefCountPtr<RHIIndexBuffer> ib;
	RefCountPtr<RHITexture> ds, rt;
	RefCountPtr<RHITexture> tex;

	RefCountPtr<RHIShaderInstance> si;
};


}