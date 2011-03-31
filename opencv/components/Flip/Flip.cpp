// -*- C++ -*-
/*!
 * @file  Flip.cpp
 * @brief Flip image component
 * @date $Date$
 *
 * $Id$
 */


#include "Flip.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* flip_spec[] =
  {
    "implementation_id", "Flip",
    "type_name",         "Flip",
    "description",       "Flip image component",
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
    "conf.default.flip_mode", "1",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Flip::Flip(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_flipOut("fliped_image", m_image_flip)
    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("fliped_image", m_image_flipOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Flip::~Flip()
{
}



RTC::ReturnCode_t Flip::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("flip_mode", m_flip_mode, "1");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Flip::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Flip::onActivated(RTC::UniqueId ec_id)
{
  // イメージ用メモリの初期化
  m_image_buff = NULL;
  m_flip_image_buff = NULL;

  // OutPortの画面サイズの初期化
  m_image_flip.width = 0;
  m_image_flip.height = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Flip::onDeactivated(RTC::UniqueId ec_id)
{
  if(m_image_buff != NULL)
  {
    // イメージ用メモリの解放
    cvReleaseImage(&m_image_buff);
    cvReleaseImage(&m_flip_image_buff);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Flip::onExecute(RTC::UniqueId ec_id)
{
	// 新しいデータのチェック
  if (m_image_origIn.isNew()) {
    // InPortデータの読み込み
    m_image_origIn.read();

	// InPortとOutPortの画面サイズ処理およびイメージ用メモリの確保
	if( m_image_orig.width != m_image_flip.width || m_image_orig.height != m_image_flip.height)
	{
		m_image_flip.width = m_image_orig.width;
		m_image_flip.height = m_image_orig.height;

		// InPortのイメージサイズが変更された場合
		if(m_image_buff != NULL)
		{
			cvReleaseImage(&m_image_buff);
			cvReleaseImage(&m_flip_image_buff);
		}

		// イメージ用メモリの確保
		m_image_buff = cvCreateImage(cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3);
		m_flip_image_buff = cvCreateImage(cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3);
	}

    // InPortの画像データをIplImageのimageDataにコピー
    memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

    // InPortからの画像データを反転する。 m_flip_mode 0: X軸周り, 1: Y軸周り, -1: 両方の軸周り
    cvFlip(m_image_buff, m_flip_image_buff, m_flip_mode);

    // 画像データのサイズ取得
    int len = m_flip_image_buff->nChannels * m_flip_image_buff->width * m_flip_image_buff->height;
    m_image_flip.pixels.length(len);

    // 反転した画像データをOutPortにコピー
    memcpy((void *)&(m_image_flip.pixels[0]),m_flip_image_buff->imageData,len);

    // 反転した画像データをOutPortから出力する。
    m_image_flipOut.write();
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Flip::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Flip::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void FlipInit(RTC::Manager* manager)
  {
    coil::Properties profile(flip_spec);
    manager->registerFactory(profile,
                             RTC::Create<Flip>,
                             RTC::Delete<Flip>);
  }
  
};


