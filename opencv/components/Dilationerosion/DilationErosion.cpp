// -*- C++ -*-
/*!
 * @file  DilationErosion.cpp
 * @brief Dilation and erosion component
 * @date $Date$
 *
 * $Id$
 */

#include "DilationErosion.h"

// Module specification
// <rtc-template block="module_spec">
static const char* dilationerosion_spec[] =
  {
    "implementation_id", "DilationErosion",
    "type_name",         "DilationErosion",
    "description",       "Dilation and erosion component",
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
//    "conf.default.output_mode", "1",
    "conf.default.dilation_count", "1",
    "conf.default.erosion_count", "1",
    "conf.default.image_height", "240",
    "conf.default.image_width", "320",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
DilationErosion::DilationErosion(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_img_origIn("original_image", m_img_orig),
    m_img_outOut("output_image", m_img_out),
	m_img_dilationOut("dilation_image", m_img_dilation),
	m_img_erosionOut("erosion_image", m_img_erosion)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_img_origIn);
  
  // Set OutPort buffer
  registerOutPort("output_image", m_img_outOut);
  registerOutPort("dilation_image", m_img_dilationOut);
  registerOutPort("erosion_image", m_img_erosionOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
DilationErosion::~DilationErosion()
{
}



RTC::ReturnCode_t DilationErosion::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  //bindParameter("output_mode", m_out_mode, "1");
  bindParameter("dilation_count", m_count_dilation, "1");
  bindParameter("erosion_count", m_count_erosion, "1");
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DilationErosion::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t DilationErosion::onActivated(RTC::UniqueId ec_id)
{
  //イメージ用メモリの確保
	
	m_image_buff = NULL;
	m_gray_buff = NULL;
	m_binary_buff = NULL;
	m_dilation_buff = NULL;
	m_erosion_buff = NULL;
	m_output_image_buff = NULL;
	m_merge_Image = NULL;
	m_dilation_image = NULL;
	m_erosion_image = NULL;
	m_dila_merge_img = NULL;
	m_ero_merge_img = NULL;
	m_noise_merge_img = NULL;
	
	return RTC::RTC_OK;
}


RTC::ReturnCode_t DilationErosion::onDeactivated(RTC::UniqueId ec_id)
{
	//メモリ解放
	if(m_image_buff != NULL){
		cvReleaseImage(&m_image_buff);
	}
	if(m_gray_buff != NULL){
		cvReleaseImage(&m_gray_buff);
	}
	if(m_binary_buff != NULL){
		cvReleaseImage(&m_binary_buff);
	}
	if(m_dilation_buff != NULL){
		cvReleaseImage(&m_dilation_buff);
	}
	if(m_erosion_buff != NULL){
		cvReleaseImage(&m_erosion_buff);
	}
	if(m_output_image_buff != NULL){
		cvReleaseImage(&m_output_image_buff);
	}
	if(m_merge_Image != NULL){
		cvReleaseImage(&m_merge_Image);
	}
	if(m_dilation_image != NULL){
		cvReleaseImage(&m_dilation_image);
	}
	if(m_erosion_image != NULL){
		cvReleaseImage(&m_erosion_image);
	}
	if(m_dila_merge_img != NULL){
		cvReleaseImage(&m_dila_merge_img);
	}
	if(m_ero_merge_img != NULL){
		cvReleaseImage(&m_ero_merge_img);
	}
	if(m_noise_merge_img != NULL){
		cvReleaseImage(&m_noise_merge_img);
	}
	
	return RTC::RTC_OK;
}


RTC::ReturnCode_t DilationErosion::onExecute(RTC::UniqueId ec_id)
{

  //新イメージのチェック
	if(m_img_origIn.isNew()){

		//データの読み込み
		m_img_origIn.read();

		m_image_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		m_gray_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_binary_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_dilation_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_erosion_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_output_image_buff = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		m_merge_Image = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		m_dilation_image = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_erosion_image = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		m_dila_merge_img = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		m_ero_merge_img = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		m_noise_merge_img = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);

		// InPortの映像データ
		memcpy(m_image_buff->imageData,(void *)&(m_img_orig.pixels[0]), m_img_orig.pixels.length());

		//	BGRからグレースケールに変換する
		cvCvtColor( m_image_buff, m_gray_buff, CV_BGR2GRAY );

		//	グレースケールから2値に変換する
		cvThreshold( m_gray_buff, m_binary_buff, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

		//Dilation/Erosionを行ってノイズを消す
		cvDilate(m_binary_buff, m_dilation_buff, NULL, m_count_dilation);
		cvErode(m_dilation_buff, m_erosion_buff, NULL, m_count_erosion);

		//Dilationのみ行う
		cvDilate(m_binary_buff, m_dilation_image, NULL, m_count_dilation);

		//Erosionのみ行う
		cvErode(m_binary_buff, m_erosion_image, NULL, m_count_erosion);
		
		// 画像データのサイズ取得
		double len = (m_output_image_buff->nChannels * m_output_image_buff->width * m_output_image_buff->height);
		m_img_out.pixels.length(len);
		m_img_dilation.pixels.length(len);
		m_img_erosion.pixels.length(len);

		//DilationImageをマージする
		cvMerge(m_dilation_image, m_dilation_image, m_dilation_image, NULL, m_dila_merge_img);
		
		//ErosionImageをマージする
		cvMerge(m_erosion_image, m_erosion_image, m_erosion_image, NULL, m_ero_merge_img);

		//ノイズを消したImageをマージする
		cvMerge(m_erosion_buff, m_erosion_buff, m_erosion_buff, NULL, m_noise_merge_img);
		
		// 該当のイメージをMemCopyする
		memcpy((void *)&(m_img_out.pixels[0]), m_noise_merge_img->imageData, len);
		memcpy((void *)&(m_img_dilation.pixels[0]), m_dila_merge_img->imageData, len);
		memcpy((void *)&(m_img_erosion.pixels[0]), m_ero_merge_img->imageData, len);
		
		// 反転した画像データをOutPortから出力する。
		m_img_out.width = m_image_buff->width;
		m_img_out.height = m_image_buff->height;

		m_img_dilation.width = m_image_buff->width;
		m_img_dilation.height = m_image_buff->height;

		m_img_erosion.width = m_image_buff->width;
		m_img_erosion.height = m_image_buff->height;

		m_img_outOut.write();
		m_img_dilationOut.write();
		m_img_erosionOut.write();

		cvReleaseImage(&m_image_buff);
		cvReleaseImage(&m_gray_buff);
		cvReleaseImage(&m_binary_buff);
		cvReleaseImage(&m_dilation_buff);
		cvReleaseImage(&m_erosion_buff);
		cvReleaseImage(&m_output_image_buff);
		cvReleaseImage(&m_merge_Image);
		cvReleaseImage(&m_dilation_image);
		cvReleaseImage(&m_erosion_image);
		cvReleaseImage(&m_dila_merge_img);
		cvReleaseImage(&m_ero_merge_img);
		cvReleaseImage(&m_noise_merge_img);

	}

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DilationErosion::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DilationErosion::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void DilationErosionInit(RTC::Manager* manager)
  {
    coil::Properties profile(dilationerosion_spec);
    manager->registerFactory(profile,
                             RTC::Create<DilationErosion>,
                             RTC::Delete<DilationErosion>);
  }
  
};


