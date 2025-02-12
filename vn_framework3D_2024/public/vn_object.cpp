//--------------------------------------------------------------//
//	"vn_object.cpp"												//
//		オブジェクト基底クラス									//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

//静的共通データ
//頂点要素
D3D12_INPUT_ELEMENT_DESC vnObject::inputElementDescs[]
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
//パイプラインステート
ID3D12PipelineState* vnObject::pPipelineState = NULL;
ID3D12PipelineState* vnObject::pPipelineState_NL = NULL;

ID3D12PipelineState* vnObject::pPipelineState_Alpha = NULL;
ID3D12PipelineState* vnObject::pPipelineState_Alpha_NL = NULL;



//静的共通データ初期化
bool vnObject::initializeCommon()
{
	HRESULT hr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = vnDirect3D::getRootSignature();

	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_3D)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_3D)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_3D)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_3D)->getLength();

	psoDesc.RasterizerState.MultisampleEnable = false;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.AntialiasedLineEnable = false;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	//ライティング有効&不透明
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState));
	pPipelineState->SetName(L"vnObject::pPipelineState");

	//ライティング無効&不透明
	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Nolight)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Nolight)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Nolight)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Nolight)->getLength();

	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState_NL));
	pPipelineState_NL->SetName(L"vnObject::pPipelineState_NL");

	//半透明描画の有効
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;

	//半透明有効&ライティング有効
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState_Alpha));
	pPipelineState_Alpha->SetName(L"vnObject::pPipelineState_Alpha");

	//半透明有効&ライティング無効
	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Nolight)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Nolight)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Nolight)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Nolight)->getLength();

	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState_Alpha_NL));
	pPipelineState_Alpha_NL->SetName(L"vnObject::pPipelineState_Alpha_NL");

	return true;
}

//静的共通データ削除
void vnObject::terminateCommon()
{
	SAFE_RELEASE(pPipelineState);
	SAFE_RELEASE(pPipelineState_NL);
	SAFE_RELEASE(pPipelineState_Alpha);
	SAFE_RELEASE(pPipelineState_Alpha_NL);
}


vnObject::vnObject()
{
	Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	Rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	
	Scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	
	Local = XMMatrixIdentity();
	World = XMMatrixIdentity();

	executeEnable = true;
	renderEnable = true;

	lighting = true;

	transparent = false;
}

vnObject::~vnObject()
{
}

void vnObject::execute()
{
}

void vnObject::render()
{
}


//移動値の設定
void vnObject::setPosition(float x, float y, float z)
{
	Position = XMVectorSet(x, y, z, 0.0f);
}

void vnObject::setPosition(const XMVECTOR *v)
{
	Position = *v;
}

void vnObject::setPositionX(float value)
{
	Position = XMVectorSetX(Position, value);
}

void vnObject::setPositionY(float value)
{
	Position = XMVectorSetY(Position, value);
}

void vnObject::setPositionZ(float value)
{
	Position = XMVectorSetZ(Position, value);
}

void vnObject::addPosition(float x, float y, float z)
{
	Position = XMVectorAdd(Position, XMVectorSet(x, y, z, 0.0f));
}

void vnObject::addPosition(const XMVECTOR *v)
{
	Position = XMVectorAdd(Position, *v);
}

void vnObject::addPositionX(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
}

void vnObject::addPositionY(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(0.0f, value, 0.0f, 0.0f));
}

void vnObject::addPositionZ(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(0.0f, 0.0f, value, 0.0f));
}

//移動値の取得
XMVECTOR *vnObject::getPosition()
{
	return &Position;
}

float vnObject::getPositionX(void)
{
	return XMVectorGetX(Position);
}

float vnObject::getPositionY(void)
{
	return XMVectorGetY(Position);
}

float vnObject::getPositionZ(void)
{
	return XMVectorGetZ(Position);
}

//回転値の設定
void vnObject::setRotation(float x, float y, float z)
{
	Rotation = XMVectorSet(x, y, z, 0.0f);
}

void vnObject::setRotation(const XMVECTOR *v)
{
	Rotation = *v;
}

void vnObject::setRotationX(float radian)
{
	Rotation = XMVectorSetX(Rotation, radian);
}

void vnObject::setRotationY(float radian)
{
	Rotation = XMVectorSetY(Rotation, radian);
}

void vnObject::setRotationZ(float radian)
{
	Rotation = XMVectorSetZ(Rotation, radian);
}

