// -*- C++ -*-
/*!
 * @file  BackGroundSubtractionSimple.cpp
 * @brief BackGroundSubtractionSimple component
 * @date $Date$
 *
 * $Id$
 */

#include "BackGroundSubtractionSimple.h"

// Module specification
// <rtc-template block="module_spec">
static const char* backgroundsubtractionsimple_spec[] =
  {
    "implementation_id", "BackGroundSubtractionSimple",
    "type_name",         "BackGroundSubtractionSimple",
    "description",       "BackGroundSubtractionSimple component",
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
    "conf.default.control_mode", "a",
    "conf.default.image_height", "240",
    "conf.default.image_width", "320",
    ""
  };
// </rtc-template>

int key;

//char windowNameCurrent[] = "Current";		//	現在の画像を表示するウィンドウの名前
//char windowNameResult[] = "Result";			//	背景差分結果を表示するウィンドウの名前
//char windowNameBackground[] = "Background";	//	背景画像を表示するウィンドウの名前

int	captureOn = CAPTURE_ON;				//	背景差分を行う画像を更新するかどうか
int	differenceMode = COLOR_DIFFERENCE;	//	差分の計算モード
int	noiseMode = NOISE_KEEP;				//	ノイズを除去するモード

int g_temp_w = 0;
int g_temp_h = 0;


IplImage *originalImage = NULL;
IplImage *currentImage = NULL;
IplImage *backgroundImage = NULL;
IplImage *resultImage = NULL;
IplImage *outputImage = NULL;

char *differenceMethod[3] = {
	"RGBの成分ごとに評価",
	"CIE L*a*b* で距離を評価",
	"グレースケールで評価"
};

char *noiseMethod[3] = {
	"なし",
	"オープニング",
	"メディアンフィルタ"
};

void colorDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){
	
	//	画像を生成する
	IplImage *differenceImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 3);	//	差分画像用IplImage
	IplImage *differenceRImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	R値の差分用IplImage
	IplImage *differenceGImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	G値の差分用IplImage
	IplImage *differenceBImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	B値の差分用IplImage

	//	現在の背景との差の絶対値を成分ごとに取る
	cvAbsDiff( currentImage, backgroundImage, differenceImage );

	//	閾値処理を行う
	cvThreshold( differenceImage, differenceImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	成分ごとの画像に分割する
	cvSplit( differenceImage, differenceBImage, differenceGImage, differenceRImage, NULL );

	//	ORで合成する
	cvOr( differenceRImage, differenceGImage, resultImage );
	cvOr( differenceBImage, resultImage, resultImage );

	//	メモリを解放する
	cvReleaseImage( &differenceImage );
	cvReleaseImage( &differenceRImage );
	cvReleaseImage( &differenceGImage );
	cvReleaseImage( &differenceBImage );

}

//
//	L*a*b*で距離を評価する
//
//	引数:
//		currentImage    : 現在の画像用IplImage
//		backgroundImage : 背景画像用IplImage
//		resultImage     : 結果画像用IplImage
//
void labDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){

	//	画像を生成する
	IplImage *currentLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height),IPL_DEPTH_32F, 3 );		//	現在の画像をL*a*b*に変換した画像用IplImage
	IplImage *backgroundLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 3 );	//	背景をL*a*b*に変換した画像用IplImage
	IplImage *differenceLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 3 );	//	差分画像用IplImage
	IplImage *differenceLImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	L*値の差分用IplImage
	IplImage *differenceAImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	a*値の差分用IplImage
	IplImage *differenceBImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	b*値の差分用IplImage
	IplImage *sqrDifferenceImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );	//	距離算出用IplImage

	//	現在の画像と背景を共に CIE L*a*b* に変換
	cvConvertScale( currentImage, currentLabImage, SCALE );
	cvConvertScale( backgroundImage, backgroundLabImage, SCALE );
	cvCvtColor( currentLabImage, currentLabImage, CV_BGR2Lab );
	cvCvtColor( backgroundLabImage, backgroundLabImage, CV_BGR2Lab );

	//	距離の二乗を計算する
	cvSub( currentLabImage, backgroundLabImage, differenceLabImage );
	cvPow( differenceLabImage, differenceLabImage, 2 );

	//	成分ごとの画像に分割する
	cvSplit( differenceLabImage, differenceLImage, differenceAImage, differenceBImage, NULL );

	cvCopy( differenceLImage, sqrDifferenceImage );
	cvAdd( differenceAImage, sqrDifferenceImage, sqrDifferenceImage );
	cvAdd( differenceBImage, sqrDifferenceImage, sqrDifferenceImage );

	//	閾値処理を行う
	cvThreshold( sqrDifferenceImage, resultImage, THRESHOLD * THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	メモリを解放する
	cvReleaseImage( &currentLabImage );
	cvReleaseImage( &backgroundLabImage );
	cvReleaseImage( &differenceLabImage );
	cvReleaseImage( &differenceLImage );
	cvReleaseImage( &differenceAImage );
	cvReleaseImage( &differenceBImage );
	cvReleaseImage( &sqrDifferenceImage );
}

