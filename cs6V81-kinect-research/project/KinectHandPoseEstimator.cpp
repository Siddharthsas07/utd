#include "KinectHandPoseEstimator.h"
#include "useful_things.h"

// Global Variables
KinectHandPoseEstimator g_Application;  // Application class

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  if ( FAILED( g_Application.init_window( hInstance, nCmdShow ) ) ) {
    return 0;
  }

  if ( FAILED( g_Application.init_d3d_device() ) ) {
    return 0;
  }

  if ( FAILED( g_Application.connect_kinect_device() ) ) {
    MessageBox( NULL, 
                L"No ready Kinect found. Close this application, plug\
                  in your Kinect, and try again.", L"Error", 
                MB_ICONHAND | MB_OK );
    return 0;
  }

  if ( FAILED( g_Application.init_estimator() ) ) {
    return 0;
  }

  // Main message loop
  MSG msg = {0};
  while ( WM_QUIT != msg.message ) {
    // If a message exists, dispatch it to registered class
    if ( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
    }

    // Otherwise render a frame, updating the model estimate each time
    else {
      g_Application.render_frame( true, true );
    }
  }

  return (int)msg.wParam;
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
  PAINTSTRUCT ps;

  g_Application.handle_window_event_messages( hWnd, message, wParam, lParam );

  switch( message ) {
    case WM_PAINT:
      BeginPaint( hWnd, &ps );
      EndPaint( hWnd, &ps );
      break;

    case WM_DESTROY:
      PostQuitMessage( 0 );
      break;      

    default:
      return DefWindowProc( hWnd, message, wParam, lParam );
  }

  return 0;
}

/// <summary>
/// Constructor
/// </summary>
KinectHandPoseEstimator::KinectHandPoseEstimator() {
  // get resolution as DWORDS, but store as LONGs to avoid casts later
  DWORD width = 0;
  DWORD height = 0;

  NuiImageResolutionToSize(m_kinect.cDepthResolution, width, height);
  m_d3d.m_depthWidth  = static_cast<LONG>(width);
  m_d3d.m_depthHeight = static_cast<LONG>(height);

  NuiImageResolutionToSize(m_kinect.cColorResolution, width, height);
  m_d3d.m_colorWidth  = static_cast<LONG>(width);
  m_d3d.m_colorHeight = static_cast<LONG>(height);

  m_d3d.m_colorToDepthDivisor = m_d3d.m_colorWidth/m_d3d.m_depthWidth;

  m_d3d.m_hInst = NULL;
  m_d3d.m_hWnd = NULL;
  m_d3d.m_featureLevel = D3D_FEATURE_LEVEL_11_0;
  m_d3d.m_pd3dDevice = NULL;
  m_d3d.m_pImmediateContext = NULL;
  m_d3d.m_pSwapChain = NULL;
  m_d3d.m_pRenderTargetView = NULL;
  m_d3d.m_pDepthStencil = NULL;
  m_d3d.m_pDepthStencilView = NULL;
  m_d3d.m_pVertexLayout = NULL;
  m_d3d.m_pVertexBuffer = NULL;
  m_d3d.m_pCBChangesEveryFrame = NULL;
  m_d3d.m_projection;

  m_d3d.m_pVertexShader = NULL;
  m_d3d.m_pPixelShader = NULL;
  m_d3d.m_pGeometryShader = NULL;

  m_d3d.m_xyScale = 0.0f;
    
  // Initial window resolution
  m_d3d.m_windowResX = 1280;
  m_d3d.m_windowResY = 960;

  m_d3d.m_pDepthTexture2D = NULL;
  m_d3d.m_pDepthTextureRV = NULL;
  m_d3d.m_pColorTexture2D = NULL;
  m_d3d.m_pColorTextureRV = NULL;
  m_d3d.m_pColorSampler = NULL;

  m_kinect.m_bDepthReceived = false;
  m_kinect.m_bColorReceived = false;

  m_kinect.m_hNextDepthFrameEvent = INVALID_HANDLE_VALUE;
  m_kinect.m_pDepthStreamHandle = INVALID_HANDLE_VALUE;
  m_kinect.m_hNextColorFrameEvent = INVALID_HANDLE_VALUE;
  m_kinect.m_pColorStreamHandle = INVALID_HANDLE_VALUE;

  m_kinect.m_depthD16 = new USHORT[m_d3d.m_depthWidth*m_d3d.m_depthHeight];
  m_kinect.m_colorCoordinates = new LONG[m_d3d.m_depthWidth*m_d3d.m_depthHeight*2];
  m_kinect.m_colorRGBX = new BYTE[m_d3d.m_colorWidth*m_d3d.m_colorHeight*m_kinect.cBytesPerPixel];

  m_kinect.m_bNearMode = false;

  m_bPaused = false;
}

