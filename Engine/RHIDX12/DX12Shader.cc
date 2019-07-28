#include "DX12Shader.h"
#include "DX12Device.h"

namespace z {

DX12Shader* DX12Shader::FromCompile(const char* data, size_t dataLen, ERHIShaderType stype) {
	// TODO cache 
	static std::string ShaderTypeToName[SHADER_TYPE_MAX]{ "", "VS", "PS" };
	static std::string ShaderTypeToTarget[SHADER_TYPE_MAX]{ "", "vs_5_0", "ps_5_0" };

	RefCountPtr<ID3D10Blob> blob{ nullptr }, error{ nullptr };
	D3D_SHADER_MACRO defines{ nullptr, nullptr };

	HRESULT hr = D3DCompile(data, dataLen, nullptr, &defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ShaderTypeToName[stype].c_str(),
		ShaderTypeToTarget[stype].c_str(),
		D3DCOMPILE_DEBUG, 0, blob.GetComRef(), error.GetComRef());
	if (error) {
		Log<LERROR>((char*)error->GetBufferPointer());
	}
	DX12_CHECK(hr);

	DX12Shader *shader = new DX12Shader();
	shader->mBlob = blob;
	shader->mType = stype;
	return shader;
}

// DX12VertexLayout

DX12VertexLayout::~DX12VertexLayout() {
	for (int i = 0; i < mNames.size(); i++) {
		delete[] mNames[i];
	}
	mNames.clear();
	mLayout.clear();
}

void DX12VertexLayout::PushLayout(const std::string& name, uint32_t index, ERHIPixelFormat format, EVertexLaytoutFlag flag) {
	static std::unordered_map<EVertexLaytoutFlag, D3D12_INPUT_CLASSIFICATION> classicationMapping = {
		{VERTEX_LAYOUT_PER_INSTANCE, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA},
		{VERTEX_LAYOUT_PER_VERTEX, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA}
	};
	char *s = new char[name.length() + 1];
	memcpy(s, name.data(), name.length());
	s[name.length()] = 0;
	mNames.push_back(s);
	D3D12_INPUT_ELEMENT_DESC desc{ s, index, FromRHIFormat(format), 0, mSize, classicationMapping[flag], 0 };
	mLayout.push_back(desc);
	mSize += GetPixelSize(FromRHIFormat(format));
}


// DX12UniformLayout
void DX12UniformLayout::PushLayout(std::string name, uint32_t registerNo, EUniformLayoutFlag flag) {
	switch (flag) {
	case EUniformLayoutFlag::UNIFORM_LAYOUT_CONSTANT_BUFFER:
		if (mCBVs.size() <= registerNo) mCBVs.resize(registerNo + 1);
		mCBVs[registerNo] = name;
		break;
	case EUniformLayoutFlag::UNIFORM_LAYOUT_TEXTURE:
		if (mSRVs.size() <= registerNo) mSRVs.resize(registerNo + 1);
		mSRVs[registerNo] = name;
		break;
	}
}


ID3D12RootSignature *DX12UniformLayout::GetRootSignature() {
	if (mRootSignature) {
		return mRootSignature;
	}

	CD3DX12_ROOT_PARAMETER rootParam[MAX_SIGNATURE_NUM];
	CD3DX12_DESCRIPTOR_RANGE cbvTable[MAX_SIGNATURE_NUM];
	uint32_t count = 0;
	for (int i = 0; i < mCBVs.size(); i++) {
		cbvTable[count].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, i);
		rootParam[count].InitAsDescriptorTable(1, &cbvTable[count]);
		count++;
	}
	for (int i = 0; i < mSRVs.size(); i++) {
		cbvTable[count].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
		rootParam[count].InitAsDescriptorTable(1, &cbvTable[count]);
		count++;
	}

	// todo static sampler
	const CD3DX12_STATIC_SAMPLER_DESC sd(0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(count, rootParam, 1, &sd,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	RefCountPtr<ID3DBlob> serialzed, error;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, serialzed.GetComRef(), error.GetComRef());
	DX12_CHECK(hr, (char*)error->GetBufferPointer());

	auto device = GDX12Device->GetIDevice();
	DX12_CHECK(device->CreateRootSignature(0, serialzed->GetBufferPointer(), 
		serialzed->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetComRef())));

	return mRootSignature;
}

}

