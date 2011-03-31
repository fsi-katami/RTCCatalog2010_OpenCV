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


//�ǉ�
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
	//	���F�𒊏o����
	//
	//	����:
	//		frameImage : �L���v�`�������摜�pIplImage
	//		hsvImage   : HSV�摜�pIplImage
	//		skinImage  : ���F���o�摜�pIplImage
	//
void extractSkinColor( IplImage *frameImage, IplImage *hsvImage, IplImage *skinImage ) {
	CvScalar color;		//	HSV�\�F�n�ŕ\�����F
	unsigned char h;	//	H����
	unsigned char s;	//	S����
	unsigned char v;	//	V����
	
	//	BGR����HSV�ɕϊ�����
	
	cvCvtColor( frameImage, hsvImage, CV_BGR2HSV );
	
	//���F���o
	for( int x = 0; x < skinImage->width; x++ ) {
		for( int y = 0 ; y < skinImage->height; y++ ) {

			color = cvGet2D( hsvImage, y, x );
			h = color.val[0];
			s = color.val[1];
			v = color.val[2];

			if( h <= HMAX && h >= HMIN &&
				s <= SMAX && s >= SMIN &&
					v <= VMAX && v >= VMIN ) {
				//	���F�̏ꍇ
				cvSetReal2D( skinImage, y, x, 255 );
			} else {
				cvSetReal2D( skinImage, y, x, 0 );
			}
		}
	}
}

	//
	//	�����̈���Ԃ���
	//
	//	����:
	//		skinImage : ���F���o�摜�pIplImage
	//		temp      : �ꎞ�ۑ��pIplImage
	//
void interpolate( IplImage *skinImage, IplImage *temp ) {
	//�c����ITERATIONS��s��
	cvDilate( skinImage, temp, NULL, ITERATIONS );

	//���k��ITERATIONS��s��
	cvErode( temp, skinImage, NULL, ITERATIONS );
}

	//
	//	�ő�̈�(��̈�)�̒��o���s��
	//
	//	����:
	//		skinImage       : ���F���o�摜�pIplImage
	//		label           : ���x�����O��������
	//		convexHullImage : ConvexHull�摜�pIplImage
	//
	//	�߂�l:
	//		��̈�̖ʐ�
	//
