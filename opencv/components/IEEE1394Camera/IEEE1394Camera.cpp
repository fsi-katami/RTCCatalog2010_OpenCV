// -*- C++ -*-
/*!
 * @file  IEEE1394Camera.cpp
 * @brief IEEE1394Camera Module
 * @date $Date$
 *
 * $Id$
 */

#include "IEEE1394Camera.h"

//�f�����㉺�ϊ�
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
  //�ϐ�������
  hr = NULL;
  pGraph = NULL;
  pSrc = NULL;
  pF = NULL;
  pBuilder = NULL;
  pGrab = NULL;
  pBuffer = NULL;
  v_Width = v_Height = v_Channel = 0;
  bFound = false;

  //�C�x���g�n���h������
  m_cb.hEvent=CreateEvent(NULL, FALSE, FALSE, NULL);

  //COM���C�u�������[�h
  hr = CoInitialize(NULL);

  if(FAILED(hr))
  {
	std::cout << "CoInitialize Failed!\n";   
	return RTC::RTC_ERROR;
  }
  
  //�f�o�C�X�h���C�o��T�����[�`��
  ICreateDevEnum *pDevEnum = NULL;
  IEnumMoniker *pClassEnum = NULL;

  CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
				 	IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));
  
  hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);

  if(SUCCEEDED(hr))
  {
	  IMoniker *pMoniker = NULL;

	  //�J�e�S���ōŏ��q�����Ă���f�o�C�X�I��
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
	  std::cout<<"�r�f�I�L���v�`���f�o�C�X�͑��݂��܂���"<<std::endl;
	  return RTC::RTC_ERROR;
  }

  //IGraphBuilder����
  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
					 IID_IGraphBuilder, reinterpret_cast<void**>(&pGraph));
  pGraph->AddFilter(pSrc, L"Video Capture");

  //MediaControl�ǉ�
  pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);

  //�_���Capture�f�o�C�X���g������CputreGraphBuilder2�g��
  CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
				   IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&pBuilder));

  pBuilder->SetFiltergraph(pGraph);

  //CAPTURE�t�B���^
  IAMStreamConfig *pConfig = NULL;
  pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0,
						  pSrc, IID_IAMStreamConfig,reinterpret_cast<void**>(&pConfig));

  //�f�o�C�X���擾
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
					  //��ʃT�C�Y
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

  //�擾����C���[�W��Buffer����
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
	
  // �r�f�I �w�b�_�[�ɂ́C�r�b�g�}�b�v��񂪊܂܂��
  // �r�b�g�}�b�v���� BITMAPINFO �\���̂ɃR�s�[
  BITMAPINFO BitmapInfo;
  ZeroMemory(&BitmapInfo, sizeof(BitmapInfo) );
  memcpy(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader), sizeof(BITMAPINFOHEADER));
  
  buffersize = BitmapInfo.bmiHeader.biSizeImage;
  pBuffer = new char[buffersize];

  //�R�[���o�b�NClass��Buffer��n��
  if(pBuffer !=NULL)
  {
	  m_cb.SetBufferPtr(pBuffer);
  }

  //�C���[�W���擾������Ă΂��R�[���o�b�N
  if(pGrab->SetCallback(&m_cb,1)!=S_OK)
  {
	  std::cout<<"SetCallback error!\n";
	  return RTC::RTC_ERROR;
  }

  FreeMediaType(mt);
  //�C���[�W�擾����
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

  //�C���[�W�擾����܂ő҂�
  int ret = WaitForSingleObject(m_cb.hEvent, INFINITE);

  if(ret==WAIT_FAILED || ret==WAIT_ABANDONED || ret==WAIT_TIMEOUT)
  {
	  std::cout<<"WatiForSingleObjec Error!"<<std::endl;
	  return RTC::RTC_ERROR;
  }

  //�㉺�ϊ�
  m_flip(pBuffer, v_Width, abs(v_Height), v_Channel);

  int len = v_Channel * v_Width * abs(v_Height);

  //�o�̓A�E�g�|�[�g�ݒ�
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


