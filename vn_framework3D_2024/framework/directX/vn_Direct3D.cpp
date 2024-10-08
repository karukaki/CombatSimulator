//--------------------------------------------------------------//
//	"vn_Direct3D.cpp"											//
//		Direct3D管理											//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../vn_environment.h"

extern HWND hWnd;	// ウィンドウのハンドル

//#define USE_GPU (1)	//ディスクリートGPUを検索して使用する

UINT vnDirect3D::rtvIndex = 0;

IDXGIFactory4* vnDirect3D::pFactory = NULL;	//DXGIファクトリ
IDXGIAdapter3* vnDirect3D::pAdapter = NULL;	//アダプター
ID3D12Device* vnDirect3D::pDevice = NULL;	//D3D12デバイス
IDXGISwapChain3* vnDirect3D::pSwapChain = NULL;	//スワップチェイン
ID3D12CommandQueue* vnDirect3D::pCmdQueue = NULL;	//コマンドキュー
HANDLE						vnDirect3D::hFenceEvent = NULL;	//フェンスイベント
ID3D12Fence* vnDirect3D::pQueueFence = NULL;	//コマンドキュー用のフェンス
ID3D12DescriptorHeap* vnDirect3D::pDH_RTV = NULL;	//ディスクリプタヒープ(RenderTargetView)
ID3D12DescriptorHeap* vnDirect3D::pDH_DSV = NULL;	//ディスクリプタヒープ(DepthStencilView)
D3D12_CPU_DESCRIPTOR_HANDLE	vnDirect3D::hRTV[frameCount];	//ディスクリプタハンドル(RenderTargetView)
D3D12_CPU_DESCRIPTOR_HANDLE	vnDirect3D::hDSV;				//ディスクリプタハンドル(DepthStencilView)
ID3D12Resource* vnDirect3D::pRenderTarget[frameCount];	//レンダーターゲット
ID3D12Resource* vnDirect3D::pDepthBuffer = NULL;
ID3D12CommandAllocator* vnDirect3D::pCmdAllocator = NULL;	//コマンドアロケータ
ID3D12GraphicsCommandList* vnDirect3D::pCmdList = NULL;	//コマンドリスト
ID3D12RootSignature* vnDirect3D::pRootSignature = NULL;	//ルートシグネチャ

UINT vnDirect3D::fenceValue = 0;

ID3D12Resource* vnDirect3D::pWhiteTex = NULL;
D3D12_SHADER_RESOURCE_VIEW_DESC vnDirect3D::srvDesc = {};


int vnDirect3D::initialize()
{
	HRESULT hr = S_OK;

	createFactory();

	createDevice();

	createCommandQueue();

	createSwapChain();

	createRenderTargetView();

	createDepthStencilBuffer();

	createCommandList();

	createRootSignature();

	createWhiteTexture();

	hr = pCmdList->Close();

	return 1;
}

void vnDirect3D::createFactory()
{
	HRESULT hr = S_OK;
	UINT dxgiFactoryFlags = 0;

	//デバッグモードの場合はデバッグレイヤーを有効にする
#if defined(_DEBUG)
	{
		ID3D12Debug* debug = NULL;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		assert(hr == S_OK);
		debug->EnableDebugLayer();

		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		debug->Release();
	}
#endif

	//ファクトリの作成
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory));
	assert(hr == S_OK);

	//フルスクリーンへの遷移をサポートしない
//	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
}

