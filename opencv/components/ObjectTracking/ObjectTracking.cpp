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

IplImage	*inputImage = NULL;			//���͂��ꂽIplImage
IplImage	*resultImage = NULL;			//	�������ʕ\���pIplImage
IplImage	*hsvImage = NULL;			//	HSV�\�F�n�pIplImage
IplImage	*hueImage = NULL;			//	HSV�\�F�n��H�`�����l���pIplImage
IplImage	*maskImage = NULL;			//	�}�X�N�摜�pIplImage
IplImage	*backprojectImage = NULL;	//	�o�b�N�v���W�F�N�V�����摜�pIplImage
IplImage	*histImage = NULL;			//	�q�X�g�O�����`��pIplImage
IplImage	*grayImage = NULL;			//	�O���[�X�P�[���摜�pIplImage

CvHistogram	*hist = NULL;				//	�q�X�g�O���������p�\����

IplImage	*frameImage;	//	�L���v�`���摜�pIplImage
CvCapture	*capture;		//	�L�[���͌��ʂ��i�[����ϐ�
int count  = 0;
int g_temp_w = 0;
int g_temp_h = 0;

//	�������[�h�I��p�t���O
int	backprojectMode = HIDDEN_BACKPROJECTION;
int	selectObject = SELECT_OFF;
int	trackObject = TRACKING_STOP;
int showHist = SHOW_HISTOGRAM;

//	CamShift�g���b�L���O�p�ϐ�
CvPoint			origin;
CvRect			selection;
CvRect			trackWindow;
CvBox2D			trackRegion;
CvConnectedComp	trackComp;

//	�q�X�g�O�����p�ϐ�
int		hdims = H_DIMENSION;		//	�q�X�g�O�����̎�����
float	hRangesArray[] = {H_RANGE_MIN, H_RANGE_MAX};	//�q�X�g�O�����̃����W
float	*hRanges = hRangesArray;
int		vmin = V_MIN;
int		vmax = V_MAX;

//char	*windowNameObjectTracking = "CaptureImage";

//
//	�}�E�X�h���b�O�ɂ���ď����ǐ՗̈���w�肷��
//
//	����:
//		event	: �}�E�X���{�^���̏��
//		x		: �}�E�X�����݃|�C���g���Ă���x���W
//		y		: �}�E�X�����݃|�C���g���Ă���y���W
//		flags	: �{�v���O�����ł͖��g�p
//		param	: �{�v���O�����ł͖��g�p
//
static void on_mouse( int event, int x, int y, int flags, void* param ){
	//	�摜���擾����Ă��Ȃ���΁A�������s��Ȃ�
	if( resultImage == NULL ){
        return;
	}

	//	���_�̈ʒu�ɉ�����y�̒l�𔽓]�i�摜�̔��]�ł͂Ȃ��j
	if( resultImage->origin == 1 ){
        y = resultImage->height - y;
	}
	//	�}�E�X�̍��{�^����������Ă���Έȉ��̏������s��
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
	//	�}�E�X�̍��{�^���̏�Ԃɂ���ď����𕪊�
    switch( event ){
		case CV_EVENT_LBUTTONDOWN:
			//	�}�E�X�̍��{�^���������ꂽ�̂ł���΁A
			//	���_����ёI�����ꂽ�̈��ݒ�
			origin = cvPoint( x, y );
			selection = cvRect( x, y, 0, 0 );
			selectObject = SELECT_ON;
			break;
		case CV_EVENT_LBUTTONUP:
			//	�}�E�X�̍��{�^���������ꂽ�Ƃ��Awidth��height���ǂ�������ł���΁A
			//	trackObject�t���O��TRACKING_START�ɂ���
			selectObject = SELECT_OFF;
			if( selection.width > 0 && selection.height > 0 ){
				trackObject = TRACKING_START;
			}
			break;
    }
}

//
//	���͂��ꂽ1�̐F���l��RGB�ɕϊ�����
//
//	����:
//		hue		: HSV�\�F�n�ɂ�����F���lH
//	�߂�l�F
//		CvScalar: RGB�̐F���BGR�̏��Ŋi�[���ꂽ�R���e�i
//
CvScalar hsv2rgb( float hue ){
	IplImage *rgbValue, *hsvValue;
	rgbValue = cvCreateImage( cvSize(1,1), IPL_DEPTH_8U, 3 );
	hsvValue = cvCreateImage( cvSize(1,1), IPL_DEPTH_8U, 3 );

	hsvValue->imageData[0] = hue;	//	�F���lH
	hsvValue->imageData[1] = 255;	//	�ʓx�lS
	hsvValue->imageData[2] = 255;	//	���x�lV
	
	//	HSV�\�F�n��RGB�\�F�n�ɕϊ�����
	cvCvtColor( hsvValue, rgbValue, CV_HSV2BGR );

	return cvScalar(	(unsigned char)rgbValue->imageData[0], 
						(unsigned char)rgbValue->imageData[1], 
						(unsigned char)rgbValue->imageData[2], 
						0 );

	//	���������������
	cvReleaseImage( &rgbValue );
	cvReleaseImage( &hsvValue );
}


