// -*- C++ -*-
/*!
 * @file  RockPaperScissors.h
 * @brief RockpaperScissors check compoenet
 * @date  $Date$
 *
 * $Id$
 */

#ifndef ROCKPAPERSCISSORS_H
#define ROCKPAPERSCISSORS_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// OpenCVHead�t�@�C����Including
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "LabellingW.h"

#define	ITERATIONS	4	//	�c���A���k�̉�

//���F���o�p臒l
#define HMAX 20			//	H�l�̏����臒l
#define HMIN 0			//	H�l�̉�����臒l
#define SMAX 255*1		//	S�l�̏����臒l
#define SMIN 255*0.2	//	S�l�̉�����臒l
#define VMAX 255*1.0	//	V�l�̏����臒l
#define VMIN 255*0		//	V�l�̉�����臒l

//�W�����P������p臒l
#define ROCKMAX 1.0		//	�O�[�Ɣ��肷������臒l
#define ROCKMIN 0.85	//	�O�[�Ɣ��肷�鉺����臒l
#define SCISSORMAX 0.85	//	�`���L�Ɣ��肷������臒l
#define SCISSORMIN 0.7	//	�`���L�Ɣ��肷�鉺����臒l
#define PAPERMAX 0.7	//	�p�[�Ɣ��肷������臒l
#define PAPERMIN 0.5	//	�p�[�Ɣ��肷�鉺����臒l

#define IGNORE_SIZE 1000	//��������̈�T�C�Y(���x�����O�p)

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class RockPaperScissors
 * @brief RockpaperScissors check compoenet
 *
 */
class RockPaperScissors
  : public RTC::DataFlowComponentBase
{
 public:

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  RockPaperScissors(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~RockPaperScissors();

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
  /*!
   * 
   * - Name:  img_height
   * - DefaultValue: 240
   */
  int m_img_height;
  /*!
   * 
   * - Name:  img_width
   * - DefaultValue: 320
   */
  int m_img_width;
  // </rtc-template>
  int m_out_mode;

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  CameraImage m_img_input;
  /*!
   */
  InPort<CameraImage> m_img_inputIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_img_output;
  /*!
   */
  OutPort<CameraImage> m_img_outputOut;
  
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
	int dummy;
	
	IplImage* m_frame_image;
	IplImage* m_image_buff;
	IplImage* m_output_buff;
	IplImage* m_hsv_buff;
	IplImage* m_convexHull_buff;
	IplImage* m_skin_buff;
	IplImage* m_temp_buff;
	IplImage* m_label_buff;

};


extern "C"
{
  DLL_EXPORT void RockPaperScissorsInit(RTC::Manager* manager);

};

#endif ROCKPAPERSCISSORS_H