void vnDirect3D::createDevice()
{
	HRESULT hr = S_OK;

	//アダプターを列挙し、最初に見つかったアダプターを使用する
	hr = pFactory->EnumAdapters(0, (IDXGIAdapter**)&pAdapter);

#ifdef USE_GPU
	//ディスクリートGPUを検索して使用する
	IDXGIAdapter3* tmpAdapter = NULL;
	for (int i = 1; pFactory->EnumAdapters(i, (IDXGIAdapter**)&tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 adesc = {};
		tmpAdapter->GetDesc1(&adesc);
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{

		}
		else
		{
			pAdapter = tmpAdapter;
			break;
		}
	}
#endif
	assert(hr == S_OK);

#if 1
	{
		DXGI_ADAPTER_DESC1 adesc = {};
		pAdapter->GetDesc1(&adesc);
		OutputDebugStringW(L"Selected Adapter : ");
		OutputDebugStringW(adesc.Description);
		OutputDebugStringW(L"\n");
	}
#endif

	//フィーチャレベル列挙
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	//デバイスの作成
	for (int i = 0; i < sizeof(levels) / sizeof(D3D_FEATURE_LEVEL); i++)
	{
		hr = D3D12CreateDevice(
			pAdapter,
			levels[i],
			IID_PPV_ARGS(&pDevice)
		);
		if (hr == S_OK)
		{
			pDevice->SetName(L"vnDirect3D::pDevice");
			break;
		}
	}
	assert(pDevice != NULL);
}

void vnDirect3D::createCommandQueue()
{
	HRESULT hr = S_OK;

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
	command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;			//コマンドリストと合わせる
	command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	//指定なし
	command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;			//タイムアウトなし
	command_queue_desc.NodeMask = 0;	//アダプターを一つしか使わないときは0

	//コマンドキューの作成
	hr = pDevice->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&pCmdQueue));
	assert(hr == S_OK);
	pCmdQueue->SetName(L"vnDirect3D::pCmdQueue");

	//フェンスイベントの作成
	hFenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	assert(hFenceEvent != NULL);
	//コマンドキュー用のフェンスの生成
	hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pQueueFence));
	assert(hr == S_OK);
	pQueueFence->SetName(L"vnDirect3D::pQueueFence");

	fenceValue = 1;
}

void vnDirect3D::createSwapChain()
{
	HRESULT hr = S_OK;

	//スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = (UINT)vnMainFrame::screenWidth;
	swapChainDesc.Height = (UINT)vnMainFrame::screenHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.BufferCount = frameCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//スワップチェインの作成
	hr = pFactory->CreateSwapChainForHwnd(
		pCmdQueue,
		hWnd,
		&swapChainDesc,
		NULL,
		NULL,
		(IDXGISwapChain1**)&pSwapChain
	);
	assert(hr == S_OK);

	//カレントのバックバッファのインデックスを取得する
	rtvIndex = pSwapChain->GetCurrentBackBufferIndex();

	//フルスクリーンへの遷移をサポートしない
	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);	//スワップチェインの作成後
}

void vnDirect3D::createRenderTargetView()
{
	HRESULT hr = S_OK;

	//RTV用デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
	heap_desc.NumDescriptors = frameCount;
	heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heap_desc.NodeMask = 0;

	//RTV用デスクリプタヒープの作成
	hr = pDevice->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&pDH_RTV));
	assert(hr == S_OK);
	pDH_RTV->SetName(L"vnDirect3D::pDH_RTV");

	//ディスクリプタのサイズを取得
	UINT size = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//レンダーターゲットを作成
	for (UINT i = 0; i < frameCount; i++)
	{
		//スワップチェインからバッファを受け取る
		hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pRenderTarget[i]));
		assert(hr == S_OK);
		pRenderTarget[i]->SetName(L"vnDirect3D::pRenderTarget");
		//RenderTargetViewを作成してヒープデスクリプタに登録
		hRTV[i] = pDH_RTV->GetCPUDescriptorHandleForHeapStart();
		hRTV[i].ptr += size * i;
		pDevice->CreateRenderTargetView(pRenderTarget[i], NULL, hRTV[i]);
	}
}

void vnDirect3D::createDepthStencilBuffer()
{
	HRESULT hr = S_OK;

	//深度バッファ作成
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Alignment = 0;
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2次元のテクスチャデータ
	depthResDesc.Width = vnMainFrame::screenWidth;					//レンダーターゲットと同じ
	depthResDesc.Height = vnMainFrame::screenHeight;				//レンダーターゲットと同じ
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;					//深度値書き込み用フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.SampleDesc.Quality = 0;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//深度ステンシルとして使用
	depthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResDesc.MipLevels = 0;

	//デプス用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	depthHeapProp.CreationNodeMask = 0;
	depthHeapProp.VisibleNodeMask = 0;

	//クリアバリュー
	D3D12_CLEAR_VALUE _depthClearValue = {};
	_depthClearValue.DepthStencil.Depth = 1.0f;
	_depthClearValue.DepthStencil.Stencil = 0;
	_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//32bit深度値としてクリア

	hr = pDevice->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
		&_depthClearValue,
		IID_PPV_ARGS(&pDepthBuffer));
	assert(hr == S_OK);
	pDepthBuffer->SetName(L"vnDirect3D::pDepthBuffer");

	//深度用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;	//深度ビュー1つのみ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビューとして使う
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	hr = pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pDH_DSV));
	assert(hr == S_OK);
	pDH_DSV->SetName(L"vnDirect3D::pDH_DSV");

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	pDevice->CreateDepthStencilView(pDepthBuffer, &dsvDesc, pDH_DSV->GetCPUDescriptorHandleForHeapStart());

	//ハンドルの取得
	hDSV = pDH_DSV->GetCPUDescriptorHandleForHeapStart();
}

