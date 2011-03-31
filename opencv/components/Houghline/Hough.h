// -*- C++ -*-
/*!
 * @file  Hough.h
 * @brief Hough line component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef HOUGH_H
#define HOUGH_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

//OpenCV header file include
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//	cvCanny�p�萔
#define	APERTURE_SIZE		3	//	Sobel�I�y���[�^�̃T�C�Y (���t�@�����X�Q��)

//	cvHoughLines2�p�萔
#define RHO					1						//	�p�����[�^��Ԃɂ�����ώ��̕���\(�s�N�Z���P��)
#define THETA				( CV_PI / 180 )			//	�p�����[�^��Ԃɂ�����Ǝ��̕���\(���W�A���P��)
#define HOUGHLINE_METHOD	CV_HOUGH_PROBABILISTIC	//	�n�t�ϊ��̎�@ (���t�@�����X�Q��)
#define HOUGHLINE_THRESHOLD	50						//	�����𒊏o����ۂ̃p�����[�^��Ԃł̓��[����臒l
#define HOUGHLINE_PARAM1	50						//	��@�ɉ�����1�Ԗڂ̃p�����[�^ (���t�@�����X�Q��)
#define HOUGHLINE_PARAM2	10						//	��@�ɉ�����2�Ԗڂ̃p�����[�^ (���t�@�����X�Q��)

//	cvLine�p�萔
#define LINE_THICKNESS	2	//	���̑���
#define LINE_TYPE		8	//	���̎��
#define SHIFT			0	//	���W�̏����_�ȉ��̌���\���r�b�g��

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class Hough
 * @brief Hough line component
 *
 */
class Hough
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Hough(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Hough();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /*!
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">

  // </rtc-template>
  int canny_threshold1;
  int canny_threshold2;
  // DataInPort declaration
  // <rtc-template block="inport_declare">
  CameraImage m_image_orig;
  /*!
   */
  InPort<CameraImage> m_image_origIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_image_hough;
  /*!
   */
  OutPort<CameraImage> m_image_houghOut;
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>
  IplImage* imageBuff;
  IplImage* grayImage;
  IplImage* edgeImage;
  IplImage* hough;
  IplImage* houghImage;
  int len;
  CvSeq *lines;
};


extern "C"
{
  DLL_EXPORT void HoughInit(RTC::Manager* manager);
};

#endif // HOUGH_H