/// <summary>
/// Destructor
/// </summary>
KinectHandPoseEstimator::~KinectHandPoseEstimator()
{
  if (NULL != m_kinect.m_pNuiSensor) {
      m_kinect.m_pNuiSensor->NuiShutdown();
      m_kinect.m_pNuiSensor->Release();
  }

  if (m_d3d.m_pImmediateContext) {
      m_d3d.m_pImmediateContext->ClearState();
  }
    
  SAFE_RELEASE(m_d3d.m_pCBChangesEveryFrame);
  SAFE_RELEASE(m_d3d.m_pGeometryShader);
  SAFE_RELEASE(m_d3d.m_pPixelShader);
  SAFE_RELEASE(m_d3d.m_pVertexBuffer);
  SAFE_RELEASE(m_d3d.m_pVertexLayout);
  SAFE_RELEASE(m_d3d.m_pVertexShader);
  SAFE_RELEASE(m_d3d.m_pDepthStencil);
  SAFE_RELEASE(m_d3d.m_pDepthStencilView);
  SAFE_RELEASE(m_d3d.m_pDepthTexture2D);
  SAFE_RELEASE(m_d3d.m_pDepthTextureRV);
  SAFE_RELEASE(m_d3d.m_pColorTexture2D);
  SAFE_RELEASE(m_d3d.m_pColorTextureRV);
  SAFE_RELEASE(m_d3d.m_pColorSampler);
  SAFE_RELEASE(m_d3d.m_pRenderTargetView);
  SAFE_RELEASE(m_d3d.m_pSwapChain);
  SAFE_RELEASE(m_d3d.m_pImmediateContext);
  SAFE_RELEASE(m_d3d.m_pd3dDevice);

  CloseHandle(m_kinect.m_hNextDepthFrameEvent);
  CloseHandle(m_kinect.m_hNextColorFrameEvent);

  // done with pixel data
  SAFE_DELETE_ARRAY( m_kinect.m_colorRGBX );
  SAFE_DELETE_ARRAY( m_kinect.m_colorCoordinates );
  SAFE_DELETE_ARRAY( m_kinect.m_depthD16 );
}

/// <summary>
/// Register class and create window
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::init_window( HINSTANCE hInstance, int nCmdShow ) {
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof( WNDCLASSEX );
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIconW( hInstance, (LPCTSTR)IDI_APP );
  wcex.hCursor = LoadCursorW( NULL, IDC_ARROW );
  wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = L"KinectHandPoseEstimator";
  wcex.hIconSm = LoadIconW( wcex.hInstance, (LPCTSTR)IDI_APP );

  if ( !RegisterClassEx( &wcex ) ) {
    return E_FAIL;
  }

  // Create window
  m_d3d.m_hInst = hInstance;
  RECT rc = { 0, 0, m_d3d.m_windowResX, m_d3d.m_windowResY };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  m_d3d.m_hWnd = CreateWindow( L"KinectHandPoseEstimator", L"KinectHandPoseEstimator-D3D", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                          NULL );
  if (NULL == m_d3d.m_hWnd) {
      return E_FAIL;
  }

  // Show window in specified state by nCmdShow
  ShowWindow( m_d3d.m_hWnd, nCmdShow );

  return S_OK;
}

/// <summary>
/// Handles window messages, used to process input
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT KinectHandPoseEstimator::handle_window_event_messages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  // update the camera
  m_camera.HandleMessages( hWnd, uMsg, wParam, lParam );

  switch( uMsg ) {
    // handle minimize
    case WM_SIZE:          
      if ( SIZE_MINIMIZED == wParam ) {
          m_bPaused = true;
      }
      break;

    // handle restore from minimized
    case WM_ACTIVATEAPP:
      if ( wParam == TRUE ) {
          m_bPaused = false;
      }
      break;

    case WM_KEYDOWN: {
      int nKey = static_cast<int>( wParam );

      if ( nKey == 'N' || nKey == 'n' ) {
          m_kinect.toggle_near_mode();
      }
      break;
    }
  }

  return S_OK;
}

HRESULT KinectHandPoseEstimator::init_d3d_device() {
  return m_d3d.init_device();
}

HRESULT KinectHandPoseEstimator::connect_kinect_device() {
  return m_kinect.create_device();
}

/// <summary>
/// Create Direct3D device and swap chain
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::init_estimator() {
  HRESULT hr = S_OK;

  // initialize the PSOer

  return hr;
}