void vnDirect3D::createCommandList()
{
	HRESULT hr = S_OK;

	//コマンドアロケータの作成
	hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCmdAllocator));
	assert(hr == S_OK);
	pCmdAllocator->SetName(L"vnDirect3D::pCmdAllocator");

	//コマンドリストの作成(コマンドアロケータとバインド)
	hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCmdAllocator, NULL, IID_PPV_ARGS(&pCmdList));
	assert(hr == S_OK);
	pCmdList->SetName(L"vnDirect3D::pCmdList");
}

void vnDirect3D::createRootSignature()
{
	HRESULT hr = S_OK;

	//ルートシグネチャの設定

	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};//テクスチャと定数の２つ
	descTblRange[0].NumDescriptors = 1;							//テクスチャひとつ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別はテクスチャ
	descTblRange[0].BaseShaderRegister = 0;						//0番スロットから
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[1].NumDescriptors = 1;							//定数ひとつ
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別は定数
	descTblRange[1].BaseShaderRegister = 0;						//0番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange[0];			//デスクリプタレンジのアドレス
	rootparam.DescriptorTable.NumDescriptorRanges = _countof(descTblRange);	//デスクリプタレンジ数
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC	root_signature_desc = {};
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	root_signature_desc.NumParameters = 1;
	root_signature_desc.pParameters = &rootparam;
	root_signature_desc.NumStaticSamplers = 1;
	root_signature_desc.pStaticSamplers = &samplerDesc;

	//ルートシグネチャのバイナリコードを作成
	ID3DBlob* rSigBlob = NULL;
	ID3DBlob* rSigBlobErr = NULL;
	hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &rSigBlob, &rSigBlobErr);
	assert(hr == S_OK);

	//ルートシグネチャの作成
	hr = pDevice->CreateRootSignature(0, rSigBlob->GetBufferPointer(), rSigBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
	assert(hr == S_OK);
	pRootSignature->SetName(L"vnDirect3D::pRootSignature");

	SAFE_RELEASE(rSigBlob);
	SAFE_RELEASE(rSigBlobErr);
}

void vnDirect3D::createWhiteTexture()
{
	HRESULT hr = S_OK;

	//テクスチャの作成

	//テクスチャのサイズ
	UINT32 texWidth = 32;
	UINT32 texHeight = 32;

	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;							//特殊な設定
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//ライトバック
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;			//転送はL0(CPU側)から直接行う
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC texResDesc = {};
	texResDesc.Alignment = 0;
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Width = texWidth;
	texResDesc.Height = texHeight;
	texResDesc.DepthOrArraySize = 1;
	texResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//RGBAフォーマット
	texResDesc.SampleDesc.Count = 1;
	texResDesc.SampleDesc.Quality = 0;
	texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResDesc.MipLevels = 1;

	hr = pDevice->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		NULL,
		IID_PPV_ARGS(&pWhiteTex)
	);
	assert(hr == S_OK);
	pWhiteTex->SetName(L"vnDirect3D::pWhiteTex");

	//ピクセルデータ
	UINT32* pixelData = new UINT32[texWidth * texHeight];
	memset(pixelData, 0xff, sizeof(UINT32) * texWidth * texHeight);

	hr = pWhiteTex->WriteToSubresource(
		0,
		NULL,
		pixelData,
		sizeof(UINT32) * texWidth,				//1ラインサイズ
		sizeof(UINT32) * texWidth * texHeight	//全サイズ
	);
	delete[] pixelData;

	//シェーダーリソースビューデスクリプション
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
}


