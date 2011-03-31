// -*- C++ -*-
/*!
 * @file  Chromakey.h
 * @brief Chromakey image component
 * @date  $Date$
 *
 * $Id$
 */

#ifndef CHROMAKEY_H
#define CHROMAKEY_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>

// For CameraImage port
#include <rtm/idl/InterfaceDataTypesSkel.h>

//OpenCV用インクルードファイルのインクルード
#include<cv.h>
#include<cxcore.h>
#include<highgui.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class Chromakey
 * @brief Chromakey image component
 *
 */
class Chromakey
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Chromakey(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Chromakey();

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
   * - Name:  nLowerBlue
   * - DefaultValue: 0
   */
  int m_nLowerBlue;
  /*!
   * 
   * - Name:  nUpperBlue
   * - DefaultValue: 0
   */
  int m_nUpperBlue;
  /*!
   * 
   * - Name:  nLowerYellow
   * - DefaultValue: 0
   */
  int m_nLowerYellow;
  /*!
   * 
   * - Name:  nUpperYellow
   * - DefaultValue: 0
   */
  int m_nUpperYellow;
  /*!
   * 
   * - Name:  nLowerRed
   * - DefaultValue: 0
   */
  int m_nLowerRed;
  /*!
   * 
   * - Name:  nUpperRed
   * - DefaultValue: 0
   */
  int m_nUpperRed;
  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
    CameraImage m_image_original;
    /*!
     */
    InPort<CameraImage> m_image_originalIn;

    CameraImage m_image_back;
    /*!
     */
    InPort<CameraImage> m_image_backIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_image_output;
  /*!
   */
  OutPort<CameraImage> m_image_outputOut;
  
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
    // Configuration fileds
    int m_in_height;                 // 入力イメージのHeight
    int m_in_width;                  // 入力イメージのWidth

    int m_in2_height;                // Background入力イメージのHeight
    int m_in2_width;                 // Background入力イメージのWidth

    IplImage* m_image_buff;			 // Original Image
    
    IplImage* m_image_extracted;     // Extracted Image

    IplImage* m_image_mask;			 // Mask Image
    IplImage* m_image_inverseMask;   // Inverse Mask Image
    
    IplImage* m_image_BG_in;         // Background Input image
    IplImage* m_image_BG;            // Background Converted Image(Resized to Camera Image)
    IplImage* m_image_extractedBG;   // Extracted Background Image

    IplImage* m_image_destination;	 // 結果出力用IplImage
    
};


extern "C"
{
  DLL_EXPORT void ChromakeyInit(RTC::Manager* manager);
};

#endif // CHROMAKEY_H
