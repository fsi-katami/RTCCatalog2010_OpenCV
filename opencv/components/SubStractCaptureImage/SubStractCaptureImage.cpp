// -*- C++ -*-
/*!
 * @file  SubStractCaptureImage.cpp
 * @brief SubStractCaptureImage component
 * @date $Date$
 *
 * $Id$
 */

#include "SubStractCaptureImage.h"

// Module specification
// <rtc-template block="module_spec">
static const char* substractcaptureimage_spec[] =
  {
    "implementation_id", "SubStractCaptureImage",
    "type_name",         "SubStractCaptureImage",
    "description",       "SubStractCaptureImage component",
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
    "conf.default.output_mode", "0",
    "conf.default.image_height", "240",
    "conf.default.image_width", "320",
    ""
  };
// </rtc-template>

CvSize imageSize;

//char *windowNameCapture = "Capture";					//	キャプチャした画像を表示するウィンドウの名前
//char *windowNameForeground  = "Foreground Mask";		//	前景マスク画像を表示するウィンドウの名前
//char *windowNameStillObjectMask  = "Still Object Mask";	//	静止物体マスク画像を表示するウィンドウの名前
//char *windowNameBackground = "Background";				//	背景画像を表示するウィンドウの名前
//char *windowNameStillObject  = "Still Object";			//	静止物体を表示するウィンドウの名前
//char *windowNameCounter = "Counter";					//	カウンタを表示するウィンドウの名前

int g_temp_w = 0;
int g_temp_h = 0;
int count = 0;

