// -*- C++ -*-
/*!
 * @file  Chromakey.cpp
 * @brief Chromakey image component
 * @date $Date$
 *
 * $Id$
 */

#include "Chromakey.h"

// Module specification
// <rtc-template block="module_spec">
static const char* chromakey_spec[] =
  {
    "implementation_id", "Chromakey",
    "type_name",         "Chromakey",
    "description",       "Chromakey image component",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate",    "1000.0",
    // Configuration variables
    "conf.default.lower_blue",   "0",
    "conf.default.upper_blue",   "0",
    "conf.default.lower_yellow", "0",
    "conf.default.upper_yellow", "0",
    "conf.default.lower_red",    "0",
    "conf.default.upper_red",    "0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Chromakey::Chromakey(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_originalIn("original_image", m_image_original),
    m_image_backIn("background_image",   m_image_back),
    m_image_outputOut("chromakey_image", m_image_output)
    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image",   m_image_originalIn);
  registerInPort("background_image", m_image_backIn);
  
  // Set OutPort buffer
  registerOutPort("chromakey_image", m_image_outputOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Chromakey::~Chromakey()
{
}



RTC::ReturnCode_t Chromakey::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
    bindParameter("lower_blue",        m_nLowerBlue,    "0");
    bindParameter("upper_blue",        m_nUpperBlue,    "0");
    bindParameter("lower_yellow",      m_nLowerYellow,  "0");
    bindParameter("upper_yellow",      m_nUpperYellow,  "0");
    bindParameter("lower_red",         m_nLowerRed,     "0");
    bindParameter("upper_red",         m_nUpperRed,     "0");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Chromakey::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Chromakey::onActivated(RTC::UniqueId ec_id)
{
    m_image_buff        = NULL;
    
    m_image_extracted   = NULL;

    m_image_mask        = NULL;
    m_image_inverseMask = NULL;
    
    m_image_BG_in       = NULL;
    m_image_BG          = NULL;
    m_image_extractedBG = NULL;

    m_image_destination = NULL;

    m_in_height         = 0;
    m_in_width          = 0;
    m_in2_height        = 0;
    m_in2_width         = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Chromakey::onDeactivated(RTC::UniqueId ec_id)
{
    // イメージ用メモリの解放
    if(m_image_buff        != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_extracted   != NULL)
        cvReleaseImage(&m_image_extracted);
    if(m_image_mask        != NULL)
        cvReleaseImage(&m_image_mask);
    if(m_image_inverseMask != NULL)
        cvReleaseImage(&m_image_inverseMask);
    if(m_image_BG_in       != NULL)
        cvReleaseImage(&m_image_BG_in);
    if(m_image_BG          != NULL)
        cvReleaseImage(&m_image_BG);
    if(m_image_extractedBG != NULL)
        cvReleaseImage(&m_image_extractedBG);
    if(m_image_destination != NULL)
        cvReleaseImage(&m_image_destination);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Chromakey::onExecute(RTC::UniqueId ec_id)
{
    // Common CV actions
    // Port for Background image
    if (m_image_backIn.isNew()) 
    {
                // InPortデータの読み込み
        m_image_backIn.read();

        // サイズが変わったときだけ再生成する
        if(m_in2_height != m_image_back.height || m_in2_width != m_image_back.width)
        {
            printf("[onExecute] Size of background image is not match!\n");

            m_in2_height = m_image_back.height;
            m_in2_width  = m_image_back.width;
            
            if(m_image_BG_in != NULL)
                cvReleaseImage(&m_image_BG_in);

            // サイズ変換のためTempメモリーをよいする
	        m_image_BG_in = cvCreateImage(cvSize(m_in2_width, m_in2_height), IPL_DEPTH_8U, 3);
        }

        // InPortの画像データをIplImageのimageDataにコピー
        memcpy(m_image_BG_in->imageData,(void *)&(m_image_back.pixels[0]), m_image_back.pixels.length());
    }

    // 新しいデータのチェック
    if (m_image_originalIn.isNew()) 
    {
        // InPortデータの読み込み
        m_image_originalIn.read();

        // サイズが変わったときだけ再生成する
        if(m_in_height != m_image_original.height || m_in_width != m_image_original.width)
        {
            printf("[onExecute] Size of input image is not match!\n");

            m_in_height = m_image_original.height;
            m_in_width  = m_image_original.width;
            
            if(m_image_buff        != NULL)
                cvReleaseImage(&m_image_buff);
            if(m_image_extracted   != NULL)
                cvReleaseImage(&m_image_extracted);
            if(m_image_mask        != NULL)
                cvReleaseImage(&m_image_mask);
            if(m_image_inverseMask != NULL)
                cvReleaseImage(&m_image_inverseMask);
            if(m_image_BG          != NULL)
                cvReleaseImage(&m_image_BG);
            if(m_image_extractedBG != NULL)
                cvReleaseImage(&m_image_extractedBG);
            if(m_image_destination != NULL)
                cvReleaseImage(&m_image_destination);


            // サイズ変換のためTempメモリーをよいする
	        m_image_buff        = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3 );
    
            m_image_extracted   = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3 );

            m_image_mask        = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1 );
            m_image_inverseMask = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1 );
            
            m_image_BG          = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3 );
            m_image_extractedBG = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3 );

            m_image_destination = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3 );
        }

        // Resize background image to fit Camera image
        if(m_image_BG_in != NULL)
            cvResize(m_image_BG_in, m_image_BG, CV_INTER_LINEAR);

        // InPortの画像データをIplImageのimageDataにコピー
        memcpy(m_image_buff->imageData,(void *)&(m_image_original.pixels[0]),m_image_original.pixels.length());

        // Anternative actions

        CvScalar lowerValue = cvScalar( m_nLowerBlue,     m_nLowerYellow,     m_nLowerRed );
		CvScalar upperValue = cvScalar( m_nUpperBlue + 1, m_nUpperYellow + 1, m_nUpperRed + 1 );

		//	RGB各チャンネルごとに範囲内の値以外の画素をマスクに設定する
		cvInRangeS( m_image_buff, lowerValue, upperValue, m_image_mask );

		//	背景画像のうち合成する物体部分の画素値を0にする
		cvSetZero( m_image_extractedBG );
		cvCopy( m_image_BG, m_image_extractedBG, m_image_mask );

		//	マスク画像の0と1を反転する
		cvNot( m_image_mask, m_image_inverseMask );

		//	トラックバーの条件を満たす合成物体が抽出された画像を作成 
		cvSetZero( m_image_extracted );
		cvCopy( m_image_buff, m_image_extracted, m_image_inverseMask );

		//	背景画像と合成物体画像の合成
		cvAdd( m_image_extractedBG, m_image_extracted, m_image_destination, NULL);

       
        // Prepare to out data
        // 画像データのサイズ取得
        int len = m_image_destination->nChannels * m_image_destination->width * m_image_destination->height;
                
        // 画面のサイズ情報を入れる
        m_image_output.pixels.length(len);        
        m_image_output.width  = m_image_destination->width;
        m_image_output.height = m_image_destination->height;

        // 反転した画像データをOutPortにコピー
        memcpy((void *)&(m_image_output.pixels[0]), m_image_destination->imageData,len);

        // 反転した画像データをOutPortから出力する。
        m_image_outputOut.write();
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Chromakey::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Chromakey::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ChromakeyInit(RTC::Manager* manager)
  {
    coil::Properties profile(chromakey_spec);
    manager->registerFactory(profile,
                             RTC::Create<Chromakey>,
                             RTC::Delete<Chromakey>);
  }
  
};