//
//	グレースケールで評価する
//
//	引数:
//		currentImage    : 現在の画像用IplImage
//		backgroundImage : 背景画像用IplImage
//		resultImage     : 結果画像用IplImage
//
void grayScaleDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){

	//	画像を生成する
	IplImage *differenceImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 3 );	//	差分画像用IplImage

	//	現在の背景との差の絶対値を成分ごとに取る
	cvAbsDiff( currentImage, backgroundImage, differenceImage );

	//	BGRからグレースケールに変換する
	cvCvtColor( differenceImage, resultImage, CV_BGR2GRAY );

	//	グレースケールから2値に変換する
	cvThreshold( resultImage, resultImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	メモリを解放する
	cvReleaseImage( &differenceImage );
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
BackGroundSubtractionSimple::BackGroundSubtractionSimple(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_img_origIn("original_image", m_img_orig),
	m_keyIn("key", m_key),
    m_img_currOut("current_image", m_img_curr),
    m_img_resuOut("result_image", m_img_resu),
    m_img_backOut("background_image", m_img_back)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_img_origIn);
  registerInPort("key", m_keyIn);

  // Set OutPort buffer
  registerOutPort("current_image", m_img_currOut);
  registerOutPort("result_image", m_img_resuOut);
  registerOutPort("background_image", m_img_backOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
BackGroundSubtractionSimple::~BackGroundSubtractionSimple()
{
}



RTC::ReturnCode_t BackGroundSubtractionSimple::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("control_mode", m_cont_mode, "a");
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t BackGroundSubtractionSimple::onActivated(RTC::UniqueId ec_id)
{	
	captureOn = CAPTURE_ON;				//	背景差分を行う画像を更新するかどうか
    differenceMode = COLOR_DIFFERENCE;	//	差分の計算モード
	noiseMode = NOISE_KEEP;				//	ノイズを除去するモード

	g_temp_w = 0;
	g_temp_h = 0;

	if(originalImage != NULL){
		cvReleaseImage(&originalImage);
	}
	if(currentImage != NULL){
		cvReleaseImage(&currentImage);
	}
	if(resultImage != NULL){
		cvReleaseImage(&resultImage);
	}
	if(outputImage != NULL){
		cvReleaseImage(&outputImage);
	}
	if(backgroundImage != NULL){
		cvReleaseImage(&backgroundImage);
	}

	return RTC::RTC_OK;
}


RTC::ReturnCode_t BackGroundSubtractionSimple::onDeactivated(RTC::UniqueId ec_id)
{
	if(originalImage != NULL){
		cvReleaseImage(&originalImage);
	}
	if(currentImage != NULL){
		cvReleaseImage(&currentImage);
	}
	if(resultImage != NULL){
		cvReleaseImage(&resultImage);
	}
	if(outputImage != NULL){
		cvReleaseImage(&outputImage);
	}
	if(backgroundImage != NULL){
		cvReleaseImage(&backgroundImage);
	}

	return RTC::RTC_OK;
}


RTC::ReturnCode_t BackGroundSubtractionSimple::onExecute(RTC::UniqueId ec_id)
{
	
	if(m_img_origIn.isNew()) {
		
		//イメージRead
		m_img_origIn.read();
		
		if(originalImage == NULL){
			originalImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		}
		if(currentImage == NULL){
			currentImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		}

		if(m_img_orig.width != g_temp_w || m_img_orig.height != g_temp_h){
			
			if(backgroundImage != NULL){
				cvReleaseImage(&backgroundImage);
			}
			backgroundImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		}
		
		if(resultImage == NULL){
			resultImage =  cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 1);
		}
		if(outputImage == NULL){
			outputImage = cvCreateImage(cvSize(m_img_orig.width, m_img_orig.height), IPL_DEPTH_8U, 3);
		}

		//	画像を1枚キャプチャし背景として保存する
		//backgroundImage = cvCloneImage( originalImage );

		//	現在の画像としても1枚確保する
		//currentImage = cvCloneImage( originalImage );

		//Key入力Read
		if(m_keyIn.isNew()){
			m_keyIn.read();
			//Intに変換
			key = (int)m_key.data;
		}
		
		//InPortの映像の取得
		memcpy(originalImage->imageData,(void *)&(m_img_orig.pixels[0]),m_img_orig.pixels.length());

		if( captureOn != 0){
			if( currentImage != NULL){
				cvReleaseImage( &currentImage );
			}
			currentImage = cvCloneImage( originalImage );
		}
		
		//	差の計算方法の切り替え
		if( differenceMode == COLOR_DIFFERENCE ){	
			//	成分ごとに評価をする
			colorDifference( currentImage, backgroundImage, resultImage );
		} else if( differenceMode == LAB_DIFFERENCE ){	
			//	L*a*b*で距離を評価する
			labDifference( currentImage, backgroundImage, resultImage );
		} else if( differenceMode == GRAY_DIFFERENCE ){
			//	グレースケールで評価をする
			grayScaleDifference( currentImage, backgroundImage, resultImage );
		}
		
		//	ノイズ除去
		if( noiseMode == NOISE_MORPHOLOGY ){
			cvErode( resultImage, resultImage );
			cvDilate( resultImage, resultImage );
		}else if ( noiseMode == NOISE_MEDIAN ){
			cvSmooth( resultImage, resultImage, CV_MEDIAN );
		}
		
		if( resultImage->origin == 0 ){
			//　左上が原点の場合
			//cvFlip( resultImage, resultImage, 0 );
		}
		
		cvMerge( resultImage, resultImage, resultImage, NULL, outputImage );

		// 画像データのサイズ取得
		double len1 = (currentImage->nChannels * currentImage->width * currentImage->height);
		double len2 = (outputImage->nChannels * outputImage->width * outputImage->height);
		double len3 = (backgroundImage->nChannels * backgroundImage->width * backgroundImage->height);

		m_img_curr.pixels.length(len1);
		m_img_resu.pixels.length(len2);
		m_img_back.pixels.length(len3);

		// 該当のイメージをMemCopyする
		memcpy((void *)&(m_img_curr.pixels[0]), currentImage->imageData, len1);
		memcpy((void *)&(m_img_resu.pixels[0]), outputImage->imageData, len2);
		memcpy((void *)&(m_img_back.pixels[0]), backgroundImage->imageData, len3);

		m_img_curr.width = originalImage->width;
		m_img_curr.height = originalImage->height;

		m_img_resu.width = originalImage->width;
		m_img_resu.height = originalImage->height;

		m_img_back.width = originalImage->width;
		m_img_back.height = originalImage->height;

		m_img_currOut.write();
		m_img_resuOut.write();
		m_img_backOut.write();

		//	画像を表示する
		//cvShowImage( windowNameCurrent, currentImage );
		//cvShowImage( windowNameResult, resultImage );
		//cvShowImage( windowNameBackground, backgroundImage );
		
		//key = cvWaitKey(1);
		cvWaitKey(1);
		
		if(key == 'b'){
			if(backgroundImage != NULL) {
				cvReleaseImage(&backgroundImage);
			}
			backgroundImage = cvCloneImage(originalImage);
			//backgroundImage = NULL;
			printf( "背景画像更新\n" );
		}else if(key == ' '){
			captureOn = 1 - captureOn;
			if(captureOn == CAPTURE_ON){
				printf("映像取得：ON\n");
			}else if(captureOn == CAPTURE_OFF){
				printf("映像取得：OFF\n");
			}
		}else if(key == 'm'){
			differenceMode = differenceMode + 1;
			if( differenceMode > GRAY_DIFFERENCE ){
				differenceMode = COLOR_DIFFERENCE;
			}
			printf( "差の評価方法: %s\n", differenceMethod[differenceMode] );
		}else if( key == 'n' ){ 
			//	'n'キーが押されたらノイズ除去方法を変更する
			noiseMode = noiseMode + 1;
			if( noiseMode > NOISE_MEDIAN ){
				noiseMode = NOISE_KEEP;
			}
			printf( "ノイズ除去方法: %s\n", noiseMethod[noiseMode] );
		}

		key = '0';

		if(originalImage != NULL){
			cvReleaseImage(&originalImage);
		}
		if(currentImage != NULL){
			cvReleaseImage(&currentImage);
		}
		if(resultImage != NULL){
			cvReleaseImage(&resultImage);
		}
		if(outputImage != NULL){
			cvReleaseImage(&outputImage);
		}
		//if(backgroundImage != NULL){
		//	cvReleaseImage(&backgroundImage);
		//}
		
		g_temp_w = m_img_orig.width;
		g_temp_h = m_img_orig.height;
			
	}

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BackGroundSubtractionSimple::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void BackGroundSubtractionSimpleInit(RTC::Manager* manager)
  {
    coil::Properties profile(backgroundsubtractionsimple_spec);
    manager->registerFactory(profile,
                             RTC::Create<BackGroundSubtractionSimple>,
                             RTC::Delete<BackGroundSubtractionSimple>);
  }
  
};