void showFlipImage( char *windowName, IplImage *image ) {
	if ( image->origin == 0 ) {
		//cvFlip( image, image, 0 );
		cvShowImage( windowName, image );
		//cvFlip( image, image, 0 );
	}
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
SubStractCaptureImage::SubStractCaptureImage(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_outOut("output_image", m_image_out),
	m_foreMaskImgOut("foreMaskImg", m_foreMaskImg),
	m_stillMaskImgOut("stillMaskImg", m_stillMaskImg),
	m_backGroundImgOut("backGroundImg", m_backGroundImg),
	m_stillImgOut("stillImg", m_stillImg)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);

  // Set OutPort buffer
  registerOutPort("output_image", m_image_outOut);
  registerOutPort("foreMaskImg", m_foreMaskImgOut);
  registerOutPort("stillMaskImg", m_stillMaskImgOut);
  registerOutPort("backGroundImg", m_backGroundImgOut);
  registerOutPort("stillImg", m_stillImgOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
SubStractCaptureImage::~SubStractCaptureImage()
{
}



RTC::ReturnCode_t SubStractCaptureImage::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("output_mode", m_output_mode, "0");
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SubStractCaptureImage::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t SubStractCaptureImage::onActivated(RTC::UniqueId ec_id)
{
	g_temp_w = 0;
	g_temp_h = 0;
	count = 0;
	
	inputImage = NULL;
	backgroundAverageImage = NULL;
	backgroundThresholdImage = NULL;
	stillObjectAverageImage = NULL;
	stillObjectThresholdImage = NULL;
	stillObjectCounterImage = NULL;
	backgroundDifferenceImage = NULL;
	stillObjectDifferenceImage = NULL;
	thresholdImage32 = NULL;
	thresholdImage = NULL;
	resultImage = NULL;
	backgroundMaskImage = NULL;
	foregroundMaskImage = NULL;
	stillObjectMaskImage = NULL;
	movingObjectMask = NULL;
	backgroundCopyMaskImage = NULL;
	tmpMaskImage = NULL;
	tmp2MaskImage = NULL;
	frameImage32 = NULL;
	backgroundImage = NULL;
	stillObjectImage = NULL;
	outputImage = NULL;

	foreGroundMaskBuff = NULL;
	stillObjectMaskBuff = NULL;
	backGroundBuff = NULL;
	stillObjectImageBuff = NULL;
	stillObjectCounterBuff = NULL;

    return RTC::RTC_OK;
}


RTC::ReturnCode_t SubStractCaptureImage::onDeactivated(RTC::UniqueId ec_id)
{	
	if(inputImage != NULL){
		cvReleaseImage( &inputImage );
	}
	if(backgroundAverageImage != NULL){
		cvReleaseImage( &backgroundAverageImage );
	}
	if(backgroundThresholdImage != NULL){
		cvReleaseImage( &backgroundThresholdImage);
	}
	if(stillObjectAverageImage != NULL){
		cvReleaseImage( &stillObjectAverageImage );
	}
	if(stillObjectThresholdImage != NULL){
		cvReleaseImage( &stillObjectThresholdImage );
	}
	if(stillObjectCounterImage != NULL){
		cvReleaseImage( &stillObjectCounterImage );
	}
	if(backgroundDifferenceImage != NULL){
		cvReleaseImage( &backgroundDifferenceImage );
	}
	if(stillObjectDifferenceImage != NULL){
		cvReleaseImage( &stillObjectDifferenceImage );
	}
	if(thresholdImage32 != NULL){
		cvReleaseImage( &thresholdImage32 );
	}
	if(thresholdImage != NULL){
		cvReleaseImage( &thresholdImage );
	}
	if(resultImage != NULL){
		cvReleaseImage( &resultImage );
	}
	if(backgroundMaskImage != NULL){
		cvReleaseImage( &backgroundMaskImage );
	}
	if(foregroundMaskImage != NULL){
		cvReleaseImage( &foregroundMaskImage );
	}
	if(stillObjectMaskImage != NULL){
		cvReleaseImage( &stillObjectMaskImage );
	}
	if(movingObjectMask != NULL){
		cvReleaseImage( &movingObjectMask );
	}
	if(backgroundCopyMaskImage != NULL){
		cvReleaseImage( &backgroundCopyMaskImage );
	}
	if(tmpMaskImage != NULL){
		cvReleaseImage( &tmpMaskImage );
	}
	if(tmp2MaskImage != NULL){
		cvReleaseImage( &tmp2MaskImage );
	}
	if(frameImage32 != NULL){
		cvReleaseImage( &frameImage32 );
	}
	if(backgroundImage != NULL){
		cvReleaseImage( &backgroundImage );
	}
	if(stillObjectImage != NULL){
		cvReleaseImage( &stillObjectImage );
	}
	if(outputImage != NULL){
		cvReleaseImage( &outputImage );
	}
	
	if(foreGroundMaskBuff != NULL){
		cvReleaseImage( &foreGroundMaskBuff);
	}
	if(stillObjectMaskBuff != NULL){
		cvReleaseImage( &stillObjectMaskBuff);
	}
	if(backGroundBuff != NULL){
		cvReleaseImage( &backGroundBuff);
	}
	if(stillObjectImageBuff != NULL){
		cvReleaseImage( &stillObjectImageBuff);
	}
	if(stillObjectCounterBuff != NULL){
		cvReleaseImage( &stillObjectCounterBuff);
	}
	
	return RTC::RTC_OK;
}


RTC::ReturnCode_t SubStractCaptureImage::onExecute(RTC::UniqueId ec_id)
{	
	
	if(m_image_origIn.isNew() && count == 0){
	
		m_image_origIn.read();

		if(g_temp_w != m_image_orig.width || g_temp_h != m_image_orig.height){
			
			//	画像サイズを保存
			imageSize = cvSize(m_image_orig.width, m_image_orig.height);

			//	画像を生成する
			inputImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			backgroundAverageImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	背景の平均値保存用IplImage
			backgroundThresholdImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	背景の閾値保存用IplImage
			stillObjectAverageImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	静止物体の平均値保存用IplImage
			stillObjectThresholdImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	静止物体の閾値保存用IplImage
			stillObjectCounterImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );		//	静止物体のカウンタ用IplImage
			backgroundDifferenceImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	背景差分画像用IplImage
			stillObjectDifferenceImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );	//	静止物体差分画像用IplIMage
			thresholdImage32 = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );				//	32bitの閾値画像用IplImage
			thresholdImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );					//	閾値画像用IplImage
			resultImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	結果画像用IplImage
			backgroundMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	背景マスク画像用IplImage
			foregroundMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	前景マスク用IplImage
			stillObjectMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	静止物体マスク用IplImage
			movingObjectMask = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );				//	動物体マスク用IplImage
			backgroundCopyMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );		//	背景にコピーする際に使用するマスク用IplImage
			tmpMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	テンポラリ用IplImage
			tmp2MaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	テンポラリ用IplImage(その2)
			frameImage32 = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );					//	32bitのキャプチャした画像用IplImage
			backgroundImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );				//	背景画像用IplImage
			stillObjectImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );
			outputImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);

			foreGroundMaskBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectMaskBuff  = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			backGroundBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectImageBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectCounterBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);

			memcpy(inputImage->imageData,(void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length());

			//	初期化する
			cvConvert( inputImage, backgroundAverageImage );
			cvSet( backgroundThresholdImage, cvScalarAll( BACKGROUND_INITIAL_THRESHOLD ) );
			cvSetZero( stillObjectAverageImage );
			cvSetZero( stillObjectThresholdImage );
			cvSetZero( stillObjectCounterImage );
			
			g_temp_w = m_image_orig.width;
			g_temp_h = m_image_orig.height;

			count = 1;

		}

	}else if(m_image_origIn.isNew() && count != 0 ){
		
		m_image_origIn.read();

		if(g_temp_w == m_image_orig.width && g_temp_h == m_image_orig.height){
			
			memcpy(inputImage->imageData,(void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length());
		
			//	float 32bitに変換する
			cvConvert( inputImage, frameImage32 );

			//	背景との差 /////////////////////////////////////////////////////////

			//	現在の背景との差の絶対値を成分ごとに取る
			cvAbsDiff( frameImage32, backgroundAverageImage, backgroundDifferenceImage );

			//	閾値の値を引く
			cvAddWeighted( backgroundDifferenceImage, 1.0, backgroundThresholdImage, -THRESHOLD_COEFFICIENT, 0.0, thresholdImage32 );

			// thresholdImage の要素が1つでも0以上だったら背景ではない
			cvConvert( thresholdImage32, thresholdImage );
			cvCvtColor( thresholdImage, resultImage, CV_BGR2GRAY );
			cvThreshold( resultImage, backgroundMaskImage, MASK_THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY_INV );

			//	背景候補との差 /////////////////////////////////////////////////////

			//	現在の背景候補との差の絶対値を成分ごとに取る
			cvAbsDiff( frameImage32, stillObjectAverageImage, stillObjectDifferenceImage );

			//	閾値の値を引く
			cvAddWeighted( stillObjectDifferenceImage, 1.0, stillObjectThresholdImage, -THRESHOLD_COEFFICIENT, 0.0, thresholdImage32 );

			//	thresholdImage の要素が1つでも0以上だったら背景候補ではない
			cvConvert( thresholdImage32, thresholdImage );
			cvCvtColor( thresholdImage, resultImage, CV_BGR2GRAY );
			cvThreshold( resultImage, stillObjectMaskImage, MASK_THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY_INV );
			
			//	ここまでで、
			//	backgroundDifferenceImage, backgroundMaskImage
			//	stillObjectDifferenceImage, stillObjectMaskImage
			//	に意味のある値が入る。

			//	各種情報を更新する /////////////////////////////////////////////////

			//	背景に同化する場合 (backgroundMaskImage=1の場合)
			cvRunningAvg( frameImage32, backgroundAverageImage, BACKGROUND_ALPHA, backgroundMaskImage );
			cvRunningAvg( backgroundDifferenceImage, backgroundThresholdImage, BACKGROUND_ALPHA, backgroundMaskImage );

			//	背景候補に同化する場合 (backgroundMaskImage=0 && stillObjectMaskImage=1)
			cvNot( backgroundMaskImage, foregroundMaskImage );
			cvAnd( foregroundMaskImage, stillObjectMaskImage, tmpMaskImage );	//	背景候補
			
			cvRunningAvg( frameImage32, stillObjectAverageImage, STILL_OBJECT_ALPHA, tmpMaskImage );
			cvRunningAvg( stillObjectDifferenceImage, stillObjectThresholdImage, STILL_OBJECT_ALPHA, tmpMaskImage );
						  
			//	背景候補カウンタを増やす
			cvAddS( stillObjectCounterImage, cvScalar( 1 ), stillObjectCounterImage, tmpMaskImage );
			
			//	カウンタが閾値以上になったら、背景候補を背景として採用する
			cvThreshold( stillObjectCounterImage, tmp2MaskImage, STILL_OBJECT_TO_BACKGROUND, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
			cvAnd( tmpMaskImage, tmp2MaskImage, backgroundCopyMaskImage );
			cvCopy( stillObjectAverageImage, backgroundAverageImage, backgroundCopyMaskImage );
			cvCopy( stillObjectThresholdImage, backgroundThresholdImage, backgroundCopyMaskImage );

			//	この backgroundCopyMaskImage は、後で背景候補を0に初期化する際に使用
			cvSet( stillObjectCounterImage, cvScalarAll( 0 ), backgroundCopyMaskImage );

			//	背景候補でもなく、背景でもない場合
			//	(foregroundMaskImage = 1 && stillObjectMaskImage = 0)
			cvNot( stillObjectMaskImage, movingObjectMask );
			
			//	カウンタを減らす(短時,間ノイズ対応)
			//	これは、背景に分類されたピクセルに対しても行う。すなわち、
			//	movingObjectMask == 1 || backgroundMaskImage == 1
			cvOr( backgroundMaskImage, movingObjectMask, tmpMaskImage );
			cvSubS( stillObjectCounterImage, cvScalarAll( NOT_STILL_DEC_STEP ), stillObjectCounterImage, tmpMaskImage );
			
			//	カウンタが0になったら背景候補を初期化する
			cvNot( stillObjectCounterImage, tmp2MaskImage );	// tmp2 = 1 なら初期化

			//	初期化する値の候補は2種類ある。
			//	(1)現在の画像で初期化 --- 背景でもなく背景候補でもない場合
			//	(2)登録なし状態で初期化 --- 背景もしくは背景候補をコピーした場合
			//	ここでは(1)で初期化しておく
			cvOr( tmpMaskImage, backgroundCopyMaskImage, tmpMaskImage );
			cvAnd( tmpMaskImage, tmp2MaskImage, tmpMaskImage );

			cvCopy( frameImage32, stillObjectAverageImage, tmpMaskImage );
			cvSet( stillObjectThresholdImage, cvScalarAll( STILL_OBJECT_INITIAL_THRESHOLD ), tmpMaskImage );
			
			//	ノイズを除去する
			cvSmooth( foregroundMaskImage, foregroundMaskImage, CV_MEDIAN );

			cvConvert( backgroundAverageImage, backgroundImage );
			cvConvert( stillObjectAverageImage, stillObjectImage );

			//	キー入力判定
			cvWaitKey( 1 );
			
			// 画像データのサイズ取得
			double len;

			len = (outputImage->nChannels * outputImage->width * outputImage->height);
			m_image_out.pixels.length(len);

			memcpy((void *)&(m_image_out.pixels[0]), inputImage->imageData, len);
			
			m_image_out.width = m_image_orig.width;
			m_image_out.height = m_image_orig.height;

			m_image_outOut.write();

			cvMerge( foregroundMaskImage, foregroundMaskImage, foregroundMaskImage, NULL, foreGroundMaskBuff);
			
			len = (foreGroundMaskBuff->nChannels * foreGroundMaskBuff->width * foreGroundMaskBuff->height);
			m_foreMaskImg.pixels.length(len);

			memcpy((void *)&(m_foreMaskImg.pixels[0]), foreGroundMaskBuff->imageData, len);
			
			m_foreMaskImg.width = m_image_orig.width;
			m_foreMaskImg.height = m_image_orig.height;

			m_foreMaskImgOut.write();


			cvMerge( stillObjectMaskImage, stillObjectMaskImage, stillObjectMaskImage, NULL, stillObjectMaskBuff );

			len = (stillObjectMaskBuff->nChannels * stillObjectMaskBuff->width * stillObjectMaskBuff->height);
			m_stillMaskImg.pixels.length(len);

			memcpy((void *)&(m_stillMaskImg.pixels[0]), stillObjectMaskBuff->imageData, len);

			m_stillMaskImg.width = m_image_orig.width;
			m_stillMaskImg.height = m_image_orig.height;

			m_stillMaskImgOut.write();

			
			len = (backgroundImage->nChannels * backgroundImage->width * backgroundImage->height);
			m_backGroundImg.pixels.length(len);

			memcpy((void *)&(m_backGroundImg.pixels[0]), backgroundImage->imageData, len);

			m_backGroundImg.width = m_image_orig.width;
			m_backGroundImg.height = m_image_orig.height;

			m_backGroundImgOut.write();


			len = (stillObjectImage->nChannels * stillObjectImage->width * stillObjectImage->height);
			m_stillImg.pixels.length(len);

			memcpy((void *)&(m_stillImg.pixels[0]), stillObjectImage->imageData, len);

			m_stillImg.width = m_image_orig.width;
			m_stillImg.height = m_image_orig.height;

			m_stillImgOut.write();

			/*
			cvMerge( stillObjectCounterImage, stillObjectCounterImage, stillObjectCounterImage, NULL, stillObjectCounterBuff );

			len = (stillObjectCounterBuff->nChannels * stillObjectCounterBuff->width * stillObjectCounterBuff->height);
			m_stillCounterImg.pixels.length(len);

			memcpy((void *)&(m_stillCounterImg.pixels[0]), stillObjectCounterBuff->imageData, len);

			m_stillCounterImg.width = m_image_orig.width;
			m_stillCounterImg.height = m_image_orig.height;

			m_stillCounterImgOut.write();
			*/
			g_temp_w = m_image_orig.width;
			g_temp_h = m_image_orig.height;

			key = '0';
		}

	}

	if(g_temp_w != m_image_orig.width || g_temp_h != m_image_orig.height){

		cvReleaseImage( &inputImage );
		cvReleaseImage( &backgroundAverageImage );
		cvReleaseImage( &backgroundThresholdImage);
		cvReleaseImage( &stillObjectAverageImage );
		cvReleaseImage( &stillObjectThresholdImage );
		cvReleaseImage( &stillObjectCounterImage );
		cvReleaseImage( &backgroundDifferenceImage );
		cvReleaseImage( &stillObjectDifferenceImage );
		cvReleaseImage( &thresholdImage32 );
		cvReleaseImage( &thresholdImage );
		cvReleaseImage( &resultImage );
		cvReleaseImage( &backgroundMaskImage );
		cvReleaseImage( &foregroundMaskImage );
		cvReleaseImage( &stillObjectMaskImage );
		cvReleaseImage( &movingObjectMask );
		cvReleaseImage( &backgroundCopyMaskImage );
		cvReleaseImage( &tmpMaskImage );
		cvReleaseImage( &tmp2MaskImage );
		cvReleaseImage( &frameImage32 );
		cvReleaseImage( &backgroundImage );
		cvReleaseImage( &stillObjectImage );
		cvReleaseImage( &outputImage );

		cvReleaseImage( &foreGroundMaskBuff);
		cvReleaseImage( &stillObjectMaskBuff);
		cvReleaseImage( &backGroundBuff);
		cvReleaseImage( &stillObjectImageBuff);
		cvReleaseImage( &stillObjectCounterBuff);
		
		//g_temp_w = m_image_orig.width;
		//g_temp_h = m_image_orig.height;

		count = 0;
	}

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SubStractCaptureImage::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SubStractCaptureImage::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void SubStractCaptureImageInit(RTC::Manager* manager)
  {
    coil::Properties profile(substractcaptureimage_spec);
    manager->registerFactory(profile,
                             RTC::Create<SubStractCaptureImage>,
                             RTC::Delete<SubStractCaptureImage>);
  }
  
};