void vnObject::addRotation(float x, float y, float z)
{
	Rotation = XMVectorAdd(Rotation, XMVectorSet(x, y, z, 0.0f));
}

void vnObject::addRotation(const XMVECTOR *v)
{
	Rotation = XMVectorAdd(Rotation, *v);
}

void vnObject::addRotationX(float radian)
{
	Rotation = XMVectorAdd(Rotation, XMVectorSet(radian, 0.0f, 0.0f, 0.0f));
}

void vnObject::addRotationY(float radian)
{
	Rotation = XMVectorAdd(Rotation, XMVectorSet(0.0f, radian, 0.0f, 0.0f));
}

void vnObject::addRotationZ(float radian)
{
	Rotation = XMVectorAdd(Rotation, XMVectorSet(0.0f, 0.0f, radian, 0.0f));
}

//回転値の取得
XMVECTOR *vnObject::getRotation()
{
	return &Rotation;
}

float vnObject::getRotationX(void)
{
	return XMVectorGetX(Rotation);
}

float vnObject::getRotationY(void)
{
	return XMVectorGetY(Rotation);
}

float vnObject::getRotationZ(void)
{
	return XMVectorGetZ(Rotation);
}

//拡大値の設定
void vnObject::setScale(float x, float y, float z)
{
	Scale = XMVectorSet(x, y, z, 0.0f);
}

void vnObject::setScale(const XMVECTOR *v)
{
	Scale = *v;
}

void vnObject::setScaleX(float value)
{
	Scale = XMVectorSetX(Scale, value);
}

void vnObject::setScaleY(float value)
{
	Scale = XMVectorSetY(Scale, value);
}

void vnObject::setScaleZ(float value)
{
	Scale = XMVectorSetZ(Scale, value);
}

void vnObject::addScale(float x, float y, float z)
{
	Scale = XMVectorAdd(Scale, XMVectorSet(x, y, z, 0.0f));
}

void vnObject::addScale(const XMVECTOR *v)
{
	Scale = XMVectorAdd(Scale, *v);
}

void vnObject::addScaleX(float value)
{
	Scale = XMVectorAdd(Scale, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
}

void vnObject::addScaleY(float value)
{
	Scale = XMVectorAdd(Scale, XMVectorSet(0.0f, value, 0.0f, 0.0f));
}

void vnObject::addScaleZ(float value)
{
	Scale = XMVectorAdd(Scale, XMVectorSet(0.0f, 0.0f, value, 0.0f));
}

//拡大値の取得
XMVECTOR *vnObject::getScale()
{
	return &Scale;
}

float vnObject::getScaleX(void)
{
	return XMVectorGetX(Scale);
}

float vnObject::getScaleY(void)
{
	return XMVectorGetY(Scale);
}

float vnObject::getScaleZ(void)
{
	return XMVectorGetZ(Scale);
}

//ワールド行列の取得
XMMATRIX *vnObject::getWorld()
{
	return &World;
}

//ワールド座標の取得
void vnObject::getWorldPosition(XMVECTOR *v)
{
	*v = World.r[3];
}

float vnObject::getWorldPositionX()
{
	return XMVectorGetX(World.r[3]);
}

float vnObject::getWorldPositionY()
{
	return XMVectorGetY(World.r[3]);
}

float vnObject::getWorldPositionZ()
{
	return XMVectorGetZ(World.r[3]);
}

//ワールド行列の設定
void vnObject::setWorld(XMMATRIX *m)
{
	if (m)
	{
		World = *m;
	}
}

//実行状態の設定
void vnObject::setExecuteEnable(bool flag)
{
	executeEnable =  flag;
}

//描画状態の設定
void vnObject::setRenderEnable(bool flag)
{
	renderEnable = flag;
}

//実行状態の取得
bool vnObject::isExecuteEnable()
{
	return executeEnable;
}

//描画状態の取得
bool vnObject::isRenderEnable()
{
	return renderEnable;
}

//ライティングの有効/無効の設定
void vnObject::setLighting(bool flag)
{
	lighting = flag;
}
//ライティングの有効/無効の取得
bool vnObject::getLighting()
{
	return lighting;
}

//半透明の有効/無効の設定
void vnObject::setTransparent(bool flag)
{
	transparent = flag;
}
//半透明の有効/無効の取得
bool vnObject::getTransparent()
{
	return transparent;
}