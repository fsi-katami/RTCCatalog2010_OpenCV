// -*- C++ -*-
/*!
 * @file  RockPaperScissors.cpp
 * @brief RockpaperScissors check compoenet
 * @date $Date$
 *
 * $Id$
 */
#include "RockPaperScissors.h"
using namespace std;


//追加
CvCapture *capture = NULL;

// Module specification
// <rtc-template block="module_spec">
static const char* rockpaperscissors_spec[] =
  {
    "implementation_id", "RockPaperScissors",
    "type_name",         "RockPaperScissors",
    "description",       "RockpaperScissors check compoenet",
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
	"conf.default.out_mode","1",
    ""
  };

// </rtc-template>

Label *createLabeling(){
	return new LabelingBS();
}

int exec(Label *label,IplImage *target,IplImage *result,
		 const bool is_sort_region,int region_size_min){
	return label->Exec((unsigned char *)target->imageData,(short *)result->imageData,
		target->width,target->height,is_sort_region,region_size_min);
}

int getNumOfResultRegions(Label *label){
	return label->GetNumOfResultRegions();
}

void releaseLabeling(Label *label){
	delete label;
}

	//
	//	肌色を抽出する
	//
	//	引数:
	//		frameImage : キャプチャした画像用IplImage
	//		hsvImage   : HSV画像用IplImage
	//		skinImage  : 肌色抽出画像用IplImage
	//
void extractSkinColor( IplImage *frameImage, IplImage *hsvImage, IplImage *skinImage ) {
	CvScalar color;		//	HSV表色系で表した色
	unsigned char h;	//	H成分
	unsigned char s;	//	S成分
	unsigned char v;	//	V成分
	
	//	BGRからHSVに変換する
	
	cvCvtColor( frameImage, hsvImage, CV_BGR2HSV );
	
	//肌色抽出
	for( int x = 0; x < skinImage->width; x++ ) {
		for( int y = 0 ; y < skinImage->height; y++ ) {

			color = cvGet2D( hsvImage, y, x );
			h = color.val[0];
			s = color.val[1];
			v = color.val[2];

			if( h <= HMAX && h >= HMIN &&
				s <= SMAX && s >= SMIN &&
					v <= VMAX && v >= VMIN ) {
				//	肌色の場合
				cvSetReal2D( skinImage, y, x, 255 );
			} else {
				cvSetReal2D( skinImage, y, x, 0 );
			}
		}
	}
}

	//
	//	欠損領域を補間する
	//
	//	引数:
	//		skinImage : 肌色抽出画像用IplImage
	//		temp      : 一時保存用IplImage
	//
void interpolate( IplImage *skinImage, IplImage *temp ) {
	//膨張をITERATIONS回行う
	cvDilate( skinImage, temp, NULL, ITERATIONS );

	//収縮をITERATIONS回行う
	cvErode( temp, skinImage, NULL, ITERATIONS );
}

	//
	//	最大領域(手領域)の抽出を行う
	//
	//	引数:
	//		skinImage       : 肌色抽出画像用IplImage
	//		label           : ラベリングした結果
	//		convexHullImage : ConvexHull画像用IplImage
	//
	//	戻り値:
	//		手領域の面積
	//
int pickupMaxArea(IplImage *skinImage, IplImage *label, IplImage *convexHullImage ) {

	int handarea = 0;	//	手領域の面積

	for(int x = 0; x < skinImage->width; x++ ) {
		for( int y=0; y < skinImage->height; y++ ) {
			if( cvGetReal2D( label, y, x ) == 1 ) {
				//	最大領域だった場合
				handarea++;
				cvSet2D( convexHullImage, y, x, CV_RGB( 255, 255, 255 ) );
			} else {
				cvSetReal2D( skinImage, y, x, 0 );
				cvSet2D( convexHullImage, y, x, CV_RGB( 0, 0, 0 ) );
			}
		}
	}
	return handarea;
}

	//
	//	ConvexHullを生成する
	//
	//	引数:
	//		skinImage   : 肌色抽出画像用IplImage
	//		handarea    : 手領域の面積(点の数)
	//		handpoint   : 手領域内の点の座標配列へのポインタ
	//		hull        : ConvexHullの頂点のhandpointにおけるindex番号へのポインタ
	//		pointMatrix : 手領域用行列へのポインタ
	//		hullMatrix  : ConvexHull用行列へのポインタ
	//