void vnDirect3D::terminate()
{
	SAFE_RELEASE(pWhiteTex);

	SAFE_RELEASE(pRootSignature);

	SAFE_RELEASE(pCmdList);
	SAFE_RELEASE(pCmdAllocator);
	for (int i = 0; i < frameCount; i++)
	{
		SAFE_RELEASE(pRenderTarget[i]);
	}
	SAFE_RELEASE(pDepthBuffer);
	SAFE_RELEASE(pDH_RTV);
	SAFE_RELEASE(pDH_DSV);
	SAFE_RELEASE(pQueueFence);
	CloseHandle(hFenceEvent);
	SAFE_RELEASE(pCmdQueue);
	SAFE_RELEASE(pSwapChain);

	//ID3D12DebugDevice* debugInterface;
	//pDevice->QueryInterface(&debugInterface);
	//debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
	//debugInterface->Release();

	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

}

void vnDirect3D::beginRender(void)
{
	HRESULT hr = S_OK;

	static D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)vnMainFrame::screenWidth, (float)vnMainFrame::screenHeight, 0.0f, 1.0f };
	static D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(vnMainFrame::screenWidth), static_cast<LONG>(vnMainFrame::screenHeight) };

	hr = pCmdAllocator->Reset();
	//assert(hr == S_OK);

	hr = pCmdList->Reset(pCmdAllocator, NULL);
	//assert(hr == S_OK);

	pCmdList->SetGraphicsRootSignature(pRootSignature);
	pCmdList->RSSetViewports(1, &viewport);
	pCmdList->RSSetScissorRects(1, &scissorRect);

	//レンダーターゲットへのリソースバリア
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = pRenderTarget[rtvIndex];
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//BarrierDesc.Aliasing.pResourceBefore
	//BarrierDesc.Aliasing.pResourceAfter
	//BarrierDesc.UAV
	pCmdList->ResourceBarrier(1, &BarrierDesc);

	//レンダーターゲットと深度バッファの設定
	pCmdList->OMSetRenderTargets(1, &hRTV[rtvIndex], true, &hDSV);

	//レンダーターゲットのクリア
	pCmdList->ClearRenderTargetView(hRTV[rtvIndex], vnMainFrame::clearColor, 0, NULL);
	//深度バッファのクリア
	pCmdList->ClearDepthStencilView(hDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

}

void vnDirect3D::beginRender2(void)
{
}

void vnDirect3D::finishRender(void)
{
	HRESULT hr = S_OK;

#if 0	//この後Direct2Dの描画がある場合はここでプレゼンテーションへ移行しない

	//プレゼンテーションへのリソースバリア
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = pRenderTarget[rtvIndex];
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	pCmdList->ResourceBarrier(1, &BarrierDesc);

#endif

	hr = pCmdList->Close();
	//assert(hr == S_OK);

	ID3D12CommandList* ppCommandLists[] = { pCmdList };
	pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void vnDirect3D::present(void)
{
	HRESULT hr = S_OK;

	//スワップチェインのプレゼンテーション
	hr = pSwapChain->Present(1, 0);

	//描画の終了待ち
	const UINT64 fence = fenceValue;
	hr = pCmdQueue->Signal(pQueueFence, fence);
	fenceValue++;

	if (pQueueFence->GetCompletedValue() < fence)
	{
		hr = pQueueFence->SetEventOnCompletion(fence, hFenceEvent);
		WaitForSingleObject(hFenceEvent, INFINITE);
	}

	//バックバッファのインデックスを取得
	rtvIndex = pSwapChain->GetCurrentBackBufferIndex();
}


ID3D12Device* vnDirect3D::getDevice(void)
{
	return pDevice;
}

ID3D12CommandQueue* vnDirect3D::getCommandQueue(void)
{
	return pCmdQueue;
}

ID3D12RootSignature* vnDirect3D::getRootSignature(void)
{
	return pRootSignature;
}

ID3D12GraphicsCommandList* vnDirect3D::getCommandList(void)
{
	return pCmdList;
}

ID3D12Resource* vnDirect3D::getRenderTarget(int id)
{
	return pRenderTarget[id];
}


UINT vnDirect3D::getIndex(void)
{
	return rtvIndex;
}

ID3D12Resource* vnDirect3D::getWhiteTexture()
{
	return pWhiteTex;
}

D3D12_SHADER_RESOURCE_VIEW_DESC* vnDirect3D::getWhiteTextueViewDesc()
{
	return &srvDesc;
}
