// -*- C++ -*-
/*!
 * @file  Edge.cpp
 * @brief Edge image component
 * @date $Date$
 *
 * $Id$
 */

#include "Edge.h"
#include <iostream>

using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* edge_spec[] =
  {
    "implementation_id", "Edge",
    "type_name",         "Edge",
    "description",       "Edge image component",
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
Edge::Edge(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_edge_sobel_xOut("Edge_image_sobel_x", m_image_edge_sobel_x),
	m_image_edge_sobel_yOut("Edge_image_sobel_y", m_image_edge_sobel_y),
	m_image_edge_LAPLACIANOut("Edge_image_LAPLACIAN",m_image_edge_LAPLACIAN)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("Edge_image_sobel_x", m_image_edge_sobel_xOut);
  registerOutPort("Edge_image_sobel_y", m_image_edge_sobel_yOut);
  registerOutPort("Edge_image_LAPLACIAN", m_image_edge_LAPLACIANOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Edge::~Edge()
{
}


/*
RTC::ReturnCode_t Edge::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable

  // </rtc-template>
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Edge::onActivated(RTC::UniqueId ec_id)
{
  //  イメージ用メモリの初期化
  imageBuff = NULL;
  grayImage = NULL;
  destinationImage_x = NULL;
  destinationImage_y = NULL;
  destinationImage_LAPLACIAN = NULL;
  destinationEdge = NULL;
  edgeImage = NULL;

  //  OutPort画面サイズの初期化
  m_image_edge_sobel_x.width = m_image_edge_sobel_y.width = m_image_edge_LAPLACIAN.width = 0;
  m_image_edge_sobel_x.height = m_image_edge_sobel_y.height = m_image_edge_LAPLACIAN.height = 0;

  len = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Edge::onDeactivated(RTC::UniqueId ec_id)
{
  if(imageBuff != NULL)
  {
    //  イメージ用メモリの解放
    cvReleaseImage(&imageBuff);
    cvReleaseImage(&destinationImage_x);
    cvReleaseImage(&destinationImage_y);
    cvReleaseImage(&destinationImage_LAPLACIAN);
    cvReleaseImage(&destinationEdge);
    cvReleaseImage(&grayImage);
    cvReleaseImage(&edgeImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Edge::onExecute(RTC::UniqueId ec_id)
{
	//  新しいデータのチェック
  if(m_image_origIn.isNew()){
	  //  InPortデータの読み込み
	  m_image_origIn.read();

	  // InPortとOutPortの画面サイズ処理およびイメージ用メモリの確保
	  if( m_image_orig.width != m_image_edge_sobel_x.width || m_image_orig.height != m_image_edge_sobel_x.height)
	  {
		  m_image_edge_sobel_x.width = m_image_edge_sobel_y.width = m_image_edge_LAPLACIAN.width = m_image_orig.width;
		  m_image_edge_sobel_x.height = m_image_edge_sobel_y.height = m_image_edge_LAPLACIAN.height = m_image_orig.height;

		  //  InPortのイメージサイズが変更された場合
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&destinationImage_x);
			  cvReleaseImage(&destinationImage_y);
			  cvReleaseImage(&destinationImage_LAPLACIAN);
			  cvReleaseImage(&destinationEdge);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&edgeImage);
		  }

		  //  イメージ用メモリの確保
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  destinationImage_x = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_16S, 1 );
		  destinationImage_y = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_16S, 1 );
		  destinationImage_LAPLACIAN = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_16S, 1 );
		  destinationEdge = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  edgeImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
	  }

	  //  InPortの画面データをコピー
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  //  RGBからグレースケールに変換
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY );

	  //  Sobel_X
	  //  X方向のSobelオペレータをかける
	  cvSobel( grayImage, destinationImage_x, 1, 0, SOBEL_X_APERTURE_SIZE );

	  //  16ビットの符号ありデータを8ビットの符号なしデータに変換する
	  cvConvertScaleAbs( destinationImage_x, destinationEdge, SCALE, SHIFT );

	  //  グレースケールからRGBに変換
	  cvCvtColor( destinationEdge, edgeImage, CV_GRAY2RGB );

	  //  画像データのサイズ取得
	  len = edgeImage->nChannels * edgeImage->width * edgeImage->height;
	  m_image_edge_sobel_x.pixels.length(len);

	  //  反転した画像データをOutPortにコピー
	  memcpy( (void *)&(m_image_edge_sobel_x.pixels[0]), edgeImage->imageData, len );

	  //  反転した画像データをOutPortから出力
	  m_image_edge_sobel_xOut.write();


	  //  Sobel_Y
	  //  Y方向のSobelオペレータをかける
	  cvSobel( grayImage, destinationImage_y, 0, 1, SOBEL_Y_APERTURE_SIZE );

	  cvConvertScaleAbs( destinationImage_y, destinationEdge, SCALE, SHIFT );

	  cvCvtColor( destinationEdge, edgeImage, CV_GRAY2RGB );

	  len = edgeImage->nChannels * edgeImage->width * edgeImage->height;
	  m_image_edge_sobel_y.pixels.length(len);
	  memcpy( (void *)&(m_image_edge_sobel_y.pixels[0]), edgeImage->imageData, len );

	  m_image_edge_sobel_yOut.write();


	  // LAPLACIAN
	  cvLaplace( grayImage, destinationImage_LAPLACIAN, LAPLACIAN_APERTURE_SIZE );

	  cvConvertScaleAbs( destinationImage_LAPLACIAN, destinationEdge, SCALE, SHIFT );

	  cvCvtColor( destinationEdge, edgeImage, CV_GRAY2RGB );

	  len = edgeImage->nChannels * edgeImage->width * edgeImage->height;
	  m_image_edge_LAPLACIAN.pixels.length(len);
	  memcpy( (void *)&(m_image_edge_LAPLACIAN.pixels[0]), edgeImage->imageData, len );

	  m_image_edge_LAPLACIANOut.write();

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Edge::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Edge::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void EdgeInit(RTC::Manager* manager)
  {
    coil::Properties profile(edge_spec);
    manager->registerFactory(profile,
                             RTC::Create<Edge>,
                             RTC::Delete<Edge>);
  }
  
};


