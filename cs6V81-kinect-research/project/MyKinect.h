#pragma once

#include "NuiApi.h"
#include "ImageMap.h"

class MyKinect {
  public:
    // Pointer to actual Kinect sensor struct
    INuiSensor*                         m_pNuiSensor;

    // Received image resolutions
    static const NUI_IMAGE_RESOLUTION   cDepthResolution = NUI_IMAGE_RESOLUTION_640x480;
    static const NUI_IMAGE_RESOLUTION   cColorResolution = NUI_IMAGE_RESOLUTION_640x480;

    // Handles for Kinect events
    HANDLE                              m_hNextDepthFrameEvent;
    HANDLE                              m_pDepthStreamHandle;
    bool                                m_bDepthReceived;
  
    HANDLE                              m_hNextColorFrameEvent;
    HANDLE                              m_pColorStreamHandle;
    bool                                m_bColorReceived;
    static const unsigned int           cBytesPerPixel = 4;

    // Storage of depth and color pixels in arrays for working with later!
    ImageMap<unsigned __int16>          m_depth_map;
    ImageMap<unsigned __int32>          m_color_map;

    USHORT*                             m_depthD16;
    BYTE*                               m_colorRGBX;
    LONG*                               m_colorCoordinates;

    bool                                m_bNearMode;

    /// <summary>
    /// Toggles between near and default mode
    /// Does nothing on a non-Kinect for Windows device
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT toggle_near_mode() {
      HRESULT hr = E_FAIL;
      if ( m_pNuiSensor ) {
        hr = m_pNuiSensor->NuiImageStreamSetImageFrameFlags( m_pDepthStreamHandle, 
                                                             m_bNearMode ? 0 : NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
        if ( SUCCEEDED(hr) ) {
          m_bNearMode = !m_bNearMode;
        }
      }
      return hr;
    }

    /// <summary>
    /// Create the first connected Kinect found 
    /// </summary>
    /// <returns>indicates success or failure</returns>
    HRESULT create_device() {
    INuiSensor * pNuiSensor = NULL;
    HRESULT hr;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if ( FAILED(hr) ) { return hr; }

    // Look at each Kinect sensor
    for ( int i = 0; i < iSensorCount; ++i ) {
      // Create the sensor. If we can't create it, try the next
      hr = NuiCreateSensorByIndex( i, &pNuiSensor );
      if ( FAILED(hr) ) {
        continue;
      }

      // If sensor is connected, initialize it
      hr = pNuiSensor->NuiStatus();
      if ( S_OK == hr ) {
        m_pNuiSensor = pNuiSensor;
        break;
      }

      // This sensor wasn't OK, so release it since we're not using it
      pNuiSensor->Release();
    }

    // Coult not connec to any Kinect sensors
    if ( NULL == m_pNuiSensor ) {
        return E_FAIL;
    }

    // Initialize the Kinect and specify to use depth (with player index) and color streams
    hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX); 
    if ( FAILED(hr) ) { return hr; }

    // Create an event that will be signaled when depth data is available
    m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    // Open a depth image stream to receive depth frames
    hr = m_pNuiSensor->NuiImageStreamOpen(
      NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
      cDepthResolution,
      0,
      2,
      m_hNextDepthFrameEvent,
      &m_pDepthStreamHandle );
    if ( FAILED(hr) ) { return hr; }

    // Create an event that will be signaled when color data is available
    m_hNextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    // Open a color image stream to receive color frames
    hr = m_pNuiSensor->NuiImageStreamOpen(
      NUI_IMAGE_TYPE_COLOR,
      cColorResolution,
      0,
      2,
      m_hNextColorFrameEvent,
      &m_pColorStreamHandle );
    if  (FAILED(hr) ) { return hr; }

    // Start with near mode on
    toggle_near_mode();

    return hr;
  }
};


// Kinect
    