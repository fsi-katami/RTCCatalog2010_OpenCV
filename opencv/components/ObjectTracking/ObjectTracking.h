// -*- C++ -*-
/*!
 * @file  ObjectTracking.h
 * @brief Objecttrack component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef OBJECTTRACKING_H
#define OBJECTTRACKING_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include <cv.h>
#include <highgui.h>

#define		SEGMENT				150		//	cvSnakeImage�ŗp���鐧��_�̐�
#define		WINDOW_WIDTH		17		//	cvSnakeImage�ōŏ��l��T������ߖT�̈�̕�
#define		WINDOW_HEIGHT		17		//	cvSnakeImage�ōŏ��l��T������ߖT�̈�̍���
#define		HISTIMAGE_WIDTH		320		//	�q�X�g�O�����摜�̕�
#define		HISTIMAGE_HEIGHT	200		//	�q�X�g�O�����摜�̍���
#define		H_DIMENSION		16		//	�q�X�g�O�����̎�����
#define		H_RANGE_MIN		0
#define		H_RANGE_MAX		180
#define		V_MIN	10		//	���x�̍ŏ��l
#define		V_MAX	256		//	���x�̍ő�l
#define		S_MIN	30		//	�ʓx�̍ŏ��l
#define		S_MAX	256		//	�ʓx�̍ŏ��l
#define		HIDDEN_BACKPROJECTION	0	//	�o�b�N�v���W�F�N�V�����摜��\�������Ȃ��t���O�l
#define		SHOW_BACKPROJECTION		1	//	�o�b�N�v���W�F�N�V�����摜��\��������t���O�l
#define		SELECT_OFF				0	//	�����ǐ՗̈悪�ݒ肳��Ă��Ȃ��Ƃ��̃t���O�l
#define		SELECT_ON				1	//	�����ǐ՗̈悪�ݒ肳��Ă���Ƃ��̃t���O�l
#define		TRACKING_STOP			0	//	�g���b�L���O���~�߂�t���O�l
#define		TRACKING_START			-1	//	�g���b�L���O���J�n����t���O�l
#define		TRACKING_NOW			1	//	�g���b�L���O���������t���O�l
#define		HIDDEN_HISTOGRAM		0	//	�q�X�g�O������\�������Ȃ��t���O�l
#define		SHOW_HISTOGRAM			1	//	�q�X�g�O������\��������t���O�l
#define		ITERATION_SNAKE			10	//	cvSnakeImage�̔�����

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class ObjectTracking
 * @brief Objecttrack component
 *
 */
class ObjectTracking
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  ObjectTracking(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~ObjectTracking();

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

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  CameraImage m_orig_img;
  /*!
   */
  InPort<CameraImage> m_orig_imgIn;

  TimedLong m_event;

  InPort<TimedLong> m_eventIn;

  TimedLong m_x;

  InPort<TimedLong> m_xIn;

  TimedLong m_y;

  InPort<TimedLong> m_yIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_out_img;
  /*!
   */
  OutPort<CameraImage> m_out_imgOut;
  CameraImage m_hist_img;
  /*!
   */
  OutPort<CameraImage> m_hist_imgOut;
  
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

};


extern "C"
{
  DLL_EXPORT void ObjectTrackingInit(RTC::Manager* manager);
};

#endif // OBJECTTRACKING_H