int pickupMaxArea(IplImage *skinImage, IplImage *label, IplImage *convexHullImage ) {

	int handarea = 0;	//	��̈�̖ʐ�

	for(int x = 0; x < skinImage->width; x++ ) {
		for( int y=0; y < skinImage->height; y++ ) {
			if( cvGetReal2D( label, y, x ) == 1 ) {
				//	�ő�̈悾�����ꍇ
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
	//	ConvexHull�𐶐�����
	//
	//	����:
	//		skinImage   : ���F���o�摜�pIplImage
	//		handarea    : ��̈�̖ʐ�(�_�̐�)
	//		handpoint   : ��̈���̓_�̍��W�z��ւ̃|�C���^
	//		hull        : ConvexHull�̒��_��handpoint�ɂ�����index�ԍ��ւ̃|�C���^
	//		pointMatrix : ��̈�p�s��ւ̃|�C���^
	//		hullMatrix  : ConvexHull�p�s��ւ̃|�C���^
	//
void createConvexHull(IplImage *skinImage, int handarea, CvPoint **handpoint, int **hull,
					  CvMat *pointMatrix, CvMat *hullMatrix ) {
	int i=0;

	//	ConvexHull���v�Z���邽�߂ɕK�v�ȍs��𐶐�����
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

	//	ConvexHull�𐶐�����
	cvConvexHull2( pointMatrix, hullMatrix, CV_CLOCKWISE, 0 );
}

	//
	//	ConvexHull��`�悷��
	//
	//	����:
	//		convexHullImage : ConvexHull�摜�pIplImage
	//		handpoint       : ��̈���̓_�̍��W�z��
	//		hull            : ConvexHull�̒��_��handpoint�ɂ�����index�ԍ�
	//		hullcount       : ConvexHull�̒��_�̐�
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
	//	ConvexHull���̖ʐς����߂�
	//
	//	����:
	//		convexHullImage : ConvexHull�摜�pIplImage
	//		handpoint       : ��̈���̓_�̍��W�z��
	//		hull            : ConvexHull�̒��_��handpoint�ɂ�����index�ԍ�
	//		hullcount       : ConvexHull�̒��_�̐��@�@
	//
	//	�߂�l:
	//		ConvexHull���̖ʐ�
	//
int calcConvexHullArea( IplImage *convexHullImage, CvPoint *handpoint, int *hull, int hullcount ) {

	//	ConvexHull�̒��_����Ȃ�s��𐶐�
	CvPoint *hullpoint = ( CvPoint * )malloc( sizeof( CvPoint ) * hullcount );
	CvMat hMatrix = cvMat( 1, hullcount, CV_32SC2, hullpoint );
	for( int i = 0; i < hullcount; i++ ) {
		hullpoint[i]=handpoint[hull[i]];
	}

	//	ConvexHull���̓_�̐��𐔂���
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
	//	�W�����P���̔�����s��
	//
	//	����:
	//		handarea : ��̈�̖ʐ�
	//		hullarea : ConvexHull���̖ʐ�
	//
void decide( int handarea, int hullarea ) {
	double ratio;	//	ConvexHull���̖ʐςɑ΂����̈�̖ʐς̊���
	
	ratio=handarea / ( double )hullarea;	
	printf( "Ratio = %lf\n", ratio );

	if( ratio >= ROCKMIN && ratio <= ROCKMAX ) {
		printf( "�O�[\n" );
	} else if( ratio >= SCISSORMIN && ratio <= SCISSORMAX ) {
		printf( "�`���L\n" );
	} else if( ratio >= PAPERMIN && ratio <= PAPERMAX ) {
		printf( "�p�[\n" );
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

	m_image_buff = NULL; // ����Image
	m_hsv_buff = NULL; // HSV�p
	m_convexHull_buff = NULL; // ConvexHull�p
	m_skin_buff = NULL; // ���F���o�p
	m_temp_buff = NULL; // �ꎞ�ۑ��p
	m_label_buff = NULL; // ���x�����ʕۑ��p
	m_output_buff = NULL; // �o�͗p

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
	
	//�V�f�[�^�̃`�F�b�N
	if(m_img_inputIn.isNew()){
		//�f�[�^�̓ǂݍ���
		m_img_inputIn.read();

		m_image_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // ����Image
		m_hsv_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // HSV�p
		m_convexHull_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // ConvexHull�p
		m_skin_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 1); // ���F���o�p
		m_temp_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 1); // �ꎞ�ۑ��p
		m_label_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_16S, 1); // ���x�����ʕۑ��p
		m_output_buff = cvCreateImage(cvSize(m_img_input.width, m_img_input.height), IPL_DEPTH_8U, 3); // �o�͗p
		
		//InPort�̉f���̎擾
		memcpy(m_image_buff->imageData,(void *)&(m_img_input.pixels[0]),m_img_input.pixels.length());
		
		// ���F�𒊏o����B
		extractSkinColor( m_image_buff, m_hsv_buff, m_skin_buff);
		
		// �����̈���Ԃ���
		interpolate( m_skin_buff, m_temp_buff );
		
		//	���x�����O���s��
		Label *labeling = createLabeling();
		exec( labeling, m_skin_buff, m_label_buff, true, IGNORE_SIZE );

		if(getNumOfResultRegions( labeling ) > 0 ) {
			//	IGNORE_SIZE�����傫�ȗ̈悪�������ꍇ
			int handarea;		//	��̈�̖ʐ�
			int hullarea;		//	ConvexHull���̖ʐ�
			int hullcount;		//	ConvexHull�̒��_�̐�
			CvPoint *handpoint;	//	��̈���̓_�̍��W�z��
			int *hull;			//	ConvexHull�̒��_��handpoint�ɂ�����index�ԍ�
			CvMat pointMatrix;	//	��̈�p�s��
			CvMat hullMatrix;	//	ConvexHull�p�s��

			//	�ő�̈�(��̈�)�̒��o���s��
			handarea = pickupMaxArea( m_skin_buff, m_label_buff, m_convexHull_buff );

			//	ConvexHull�𐶐�����
			createConvexHull( m_skin_buff, handarea, &handpoint, &hull, &pointMatrix, &hullMatrix );
			
			hullcount = hullMatrix.cols;

			//	ConvexHull��`�悷��
			drawConvexHull( m_convexHull_buff, handpoint, hull, hullcount );

			//	ConvexHull���̖ʐς����߂�
			hullarea = calcConvexHullArea( m_convexHull_buff, handpoint,hull, hullcount );

			//	�W�����P���̔�����s��
			decide( handarea, hullarea );

			//	���������������
			free( handpoint );
			free( hull );
			
		} else {
		
			//	�摜������������
			cvSetZero( m_convexHull_buff );

		}
	
		releaseLabeling( labeling );

		if ( m_skin_buff->origin == 0 ) {
			//�@���オ���_�̏ꍇ
			cvFlip( m_skin_buff, m_skin_buff, 0 );
		}
		if ( m_convexHull_buff->origin == 0 ) {
			//�@���オ���_�̏ꍇ
			cvFlip( m_convexHull_buff, m_convexHull_buff, 0 );
		}

		// �摜�f�[�^�̃T�C�Y�擾
		double len = (m_output_buff->nChannels * m_output_buff->width * m_output_buff->height);
		
		m_img_output.pixels.length(len);

		// �Y���̃C���[�W��MemCopy����
		memcpy((void *)&(m_img_output.pixels[0]), m_convexHull_buff->imageData, len);
		
		// ���]�����摜�f�[�^��OutPort����o�͂���B
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


