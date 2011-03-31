// -*- C++ -*-
/*!
 * @file  ImageSubstraction.cpp
 * @brief Image substraction conponent
 * @date $Date$
 *
 * $Id$
 */

#include "ImageSubstraction.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imagesubstraction_spec[] =
  {
    "implementation_id", "ImageSubstraction",
    "type_name",         "ImageSubstraction",
    "description",       "Image substraction conponent",
    "version",           "1.0.0",
    "vendor",            "ASIT",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    // Configuration variables
    "conf.default.image_height", "240",
    "conf.default.image_width", "320",
    ""
  };
// </rtc-template>

int count = 0;
int key;		//	キー入力用の変数
int	mode = 0;	// 0: 画素ごとに異なる閾値 / 1: 画像全体で一つの閾値
int g_temp_w = 0;
int g_temp_h = 0;

//char *windowNameCapture = "Capture";		//	キャプチャした画像を表示するウィンドウの名前
//char *windowNameResult  = "Result";			//	背景差分結果を表示するウィンドウの名前
//char *windowNameBackground = "Background";	//	背景画像を表示するウィンドウの名前
//char *windowNameThreshold = "Threshold";	//	閾値を表示するウィンドウの名前

char *mode_str[2] = {
	"画素単位",
	"画像で一つ"
};

IplImage *backgroundAverageImage = NULL;	//	背景の平均値保存用IplImage
IplImage *backgroundThresholdImage = NULL;	//	背景の閾値保存用IplImage

IplImage *originalImage;		//	キャプチャ画像用IplImage
IplImage *differenceImage;	//	差分画像用IplImage
IplImage *resultImage;	
IplImage *outputImage;

//CvCapture *capture = NULL;	//	カメラキャプチャ用の構造体

//
//	画像を上下反転して表示する
//
//	引数:
//		windowName : 画像を表示するウィンドウの名前
//		image      : 上下反転して表示する画像用IplImage
//
void showFlipImage( char *windowName, IplImage *image ) {
	//if ( image->origin == 0 ) {
		//cvFlip( image, image, 0 );
		//cvShowImage( windowName, image );
		//cvFlip( image, image, 0 );
	//}
}

