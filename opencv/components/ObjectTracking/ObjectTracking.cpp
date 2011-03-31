// -*- C++ -*-
/*!
 * @file  ObjectTracking.cpp
 * @brief Objecttrack component
 * @date $Date$
 *
 * $Id$
 */

#include "ObjectTracking.h"

// Module specification
// <rtc-template block="module_spec">
static const char* objecttracking_spec[] =
  {
    "implementation_id", "ObjectTracking",
    "type_name",         "ObjectTracking",
    "description",       "Objecttrack component",
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
    "conf.default.image_height", "240",
    "conf.default.image_width", "320",
    ""
  };
// </rtc-template>

IplImage	*inputImage = NULL;			//入力されたIplImage
IplImage	*resultImage = NULL;			//	処理結果表示用IplImage
IplImage	*hsvImage = NULL;			//	HSV表色系用IplImage
IplImage	*hueImage = NULL;			//	HSV表色系のHチャンネル用IplImage
IplImage	*maskImage = NULL;			//	マスク画像用IplImage
IplImage	*backprojectImage = NULL;	//	バックプロジェクション画像用IplImage
IplImage	*histImage = NULL;			//	ヒストグラム描画用IplImage
IplImage	*grayImage = NULL;			//	グレースケール画像用IplImage

CvHistogram	*hist = NULL;				//	ヒストグラム処理用構造体

IplImage	*frameImage;	//	キャプチャ画像用IplImage
CvCapture	*capture;		//	キー入力結果を格納する変数
int count  = 0;
int g_temp_w = 0;
int g_temp_h = 0;

//	処理モード選択用フラグ
int	backprojectMode = HIDDEN_BACKPROJECTION;
int	selectObject = SELECT_OFF;
int	trackObject = TRACKING_STOP;
int showHist = SHOW_HISTOGRAM;

//	CamShiftトラッキング用変数
CvPoint			origin;
CvRect			selection;
CvRect			trackWindow;
CvBox2D			trackRegion;
CvConnectedComp	trackComp;

//	ヒストグラム用変数
int		hdims = H_DIMENSION;		//	ヒストグラムの次元数
float	hRangesArray[] = {H_RANGE_MIN, H_RANGE_MAX};	//ヒストグラムのレンジ
float	*hRanges = hRangesArray;
int		vmin = V_MIN;
int		vmax = V_MAX;

//char	*windowNameObjectTracking = "CaptureImage";

//
//	マウスドラッグによって初期追跡領域を指定する
//
//	引数:
//		event	: マウス左ボタンの状態
//		x		: マウスが現在ポイントしているx座標
//		y		: マウスが現在ポイントしているy座標
//		flags	: 本プログラムでは未使用
//		param	: 本プログラムでは未使用
//
static void on_mouse( int event, int x, int y, int flags, void* param ){
	//	画像が取得されていなければ、処理を行わない
	if( resultImage == NULL ){
        return;
	}

	//	原点の位置に応じてyの値を反転（画像の反転ではない）
	if( resultImage->origin == 1 ){
        y = resultImage->height - y;
	}
	//	マウスの左ボタンが押されていれば以下の処理を行う
    if( selectObject == SELECT_ON ){
        selection.x = MIN( x, origin.x );
        selection.y = MIN( y, origin.y );
        selection.width = selection.x + CV_IABS( x - origin.x );
        selection.height = selection.y + CV_IABS( y - origin.y );
        
        selection.x = MAX( selection.x, 0 );
        selection.y = MAX( selection.y, 0 );
        selection.width = MIN( selection.width, resultImage->width );
        selection.height = MIN( selection.height, resultImage->height );
        selection.width = selection.width - selection.x;
        selection.height = selection.height - selection.y;
    }
	//	マウスの左ボタンの状態によって処理を分岐
    switch( event ){
		case CV_EVENT_LBUTTONDOWN:
			//	マウスの左ボタンが押されたのであれば、
			//	原点および選択された領域を設定
			origin = cvPoint( x, y );
			selection = cvRect( x, y, 0, 0 );
			selectObject = SELECT_ON;
			break;
		case CV_EVENT_LBUTTONUP:
			//	マウスの左ボタンが離されたとき、widthとheightがどちらも正であれば、
			//	trackObjectフラグをTRACKING_STARTにする
			selectObject = SELECT_OFF;
			if( selection.width > 0 && selection.height > 0 ){
				trackObject = TRACKING_START;
			}
			break;
    }
}

