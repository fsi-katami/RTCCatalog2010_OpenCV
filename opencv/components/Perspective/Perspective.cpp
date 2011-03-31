// -*- C++ -*-
/*!
 * @file  Perspective.cpp
 * @brief Perspective image component
 * @date $Date$
 *
 * $Id$
 */

#include "Perspective.h"

// Module specification
// <rtc-template block="module_spec">
static const char* perspective_spec[] =
  {
    "implementation_id", "Perspective",
    "type_name",         "Perspective",
    "description",       "Perspective image component",
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
Perspective::Perspective(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_outOut("out_image", m_image_out)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("out_image", m_image_outOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Perspective::~Perspective()
{
}


RTC::ReturnCode_t Perspective::onInitialize()
{
    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Perspective::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Perspective::onActivated(RTC::UniqueId ec_id)
{
    // イメージ用メモリの確保
    m_image_buff       = NULL;
    m_image_dest       = NULL;

    m_in_height  = 0;
    m_in_width   = 0;

    //	行列を生成する
	m_perspectiveMatrix = cvCreateMat( 3, 3, CV_32FC1);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Perspective::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_dest         != NULL)
        cvReleaseImage(&m_image_dest);

    cvReleaseMat(&m_perspectiveMatrix);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Perspective::onExecute(RTC::UniqueId ec_id)
{
    // Common CV actions
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
            
            if(m_image_buff       != NULL)
                cvReleaseImage(&m_image_buff);
            if(m_image_dest         != NULL)
                cvReleaseImage(&m_image_dest);


            // サイズ変換のためTempメモリーをよいする
	        m_image_buff = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_image_dest = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPortの画像データをIplImageのimageDataにコピー
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative actions

	    CvPoint2D32f original[4];	//	変換前座標
	    CvPoint2D32f translate[4];	//	変換後座標

	    //	変換前の座標を設定する
	    original[0] = cvPoint2D32f( 0, 0 );
	    original[1] = cvPoint2D32f( m_image_buff->width, 0 );
	    original[2] = cvPoint2D32f( 0, m_image_buff->height );
	    original[3] = cvPoint2D32f( m_image_buff->width, m_image_buff->height );

	    //	変換後の座標を設定する
	    translate[0] = cvPoint2D32f( m_image_buff->width / 5 * 1, m_image_buff->height / 5 * 2 );
	    translate[1] = cvPoint2D32f( m_image_buff->width / 5 * 4, m_image_buff->height / 5 * 2 );
	    translate[2] = cvPoint2D32f(                           0, m_image_buff->height / 5 * 4 );
	    translate[3] = cvPoint2D32f( m_image_buff->width        , m_image_buff->height / 5 * 4 );

	    //	変換行列を求める
	    cvGetPerspectiveTransform( original, translate, m_perspectiveMatrix );

	    //	変換行列を反映させる
	    cvWarpPerspective( m_image_buff, m_image_dest, m_perspectiveMatrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll( 0 ) );

        // 画像データのサイズ取得
        int len = m_image_dest->nChannels * m_image_dest->width * m_image_dest->height;
                
        // 画面のサイズ情報を入れる
        m_image_out.pixels.length(len);        
        m_image_out.width  = m_image_dest->width;
        m_image_out.height = m_image_dest->height;

        // 反転した画像データをOutPortにコピー
        memcpy((void *)&(m_image_out.pixels[0]), m_image_dest->imageData,len);

        // 反転した画像データをOutPortから出力する。
        m_image_outOut.write();
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Perspective::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void PerspectiveInit(RTC::Manager* manager)
  {
    coil::Properties profile(perspective_spec);
    manager->registerFactory(profile,
                             RTC::Create<Perspective>,
                             RTC::Delete<Perspective>);
  }
  
};


