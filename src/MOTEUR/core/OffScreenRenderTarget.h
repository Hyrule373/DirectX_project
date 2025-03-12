#pragma once

class OffscreenRenderTarget {
public:
    OffscreenRenderTarget();
    ~OffscreenRenderTarget();

    void Initialize(ID3D12Device* device, int width, int height);
    ID3D12Resource* GetResource() const { return renderTarget.Get(); }
    void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget;
};