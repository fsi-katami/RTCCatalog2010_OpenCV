// -*- C++ -*-
/*!
 * @file  Rotate.cpp
 * @brief Rotate image component
 * @date $Date$
 *
 * $Id$
 */

#include "Rotate.h"

// Module specification
// <rtc-template block="module_spec">
static const char* rotate_spec[] =
  {
    "implementation_id", "Rotate",
    "type_name",         "Rotate",
    "description",       "Rotate image component",
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
    "conf.default.rotate_angle", "60",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Rotate::Rotate(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_outputOut("output_image", m_image_output)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("output_image", m_image_outputOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Rotate::~Rotate()
{
}



RTC::ReturnCode_t Rotate::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("rotate_angle", m_dbRotate, "60");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Rotate::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Rotate::onActivated(RTC::UniqueId ec_id)
{
    // イメージ用メモリの確保
    m_image_buff = NULL;
    m_image_dest = NULL;

    m_in_height  = 0;
    m_in_width   = 0;

    //	行列を生成する
	m_transformMatrix = cvCreateMat( 2, 3, CV_32FC1);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Rotate::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_dest       != NULL)
        cvReleaseImage(&m_image_dest);

    cvReleaseMat(&m_transformMatrix);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Rotate::onExecute(RTC::UniqueId ec_id)
{
    // Common CV process
    // 新しいデータのチェック
    if (m_image_origIn.isNew()) 
    {
        // InPortデータの読み込み
        m_image_origIn.read();

        // サイズが変わったときだけ再生成する
        if(m_in_height != m_image_orig.height || m_in_width != m_image_orig.width)
        {
            printf("[onExecute] Size of input image is not match!\n");

            m_in_height = m_image_orig.height;
            m_in_width  = m_image_orig.width;
            
            if(m_image_buff != NULL)
                cvReleaseImage(&m_image_buff);
            if(m_image_dest != NULL)
                cvReleaseImage(&m_image_dest);


            // サイズ変換のためTempメモリーをよいする
	        m_image_buff = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_image_dest = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPortの画像データをIplImageのimageDataにコピー
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative process
        CvPoint2D32f center = cvPoint2D32f( m_image_buff->width / 2.0, m_image_buff->height / 2.0);	//回転中心

        //	変換行列を求める
	    cv2DRotationMatrix( center, m_dbRotate, SCALE, m_transformMatrix);

        //	画像の拡大、縮小、回転を行う
	    cvWarpAffine( m_image_buff, m_image_dest, m_transformMatrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll( 0 ) );

        // Common process
        // 画像データのサイズ取得
        int len = m_image_dest->nChannels * m_image_dest->width * m_image_dest->height;

        // 画面のサイズ情報を入れる
        m_image_output.pixels.length(len);        
        m_image_output.width  = m_image_dest->width;
        m_image_output.height = m_image_dest->height;

        // 反転した画像データをOutPortにコピー
        memcpy((void *)&(m_image_output.pixels[0]), m_image_dest->imageData,len);

        // 反転した画像データをOutPortから出力する。
        m_image_outputOut.write();
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Rotate::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Rotate::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void RotateInit(RTC::Manager* manager)
  {
    coil::Properties profile(rotate_spec);
    manager->registerFactory(profile,
                             RTC::Create<Rotate>,
                             RTC::Delete<Rotate>);
  }
  
};


