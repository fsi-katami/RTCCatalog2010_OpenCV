// -*- C++ -*-
/*!
 * @file  BackGroundSubtractionSimple.h
 * @brief BackGroundSubtractionSimple component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef BACKGROUNDSUBTRACTIONSIMPLE_H
#define BACKGROUNDSUBTRACTIONSIMPLE_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define	THRESHOLD			20				//	���������߂�ۂ�臒l
#define THRESHOLD_MAX_VALUE	255				//	2�l���̍ۂɎg�p����ő�l
#define	SCALE				( 1.0 / 255.0 )	//	L*a*b*�ɕϊ����邽�߂ɕK�v�ȃX�P�[���t�@�N�^

#define CAPTURE_OFF			0	//	�摜�̃L���v�`���𒆎~����t���O�l
#define CAPTURE_ON			1	//	�摜�̃L���v�`�����J�n����t���O�l
#define COLOR_DIFFERENCE	0	//	RGB�e�����ɂ����鍷���Z�o�̃t���O�l
#define LAB_DIFFERENCE		1	//	L*a*b*�\�F�n�ɂ����鍷���Z�o�̃t���O�l
#define GRAY_DIFFERENCE		2	//	�O���[�X�P�[���ɂ����鍷���Z�o�̃t���O�l
#define NOISE_KEEP			0	//	�m�C�Y���������Ȃ��t���O�l
#define	NOISE_MORPHOLOGY	1	//	�����t�H���W�[���Z�ɂ��m�C�Y�����̃t���O�l
#define NOISE_MEDIAN		2	//	���f�B�A���t�B���^�ɂ��m�C�Y�����̃t���O�l

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class BackGroundSubtractionSimple
 * @brief BackGroundSubtractionSimple component
 *
 */
class BackGroundSubtractionSimple
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  BackGroundSubtractionSimple(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~BackGroundSubtractionSimple();

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
   * - Name:  cont_mode
   * - DefaultValue: a
   */
  char m_cont_mode;
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
  CameraImage m_img_orig;
  /*!
   */
  InPort<CameraImage> m_img_origIn;

  TimedLong m_key;

  InPort<TimedLong> m_keyIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_img_curr;
  /*!
   */
  OutPort<CameraImage> m_img_currOut;
  CameraImage m_img_resu;
  /*!
   */
  OutPort<CameraImage> m_img_resuOut;
  CameraImage m_img_back;
  /*!
   */
  OutPort<CameraImage> m_img_backOut;
  
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
	//IplImage* originalImage;
	//IplImage* currentImage;
	//IplImage* backgroundImage;
	//IplImage* resultImage;
	//IplImage* outputImage;

};


extern "C"
{
  DLL_EXPORT void BackGroundSubtractionSimpleInit(RTC::Manager* manager);
};

#endif // BACKGROUNDSUBTRACTIONSIMPLE_H