void createConvexHull(IplImage *skinImage, int handarea, CvPoint **handpoint, int **hull,
					  CvMat *pointMatrix, CvMat *hullMatrix ) {
	int i=0;

	//	ConvexHullを計算するために必要な行列を生成する
	*handpoint=( CvPoint * )malloc( sizeof( CvPoint ) * handarea );
	*hull = ( int * )malloc( sizeof( int ) * handarea );
	*pointMatrix = cvMat( 1, handarea, CV_32SC2, *handpoint );
	*hullMatrix = cvMat( 1, handarea, CV_32SC1, *hull );

	for( int x = 0; x < skinImage->width; x++ ) {
		for(  int y = 0; y < skinImage->height; y++ ) {
			if( cvGetReal2D( skinImage, y, x ) == 255 ) {
				( *handpoint )[i].x = x;
				( *handpoint )[i].y = y;
				i++;
			}
		}
	}

	//	ConvexHullを生成する
	cvConvexHull2( pointMatrix, hullMatrix, CV_CLOCKWISE, 0 );
}

	//
	//	ConvexHullを描画する
	//
	//	引数:
	//		convexHullImage : ConvexHull画像用IplImage
	//		handpoint       : 手領域内の点の座標配列
	//		hull            : ConvexHullの頂点のhandpointにおけるindex番号
	//		hullcount       : ConvexHullの頂点の数
	//
void drawConvexHull(IplImage *convexHullImage, CvPoint *handpoint, int *hull, int hullcount ) {
	CvPoint pt0 = handpoint[hull[hullcount-1]];
	for( int i = 0; i < hullcount; i++ ) {
		CvPoint pt = handpoint[hull[i]];
		cvLine( convexHullImage, pt0, pt, CV_RGB( 0, 255, 0 ) );
		pt0 = pt;
	}
}

	//
	//	ConvexHull内の面積を求める
	//
	//	引数:
	//		convexHullImage : ConvexHull画像用IplImage
	//		handpoint       : 手領域内の点の座標配列
	//		hull            : ConvexHullの頂点のhandpointにおけるindex番号
	//		hullcount       : ConvexHullの頂点の数　　
	//
	//	戻り値:
	//		ConvexHull内の面積
	//
int calcConvexHullArea( IplImage *convexHullImage, CvPoint *handpoint, int *hull, int hullcount ) {

	//	ConvexHullの頂点からなる行列を生成
	CvPoint *hullpoint = ( CvPoint * )malloc( sizeof( CvPoint ) * hullcount );
	CvMat hMatrix = cvMat( 1, hullcount, CV_32SC2, hullpoint );
	for( int i = 0; i < hullcount; i++ ) {
		hullpoint[i]=handpoint[hull[i]];
	}

	//	ConvexHull内の点の数を数える
	int hullarea = 0;
	for( int x = 0; x < convexHullImage->width; x++ ) {
		for( int y = 0;y < convexHullImage->height; y++ ) {
			if( cvPointPolygonTest( &hMatrix, cvPoint2D32f( x, y ), 0 ) > 0) {
				hullarea++;
			}
		}
	}

	free( hullpoint );
	return hullarea;
}

	//
	//	ジャンケンの判定を行う
	//
	//	引数:
	//		handarea : 手領域の面積
	//		hullarea : ConvexHull内の面積
	//
