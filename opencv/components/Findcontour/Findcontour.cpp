// -*- C++ -*-
/*!
 * @file  Findcontour.cpp
 * @brief Findcontour component
 * @date $Date$
 *
 * $Id$
 */

#include "Findcontour.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* findcontour_spec[] =
  {
    "implementation_id", "Findcontour",
    "type_name",         "Findcontour",
    "description",       "Findcontour component",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Findcontour::Findcontour(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_contourOut("contour_image", m_image_contour)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("contour_image", m_image_contourOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Findcontour::~Findcontour()
{
}


/*
RTC::ReturnCode_t Findcontour::onInitialize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Findcontour::onActivated(RTC::UniqueId ec_id)
{
  //  イメージ用メモリの初期化
  imageBuff = NULL;
  grayImage = NULL;
  binaryImage = NULL;
  contourImage = NULL;

  //  OutPort画面サイズの初期化
  m_image_contour.width = 0;
  m_image_contour.height = 0;

  find_contour = NULL;
  red = CV_RGB( 255, 0, 0 );

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Findcontour::onDeactivated(RTC::UniqueId ec_id)
{
  if(imageBuff != NULL )
  {
    //  イメージ用メモリの解放
	cvReleaseImage(&imageBuff);
	cvReleaseImage(&grayImage);
    cvReleaseImage(&binaryImage);
    cvReleaseImage(&contourImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Findcontour::onExecute(RTC::UniqueId ec_id)
{
	//  新しいデータのチェック
  if(m_image_origIn.isNew()){
	  //  InPortデータの読み込み
	  m_image_origIn.read();

	  //  InPortとOutPortの画面サイズ処理およびイメージ用メモリ確保
	  if( m_image_orig.width != m_image_contour.width || m_image_orig.height != m_image_contour.height)
	  {
		  m_image_contour.width = m_image_orig.width;
		  m_image_contour.height = m_image_orig.height;

		  //  InPortのイメージサイズが変更された場合
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&binaryImage);
			  cvReleaseImage(&contourImage);
		  }

		  //  イメージ用メモリの確保
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  binaryImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1);
		  contourImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3);
	  }

	  //  InPortの画面データをコピー
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );
	  memcpy( contourImage->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  //  RGBからグレースケールに変換
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY);

	  //  グレースケールから2値に変換する
	  cvThreshold( grayImage, binaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	  //  抽出された輪郭を保存する領域 
	  CvMemStorage* storage = cvCreateMemStorage( 0 );
	
	  //  2値画像中の輪郭を見つけ、その数を返す
	  find_contour_num = cvFindContours( 
		binaryImage,			//	入力画像(８ビットシングルチャンネル）
		storage,				//	抽出された輪郭を保存する領域
		&find_contour,			//	一番外側の輪郭へのポインタへのポインタ
		sizeof( CvContour ),	//	シーケンスヘッダのサイズ
		CV_RETR_LIST,			//	抽出モード 
		CV_CHAIN_APPROX_NONE,	//	推定手法
		cvPoint( 0, 0 )			//	オフセット
	  );

	  cvDrawContours( 
		contourImage,			//	輪郭を描画する画像
		find_contour,			//	最初の輪郭へのポインタ
		red,					//	外側輪郭線の色
		red,					//	内側輪郭線（穴）の色
		CONTOUR_MAX_LEVEL,		//	描画される輪郭の最大レベル
		LINE_THICKNESS,			//	描画される輪郭線の太さ
		LINE_TYPE,				//	線の種類
		cvPoint( 0, 0 )			//	オフセット
	  );

	  //  画像データのサイズ取得
	  int len = contourImage->nChannels * contourImage->width * contourImage->height;
	  m_image_contour.pixels.length(len);

	  //  変転した画像データをOutPortにコピー
	  memcpy((void *)&(m_image_contour.pixels[0]), contourImage->imageData, len);

	  //  変転した画像データをOutPortから出力
	  m_image_contourOut.write();

	  //  抽出された輪郭を解放
	  cvReleaseMemStorage( &storage );

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Findcontour::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void FindcontourInit(RTC::Manager* manager)
  {
    coil::Properties profile(findcontour_spec);
    manager->registerFactory(profile,
                             RTC::Create<Findcontour>,
                             RTC::Delete<Findcontour>);
  }
  
};


