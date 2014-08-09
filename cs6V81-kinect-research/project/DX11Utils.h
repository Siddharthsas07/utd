//------------------------------------------------------------------------------
// <copyright file="DX11Utils.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <xnamath.h>
#include "NuiApi.h"
#include "useful_things.h"

/// <summary>
/// Helper for compiling shaders with D3DX11
/// </summary>
/// <param name="szFileName">full path to shader to compile</param>
/// <param name="szEntryPoint">entry point of shader</param>
/// <param name="szShaderModel">shader model to compile for</param>
/// <param name="ppBlobOut">holds result of compilation</param>
/// <returns>S_OK for success, or failure code</returns>
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

/// <summary>
/// Constant buffer for shader
/// </summary>
struct CBChangesEveryFrame {
  XMMATRIX View;
  XMMATRIX Projection;
  XMFLOAT4 XYScale;
};

class MyD3DDevice {
  public:
    // Things for the D3D Device
    HINSTANCE                           m_hInst;
    HWND                                m_hWnd;
    D3D_FEATURE_LEVEL                   m_featureLevel;
    ID3D11Device*                       m_pd3dDevice;
    ID3D11DeviceContext*                m_pImmediateContext;
    IDXGISwapChain*                     m_pSwapChain;
    ID3D11RenderTargetView*             m_pRenderTargetView;
    ID3D11Texture2D*                    m_pDepthStencil;
    ID3D11DepthStencilView*             m_pDepthStencilView;
    ID3D11InputLayout*                  m_pVertexLayout;
    ID3D11Buffer*                       m_pVertexBuffer;
    ID3D11Buffer*                       m_pCBChangesEveryFrame;
    XMMATRIX                            m_projection;

    // Shaders
    ID3D11VertexShader*                 m_pVertexShader;
    ID3D11PixelShader*                  m_pPixelShader;
    ID3D11GeometryShader*               m_pGeometryShader;

    LONG                                m_depthWidth;
    LONG                                m_depthHeight;

    LONG                                m_colorWidth;
    LONG                                m_colorHeight;

    LONG                                m_colorToDepthDivisor;

    float                               m_xyScale;

    // Initial window resolution
    int                                 m_windowResX;
    int                                 m_windowResY;
    
    // for passing depth data as a texture
    ID3D11Texture2D*                    m_pDepthTexture2D;
    ID3D11ShaderResourceView*           m_pDepthTextureRV;

    // for passing color data as a texture
    ID3D11Texture2D*                    m_pColorTexture2D;
    ID3D11ShaderResourceView*           m_pColorTextureRV;
    ID3D11SamplerState*                 m_pColorSampler;

    /// <summary>
    /// Compile and set layout for shaders
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT load_shaders() {
      ID3DBlob* pBlob = NULL;
      HRESULT hr;

      // Compile the geometry shader    
      hr = CompileShaderFromFile( L"KinectHandPoseEstimatorShaders.fx", "GS", "gs_4_0", &pBlob );
      if ( FAILED(hr) ) { return hr; };

      // Create the geometry shader
      hr = m_pd3dDevice->CreateGeometryShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pGeometryShader );
      SAFE_RELEASE( pBlob );
      if ( FAILED(hr) ) { return hr; }

      // Compile the pixel shader
      hr = CompileShaderFromFile(L"KinectHandPoseEstimatorShaders.fx", "PS", "ps_4_0", &pBlob);
      if ( FAILED(hr) ) { return hr; }

