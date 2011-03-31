// -*- C++ -*-
/*!
 * @file  SubStractCaptureImage.h
 * @brief SubStractCaptureImage component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef SUBSTRACTCAPTUREIMAGE_H
#define SUBSTRACTCAPTUREIMAGE_H

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

#define	MASK_THRESHOLD		1	//	backgroundMaskImage��stillObjectMaskImage�𐶐����邽�߂�臒l
#define THRESHOLD_MAX_VALUE	255	//	2�l���̍ۂɎg�p����ő�l

#define	BACKGROUND_ALPHA				0.01	//	�w�i�X�V�̍ۂ̍����䗦
#define	BACKGROUND_INITIAL_THRESHOLD	20		//	�w�i�̏���臒l

#define	STILL_OBJECT_ALPHA				0.1		//	�Î~���̍X�V�̍ۂ̍����䗦
#define	STILL_OBJECT_INITIAL_THRESHOLD	255		//	�Î~���̂̏���臒l

#define	THRESHOLD_COEFFICIENT	5.0	//	臒l�̒l�������ۂ̎g�p����臒l�ɂ����鐔

#define	NOT_STILL_DEC_STEP			10	//	�������ꍇ�̃J�E���^���Z��
#define	STILL_OBJECT_TO_BACKGROUND	100	//	�w�i�Ƃ��č̗p����


// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class SubStractCaptureImage
 * @brief SubStractCaptureImage component
 *
 */
class SubStractCaptureImage
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  SubStractCaptureImage(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~SubStractCaptureImage();

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
   * - Name: output_mode conf_varname
   * - DefaultValue: 0
   */
  int m_output_mode;
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
  CameraImage m_image_orig;
  /*!
   */
  InPort<CameraImage> m_image_origIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_image_out;
  /*!
   */
  OutPort<CameraImage> m_image_outOut;

  CameraImage m_foreMaskImg;

  OutPort<CameraImage> m_foreMaskImgOut;

  CameraImage m_stillMaskImg;

  OutPort<CameraImage> m_stillMaskImgOut;

  CameraImage m_backGroundImg;

  OutPort<CameraImage> m_backGroundImgOut;

  CameraImage m_stillImg;

  OutPort<CameraImage> m_stillImgOut;
  
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
	//	�摜�𐶐�����
	IplImage* inputImage;
	IplImage* backgroundAverageImage;		//	�w�i�̕��ϒl�ۑ��pIplImage
	IplImage* backgroundThresholdImage;		//	�w�i��臒l�ۑ��pIplImage
	IplImage* stillObjectAverageImage;		//	�Î~���̂̕��ϒl�ۑ��pIplImage
	IplImage* stillObjectThresholdImage;		//	�Î~���̂�臒l�ۑ��pIplImage
	IplImage* stillObjectCounterImage;		//	�Î~���̂̃J�E���^�pIplImage
	IplImage* backgroundDifferenceImage;		//	�w�i�����摜�pIplImage
	IplImage* stillObjectDifferenceImage;	//	�Î~���̍����摜�pIplIMage
	IplImage* thresholdImage32;				//	32bit��臒l�摜�pIplImage
	IplImage* thresholdImage;					//	臒l�摜�pIplImage
	IplImage* resultImage;					//	���ʉ摜�pIplImage
	IplImage* backgroundMaskImage;			//	�w�i�}�X�N�摜�pIplImage
	IplImage* foregroundMaskImage;			//	�O�i�}�X�N�pIplImage
	IplImage* stillObjectMaskImage;			//	�Î~���̃}�X�N�pIplImage
	IplImage* movingObjectMask;				//	�����̃}�X�N�pIplImage
	IplImage* backgroundCopyMaskImage;		//	�w�i�ɃR�s�[����ۂɎg�p����}�X�N�pIplImage
	IplImage* tmpMaskImage;					//	�e���|�����pIplImage
	IplImage* tmp2MaskImage;					//	�e���|�����pIplImage(����2)
	IplImage* frameImage32;					//	32bit�̃L���v�`�������摜�pIplImage
	IplImage* backgroundImage;				//	�w�i�摜�pIplImage
	IplImage* stillObjectImage;				//	�Î~���̉摜�pIplImage
	IplImage* outputImage;

	IplImage* foreGroundMaskBuff;
	IplImage* stillObjectMaskBuff;
	IplImage* backGroundBuff;
	IplImage* stillObjectImageBuff;
	IplImage* stillObjectCounterBuff;

	int key;					//	�L�[���͗p�̕ϐ�
	//int count;
};


extern "C"
{
  DLL_EXPORT void SubStractCaptureImageInit(RTC::Manager* manager);
};

#endif // SUBSTRACTCAPTUREIMAGE_H