/// <summary>
/// Process depth data received from Kinect
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::process_depth() {
  NUI_IMAGE_FRAME imageFrame;

  HRESULT hr = m_kinect.m_pNuiSensor->NuiImageStreamGetNextFrame(m_kinect.m_pDepthStreamHandle, 0, &imageFrame);
  if ( FAILED(hr) ) { return hr; }
   
  NUI_LOCKED_RECT LockedRect;
  hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
  if ( FAILED(hr) ) { return hr; }

  memcpy(m_kinect.m_depthD16, LockedRect.pBits, LockedRect.size);
  m_kinect.m_bDepthReceived = true;

  hr = imageFrame.pFrameTexture->UnlockRect(0);
  if ( FAILED(hr) ) { return hr; };

  hr = m_kinect.m_pNuiSensor->NuiImageStreamReleaseFrame(m_kinect.m_pDepthStreamHandle, &imageFrame);

  // copy to our d3d 11 depth texture // black screen w/o mapping this
  D3D11_MAPPED_SUBRESOURCE msT;
  hr = m_d3d.m_pImmediateContext->Map(m_d3d.m_pDepthTexture2D, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
  if ( FAILED(hr) ) { return hr; }

  memcpy(msT.pData, m_kinect.m_depthD16, LockedRect.size);    
  m_d3d.m_pImmediateContext->Unmap(m_d3d.m_pDepthTexture2D, NULL);

  return hr;
}

/// <summary>
/// Process color data received from Kinect
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::process_color() {
  NUI_IMAGE_FRAME imageFrame;

  HRESULT hr = m_kinect.m_pNuiSensor->NuiImageStreamGetNextFrame(m_kinect.m_pColorStreamHandle, 0, &imageFrame);
  if ( FAILED(hr) ) { return hr; }
  
  NUI_LOCKED_RECT LockedRect;
  hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
  if ( FAILED(hr) ) { return hr; }

  memcpy(m_kinect.m_colorRGBX, LockedRect.pBits, LockedRect.size);
  m_kinect.m_bColorReceived = true;

  hr = imageFrame.pFrameTexture->UnlockRect(0);
  if ( FAILED(hr) ) { return hr; };

  hr = m_kinect.m_pNuiSensor->NuiImageStreamReleaseFrame(m_kinect.m_pColorStreamHandle, &imageFrame);
    
  return hr;
}

/// <summary>
/// Process color data received from Kinect
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::map_color_to_depth()
{
  HRESULT hr;

  // Get of x, y coordinates for color in depth space
  // This will allow us to later compensate for the differences in location, angle, etc between the depth and color cameras
  m_kinect.m_pNuiSensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
      m_kinect.cColorResolution,
      m_kinect.cDepthResolution,
      m_d3d.m_depthWidth*m_d3d.m_depthHeight,
      m_kinect.m_depthD16,
      m_d3d.m_depthWidth*m_d3d.m_depthHeight*2,
      m_kinect.m_colorCoordinates
      );

  // copy to our d3d 11 color texture
  D3D11_MAPPED_SUBRESOURCE msT;
  hr = m_d3d.m_pImmediateContext->Map(m_d3d.m_pColorTexture2D, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
  if ( FAILED(hr) ) { return hr; }
    
  // loop over each row and column of the color
  for (LONG y = 0; y < m_d3d.m_colorHeight; ++y) {
    LONG* pDest = (LONG*)((BYTE*)msT.pData + msT.RowPitch * y);

    for (LONG x = 0; x < m_d3d.m_colorWidth; ++x) {
      // calculate index into depth array
      int depthIndex = x/m_d3d.m_colorToDepthDivisor + y/m_d3d.m_colorToDepthDivisor * m_d3d.m_depthWidth;

      // retrieve the depth to color mapping for the current depth pixel
      LONG colorInDepthX = m_kinect.m_colorCoordinates[depthIndex * 2];
      LONG colorInDepthY = m_kinect.m_colorCoordinates[depthIndex * 2 + 1];

      // make sure the depth pixel maps to a valid point in color space
      if ( colorInDepthX >= 0 && colorInDepthX < m_d3d.m_colorWidth && colorInDepthY >= 0 && colorInDepthY < m_d3d.m_colorHeight ) {
        // calculate index into color array
        LONG colorIndex = colorInDepthX + colorInDepthY * m_d3d.m_colorWidth;

        // set source for copy to the color pixel
        LONG* pSrc = (LONG *)m_kinect.m_colorRGBX + colorIndex;
        //*pDest = *pSrc |= 0x00FF00FF; // everything is purple/pink
        //*pDest = *pSrc |= 0xFFFF0000; // everything is red
        *pDest = *pSrc |= 0xFF00FF00; // everything is green?
        //*pDest = *pSrc |= 0xFF0000FF; // everything is blue
        //*pDest = *pSrc |= 0x00FFFF00; // everything is yellow
        //*pDest = *pSrc;
      }

      else {
        *pDest = 0;
      }

      //*pDest = 0xFFFFFF00; // everything is solid yellow


      pDest++;
    }
  }

  m_d3d.m_pImmediateContext->Unmap(m_d3d.m_pColorTexture2D, NULL);

  return hr;
}

/// <summary>
/// Renders a frame
/// </summary>
/// <param name="update_estimate">update pose estimate before rendering</param>
/// <param name="update_screen">update Kinect rgb image before rendering</param>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectHandPoseEstimator::render_frame( bool update_estimate, bool update_screen ) {
  // don't render when paused
  if ( m_bPaused ) {
    return S_OK;
  }

  bool needTomap_color_to_depth = false;

  // Wait for depth data 
  if ( WAIT_OBJECT_0 == WaitForSingleObject(m_kinect.m_hNextDepthFrameEvent, 0) ) {
    // Process depth data
    if ( SUCCEEDED(process_depth()) ) {
      needTomap_color_to_depth = true;
    }
  }

  // Wait for color data
  if ( WAIT_OBJECT_0 == WaitForSingleObject(m_kinect.m_hNextColorFrameEvent, 0) ) {
      // Process color data
    if ( SUCCEEDED(process_color()) ) {
      needTomap_color_to_depth = true;
    }
  }

  

  // 0 - recv depth data and store 
  // 0 - recv color data and store
  // 1 - detect skin blob in color map, save blob to temp map
  // 2 - skin colored pxls within 25cm of hand from last frame = pxl value, 0 else (o_s)
  // 3 - clean depth map based on 2, if a pxl in 2 is 0, it is also 0 here (o_d)
  // 4 - pass o_s, o_d, the current hand model m_pHand, and the camera to the PSOer
  // 5 - receive best guess model from PSOer
  // 6 - render hand model atop the Kinect RGB image



  // Don't map color to depth unless new data from both streams
  if (!m_kinect.m_bDepthReceived || !m_kinect.m_bColorReceived) {
    needTomap_color_to_depth = false;
  }

  if (needTomap_color_to_depth) {
     map_color_to_depth();
  }

  /*for ( unsigned int i = 0; i < m_d3d.m_colorHeight*m_d3d.m_colorWidth*m_kinect.cBytesPerPixel; i+=m_kinect.cBytesPerPixel ) {
    // set the blue byte for this pixel to maximum!
    m_kinect.m_colorRGBX[i] |= 0x00FF00FF;
  }*/

  // Clear the back buffer
  static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  m_d3d.m_pImmediateContext->ClearRenderTargetView( m_d3d.m_pRenderTargetView, ClearColor );

  // Clear the depth buffer to 1.0 (max depth)
  m_d3d.m_pImmediateContext->ClearDepthStencilView( m_d3d.m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

  // Update the view matrix
  m_camera.Update();
    
  // Update variables that change once per frame
  CBChangesEveryFrame cb;
  cb.View = XMMatrixTranspose(m_camera.View);
  cb.Projection = XMMatrixTranspose(m_d3d.m_projection);
  cb.XYScale = XMFLOAT4(m_d3d.m_xyScale, -m_d3d.m_xyScale, 0.f, 0.f); 
  m_d3d.m_pImmediateContext->UpdateSubresource(m_d3d.m_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);

  // Set up shaders
  m_d3d.m_pImmediateContext->VSSetShader(m_d3d.m_pVertexShader, NULL, 0);

  m_d3d.m_pImmediateContext->GSSetShader(m_d3d.m_pGeometryShader, NULL, 0);
  m_d3d.m_pImmediateContext->GSSetConstantBuffers(0, 1, &m_d3d.m_pCBChangesEveryFrame);
  m_d3d.m_pImmediateContext->GSSetShaderResources(0, 1, &m_d3d.m_pDepthTextureRV);
  m_d3d.m_pImmediateContext->GSSetShaderResources(1, 1, &m_d3d.m_pColorTextureRV);
  m_d3d.m_pImmediateContext->GSSetSamplers(0, 1, &m_d3d.m_pColorSampler);

  m_d3d.m_pImmediateContext->PSSetShader(m_d3d.m_pPixelShader, NULL, 0);

  // Draw the scene
  m_d3d.m_pImmediateContext->Draw(m_d3d.m_depthWidth * m_d3d.m_depthHeight, 0);

  // Present our back buffer to our front buffer
  return m_d3d.m_pSwapChain->Present(0, 0);
}