//
//	入力された1つの色相値をRGBに変換する
//
//	引数:
//		hue		: HSV表色系における色相値H
//	戻り値：
//		CvScalar: RGBの色情報がBGRの順で格納されたコンテナ
//
CvScalar hsv2rgb( float hue ){
	IplImage *rgbValue, *hsvValue;
	rgbValue = cvCreateImage( cvSize(1,1), IPL_DEPTH_8U, 3 );
	hsvValue = cvCreateImage( cvSize(1,1), IPL_DEPTH_8U, 3 );

	hsvValue->imageData[0] = hue;	//	色相値H
	hsvValue->imageData[1] = 255;	//	彩度値S
	hsvValue->imageData[2] = 255;	//	明度値V
	
	//	HSV表色系をRGB表色系に変換する
	cvCvtColor( hsvValue, rgbValue, CV_HSV2BGR );

	return cvScalar(	(unsigned char)rgbValue->imageData[0], 
						(unsigned char)rgbValue->imageData[1], 
						(unsigned char)rgbValue->imageData[2], 
						0 );

	//	メモリを解放する
	cvReleaseImage( &rgbValue );
	cvReleaseImage( &hsvValue );
}


//
//	マウス選択された初期追跡領域におけるHSVのH値でヒストグラムを作成し、ヒストグラムの描画までを行う
//
//	引数:
//		hist		: mainで宣言されたヒストグラム用構造体
//		hsvImage	: 入力画像がHSV表色系に変換された後のIplImage
//		maskImage	: マスク画像用IplImage
//		selection	: マウスで選択された矩形領域
//
void CalculateHist( CvHistogram	*hist, IplImage *hsvImage, IplImage *maskImage, CvRect selection ){
	int		i;
	int		binW;	//	ヒストグラムの各ビンの、画像上での幅
	int		val;	//	ヒストグラムの頻度
	float	maxVal;	//	ヒストグラムの最大頻度


	//	hsv画像の各画素が値の範囲内に入っているかチェックし、
	//	マスク画像maskImageを作成する
	cvInRangeS( hsvImage, 
				cvScalar( H_RANGE_MIN, S_MIN, MIN(V_MIN,V_MAX), 0 ),
				cvScalar( H_RANGE_MAX, S_MAX, MAX(V_MIN,V_MAX), 0 ), 
				maskImage );
	//	hsvImageのうち、とくに必要なHチャンネルをhueImageとして分離する
	cvSplit( hsvImage, hueImage, 0, 0, 0 );
	//	trackObjectがTRACKING_START状態なら、以下の処理を行う
	if( trackObject == TRACKING_START ){
		//	追跡領域のヒストグラム計算とhistImageへの描画
		maxVal = 0.0;

		cvSetImageROI( hueImage, selection );
        cvSetImageROI( maskImage, selection );
        //	ヒストグラムを計算し、最大値を求める
		cvCalcHist( &hueImage, hist, 0, maskImage );
		cvGetMinMaxHistValue( hist, 0, &maxVal, 0, 0 );
        //	ヒストグラムの縦軸（頻度）を0-255のダイナミックレンジに正規化
		if( maxVal == 0.0 ){
			cvConvertScale( hist->bins, hist->bins, 0.0, 0 );
		} else{
			cvConvertScale( hist->bins, hist->bins, 255.0 / maxVal, 0 );
		}
		//	hue,mask画像に設定されたROIをリセット
		cvResetImageROI( hueImage );
        cvResetImageROI( maskImage );

        trackWindow = selection;
        //	trackObjectをTRACKING_NOWにする
		trackObject = TRACKING_NOW;

		//	ヒストグラム画像をゼロクリア
        cvSetZero( histImage );
		//	各ビンの幅を決める
        binW = histImage->width / hdims;
		//	ヒストグラムを描画する
        for( i = 0; i < hdims; i++ ){
			val = cvRound( cvGetReal1D(hist->bins,i) * histImage->height / 255 );
            CvScalar color = hsv2rgb( i * 180.0 / hdims );
            cvRectangle(	histImage, 
							cvPoint( i * binW, histImage->height ), 
							cvPoint( (i+1) * binW, histImage->height - val ),
							color,
							-1, 
							8, 
							0	);
		}
	}
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ObjectTracking::ObjectTracking(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_orig_imgIn("img_original", m_orig_img),
	m_eventIn("m_event", m_event),
	m_xIn("m_x", m_x),
	m_yIn("m_y", m_y),
    m_out_imgOut("img_output", m_out_img),
    m_hist_imgOut("img_hist", m_hist_img)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("img_original", m_orig_imgIn);
  registerInPort("m_event", m_eventIn);
  registerInPort("m_x", m_xIn);
  registerInPort("m_y", m_yIn);
  // Set OutPort buffer
  registerOutPort("img_output", m_out_imgOut);
  registerOutPort("img_hist", m_hist_imgOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
ObjectTracking::~ObjectTracking()
{
}



RTC::ReturnCode_t ObjectTracking::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ObjectTracking::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ObjectTracking::onActivated(RTC::UniqueId ec_id)
{	
	g_temp_w = 0;
	g_temp_h = 0;
	
	//各メモリ確保
	inputImage = NULL;
	resultImage = NULL;
	hsvImage = NULL;
	hueImage = NULL;
	maskImage = NULL;
	backprojectImage = NULL;
	grayImage = NULL;
	histImage = NULL;

	return RTC::RTC_OK;
}


RTC::ReturnCode_t ObjectTracking::onDeactivated(RTC::UniqueId ec_id)
{
	//メモリから解放
	if(inputImage != NULL){
		cvReleaseImage(&inputImage);
	}
	if(resultImage != NULL){
		cvReleaseImage(&resultImage);
	}
	if(hsvImage != NULL){
		cvReleaseImage(&hsvImage);
	}
	if(hueImage != NULL){
		cvReleaseImage(&hueImage);
	}
	if(hueImage != NULL){
		cvReleaseImage(&maskImage);
	}
	if(backprojectImage != NULL){
		cvReleaseImage(&backprojectImage);
	}
	if(grayImage != NULL){
		cvReleaseImage(&grayImage);
	}
	if(histImage != NULL){
		cvReleaseImage(&histImage);
	}

	return RTC::RTC_OK;
}


RTC::ReturnCode_t ObjectTracking::onExecute(RTC::UniqueId ec_id)
{
	int		key;	//	キー入力結果を格納する変数
	int		i;
	int		j;
	
	int x;
	int y;
	int mouse_event;

	//	Snake用のパラメータ
	float alpha = 1.0;		//	連続エネルギーの重みパラメータ
	float beta = 0.5;		//	曲率の重みパラメータ
	float gamma = 1.5;		//	画像エネルギーの重みパラメータ
	CvPoint pt[SEGMENT];	//	制御点の座標
	CvSize window;			//	最小値を探索する近傍サイズ
	window.width = WINDOW_WIDTH;	
	window.height = WINDOW_HEIGHT;
	CvTermCriteria crit;
	crit.type = CV_TERMCRIT_ITER;		//	終了条件の設定
	crit.max_iter = ITERATION_SNAKE;	//	関数の最大反復数

	if(m_orig_imgIn.isNew()){

		m_orig_imgIn.read();
			
		//各メモリ確保
		if(inputImage == NULL){
			inputImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&inputImage);
			inputImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		if(resultImage == NULL){
			resultImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&resultImage);
			resultImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		resultImage->origin = inputImage->origin;
		if(hsvImage == NULL){
			hsvImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&hsvImage);
			hsvImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		if(hueImage == NULL){
			hueImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&hueImage);
			hueImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(maskImage == NULL){
			maskImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&maskImage);
			maskImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(backprojectImage == NULL){
			backprojectImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&backprojectImage);
			backprojectImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(grayImage == NULL){
			grayImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&grayImage);
			grayImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 1);
		}
		//	ヒストグラム構造体の使用を宣言
		if(hist == NULL){
			hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hRanges, 1 );
		}
		//	ヒストグラム用の画像を確保し、ゼロクリア
		if(histImage == NULL){
			histImage = cvCreateImage( cvSize(HISTIMAGE_WIDTH, HISTIMAGE_HEIGHT), IPL_DEPTH_8U, 3 );
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&histImage);
			histImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		cvSetZero( histImage );

		//InPortの映像の取得
		memcpy(inputImage->imageData,(void *)&(m_orig_img.pixels[0]),m_orig_img.pixels.length());

		//	キャプチャされた画像をresultImageにコピーし、HSV表色系に変換してhsvImageに格納
		cvCopy( inputImage, resultImage, NULL );
		cvCvtColor( resultImage, hsvImage, CV_BGR2HSV );

		//WindowのEvent情報の取得に対する処理
		if(m_eventIn.isNew() && m_xIn.isNew() && m_yIn.isNew()){
			m_xIn.read();
			m_yIn.read();
			m_eventIn.read();

			x = m_x.data;
			y = m_y.data;
			mouse_event = m_event.data;

			on_mouse(mouse_event, x, y, 0, 0);

			x= 0;
			y= 0;
			mouse_event = 0;
		}
		
		//	trackObjectフラグがTRACKING_STOP以外なら、以下の処理を行う
        if( trackObject != TRACKING_STOP ){
			
			//追跡領域のヒストグラム計算と描画
			CalculateHist(	hist, hsvImage, maskImage, selection );

			//	バックプロジェクションを計算する
            cvCalcBackProject( &hueImage, backprojectImage, hist );
            //	backProjectionのうち、マスクが1であるとされた部分のみ残す
			cvAnd( backprojectImage, maskImage, backprojectImage, 0 );

			//	CamShift法による領域追跡を実行する
			cvCamShift( backprojectImage, 
						trackWindow, 
						cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), 
						&trackComp, 
						&trackRegion );
			
			trackWindow = trackComp.rect;

			//	SnakeImage用のグレースケール画像を作成する
			cvCvtColor( resultImage, grayImage, CV_BGR2GRAY );

			if( backprojectMode == SHOW_BACKPROJECTION ){
                cvCvtColor( backprojectImage, resultImage, CV_GRAY2BGR );
			}
			if( resultImage->origin == 1 ){
                trackRegion.angle = -trackRegion.angle;
			}

			//	CamShiftでの領域追跡結果をSnakeの初期位置に設定する
			for( i=0; i<SEGMENT; i++ ){
				pt[i].x = cvRound(	trackRegion.size.width 
									* cos(i * 6.28 / SEGMENT + trackRegion.angle) 
									/ 2.0 + trackRegion.center.x );
				pt[i].y = cvRound(	trackRegion.size.height 
									* sin(i * 6.28 / SEGMENT + trackRegion.angle) 
									/ 2.0 + trackRegion.center.y );
			}
			//	Snakeによる輪郭抽出を実行する
			for( i=0; i<ITERATION_SNAKE; i++ ){
				cvSnakeImage(	grayImage, 
								pt, 
								SEGMENT, 
								&alpha, 
								&beta, 
								&gamma, 
								CV_VALUE, 
								window, 
								crit, 
								1);
				//	各輪郭点の間に線をひいて輪郭線を描画する
				for( j=0; j<SEGMENT; j++ ){
					if( j < SEGMENT-1 ){
						cvLine( resultImage, pt[j], pt[j+1], 
						  cvScalar(0,0,255,0), 2, 8, 0 );
					}
					else{ 
						cvLine( resultImage, pt[j], pt[0], 
						  cvScalar(0,0,255,0),  2, 8, 0 );
					}
				}
			}
        }

		//	マウスで選択中の初期追跡領域の色を反転させる
		if( selectObject == SELECT_ON && selection.width > 0 && selection.height > 0 ){
			
			cvSetImageROI( resultImage, selection );
			cvXorS( resultImage, cvScalarAll(255), resultImage, 0 );
			cvResetImageROI( resultImage );
		}
		//	backprojectImageの座標原点が左上の場合、上下を反転させる
		if( backprojectImage->origin == 0 ){
			cvFlip( backprojectImage, backprojectImage, 0 );
		}
				
		//	画像を表示する
		//cvShowImage( windowNameObjectTracking, resultImage );
		//key = cvWaitKey(1);

		// 画像データのサイズ取得
		double len = (resultImage->nChannels * resultImage->width * resultImage->height);
		double leng = (histImage->nChannels * histImage->width * histImage->height);
		m_out_img.pixels.length(len);
		m_hist_img.pixels.length(leng);

		// 該当のイメージをMemCopyする
		memcpy((void *)&(m_out_img.pixels[0]), resultImage->imageData, len);
		memcpy((void *)&(m_hist_img.pixels[0]), histImage->imageData, leng);

		// 反転した画像データをOutPortから出力する。
		m_out_img.width = inputImage->width;
		m_out_img.height = inputImage->height;

		m_hist_img.width = inputImage->width;
		m_hist_img.height = inputImage->height;

		m_out_imgOut.write();
		m_hist_imgOut.write();
		
		if(inputImage != NULL){
			cvReleaseImage(&inputImage);
		}
		if(resultImage != NULL){
			cvReleaseImage(&resultImage);
		}
		if(hsvImage != NULL){
			cvReleaseImage(&hsvImage);
		}
		if(hueImage != NULL){
			cvReleaseImage(&hueImage);
		}
		if(hueImage != NULL){
			cvReleaseImage(&maskImage);
		}
		if(backprojectImage != NULL){
			cvReleaseImage(&backprojectImage);
		}
		if(grayImage != NULL){
			cvReleaseImage(&grayImage);
		}
		if(histImage != NULL){
			cvReleaseImage(&histImage);
		}
	
	}

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ObjectTracking::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ObjectTracking::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ObjectTrackingInit(RTC::Manager* manager)
  {
    coil::Properties profile(objecttracking_spec);
    manager->registerFactory(profile,
                             RTC::Create<ObjectTracking>,
                             RTC::Delete<ObjectTracking>);
  }
  
};


