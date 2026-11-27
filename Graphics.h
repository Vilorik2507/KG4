
#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class Graphics
{
public:
    Graphics() = default;
    ~Graphics();

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    bool Initialize(HWND window, int width, int height);

    void Clear();
    void Present();

    void WaitForGPU();

    bool IsInitialized() const noexcept;

private:
    bool CreateDevice();
    bool CreateCommandQueue();
    bool CreateSwapChain(HWND window, int width, int height);
    bool CreateDescriptorHeap();
    bool CreateRenderTargetViews();

private:
    static constexpr UINT FrameCount = 2;

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators_[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets_[FrameCount];

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    HANDLE fenceEvent_ = nullptr;
    UINT64 fenceValue_ = 0;

    UINT rtvDescriptorSize_ = 0;
    UINT frameIndex_ = 0;

    int width_ = 0;
    int height_ = 0;

    bool initialized_ = false;
};