      // Create the pixel shader
      hr = m_pd3dDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pPixelShader );
      SAFE_RELEASE( pBlob );
      if ( FAILED(hr) ) { return hr; }

      // Compile the vertex shader
      hr = CompileShaderFromFile( L"KinectHandPoseEstimatorShaders.fx", "VS", "vs_4_0", &pBlob );
      if ( FAILED(hr) ) { return hr; }

      // Create the vertex shader
      hr = m_pd3dDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pVertexShader );
      if ( SUCCEEDED(hr) ) {
        // Define the vertex input layout
        D3D11_INPUT_ELEMENT_DESC layout[] = { { "POSITION", 0, DXGI_FORMAT_R16_SINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

        // Create the vertex input layout
        hr = m_pd3dDevice->CreateInputLayout( layout, ARRAYSIZE(layout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pVertexLayout );
      }

      SAFE_RELEASE( pBlob );
      if ( FAILED(hr) ) { return hr; }

      // Set the input vertex layout
      // In this case we don't actually use it for anything
      // All the work is done in the geometry shader, but we need something here
      // We only need to set this once since we have only one vertex format
      m_pImmediateContext->IASetInputLayout( m_pVertexLayout );

      return hr;
   }

    /// <summary>
    /// Create Direct3D device and swap chain
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT init_device() {
      HRESULT hr = S_OK;
      RECT rc;
      GetClientRect( m_hWnd, &rc );
      UINT width = rc.right - rc.left;
      UINT height = rc.bottom - rc.top;

      UINT createDeviceFlags = 0;

      // Likely won't be very performant in reference
      D3D_DRIVER_TYPE driverTypes[] =
      {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
      };
      UINT numDriverTypes = ARRAYSIZE(driverTypes);

      // DX10 or 11 devices are suitable
      D3D_FEATURE_LEVEL featureLevels[] =
      {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
      };
      UINT numFeatureLevels = ARRAYSIZE(featureLevels);

      DXGI_SWAP_CHAIN_DESC sd = {0};
      sd.BufferCount = 1;
      sd.BufferDesc.Width = width;
      sd.BufferDesc.Height = height;
      sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      sd.BufferDesc.RefreshRate.Numerator = 60;
      sd.BufferDesc.RefreshRate.Denominator = 1;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.OutputWindow = m_hWnd;
      sd.SampleDesc.Count = 1;
      sd.SampleDesc.Quality = 0;
      sd.Windowed = TRUE;

      for ( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex ) {
        hr = D3D11CreateDeviceAndSwapChain( NULL, driverTypes[driverTypeIndex], NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );
        if ( SUCCEEDED(hr) ) {
            break;
        }
      }

      if ( FAILED(hr) )
      {
        MessageBox(NULL, L"Could not create a Direct3D 10 or 11 device.", L"Error", MB_ICONHAND | MB_OK);
        return hr;
      }

      // Create a render target view
      ID3D11Texture2D* pBackBuffer = NULL;
      hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
      if ( FAILED(hr) ) { return hr; }

      hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
      pBackBuffer->Release();
      if ( FAILED(hr) ) { return hr; }

      // Create depth stencil texture
      D3D11_TEXTURE2D_DESC descDepth = {0};
      descDepth.Width = width;
      descDepth.Height = height;
      descDepth.MipLevels = 1;
      descDepth.ArraySize = 1;
      descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
      descDepth.SampleDesc.Count = 1;
      descDepth.SampleDesc.Quality = 0;
      descDepth.Usage = D3D11_USAGE_DEFAULT;
      descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      descDepth.CPUAccessFlags = 0;
      descDepth.MiscFlags = 0;
      hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
      if ( FAILED(hr) ) { return hr; }

      // Create the depth stencil view
      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
      ZeroMemory( &descDSV, sizeof(descDSV) );
      descDSV.Format = descDepth.Format;
      descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      descDSV.Texture2D.MipSlice = 0;
      hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
      if ( FAILED(hr) ) { return hr; }

      m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

      // Create depth texture
      D3D11_TEXTURE2D_DESC depthTexDesc = {0};
      depthTexDesc.Width = m_depthWidth;
      depthTexDesc.Height = m_depthHeight;
      depthTexDesc.MipLevels = 1;
      depthTexDesc.ArraySize = 1;
      depthTexDesc.Format = DXGI_FORMAT_R16_SINT;
      depthTexDesc.SampleDesc.Count = 1;
      depthTexDesc.SampleDesc.Quality = 0;
      depthTexDesc.Usage = D3D11_USAGE_DYNAMIC;
      depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      depthTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      depthTexDesc.MiscFlags = 0;

      hr = m_pd3dDevice->CreateTexture2D(&depthTexDesc, NULL, &m_pDepthTexture2D);
      if ( FAILED(hr) ) { return hr; }
    
      hr = m_pd3dDevice->CreateShaderResourceView(m_pDepthTexture2D, NULL, &m_pDepthTextureRV);
      if ( FAILED(hr) ) { return hr; }

      // Create color texture
      D3D11_TEXTURE2D_DESC colorTexDesc = {0};
      colorTexDesc.Width = m_colorWidth;
      colorTexDesc.Height = m_colorHeight;
      colorTexDesc.MipLevels = 1;
      colorTexDesc.ArraySize = 1;
      colorTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      colorTexDesc.SampleDesc.Count = 1;
      colorTexDesc.SampleDesc.Quality = 0;
      colorTexDesc.Usage = D3D11_USAGE_DYNAMIC;
      colorTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      colorTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      colorTexDesc.MiscFlags = 0;

      hr = m_pd3dDevice->CreateTexture2D( &colorTexDesc, NULL, &m_pColorTexture2D );
      if ( FAILED(hr) ) { return hr; }

      hr = m_pd3dDevice->CreateShaderResourceView(m_pColorTexture2D, NULL, &m_pColorTextureRV);
      if ( FAILED(hr) ) { return hr; }

      // Setup the viewport
      D3D11_VIEWPORT vp;
      vp.Width = static_cast<FLOAT>(width);
      vp.Height = static_cast<FLOAT>(height);
      vp.MinDepth = 0.0f;
      vp.MaxDepth = 1.0f;
      vp.TopLeftX = 0;
      vp.TopLeftY = 0;
      m_pImmediateContext->RSSetViewports(1, &vp);

      hr = load_shaders();

      if ( FAILED(hr) ) {
        MessageBox(NULL, L"Could not load shaders.", L"Error", MB_ICONHAND | MB_OK);
        return hr;
      }

      // Create the vertex buffer
      D3D11_BUFFER_DESC bd = {0};
      bd.Usage = D3D11_USAGE_DEFAULT;
      bd.ByteWidth = sizeof(short);
      bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bd.CPUAccessFlags = 0;

      hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
      if ( FAILED(hr) ) { return hr; }

      // Set vertex buffer
      UINT stride = 0;
      UINT offset = 0;
      m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

      // Set primitive topology
      m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    
      // Create the constant buffers
      bd.Usage = D3D11_USAGE_DEFAULT;
      bd.ByteWidth = sizeof(CBChangesEveryFrame);
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      bd.CPUAccessFlags = 0;
      hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBChangesEveryFrame);
      if ( FAILED(hr) ) { return hr; }

      // Create the sample state
      D3D11_SAMPLER_DESC sampDesc;
      ZeroMemory( &sampDesc, sizeof(sampDesc) );
      sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      sampDesc.MinLOD = 0;
      sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
      hr = m_pd3dDevice->CreateSamplerState( &sampDesc, &m_pColorSampler );
      if ( FAILED(hr) ) { return hr; }

      // Initialize the projection matrix
      m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / static_cast<FLOAT>(height), 0.1f, 100.f);
    
      // Calculate correct XY scaling factor so that our vertices are correctly placed in the world
      // This helps us to unproject from the Kinect's depth camera back to a 3d world
      // Since the Horizontal and Vertical FOVs are proportional with the sensor's resolution along those axes
      // We only need to do this for horizontal
      // I.e. tan(horizontalFOV)/depthWidth == tan(verticalFOV)/depthHeight
      // Essentially we're computing the vector that light comes in on for a given pixel on the depth camera
      // We can then scale our x&y depth position by this and the depth to get how far along that vector we are
      const float DegreesToRadians = 3.14159265359f / 180.0f;
      m_xyScale = tanf(NUI_CAMERA_DEPTH_NOMINAL_HORIZONTAL_FOV * DegreesToRadians * 0.5f) / (m_depthWidth * 0.5f);    

      // Set rasterizer state to disable backface culling
      D3D11_RASTERIZER_DESC rasterDesc;
      rasterDesc.FillMode = D3D11_FILL_SOLID;
      rasterDesc.CullMode = D3D11_CULL_NONE;
      rasterDesc.FrontCounterClockwise = true;
      rasterDesc.DepthBias = false;
      rasterDesc.DepthBiasClamp = 0;
      rasterDesc.SlopeScaledDepthBias = 0;
      rasterDesc.DepthClipEnable = true;
      rasterDesc.ScissorEnable = false;
      rasterDesc.MultisampleEnable = false;
      rasterDesc.AntialiasedLineEnable = false;
    
      ID3D11RasterizerState* pState = NULL;

      hr = m_pd3dDevice->CreateRasterizerState(&rasterDesc, &pState);
      if ( FAILED(hr) ) { return hr; }

      m_pImmediateContext->RSSetState(pState);

      SAFE_RELEASE(pState);

      return S_OK;

    }
};