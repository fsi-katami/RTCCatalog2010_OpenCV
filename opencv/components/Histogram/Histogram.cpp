// -*- C++ -*-
/*!
 * @file  Histogram.cpp
 * @brief Histogram image component
 * @date $Date$
 *
 * $Id$
 */

#include "Histogram.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* histogram_spec[] =
  {
    "implementation_id", "Histogram",
    "type_name",         "Histogram",
    "description",       "Histogram image component",
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
    "conf.default.brightness", "100",
    "conf.default.contrast", "100",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Histogram::Histogram(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
	m_image_histogramImageOut("histogram_image", m_image_histogramImage),
    m_image_histogramOut("histogram", m_image_histogram)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);

  // Set OutPort buffer
  registerOutPort("histogram_image", m_image_histogramImageOut);
  
  // Set OutPort buffer
  registerOutPort("histogram", m_image_histogramOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Histogram::~Histogram()
{
}



RTC::ReturnCode_t Histogram::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("brightness", m_brightness, "100");
  bindParameter("contrast", m_contrast, "100");

  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Histogram::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Histogram::onActivated(RTC::UniqueId ec_id)
{
  //  イメージ用メモリの初期化
  imageBuff = NULL;
  grayImage = NULL;
  destinationImage = NULL;
  histogramImage = NULL;
  histogramBarImage = NULL;

  //  OutPort画面サイズの初期化
  m_image_histogram.width = 0;
  m_image_histogram.height = 0;
  m_image_histogramImage.width = 0;
  m_image_histogramImage.height = 0;

  //  ヒストグラムに描画される縦棒の数
  histogramSize = 128;
  //  ヒストグラムの範囲
  range_0[0] = 0;
  range_0[1] = 256;

  //  ヒストグラム各次元の範囲を示す配列のポインタ
  ranges[0] = range_0 ;

  //  ヒストグラムを生成
  histogram = cvCreateHist( DIMENSIONS, &histogramSize, CV_HIST_ARRAY, ranges, UNIFORM );

  //  行列を生成
  lookUpTableMatrix = cvCreateMatHeader( 1, 256, CV_8UC1 );

  //  濃度対応行列に濃度対応表をセット
  cvSetData( lookUpTableMatrix, lookUpTable, NULL );

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Histogram::onDeactivated(RTC::UniqueId ec_id)
{
  if( imageBuff != NULL )
  {
	  //  イメージ用メモリの解放
	  cvReleaseImage(&imageBuff);
	  cvReleaseImage(&grayImage);
	  cvReleaseImage(&destinationImage);
	  cvReleaseImage(&histogramImage);
	  cvReleaseImage(&histogramBarImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Histogram::onExecute(RTC::UniqueId ec_id)
{
	// 新しいデータのチェック
  if(m_image_origIn.isNew()){
	  //  InPortデータの読み込み
	  m_image_origIn.read();

	  //  InPortとOutPortの画面サイズ処理およびイメージ用メモリ確保
	  if(m_image_orig.width != m_image_histogram.width || m_image_orig.height != m_image_histogram.height)
	  {
		  m_image_histogram.width = m_image_histogramImage.width = m_image_orig.width;
		  m_image_histogram.height = m_image_histogramImage.height = m_image_orig.height;

		  //  InPortのイメージサイズが変更された場合
		  if( imageBuff != NULL )
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&destinationImage);
			  cvReleaseImage(&histogramImage);
			  cvReleaseImage(&histogramBarImage);
		  }

		  //  イメージ用メモリの確保
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  destinationImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  histogramImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  histogramBarImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );

		  //	ヒストグラムの縦棒の横幅を計算する
		  bin_w = cvRound( ( double )histogramBarImage->width / histogramSize );
	  }

	  //  InPortの画面データをコピー
	  memcpy(imageBuff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

	  //  RGBからグレースケールに変換
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY);
	  
	  int brightness = m_brightness - TRACKBAR_MAX_VALUE / 2;	//	輝度値
      int contrast = m_contrast - TRACKBAR_MAX_VALUE / 2;		//	コントラスト

	  if ( contrast > 0 ) {
        double delta = 127.0 * contrast / 100.0;
        double a = 255.0 / ( 255.0 - delta * 2 );
        double b = a * ( brightness - delta );
        for (int i = 0; i < 256; i++ ){
			//	変換後の階調を求める
            int v = cvRound( a * i + b );
			if( v < 0 ){
                v = 0;
			}
			if( v > 255 ){
                v = 255;
			}
            lookUpTable[i] = ( unsigned char )v;
        }
	   } else {
        double delta = -128.0 * contrast / 100.0;
        double a = (256.0 - delta * 2.0) / 255.0;
        double b = a * brightness + delta;
        for(int i = 0; i < 256; i++ ){
            int v = cvRound( a * i + b);
			if( v < 0 ){
                v = 0;
			}
			if( v > 255 ){
                v = 255;
			}
            lookUpTable[i] = ( unsigned char )v;
        }
      }
	  
	  //  濃度対応行列を用いた濃度階調変換を行う
	  cvLUT( grayImage, destinationImage, lookUpTableMatrix );

	  //  グレースケールからRGBに変換
	  cvCvtColor( destinationImage, histogramImage, CV_GRAY2RGB );

	  //  画像データのサイズ取得
	  int len = histogramImage->nChannels * histogramImage->width * histogramImage->height;
	  m_image_histogramImage.pixels.length(len);

	  //  変転した画像データをOutPortにコピー
	  memcpy((void *)&(m_image_histogramImage.pixels[0]), histogramImage->imageData,len);

	  //  変転した画像データをOutPortから出力
	  m_image_histogramImageOut.write();

	  //  画像のヒストグラムを計算する
	  cvCalcHist( &destinationImage, histogram, ACCUMULATE, NULL );

	  float max_value = 0;
	  //  ヒストグラム値の最大値を得る
      cvGetMinMaxHistValue( histogram, NULL, &max_value, NULL, NULL );

	  //  ヒストグラムを最大値によって正規化する
	  cvConvertScale( histogram->bins, histogram->bins, 
		( ( double )histogramBarImage->height ) / max_value, SCALE_SHIFT );

	  //	ヒストグラム画像を白で初期化する
	  cvSet( histogramBarImage, cvScalarAll( 255 ), NULL );

	  //	ヒストグラムの縦棒を描画する
	  for ( int i = 0; i < histogramSize; i++ ) {
          cvRectangle(
		  	histogramBarImage,
		  	cvPoint( i * bin_w, histogramBarImage->height ),
			cvPoint( ( i + 1 ) * bin_w,histogramBarImage->height - cvRound( cvGetReal1D( histogram->bins, i) ) ),
			cvScalarAll( 0 ),
			LINE_THICKNESS,
			LINE_TYPE,
			SHIFT
		  );
	  }

	  //  画像データのサイズ取得
	  len = histogramBarImage->nChannels * histogramBarImage->width * histogramBarImage->height;
	  m_image_histogram.pixels.length(len);

	  //  変転した画像データをOutPortにコピー
	  memcpy((void *)&(m_image_histogram.pixels[0]), histogramBarImage->imageData,len);

	  //  変転した画像データをOutPortから出力
	  m_image_histogramOut.write();

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Histogram::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void HistogramInit(RTC::Manager* manager)
  {
    coil::Properties profile(histogram_spec);
    manager->registerFactory(profile,
                             RTC::Create<Histogram>,
                             RTC::Delete<Histogram>);
  }
  
};


