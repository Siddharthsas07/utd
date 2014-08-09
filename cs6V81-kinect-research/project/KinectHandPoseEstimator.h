//------------------------------------------------------------------------------
// <copyright file="DepthWithColor-D3D.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <windows.h>

// This file requires the installation of the DirectX SDK, a link for which is included in the Toolkit Browser
#include <d3d11.h>
#include <xnamath.h>

//#include "NuiApi.h"
#include "MyKinect.h"

#include "Camera.h"
#include "DX11Utils.h"
#include "resource.h"
#include "HandModel.h"
#include "ParticleSwarmOptimizer.h"
#include "SkinBlobClassifier.h"

class KinectHandPoseEstimator {
  public:
    /// <summary>
    /// Constructor
    /// </summary>
    KinectHandPoseEstimator();

    /// <summary>
    /// Denstructor
    /// </summary>
    ~KinectHandPoseEstimator();

    /// <summary>
    /// Register class and create window
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             init_window( HINSTANCE hInstance, int nCmdShow );

    /// <summary>
    /// Handles window messages, used to process input
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT handle_window_event_messages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    /// <summary>
    /// Create Direct3D device and swap chain
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             init_d3d_device();

    /// <summary>
    /// Create the first connected Kinect found 
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                             connect_kinect_device();
  
    /// <summary>
    /// Create the particle swarm optimizer
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                             init_estimator();

    /// <summary>
    /// Renders a frame
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             render_frame( bool update_estimate, bool update_screen );


  private:
    // 3d camera
    CCamera                             m_camera;

    // The kinect
    MyKinect                            m_kinect;

    // The hand model to draw each frame
    HandModel                           m_hand;

    // the optimizer for the hand model
    ParticleSwarmOptimizer              m_pso;

    // skin blob classifier
    SkinBlobClassifier                  m_skin_blobber;

    // The Direct3D device (running on DirectX11 or DirectX10)
    MyD3DDevice                         m_d3d;

    // if the application is paused, for example in the minimized case
    bool                                m_bPaused;

    /// <summary>
    /// Process depth data received from Kinect
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             process_depth();

    /// <summar>
    /// Capture the depth data from the kinect into a workable 16bit array
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             capture_depth();

    /// <summary>
    /// Process color data received from Kinect
    /// </summary>
    /// <returns>S_OK for success, or failure code</returns>
    HRESULT                             process_color();

    /// <summary>
    /// Adjust color to the same space as depth
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                            map_color_to_depth();
};