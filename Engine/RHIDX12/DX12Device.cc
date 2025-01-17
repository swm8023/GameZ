#include "DX12Device.h"
#include "DX12Executor.h"
#include "DX12View.h"
#include "DX12Viewport.h"
#include "DX12Shader.h"
#include "DX12PipelineState.h"

namespace z {

DX12Device* GDX12Device = nullptr;

DX12Device::DX12Device() :
	RHIDevice(),
	mExecutor(nullptr) {
	InitializeSingleton<DX12Device>(GDX12Device, this);

	InitDevice();
}

DX12Device::~DX12Device() {
	DX12DescriptorHeapAllocator::DestroyHeapAllocators();
	SAFE_DELETE(mExecutor);

	FinalizeSingleton<DX12Device>(GDX12Device, this);
}


void DX12Device::InitDevice() {
	UINT createFactoryFlag = 0;
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			createFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}

	// create dxgi factory
	DX12_CHECK(CreateDXGIFactory2(createFactoryFlag, IID_PPV_ARGS(mDxgiFactory.GetComRef())));

	// enumrate hardware adapters and create device
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT idx = 0; DXGI_ERROR_NOT_FOUND != mDxgiFactory->EnumAdapters1(idx, &adapter); idx++) {
		DXGI_ADAPTER_DESC1 desc = {};
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(mDevice.GetComRef())))) {
			Log<LINFO>("Find Adapter: ", desc.Description);

			D3D12_FEATURE_DATA_ARCHITECTURE arch{};
			mDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &arch, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));
			if (arch.UMA) {
				Log<LINFO>("Skip this adapter.....");
				mDevice.Reset();
				continue;
			}
			break;
		}
	}
	CHECK(mDevice.GetRef() != nullptr, "Device not found!");

	// craete executor
	mExecutor = new DX12Executor(this);

	// create descriptor heap
	DX12DescriptorHeapAllocator::CreateHeapAllocators();

	Log<LINFO>("Device init done.");
}



RHIViewport* DX12Device::CreateViewport(uint32_t width, uint32_t height, ERHIPixelFormat format, void *window) {
	return new DX12Viewport(width, height, FromRHIFormat(format), window);
}

RHIShader* DX12Device::CreateShader() {
	return new DX12Shader();
}

RHIShaderStage* DX12Device::CreateShaderStage(const char* data, size_t dataLen, ERHIShaderStage stage) {
	return DX12ShaderStage::FromCompile(data, dataLen, stage);
}

RHIShaderInstance* DX12Device::CreateShaderInstance(RHIShader* shader) {
	return new DX12ShaderInstance(static_cast<DX12Shader*>(shader));
}

RHIIndexBuffer* DX12Device::CreateIndexBuffer(uint32_t num, uint32_t stride, const void* data, bool dynamic) {
	return new DX12IndexBuffer(num, stride, data, dynamic);
}

RHIVertexBuffer* DX12Device::CreateVertexBuffer(uint32_t num, const std::vector<ERHIInputSemantic>& sems, const void* data, bool dynamic) {
	return new DX12VertexBuffer(num, sems, data, dynamic);
}

RHITexture* DX12Device::CreateTexture(const RHITextureDesc& desc, const uint8_t* data) {
	return new DX12Texture2D(desc, data);
}


RHITexture* DX12Device::CreateRenderTarget(uint32_t width, uint32_t height, ERHIPixelFormat format) {
	return new DX12RenderTarget(width, height, FromRHIFormat(format));
};

RHITexture* DX12Device::CreateDepthStencil(uint32_t width, uint32_t height, ERHIPixelFormat format) {
	return new DX12DepthStencil(width, height, FromRHIFormat(format));
}


void DX12Device::GetAndClearStats(RHIStats& stat) {
	auto [dp, face] = mExecutor->GetStats();
	stat.DrawCalls = dp;
	stat.Faces = face;
	mExecutor->ClearStats();
}

void DX12Device::ReloadShaders() {
	DX12PipelineStateCache::ClearCache();
}

void DX12Device::SetOutputs(const std::vector<RHITexture*>& rts, RHITexture* ds) {
	std::vector<DX12RenderTarget*> res;
	// no memcpy, ref count point used here 
	for (int i = 0; i < rts.size(); i++) {
		res.push_back(static_cast<DX12RenderTarget*>(rts[i]));
	}
	mExecutor->SetRenderTargets(res);
	mExecutor->SetDepthStencil(static_cast<DX12DepthStencil*>(ds));

}

void DX12Device::SetRenderRect(const RHIRenderRect& rect) {
	D3D12_VIEWPORT dxRect;
	dxRect = { rect.TopLeftX, rect.TopLeftY, rect.Width, rect.Height, rect.MinDepth, rect.MaxDepth };

	GDX12Device->GetExecutor()->GetCommandList()->RSSetViewports(1, &dxRect);
}


void DX12Device::SetScissorRect(const RHIScissorRect& rect) {
	D3D12_RECT scissorRect;
	scissorRect = { (long)rect.Left, (long)rect.Top, (long)rect.Right, (long)rect.Bottom };

	GDX12Device->GetExecutor()->GetCommandList()->RSSetScissorRects(1, &scissorRect);
}


void DX12Device::DrawIndexed(RHIShaderInstance* shaderInst, RHIVertexBuffer* vb, RHIIndexBuffer* ib, RHIRenderState state, uint32_t numIndex, uint32_t baseIndex, uint32_t baseVertex) {
	DX12ShaderInstance* inst = static_cast<DX12ShaderInstance*>(shaderInst);
	DX12VertexBuffer* vbuffer = static_cast<DX12VertexBuffer*>(vb);
	DX12IndexBuffer* ibuffer = static_cast<DX12IndexBuffer*>(ib);

	// get pipeline, create if not exist	
	std::vector<DX12RenderTarget*> rts = mExecutor->GetCurRenderTargets();
	DX12DepthStencil* ds = mExecutor->GetCurDepthStencil();

	DX12PipelineState* ppState = DX12PipelineStateCache::Get(inst->GetShader(), vbuffer->mSemanticsOffset, rts, ds, state);

	mExecutor->SetPipelineState(ppState);
	mExecutor->SetVertexBuffer(static_cast<DX12VertexBuffer*>(vbuffer));
	mExecutor->SetIndexBuffer(static_cast<DX12IndexBuffer*>(ibuffer));
	mExecutor->DrawShaderInstance(inst, numIndex, baseIndex, baseVertex);
}



}