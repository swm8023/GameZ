#pragma once

#include "DX12Header.h"
#include "DX12Device.h"
#include "DX12Shader.h"
#include "DX12Texture.h"
#include <Core/CoreHeader.h>

namespace z {



class DX12PipelineState : public RHIPipelineState {
public:
	DX12PipelineState();

	void SetShaderVS(DX12Shader* shader) {
		mShaderVS = shader;
	}

	void SetShaderPS(DX12Shader* shader) {
		mShaderPS = shader;
	}

	void SetVertexLayout(DX12VertexLayout* layout) {
		mVertexLayout = layout;
	}

	void SetUniformLayout(DX12UniformLayout* layout) {
		mUniformLayout = layout;
	}

	void SetDepthStencilFormat(DXGI_FORMAT format) {
		mDepthStencilFormat = format;
	}

	void SetRenderTargetsFormat(const std::vector<DXGI_FORMAT> &format) {
		mRenderTargetsFormat = format;
	}

	void Create();

	ID3D12PipelineState* GetIPipelineState();
	ID3D12RootSignature* GetIRootSignature();

private:
	RefCountPtr<DX12Shader> mShaderVS;
	RefCountPtr<DX12Shader> mShaderPS;
	RefCountPtr<DX12VertexLayout> mVertexLayout;
	RefCountPtr<DX12UniformLayout> mUniformLayout;

	std::vector<DXGI_FORMAT> mRenderTargetsFormat;
	DXGI_FORMAT mDepthStencilFormat;

	RefCountPtr<ID3D12PipelineState> mState;
};



}
