
#include "Graphics.h"

#include <dxgi1_6.h>
#include <stdexcept>

Graphics::~Graphics()
{
    WaitForGPU();

    if (fenceEvent_)
    {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
}

bool Graphics::Initialize(HWND window, int width, int height)
{
    if (!window || width <= 0 || height <= 0)
        return false;

    width_ = width;
    height_ = height;

    if (!CreateDevice())
        return false;

    if (!CreateCommandQueue())
        return false;

    if (!CreateSwapChain(window, width, height))
        return false;

    if (!CreateDescriptorHeap())
        return false;

    if (!CreateRenderTargetViews())
        return false;

    if (FAILED(device_->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence_))))
        return false;

    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (!fenceEvent_)
        return false;

    initialized_ = true;
    return true;
}

bool Graphics::CreateDevice()
{
    if (FAILED(CreateDXGIFactory2(
        0,
        IID_PPV_ARGS(&factory_))))
        return false;

    HRESULT result = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device_));

    return SUCCEEDED(result);
}

bool Graphics::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc{};

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    if (FAILED(device_->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&commandQueue_))))
        return false;

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(device_->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&commandAllocators_[i]))))
            return false;
    }

    if (FAILED(device_->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocators_[0].Get(),
        nullptr,
        IID_PPV_ARGS(&commandList_))))
        return false;

    commandList_->Close();

    return true;
}

bool Graphics::CreateSwapChain(
    HWND window,
    int width,
    int height)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

    if (FAILED(factory_->CreateSwapChainForHwnd(
        commandQueue_.Get(),
        window,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1)))
        return false;

    if (FAILED(swapChain1.As(&swapChain_)))
        return false;

    frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

    return true;
}

bool Graphics::CreateDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};

    heapDesc.NumDescriptors = FrameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(device_->CreateDescriptorHeap(
        &heapDesc,
        IID_PPV_ARGS(&rtvHeap_))))
        return false;

    rtvDescriptorSize_ =
        device_->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    return true;
}

bool Graphics::CreateRenderTargetViews()
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle =
        rtvHeap_->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(swapChain_->GetBuffer(
            i,
            IID_PPV_ARGS(&renderTargets_[i]))))
            return false;

        device_->CreateRenderTargetView(
            renderTargets_[i].Get(),
            nullptr,
            handle);

        handle.ptr += rtvDescriptorSize_;
    }

    return true;
}

void Graphics::Clear()
{
    if (!initialized_)
        return;

    frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

    commandAllocators_[frameIndex_]->Reset();

    commandList_->Reset(
        commandAllocators_[frameIndex_].Get(),
        nullptr);

    D3D12_RESOURCE_BARRIER barrier{};

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource =
        renderTargets_[frameIndex_].Get();

    barrier.Transition.StateBefore =
        D3D12_RESOURCE_STATE_PRESENT;

    barrier.Transition.StateAfter =
        D3D12_RESOURCE_STATE_RENDER_TARGET;

    barrier.Transition.Subresource =
        D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList_->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
        rtvHeap_->GetCPUDescriptorHandleForHeapStart();

    rtvHandle.ptr += frameIndex_ * rtvDescriptorSize_;

    commandList_->OMSetRenderTargets(
        1,
        &rtvHandle,
        FALSE,
        nullptr);

    const float clearColor[] = {
        0.1f, 0.2f, 0.4f, 1.0f
    };

    commandList_->ClearRenderTargetView(
        rtvHandle,
        clearColor,
        0,
        nullptr);

    barrier.Transition.StateBefore =
        D3D12_RESOURCE_STATE_RENDER_TARGET;

    barrier.Transition.StateAfter =
        D3D12_RESOURCE_STATE_PRESENT;

    commandList_->ResourceBarrier(1, &barrier);

    commandList_->Close();

    ID3D12CommandList* lists[] = {
        commandList_.Get()
    };

    commandQueue_->ExecuteCommandLists(
        1,
        lists);
}

void Graphics::Present()
{
    if (!initialized_)
        return;

    swapChain_->Present(1, 0);

    WaitForGPU();
}

void Graphics::WaitForGPU()
{
    if (!commandQueue_ || !fence_ || !fenceEvent_)
        return;

    const UINT64 value = ++fenceValue_;

    if (FAILED(commandQueue_->Signal(fence_.Get(), value)))
        return;

    if (fence_->GetCompletedValue() < value)
    {
        if (SUCCEEDED(fence_->SetEventOnCompletion(
            value, fenceEvent_)))
        {
            WaitForSingleObject(fenceEvent_, INFINITE);
        }
    }
}

bool Graphics::IsInitialized() const noexcept
{
    return initialized_;
}
