// -*- C++ -*-
/*!
 * @file  IEEE1394Camera.h
 * @brief IEEE1394Camera Module
 * @date  $Date$
 *
 * $Id$
 */

#ifndef IEEE1394CAMERA_H
#define IEEE1394CAMERA_H

//DirectShow
#include <dshow.h>
#include <streams.h>
#include <qedit.h>
#include <comdef.h>

//OpenRTM
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

//C++
#include <string>
#include <iostream>

#define IBITCOUNT 16
#define RGB_CHANNELBIT 8
#define RGB_CHANNEL 3

//コールバッククラス
//カメラから映像を取得した後、呼ばれるクラス、映像はBufferCBメソッドにコピー
//ソースはDirextShowエグザンプル(SampleGrabCB)参考
class CGrabCB: public ISampleGrabberCB
{
public:
	HANDLE hEvent;
    char *m_pImagePtr;
    void SetBufferPtr(char *pBuffer) { m_pImagePtr = pBuffer;}
	void DetachBuffer(void)
	{
		if(m_pImagePtr!=NULL){
			m_pImagePtr = NULL;
		}
	}

    // fake out any COM ref counting
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

    // fake out any COM QI'ing
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
    {
        if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
        {
            *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
            return NOERROR;
        }    
        return E_NOINTERFACE;
    }
    // ISampleGrabberCB methods
    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample) 
    {
        return E_NOTIMPL;
    }
    STDMETHODIMP BufferCB(double SampleTime, BYTE *pSrcBuffer, long BufferLen) 
    {
        memcpy(m_pImagePtr, pSrcBuffer, BufferLen);
		SetEvent(hEvent);
        return S_OK;
    }
    // Constructor
    CGrabCB( )
    {
        m_pImagePtr = NULL;
    }
};

/*!
 * @class IEEE1394Camera
 * @brief IEEE1394Camera Module
 *
 */
class IEEE1394Camera
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  IEEE1394Camera(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~IEEE1394Camera();

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

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  CameraImage m_FIN_OUTPORT;
  /*!
   */
  OutPort<CameraImage> m_FIN_OUTPORTOut;
  
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
  HRESULT hr;
  IGraphBuilder *pGraph;
  ICaptureGraphBuilder2 *pBuilder;
  IBaseFilter *pSrc;
  IBaseFilter *pF;
  IMediaControl *pMediaControl;
  ISampleGrabber  *pGrab;

  char *pBuffer;
  long buffersize;
  int v_Width;
  int v_Height;
  int v_Channel;
  bool bFound;

  CGrabCB m_cb;
};


extern "C"
{
  DLL_EXPORT void IEEE1394CameraInit(RTC::Manager* manager);
};

#endif // IEEE1394CAMERA_H
