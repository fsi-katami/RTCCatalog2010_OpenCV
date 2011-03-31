// -*- C++ -*-
/*!
 * @file  IEEE1394Camera.cpp
 * @brief IEEE1394Camera Module
 * @date $Date$
 *
 * $Id$
 */

#include "IEEE1394Camera.h"

//映像を上下変換
void m_flip(char *pBuffer,int Width, int Height, int Channel)
{
	int i;
	int nHeight;
	int nStride;
	char *pSource, *pDestination;
	char *pTempBuffer;

	nStride = Width * Channel;
	nHeight = Height;
	pSource = pBuffer;
	pDestination = pBuffer;
	pDestination += nStride * (nHeight -1);
	pTempBuffer = new char[nStride];

	for(i=0;i<nHeight/2;i++)
	{
		memcpy(pTempBuffer, pDestination, nStride);
		memcpy(pDestination, pSource, nStride);
		memcpy(pSource, pTempBuffer, nStride);
		pSource += nStride;
		pDestination -= nStride;
	}

	delete [] pTempBuffer;

}

// Module specification
// <rtc-template block="module_spec">
static const char* ieee1394camera_spec[] =
  {
    "implementation_id", "IEEE1394Camera",
    "type_name",         "IEEE1394Camera",
    "description",       "IEEE1394Camera Module",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    // Configuration variables
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
IEEE1394Camera::IEEE1394Camera(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_FIN_OUTPORTOut("m_FIN_OUTPORT", m_FIN_OUTPORT)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  registerOutPort("m_FIN_OUTPORT", m_FIN_OUTPORTOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
IEEE1394Camera::~IEEE1394Camera()
{
}



RTC::ReturnCode_t IEEE1394Camera::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t IEEE1394Camera::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t IEEE1394Camera::onActivated(RTC::UniqueId ec_id)
{
  //変数初期化
  hr = NULL;
  pGraph = NULL;
  pSrc = NULL;
  pF = NULL;
  pBuilder = NULL;
  pGrab = NULL;
  pBuffer = NULL;
  v_Width = v_Height = v_Channel = 0;
  bFound = false;

  //イベントハンドル生成
  m_cb.hEvent=CreateEvent(NULL, FALSE, FALSE, NULL);

  //COMライブラリロード
  hr = CoInitialize(NULL);

  if(FAILED(hr))
  {
	std::cout << "CoInitialize Failed!\n";   
	return RTC::RTC_ERROR;
  }
  
  //デバイスドライバを探索ルーチン
  ICreateDevEnum *pDevEnum = NULL;
  IEnumMoniker *pClassEnum = NULL;

  CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
				 	IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));
  
  hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);

  if(SUCCEEDED(hr))
  {
	  IMoniker *pMoniker = NULL;

	  //カテゴリで最初繋がっているデバイス選択
	  if (pClassEnum->Next(1, &pMoniker, NULL) == S_OK)
	  {
		  pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pSrc);
		  pMoniker->Release();
		  bFound = true;
	  }
	  pClassEnum->Release();
  }
  pDevEnum->Release();

  if(!bFound)
  {
	  std::cout<<"ビデオキャプチャデバイスは存在しません"<<std::endl;
	  return RTC::RTC_ERROR;
  }

  //IGraphBuilder生成
  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
					 IID_IGraphBuilder, reinterpret_cast<void**>(&pGraph));
  pGraph->AddFilter(pSrc, L"Video Capture");

  //MediaControl追加
  pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);

  //柔軟なCaptureデバイスを使うためCputreGraphBuilder2使う
  CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
				   IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&pBuilder));

  pBuilder->SetFiltergraph(pGraph);

  //CAPTUREフィルタ
  IAMStreamConfig *pConfig = NULL;
  pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0,
						  pSrc, IID_IAMStreamConfig,reinterpret_cast<void**>(&pConfig));

  //デバイス情報取得
  bFound = false;
  int iCount=0, iSize=0;
  pConfig->GetNumberOfCapabilities(&iCount,&iSize);	
	
  if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
  {
	  for(int iFormat = 0; iFormat < iCount; iFormat++)
	  {
		  VIDEO_STREAM_CONFIG_CAPS scc;
		  AM_MEDIA_TYPE *pmtConfig;
		  hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
		
		  if(SUCCEEDED(hr))
		  {
			  if(pmtConfig->majortype == MEDIATYPE_Video)
			  {
				  VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
				  std::cout<<"Width : "<<pVih->bmiHeader.biWidth<<", Height : "<<abs(pVih->bmiHeader.biHeight);
				  std::cout<<", biBitCount : "<<pVih->bmiHeader.biBitCount<<std::endl;
				  if(RGB_CHANNELBIT <= pVih->bmiHeader.biBitCount)
				  {
					  //画面サイズ
					  v_Width = pVih->bmiHeader.biWidth;
					  v_Height = pVih->bmiHeader.biHeight;
					  v_Channel = RGB_CHANNEL;
					  pConfig->SetFormat(pmtConfig);
					  bFound = true;
				  }
			  }
			  DeleteMediaType(pmtConfig);

			  if(bFound){
				  break;
			  }
		  }
	  }
  }

  //取得するイメージのBuffer準備
  CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
				   IID_IBaseFilter, reinterpret_cast<void**>(&pF));
  pF->QueryInterface(IID_ISampleGrabber,(void**)&pGrab);
  pGraph->AddFilter(pF, L"Sample Grabber");

  AM_MEDIA_TYPE mt;
  ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
  mt.majortype = MEDIATYPE_Video;
  mt.subtype = MEDIASUBTYPE_RGB24;
  pGrab->SetMediaType(&mt);

  pGrab->SetOneShot(FALSE);
  pGrab->SetBufferSamples(FALSE);

  pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSrc, NULL, pF );

  pGrab->GetConnectedMediaType(&mt);
  VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)mt.pbFormat;
	
  // ビデオ ヘッダーには，ビットマップ情報が含まれる
  // ビットマップ情報を BITMAPINFO 構造体にコピー
  BITMAPINFO BitmapInfo;
  ZeroMemory(&BitmapInfo, sizeof(BitmapInfo) );
  memcpy(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader), sizeof(BITMAPINFOHEADER));
  
  buffersize = BitmapInfo.bmiHeader.biSizeImage;
  pBuffer = new char[buffersize];

  //コールバックClassにBufferを渡す
  if(pBuffer !=NULL)
  {
	  m_cb.SetBufferPtr(pBuffer);
  }

  //イメージを取得した後呼ばれるコールバック
  if(pGrab->SetCallback(&m_cb,1)!=S_OK)
  {
	  std::cout<<"SetCallback error!\n";
	  return RTC::RTC_ERROR;
  }

  FreeMediaType(mt);
  //イメージ取得する
  pMediaControl->Run();

  return RTC::RTC_OK;
}