void decide( int handarea, int hullarea ) {
	double ratio;	//	ConvexHull内の面積に対する手領域の面積の割合
	
	ratio=handarea / ( double )hullarea;	
	printf( "Ratio = %lf\n", ratio );

	if( ratio >= ROCKMIN && ratio <= ROCKMAX ) {
		printf( "グー\n" );
	} else if( ratio >= SCISSORMIN && ratio <= SCISSORMAX ) {
		printf( "チョキ\n" );
	} else if( ratio >= PAPERMIN && ratio <= PAPERMAX ) {
		printf( "パー\n" );
	}
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
RockPaperScissors::RockPaperScissors(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_img_inputIn("image_input", m_img_input),
    m_img_outputOut("image_output", m_img_output)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("image_input", m_img_inputIn);
  
  // Set OutPort buffer
  registerOutPort("image_output", m_img_outputOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
RockPaperScissors::~RockPaperScissors()
{
}



RTC::ReturnCode_t RockPaperScissors::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("image_height", m_img_height, "240");
  bindParameter("image_width", m_img_width, "320");
  bindParameter("out_mode", m_out_mode, "1");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RockPaperScissors::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t RockPaperScissors::onActivated(RTC::UniqueId ec_id)
{

	m_image_buff = NULL; // 入力Image
	m_hsv_buff = NULL; // HSV用
	m_convexHull_buff = NULL; // ConvexHull用
	m_skin_buff = NULL; // 肌色抽出用
	m_temp_buff = NULL; // 一時保存用
	m_label_buff = NULL; // ラベル結果保存用
	m_output_buff = NULL; // 出力用

	return RTC::RTC_OK;
}


RTC::ReturnCode_t RockPaperScissors::onDeactivated(RTC::UniqueId ec_id)
{
	if(m_image_buff != NULL){
		cvReleaseImage(&m_image_buff);
	}
	if(m_hsv_buff != NULL){
		cvReleaseImage(&m_hsv_buff);
	}
	if(m_convexHull_buff != NULL){
		cvReleaseImage(&m_convexHull_buff);
	}
	if(m_skin_buff != NULL){
		cvReleaseImage(&m_skin_buff);
	}
	if(m_temp_buff != NULL){
		cvReleaseImage(&m_temp_buff);
	}
	if(m_label_buff != NULL){
		cvReleaseImage(&m_label_buff);
	}
	if(m_output_buff != NULL){
		cvReleaseImage(&m_output_buff);
	}
	
	return RTC::RTC_OK;
}


RTC::ReturnCode_t RockPaperScissors::onExecute(RTC::UniqueId ec_id)
{	
	int key;
	
	//新データのチェック
	if(m_img_inputIn.isNew()){
		//データの読み込み
		m_img_inputIn.read();

		m_image_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // 入力Image
		m_hsv_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // HSV用
		m_convexHull_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // ConvexHull用
		m_skin_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 1); // 肌色抽出用
		m_temp_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 1); // 一時保存用
		m_label_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_16S, 1); // ラベル結果保存用
		m_output_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // 出力用
		
		//InPortの映像の取得
		memcpy(m_image_buff->imageData,(void *)&(m_img_input.pixels[0]),m_img_input.pixels.length());
		
		// 肌色を抽出する。
		extractSkinColor( m_image_buff, m_hsv_buff, m_skin_buff);
		
		// 欠損領域を補間する
		interpolate( m_skin_buff, m_temp_buff );
		
		//	ラベリングを行う
		Label *labeling = createLabeling();
		exec( labeling, m_skin_buff, m_label_buff, true, IGNORE_SIZE );

		if(getNumOfResultRegions( labeling ) > 0 ) {
			//	IGNORE_SIZEよりも大きな領域があった場合
			int handarea;		//	手領域の面積
			int hullarea;		//	ConvexHull内の面積
			int hullcount;		//	ConvexHullの頂点の数
			CvPoint *handpoint;	//	手領域内の点の座標配列
			int *hull;			//	ConvexHullの頂点のhandpointにおけるindex番号
			CvMat pointMatrix;	//	手領域用行列
			CvMat hullMatrix;	//	ConvexHull用行列

			//	最大領域(手領域)の抽出を行う
			handarea = pickupMaxArea( m_skin_buff, m_label_buff, m_convexHull_buff );

			//	ConvexHullを生成する
			createConvexHull( m_skin_buff, handarea, &handpoint, &hull, &pointMatrix, &hullMatrix );
			
			hullcount = hullMatrix.cols;

			//	ConvexHullを描画する
			drawConvexHull( m_convexHull_buff, handpoint, hull, hullcount );

			//	ConvexHull内の面積を求める
			hullarea = calcConvexHullArea( m_convexHull_buff, handpoint,hull, hullcount );

			//	ジャンケンの判定を行う
			decide( handarea, hullarea );

			//	メモリを解放する
			free( handpoint );
			free( hull );
			
		} else {
		
			//	画像を初期化する
			cvSetZero( m_convexHull_buff );

		}
	
		releaseLabeling( labeling );

		if ( m_skin_buff->origin == 0 ) {
			//　左上が原点の場合
			cvFlip( m_skin_buff, m_skin_buff, 0 );
		}
		if ( m_convexHull_buff->origin == 0 ) {
			//　左上が原点の場合
			cvFlip( m_convexHull_buff, m_convexHull_buff, 0 );
		}

		// 画像データのサイズ取得
		double len = (m_output_buff->nChannels * m_output_buff->width * m_output_buff->height);
		
		m_img_output.pixels.length(len);

		// 該当のイメージをMemCopyする
		memcpy((void *)&(m_img_output.pixels[0]), m_convexHull_buff->imageData, len);
		
		// 反転した画像データをOutPortから出力する。
		m_img_output.width = m_image_buff->width;
		m_img_output.height = m_image_buff->height;

		m_img_outputOut.write();

		cvReleaseImage(&m_image_buff);
		cvReleaseImage(&m_hsv_buff);
		cvReleaseImage(&m_convexHull_buff);
		cvReleaseImage(&m_skin_buff);
		cvReleaseImage(&m_temp_buff);
		cvReleaseImage(&m_label_buff);
		cvReleaseImage(&m_output_buff);

	}

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t RockPaperScissors::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t RockPaperScissors::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void RockPaperScissorsInit(RTC::Manager* manager)
  {
    coil::Properties profile(rockpaperscissors_spec);
    manager->registerFactory(profile,
                             RTC::Create<RockPaperScissors>,
                             RTC::Delete<RockPaperScissors>);
  }

  
};


