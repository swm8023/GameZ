#pragma once

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "DX12/d3dx12.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <RHI/RHIConst.h>
#include <RHI/RHIResource.h>
#include <Core/CoreHeader.h>

class COMException {
public:
	COMException(HRESULT hr) : error_(hr) {}
	HRESULT Error() const {
		return error_;
	}
private:
	const HRESULT error_;
};

#define DX12_CHECK(hr) if (FAILED(hr)){ throw COMException(hr); }