//
//	背景モデルを初期化する
//
//	引数:
//		num  : 背景モデルを生成するのに使用する画像の枚数
//		size : 画像サイズ
//
void initializeBackgroundModel( int num, CvSize size ){
	int i;

	// 以前の背景情報があれば破棄
	if( backgroundAverageImage != NULL ){
		cvReleaseImage( &backgroundAverageImage );
	}
	if( backgroundThresholdImage != NULL ){
		cvReleaseImage( &backgroundThresholdImage );
	}

	//	画像情報蓄積用バッファを確保する
	IplImage *acc = cvCreateImage( size, IPL_DEPTH_32F, 3 );
	IplImage *acc2 = cvCreateImage( size, IPL_DEPTH_32F, 3 );
	
	//	画像の初期化を行う
	cvSetZero( acc );
	cvSetZero( acc2 );

	//	画像情報の蓄積
	printf( "背景取得中...\n" );
	//IplImage *frameImage;
	for( i = 0; i < num; i++ ){
		//frameImage = cvQueryFrame( capture );
		cvAcc( originalImage, acc );
		cvSquareAcc( originalImage, acc2 );
		printf( "%d 枚中 %d 枚目\n", num, i + 1 );
	}
	printf( "背景取得完了\n" );

	//	cvAddS, cvSubS はあるが cvMulS はないので、cvConvertScale を使う
	cvConvertScale( acc, acc, 1.0 / num );		// 平均
	cvConvertScale( acc2, acc2, 1.0 / num );	// 二乗和の平均

	//	平均が求まったので backgroundAverageImage に格納する。
	backgroundAverageImage = cvCreateImage( size, IPL_DEPTH_8U, 3 );
	cvConvert( acc, backgroundAverageImage );

	//	分散を計算する
	IplImage *dispersion = cvCreateImage( size, IPL_DEPTH_32F, 3 );
	cvMul( acc, acc, acc );
	cvSub( acc2, acc, dispersion );

	//	標準偏差を計算する
	IplImage *sd = cvCreateImage( size, IPL_DEPTH_32F, 3 );
	cvPow( dispersion, sd, 0.5 );

	//	閾値を計算する
	backgroundThresholdImage = cvCreateImage( size, IPL_DEPTH_8U, 3 );
	cvConvertScale( sd, backgroundThresholdImage, THRESHOLD_COEFFICIENT );

	//	メモリを解放する
	cvReleaseImage( &acc );
	cvReleaseImage( &acc2 );
	cvReleaseImage( &dispersion );
	cvReleaseImage( &sd );
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageSubstraction::ImageSubstraction(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_img_origIn("original_image", m_img_orig),
	m_keyIn("Key", m_key),
    m_img_captureOut("capture_image", m_img_capture),
    m_img_resultOut("result_image", m_img_result),
    m_img_backOut("back_image", m_img_back),
    m_img_thresholdOut("threshold_image", m_img_threshold)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_img_origIn);
  registerInPort("key", m_keyIn);
  // Set OutPort buffer
  registerOutPort("capture_image", m_img_captureOut);
  registerOutPort("result_image", m_img_resultOut);
  registerOutPort("back_image", m_img_backOut);
  registerOutPort("threshold_image", m_img_thresholdOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
ImageSubstraction::~ImageSubstraction()
{
}



RTC::ReturnCode_t ImageSubstraction::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageSubstraction::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageSubstraction::onActivated(RTC::UniqueId ec_id)
{
	count = 0;
	g_temp_w = 0;
	g_temp_h = 0;

	originalImage = NULL;
	outputImage = NULL;
	resultImage = NULL;
	differenceImage = NULL;

	return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageSubstraction::onDeactivated(RTC::UniqueId ec_id)
{
	
	if(differenceImage != NULL){
		cvReleaseImage(&differenceImage);
	}
	if(originalImage != NULL){
		cvReleaseImage(&originalImage);
	}
	if(resultImage != NULL){
		cvReleaseImage(&resultImage);
	}
	if(outputImage != NULL){
		cvReleaseImage(&outputImage);
	}
  
	return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageSubstraction::onExecute(RTC::UniqueId ec_id)
{	
	
	//初期値を取得する。
	if(count == 0 && m_img_origIn.isNew()) {
		
		m_img_origIn.read();
		
		if(g_temp_w != m_img_orig.width || g_temp_h != m_img_orig.height){
		
			if(originalImage != NULL){
				cvReleaseImage(&originalImage);	
			}
			if(originalImage == NULL){
				originalImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);		//	キャプチャ画像用IplImage
			}
			if(outputImage != NULL){
				cvReleaseImage(&outputImage);
			}
			if(outputImage == NULL){
				outputImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
			}
			
			memcpy(originalImage->imageData,(void *)&(m_img_orig.pixels[0]), m_img_orig.pixels.length());
			
			if(differenceImage != NULL){
				cvReleaseImage(&differenceImage);
			}
			if(differenceImage == NULL){
				differenceImage = cvCloneImage(originalImage);
			}
			
			if(resultImage != NULL){
				cvReleaseImage(&resultImage);
			}
			if(resultImage == NULL){
				resultImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height),IPL_DEPTH_8U, 1);
			}

			initializeBackgroundModel( NUM_OF_BACKGROUND_FRAMES, cvSize(m_img_orig.width, m_img_orig.height) );
			
			count = 1;
			g_temp_w = m_img_orig.width;
			g_temp_h = m_img_orig.height;
		}
	}

	if(count == 1 && m_img_origIn.isNew()) {
		
		m_img_origIn.read();
		
		if(g_temp_w == m_img_orig.width && g_temp_h == m_img_orig.height){

			if(originalImage != NULL){
				cvReleaseImage(&originalImage);	
			}

			if(originalImage == NULL){
				originalImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);		//	キャプチャ画像用IplImage
			}
			
			if(outputImage != NULL){
				cvReleaseImage(&outputImage);
			}

			if(outputImage == NULL){
				outputImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
			}

			if(m_keyIn.isNew()) {
				m_keyIn.read();
				key = (int)m_key.data;
			}

			memcpy(originalImage->imageData,(void *)&(m_img_orig.pixels[0]), m_img_orig.pixels.length());

			//	現在の背景との差の絶対値を成分ごとに取る
			cvAbsDiff( originalImage, backgroundAverageImage, differenceImage );
			
			//	Sub はマイナスになったら0に切り詰めてくれる
			if( mode == 0 ){
				cvSub( differenceImage, backgroundThresholdImage, differenceImage );
			} else{
				cvSubS( differenceImage, cvScalarAll( CONSTANT_THRESHOLD ), differenceImage );
			}
			
			//	differenceImage の要素が1つでも0以上だったら前景
			cvCvtColor( differenceImage, resultImage, CV_BGR2GRAY );
			cvThreshold( resultImage, resultImage, 0, 255, CV_THRESH_BINARY );
			
			//	メディアンフィルタでノイズを除去する
			cvSmooth( resultImage, resultImage, CV_MEDIAN );
			
			//	画像を表示する
			//cvShowImage( windowNameCapture, originalImage );
			//showFlipImage( windowNameResult, resultImage );
			//showFlipImage( windowNameBackground, backgroundAverageImage );
			
			IplImage *tmp = cvCloneImage( differenceImage );
			cvConvertScale( tmp, tmp, 3 );
			//showFlipImage( windowNameThreshold, tmp );

			cvMerge( resultImage, resultImage, resultImage, NULL, outputImage );
			
			// 画像データのサイズ取得
			double len1 = (originalImage->nChannels * originalImage->width * originalImage->height);
			double len2 = (outputImage->nChannels * outputImage->width * outputImage->height);
			double len3 = (backgroundAverageImage->nChannels * backgroundAverageImage->width * backgroundAverageImage->height);
			double len4 = (tmp->nChannels * tmp->width * tmp->height);
			
			m_img_capture.pixels.length(len1);
			m_img_result.pixels.length(len2);
			m_img_back.pixels.length(len3);
			m_img_threshold.pixels.length(len4);

			// 該当のイメージをMemCopyする
			memcpy((void *)&(m_img_capture.pixels[0]), originalImage->imageData, len1);
			memcpy((void *)&(m_img_result.pixels[0]), outputImage->imageData, len2);
			memcpy((void *)&(m_img_back.pixels[0]), backgroundAverageImage->imageData, len3);
			memcpy((void *)&(m_img_threshold.pixels[0]), tmp->imageData, len4);

			m_img_capture.width = originalImage->width;
			m_img_capture.height = originalImage->height;

			m_img_result.width = originalImage->width;
			m_img_result.height = originalImage->height;

			m_img_back.width = originalImage->width;
			m_img_back.height = originalImage->height;

			m_img_threshold.width = originalImage->width;
			m_img_threshold.height = originalImage->height;

			m_img_captureOut.write();
			m_img_resultOut.write();
			m_img_backOut.write();
			m_img_thresholdOut.write();
			
			cvReleaseImage( &tmp );
			//	キー入力判定
			cvWaitKey( 1 );
			//key = (int)m_key.data;

			if( key == 'b' ){
				//	'b'キーが押されたらその時点での画像を背景画像とする
				initializeBackgroundModel( NUM_OF_BACKGROUND_FRAMES, cvSize(m_img_width, m_img_height));
				
				printf( "背景情報更新\n" );
				
			} else if( key == 'm' ){
				//	'm'キーが押されたら閾値の設定方法を変更する
				mode = 1 - mode;
				printf( "閾値: %s\n", mode_str[mode] );
			}

			cvReleaseImage(&originalImage);
			cvReleaseImage(&outputImage);
			
			key = '0';
			g_temp_w = m_img_orig.width;
			g_temp_h = m_img_orig.height;
		
		}else if(g_temp_w != m_img_orig.width || g_temp_h != m_img_orig.height){
			count = 0;
		}
		
	}

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageSubstraction::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageSubstraction::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ImageSubstractionInit(RTC::Manager* manager)
  {
    coil::Properties profile(imagesubstraction_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageSubstraction>,
                             RTC::Delete<ImageSubstraction>);
  }
  
};