//
//	�}�E�X�I�����ꂽ�����ǐ՗̈�ɂ�����HSV��H�l�Ńq�X�g�O�������쐬���A�q�X�g�O�����̕`��܂ł��s��
//
//	����:
//		hist		: main�Ő錾���ꂽ�q�X�g�O�����p�\����
//		hsvImage	: ���͉摜��HSV�\�F�n�ɕϊ����ꂽ���IplImage
//		maskImage	: �}�X�N�摜�pIplImage
//		selection	: �}�E�X�őI�����ꂽ��`�̈�
//
void CalculateHist( CvHistogram	*hist, IplImage *hsvImage, IplImage *maskImage, CvRect selection ){
	int		i;
	int		binW;	//	�q�X�g�O�����̊e�r���́A�摜��ł̕�
	int		val;	//	�q�X�g�O�����̕p�x
	float	maxVal;	//	�q�X�g�O�����̍ő�p�x


	//	hsv�摜�̊e��f���l�͈͓̔��ɓ����Ă��邩�`�F�b�N���A
	//	�}�X�N�摜maskImage���쐬����
	cvInRangeS( hsvImage, 
				cvScalar( H_RANGE_MIN, S_MIN, MIN(V_MIN,V_MAX), 0 ),
				cvScalar( H_RANGE_MAX, S_MAX, MAX(V_MIN,V_MAX), 0 ), 
				maskImage );
	//	hsvImage�̂����A�Ƃ��ɕK�v��H�`�����l����hueImage�Ƃ��ĕ�������
	cvSplit( hsvImage, hueImage, 0, 0, 0 );
	//	trackObject��TRACKING_START��ԂȂ�A�ȉ��̏������s��
	if( trackObject == TRACKING_START ){
		//	�ǐ՗̈�̃q�X�g�O�����v�Z��histImage�ւ̕`��
		maxVal = 0.0;

		cvSetImageROI( hueImage, selection );
        cvSetImageROI( maskImage, selection );
        //	�q�X�g�O�������v�Z���A�ő�l�����߂�
		cvCalcHist( &hueImage, hist, 0, maskImage );
		cvGetMinMaxHistValue( hist, 0, &maxVal, 0, 0 );
        //	�q�X�g�O�����̏c���i�p�x�j��0-255�̃_�C�i�~�b�N�����W�ɐ��K��
		if( maxVal == 0.0 ){
			cvConvertScale( hist->bins, hist->bins, 0.0, 0 );
		} else{
			cvConvertScale( hist->bins, hist->bins, 255.0 / maxVal, 0 );
		}
		//	hue,mask�摜�ɐݒ肳�ꂽROI�����Z�b�g
		cvResetImageROI( hueImage );
        cvResetImageROI( maskImage );

        trackWindow = selection;
        //	trackObject��TRACKING_NOW�ɂ���
		trackObject = TRACKING_NOW;

		//	�q�X�g�O�����摜���[���N���A
        cvSetZero( histImage );
		//	�e�r���̕������߂�
        binW = histImage->width / hdims;
		//	�q�X�g�O������`�悷��
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
	
	//�e�������m��
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
	//������������
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
	int		key;	//	�L�[���͌��ʂ��i�[����ϐ�
	int		i;
	int		j;
	
	int x;
	int y;
	int mouse_event;

	//	Snake�p�̃p�����[�^
	float alpha = 1.0;		//	�A���G�l���M�[�̏d�݃p�����[�^
	float beta = 0.5;		//	�ȗ��̏d�݃p�����[�^
	float gamma = 1.5;		//	�摜�G�l���M�[�̏d�݃p�����[�^
	CvPoint pt[SEGMENT];	//	����_�̍��W
	CvSize window;			//	�ŏ��l��T������ߖT�T�C�Y
	window.width = WINDOW_WIDTH;	
	window.height = WINDOW_HEIGHT;
	CvTermCriteria crit;
	crit.type = CV_TERMCRIT_ITER;		//	�I�������̐ݒ�
	crit.max_iter = ITERATION_SNAKE;	//	�֐��̍ő唽����

	if(m_orig_imgIn.isNew()){

		m_orig_imgIn.read();
			
		//�e�������m��
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
		//	�q�X�g�O�����\���̂̎g�p��錾
		if(hist == NULL){
			hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hRanges, 1 );
		}
		//	�q�X�g�O�����p�̉摜���m�ۂ��A�[���N���A
		if(histImage == NULL){
			histImage = cvCreateImage( cvSize(HISTIMAGE_WIDTH, HISTIMAGE_HEIGHT), IPL_DEPTH_8U, 3 );
		}
		if(g_temp_w != m_orig_img.width || g_temp_h != m_orig_img.height){
			cvReleaseImage(&histImage);
			histImage = cvCreateImage(cvSize(m_orig_img.width, m_orig_img.height), IPL_DEPTH_8U, 3);
		}
		cvSetZero( histImage );

		//InPort�̉f���̎擾
		memcpy(inputImage->imageData,(void *)&(m_orig_img.pixels[0]),m_orig_img.pixels.length());

		//	�L���v�`�����ꂽ�摜��resultImage�ɃR�s�[���AHSV�\�F�n�ɕϊ�����hsvImage�Ɋi�[
		cvCopy( inputImage, resultImage, NULL );
		cvCvtColor( resultImage, hsvImage, CV_BGR2HSV );

		//Window��Event���̎擾�ɑ΂��鏈��
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
		
		//	trackObject�t���O��TRACKING_STOP�ȊO�Ȃ�A�ȉ��̏������s��
        if( trackObject != TRACKING_STOP ){
			
			//�ǐ՗̈�̃q�X�g�O�����v�Z�ƕ`��
			CalculateHist(	hist, hsvImage, maskImage, selection );

			//	�o�b�N�v���W�F�N�V�������v�Z����
            cvCalcBackProject( &hueImage, backprojectImage, hist );
            //	backProjection�̂����A�}�X�N��1�ł���Ƃ��ꂽ�����̂ݎc��
			cvAnd( backprojectImage, maskImage, backprojectImage, 0 );

			//	CamShift�@�ɂ��̈�ǐՂ����s����
			cvCamShift( backprojectImage, 
						trackWindow, 
						cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), 
						&trackComp, 
						&trackRegion );
			
			trackWindow = trackComp.rect;

			//	SnakeImage�p�̃O���[�X�P�[���摜���쐬����
			cvCvtColor( resultImage, grayImage, CV_BGR2GRAY );

			if( backprojectMode == SHOW_BACKPROJECTION ){
                cvCvtColor( backprojectImage, resultImage, CV_GRAY2BGR );
			}
			if( resultImage->origin == 1 ){
                trackRegion.angle = -trackRegion.angle;
			}

			//	CamShift�ł̗̈�ǐՌ��ʂ�Snake�̏����ʒu�ɐݒ肷��
			for( i=0; i<SEGMENT; i++ ){
				pt[i].x = cvRound(	trackRegion.size.width 
									* cos(i * 6.28 / SEGMENT + trackRegion.angle) 
									/ 2.0 + trackRegion.center.x );
				pt[i].y = cvRound(	trackRegion.size.height 
									* sin(i * 6.28 / SEGMENT + trackRegion.angle) 
									/ 2.0 + trackRegion.center.y );
			}
			//	Snake�ɂ��֊s���o�����s����
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
				//	�e�֊s�_�̊Ԃɐ����Ђ��ė֊s����`�悷��
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

		//	�}�E�X�őI�𒆂̏����ǐ՗̈�̐F�𔽓]������
		if( selectObject == SELECT_ON && selection.width > 0 && selection.height > 0 ){
			
			cvSetImageROI( resultImage, selection );
			cvXorS( resultImage, cvScalarAll(255), resultImage, 0 );
			cvResetImageROI( resultImage );
		}
		//	backprojectImage�̍��W���_������̏ꍇ�A�㉺�𔽓]������
		if( backprojectImage->origin == 0 ){
			cvFlip( backprojectImage, backprojectImage, 0 );
		}
				
		//	�摜��\������
		//cvShowImage( windowNameObjectTracking, resultImage );
		//key = cvWaitKey(1);

		// �摜�f�[�^�̃T�C�Y�擾
		double len = (resultImage->nChannels * resultImage->width * resultImage->height);
		double leng = (histImage->nChannels * histImage->width * histImage->height);
		m_out_img.pixels.length(len);
		m_hist_img.pixels.length(leng);

		// �Y���̃C���[�W��MemCopy����
		memcpy((void *)&(m_out_img.pixels[0]), resultImage->imageData, len);
		memcpy((void *)&(m_hist_img.pixels[0]), histImage->imageData, leng);

		// ���]�����摜�f�[�^��OutPort����o�͂���B
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


