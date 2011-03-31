// -*- C++ -*-
/*!
 * @file  OpenCVCamera.cpp
 * @brief OpenCV Camera component
 * @date $Date$
 *
 * $Id$
 */

#include "OpenCVCamera.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* OpenCVCamera_spec[] =
  {
    "implementation_id", "OpenCVCamera",
    "type_name",         "OpenCVCamera",
    "description",       "USB Camera Acquire component",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "example",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    "conf.default.device_num",  "0",
    ""
  };
// </rtc-template>
  int device_num_old = 1000;
  bool imgflg = false;
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
OpenCVCamera::OpenCVCamera(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_outOut("out", m_out),

    // </rtc-template>
    dummy(0)
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  registerOutPort("out", m_outOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
OpenCVCamera::~OpenCVCamera()
{
}



RTC::ReturnCode_t OpenCVCamera::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("device_num", m_device_num, "0");

  // </rtc-template>
  return RTC::RTC_OK;
}



RTC::ReturnCode_t OpenCVCamera::onFinalize()
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t OpenCVCamera::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenCVCamera::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t OpenCVCamera::onActivated(RTC::UniqueId ec_id)
{
    //カメラデバイスの探索
/*
    if(NULL==(m_capture = cvCaptureFromCAM(CV_CAP_ANY))){
        cout<<"No Camera Device"<<endl;
        return RTC::RTC_ERROR;
    }

    return RTC::RTC_OK;
*/
    return RTC::RTC_OK;
}


RTC::ReturnCode_t OpenCVCamera::onDeactivated(RTC::UniqueId ec_id)
{
  //カメラ用メモリの解放
  if(m_capture != NULL)
  {
	cvReleaseCapture(&m_capture);
  }

  device_num_old = 1000;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t OpenCVCamera::onExecute(RTC::UniqueId ec_id)
{
    static coil::TimeValue tm_pre;
    static int count = 0;
    IplImage *cam_frame = NULL;
    
    if(m_device_num != device_num_old){
    
		if (device_num_old != 1000)
		{
			cvReleaseCapture(&m_capture);
		}

		device_num_old = m_device_num;

		if(NULL==(m_capture = cvCaptureFromCAM(device_num_old))){
			cout<<"No Camera Device"<<endl;
			imgflg = false;
			//return RTC::RTC_ERROR;
		}else{
			imgflg = true;	
		}
    }

	if(imgflg == true){
	    cam_frame = cvQueryFrame(m_capture);
	    if(NULL == cam_frame)
	    {
			std::cout << "Bad frame or no frame!!" << std::endl;
			return RTC::RTC_ERROR;
	    }

	    IplImage* frame = cvCreateImage(cvGetSize(cam_frame), 8, 3);

	    if(cam_frame ->origin == IPL_ORIGIN_TL)
	    cvCopy(cam_frame, frame);
	    else
	    cvFlip(cam_frame, frame);

	    int len = frame->nChannels * frame->width * frame->height;

	    // 画面のサイズ情報を入れる
	    m_out.pixels.length(len);
	    m_out.width  = frame->width;
	    m_out.height = frame->height;

	    memcpy((void *)&(m_out.pixels[0]), frame->imageData,len);
	    cvReleaseImage(&frame);

	    // 繋がってるコンポーネントがしんでしまうと問題発生
	    m_outOut.write();

	    if (count > 100)
	    {
		count = 0;
		coil::TimeValue tm;
		tm = coil::gettimeofday();

		double sec(tm - tm_pre);
		
		if (sec > 1.0 && sec < 1000.0)
		{
		    std::cout << 100/sec << " [FPS]" << std::endl;
		}

		tm_pre = tm;
	    }
	    ++count;
    }
	

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t OpenCVCamera::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenCVCamera::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenCVCamera::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenCVCamera::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t OpenCVCamera::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void OpenCVCameraInit(RTC::Manager* manager)
  {
    coil::Properties profile(OpenCVCamera_spec);
    manager->registerFactory(profile,
                             RTC::Create<OpenCVCamera>,
                             RTC::Delete<OpenCVCamera>);
  }
  
};