RTC::ReturnCode_t IEEE1394Camera::onDeactivated(RTC::UniqueId ec_id)
{

  if(bFound)
  {
	  pMediaControl->Stop();
	  pMediaControl->Release();
	  pF->Release();
	  
	  pSrc->Release();
	  pGrab->Release();
	  pBuilder->Release();
	  pGraph->Release();
	  
	  m_cb.DetachBuffer();
	  delete [] pBuffer;
  }
  
  if(pSrc!=NULL)
	  pSrc->Release();

  CoUninitialize();
  CloseHandle(m_cb.hEvent);

  return RTC::RTC_OK;
}


RTC::ReturnCode_t IEEE1394Camera::onExecute(RTC::UniqueId ec_id)
{
  static coil::TimeValue tm_pre;
  static int count = 0;

  //イメージ取得するまで待つ
  int ret = WaitForSingleObject(m_cb.hEvent, INFINITE);

  if(ret==WAIT_FAILED || ret==WAIT_ABANDONED || ret==WAIT_TIMEOUT)
  {
	  std::cout<<"WatiForSingleObjec Error!"<<std::endl;
	  return RTC::RTC_ERROR;
  }

  //上下変換
  m_flip(pBuffer, v_Width, abs(v_Height), v_Channel);

  int len = v_Channel * v_Width * abs(v_Height);

  //出力アウトポート設定
  m_FIN_OUTPORT.pixels.length(len);
  m_FIN_OUTPORT.width = v_Width;
  m_FIN_OUTPORT.height = abs(v_Height);

  memcpy((void *)&(m_FIN_OUTPORT.pixels[0]), pBuffer, len);

  m_FIN_OUTPORTOut.write();

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

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t IEEE1394Camera::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t IEEE1394Camera::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void IEEE1394CameraInit(RTC::Manager* manager)
  {
    coil::Properties profile(ieee1394camera_spec);
    manager->registerFactory(profile,
                             RTC::Create<IEEE1394Camera>,
                             RTC::Delete<IEEE1394Camera>);
  }
  
};


