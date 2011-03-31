// -*- C++ -*-
/*!
 * @file  ImageCalibration.h
 * @brief ImageCalibration component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef IMAGECALIBRATION_H
#define IMAGECALIBRATION_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#define	NUM_OF_BACKGROUND_FRAMES	5	//	chessboardを生成するのに使用する画像の枚数
//#define CORNER_WIDTH 11
//#define CORNER_HEIGHT 8
//#define CORNER_NUMBER (CORNER_WIDTH * CORNER_HEIGHT) 
#define UNIT 5

//	cvFindChessboardCornersのフラグ用定数
#define ADAPTIVE_THRESH	1	//	CV_CALIB_CB_ADAPTIVE_THRESHを設定するかどうか
#define NORMALIZE_IMAGE	1	//	CV_CALIB_CB_NORMALIZE_IMAGEを設定するかどうか
#define FILTER_QUADS	1	//	CV_CALIB_CB_FILTER_QUADSを設定するかどうか

//	cvTermCriteria用定数
#define MAX_ITERATIONS	20		//	反復数の最大値
#define EPSILON			0.001	//	目標精度

//	cvFindCornerSubPix用定数
#define SEARCH_WINDOW_HALF_WIDTH	5	//	検索ウィンドウの横幅の半分のサイズ
#define SEARCH_WINDOW_HALF_HEIGHT	5	//	検索ウィンドウの縦幅の半分のサイズ
#define DEAD_REGION_HALF_WIDTH		-1	//	総和対象外領域の横幅の半分のサイズ
#define DEAD_REGION_HALF_HEIGHT	-1	//	総和対象外領域の縦幅の半分のサイズ

#define	THRESHOLD_COEFFICIENT	5.0	//	閾値の値を引く際の使用する閾値にかける数

#define CALIBRATE_CAMERA_FLAG 0

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class ImageCalibration
 * @brief ImageCalibration component
 *
 */
class ImageCalibration
  : public RTC::DataFlowComponentBase
{
 public:

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  ImageCalibration(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~ImageCalibration();

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
   /*!
   * 
   * - Name:  board_w
   * - DefaultValue: 13
   */
   CvPoint2D32f* corners;
  
 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  //int m_board_w;
  /*!
   * 
   * - Name:  board_h
   * - DefaultValue: 13
   */
  //int m_board_h;

  float m_camera_Height;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  CameraImage m_inputImage;
  /*!
   */
  InPort<CameraImage> m_inputImageIn;

  TimedLong m_key;

  InPort<TimedLong> m_keyIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_origImage;
  /*!
   */
  OutPort<CameraImage> m_origImageOut;
  //CameraImage m_calbImage;
  /*!
   */
 // OutPort<CameraImage> m_calbImageOut;
  CameraImage m_birdImage;
  /*!
   */
  OutPort<CameraImage> m_birdImageOut;
  TimedString m_internalParameter;
  /*!
   */
  OutPort<TimedString> m_internalParameterOut;
  TimedString m_externalParameter;
  /*!
   */
  OutPort<TimedString> m_externalParameterOut;
  TimedString m_renseParameter;
  /*!
   */
  OutPort<TimedString> m_renseParameterOut;
  
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
	
	 CvMat *intrinsicMatrix;
	 CvMat *distortionCoefficient;

	 IplImage* mapx;
	 IplImage* mapy;
	 IplImage* undistortionImage;
	 IplImage* birds_image;
	 IplImage* tempImage_buff;
	 IplImage* inputImage_buff;
	 IplImage* outputImage_buff;
	 
	 int key;
	 int captureCount;
	 int findFlag;

	 int InParameter;
	 int outParameter;

	 int dummy;

};


extern "C"
{
  DLL_EXPORT void ImageCalibrationInit(RTC::Manager* manager);
};

#endif // IMAGECALIBRATION_H
