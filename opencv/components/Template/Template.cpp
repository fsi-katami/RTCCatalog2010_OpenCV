// -*- C++ -*-
/*!
 * @file  Template.cpp
 * @brief Template image component
 * @date $Date$
 *
 * $Id$
 */

#include "Template.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* template_spec[] =
  {
    "implementation_id", "Template",
    "type_name",         "Template",
    "description",       "Template image component",
    "version",           "1.0.0",
    "vendor",            "VenderName",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    // Configuration variables
	"conf.default.image_path","",
	//"conf.default.image_path","template.bmp",
    ""
  };
// </rtc-template>
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Template::Template(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_templateOut("template", m_image_template),
	m_image_pictureOut("picture", m_image_picture)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("template", m_image_templateOut);
  registerOutPort("picture", m_image_pictureOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Template::~Template()
{
}



RTC::ReturnCode_t Template::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("image_path",m_img_path, "template.bmp");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Template::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Template::onActivated(RTC::UniqueId ec_id)
{

  // 対象画像用メモリの初期化
  templateID = -1;
  templateWidth = 0;
  templateHeight = 0;
  templateImage = NULL;
  templateGrayImage = NULL;
  templateBinaryImage = NULL;

  // イメージ用メモリの初期化
  imageBuff = NULL;
  sourceGrayImage = NULL;
  sourceBinaryImage = NULL;
  differenceMapImage = NULL;

  // OutPort１の画面サイズの初期化
  m_image_template.width = 0;
  m_image_template.height = 0;

  len = 0;
  flag = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Template::onDeactivated(RTC::UniqueId ec_id)
{

  if(imageBuff != NULL)
  {
    // イメージ用メモリの解放
	cvReleaseImage(&imageBuff);
	cvReleaseImage(&sourceGrayImage);
	cvReleaseImage(&sourceBinaryImage);
	cvReleaseImage(&differenceMapImage);
  }

  if( templateImage != NULL )
  {
	  cvReleaseImage(&templateImage);
	  cvReleaseImage(&templateGrayImage);
      cvReleaseImage(&templateBinaryImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Template::onExecute(RTC::UniqueId ec_id)
{
  // 新しいデータのチェック
  if(m_image_origIn.isNew())
  {
	  // InPortデータの読み込み
	  m_image_origIn.read();

	  // 対象画像を読み込む
	  templateImage = cvLoadImage( m_img_path, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );

	  if( templateImage == NULL )
	  {
		  templateID = -1 ;
		  templateWidth = templateHeight = 0;
	  }

	  // 対象画像チェック
	  // 対象画像のPathとか名が無い場合テンプレートマッチングしなくて入力されたイメージをそのまま出力
	  if( templateImage != NULL && templateID != templateImage->ID )
	  {
		  // フラッグ設定(正しい対象画像が入力）
		  flag = 1;
		  templateID = templateImage->ID;
		  templateWidth = templateImage->width;
		  templateHeight = templateImage->height;

		  if(templateGrayImage != NULL)
		  {
			  cvReleaseImage(&templateGrayImage);
			  cvReleaseImage(&templateBinaryImage);
		  }

		  // 対象画像用のメモリ確保
		  templateGrayImage = cvCreateImage( cvGetSize(templateImage), IPL_DEPTH_8U, 1 );
		  templateBinaryImage = cvCreateImage( cvGetSize(templateImage), IPL_DEPTH_8U, 1 );

		  // 対象画像ID、サイズ出力
		  cout << "templateID : "<<templateID<<endl;
		  cout << "template - width :"<<templateWidth<<endl;
		  cout << "template - height :"<<templateHeight<<endl;

		  //  RGBからグレースケールに変換する
		  cvCvtColor( templateImage, templateGrayImage, CV_RGB2GRAY );

		  //  グレースケールから2値に変換する
		  cvThreshold( templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

		  // OutPort２用の画面サイズ初期化
		  m_image_picture.width = templateImage->width;
		  m_image_picture.height = templateImage->height;
	  }

	  // InPortとOutPortの画面サイズ処理およびイメージ用メモリの確保(正しい対象画像が入れるとdifferenceMapImageが変換される-フラッグを見て判断）
	  if(m_image_orig.width != m_image_template.width || m_image_orig.height != m_image_template.height || flag == 1)
	  {
		  flag = 0;
		  m_image_template.width = m_image_orig.width;
		  m_image_template.height = m_image_orig.height;

		  // InPortのイメージサイズが変更された場合
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&sourceGrayImage);
			  cvReleaseImage(&sourceBinaryImage);
			  cvReleaseImage(&differenceMapImage);
		  }
		  // イメージ用メモリの確保
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  sourceGrayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  sourceBinaryImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  differenceMapImage = cvCreateImage( cvSize( m_image_orig.width - templateWidth + 1, m_image_orig.height - templateHeight + 1 ), IPL_DEPTH_32F, 1 );
	  }

	  // InPortの画像データをコピー
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  if( templateImage != NULL )
	  {
		  //  RGBからグレースケールに変換する
		  cvCvtColor( imageBuff, sourceGrayImage, CV_RGB2GRAY );

		  //  グレースケールから2値に変換する
		  cvThreshold( sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

		  //  テンプレートマッチングを行う
		  cvMatchTemplate( sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF );

		  //  テンプレートが元画像のどの部分にあるのかという情報を得る
		  cvMinMaxLoc( differenceMapImage, NULL, NULL, &minLocation, NULL, NULL );

		  //  一致する場所を元画像に四角で描く
		  cvRectangle(
			  imageBuff,
			  minLocation,
			  cvPoint( minLocation.x + templateImage->width, minLocation.y + templateImage->height ),
			  CV_RGB( 255, 0, 0 ),
			  LINE_THICKNESS,
			  LINE_TYPE,
			  SHIFT
		  );

		  // 画像データのサイズ取得
		  len = imageBuff->nChannels * imageBuff->width * imageBuff->height;
		  m_image_template.pixels.length(len);

		  //  反転した画像データをOutPortにコピー
		  memcpy( (void *)&(m_image_template.pixels[0]), imageBuff->imageData, len );
	  
		  //  反転した画像データをOutPortから出力
		  m_image_templateOut.write();

		  //  対象画像データのサイズ取得
		  len = templateImage->nChannels * templateImage->width * templateImage->height;
		  m_image_picture.pixels.length(len);

		  //  反転した対象画像データをOutPortにコピー

		  memcpy( (void *)&(m_image_picture.pixels[0]), templateImage->imageData, len );

		  //  反転した対象画像データをOutPortから出力
		  m_image_pictureOut.write();

	  }else{

		  // 画像データのサイズ取得
		  len = imageBuff->nChannels * imageBuff->width * imageBuff->height;
		  m_image_template.pixels.length(len);

		  //  反転した画像データをOutPortにコピー
		  memcpy( (void *)&(m_image_template.pixels[0]), imageBuff->imageData, len );
	  
		  //  反転した画像データをOutPortから出力
		  m_image_templateOut.write();

	  }

	  cvReleaseImage(&templateImage);
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Template::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void TemplateInit(RTC::Manager* manager)
  {
    coil::Properties profile(template_spec);
    manager->registerFactory(profile,
                             RTC::Create<Template>,
                             RTC::Delete<Template>);
  }
  
};


