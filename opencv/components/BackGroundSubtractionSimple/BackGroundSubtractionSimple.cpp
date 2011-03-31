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

//char windowNameCurrent[] = "Current";		//	���݂̉摜��\������E�B���h�E�̖��O
//char windowNameResult[] = "Result";			//	�w�i�������ʂ�\������E�B���h�E�̖��O
//char windowNameBackground[] = "Background";	//	�w�i�摜��\������E�B���h�E�̖��O

int	captureOn = CAPTURE_ON;				//	�w�i�������s���摜���X�V���邩�ǂ���
int	differenceMode = COLOR_DIFFERENCE;	//	�����̌v�Z���[�h
int	noiseMode = NOISE_KEEP;				//	�m�C�Y���������郂�[�h

int g_temp_w = 0;
int g_temp_h = 0;


IplImage *originalImage = NULL;
IplImage *currentImage = NULL;
IplImage *backgroundImage = NULL;
IplImage *resultImage = NULL;
IplImage *outputImage = NULL;

char *differenceMethod[3] = {
	"RGB�̐������Ƃɕ]��",
	"CIE L*a*b* �ŋ�����]��",
	"�O���[�X�P�[���ŕ]��"
};

char *noiseMethod[3] = {
	"�Ȃ�",
	"�I�[�v�j���O",
	"���f�B�A���t�B���^"
};

void colorDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){
	
	//	�摜�𐶐�����
	IplImage *differenceImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 3);	//	�����摜�pIplImage
	IplImage *differenceRImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	R�l�̍����pIplImage
	IplImage *differenceGImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	G�l�̍����pIplImage
	IplImage *differenceBImage = cvCreateImage(cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 1);	//	B�l�̍����pIplImage

	//	���݂̔w�i�Ƃ̍��̐�Βl�𐬕����ƂɎ��
	cvAbsDiff( currentImage, backgroundImage, differenceImage );

	//	臒l�������s��
	cvThreshold( differenceImage, differenceImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	�������Ƃ̉摜�ɕ�������
	cvSplit( differenceImage, differenceBImage, differenceGImage, differenceRImage, NULL );

	//	OR�ō�������
	cvOr( differenceRImage, differenceGImage, resultImage );
	cvOr( differenceBImage, resultImage, resultImage );

	//	���������������
	cvReleaseImage( &differenceImage );
	cvReleaseImage( &differenceRImage );
	cvReleaseImage( &differenceGImage );
	cvReleaseImage( &differenceBImage );

}

//
//	L*a*b*�ŋ�����]������
//
//	����:
//		currentImage    : ���݂̉摜�pIplImage
//		backgroundImage : �w�i�摜�pIplImage
//		resultImage     : ���ʉ摜�pIplImage
//
void labDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){

	//	�摜�𐶐�����
	IplImage *currentLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height),IPL_DEPTH_32F, 3 );		//	���݂̉摜��L*a*b*�ɕϊ������摜�pIplImage
	IplImage *backgroundLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 3 );	//	�w�i��L*a*b*�ɕϊ������摜�pIplImage
	IplImage *differenceLabImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 3 );	//	�����摜�pIplImage
	IplImage *differenceLImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	L*�l�̍����pIplImage
	IplImage *differenceAImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	a*�l�̍����pIplImage
	IplImage *differenceBImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );		//	b*�l�̍����pIplImage
	IplImage *sqrDifferenceImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_32F, 1 );	//	�����Z�o�pIplImage

	//	���݂̉摜�Ɣw�i������ CIE L*a*b* �ɕϊ�
	cvConvertScale( currentImage, currentLabImage, SCALE );
	cvConvertScale( backgroundImage, backgroundLabImage, SCALE );
	cvCvtColor( currentLabImage, currentLabImage, CV_BGR2Lab );
	cvCvtColor( backgroundLabImage, backgroundLabImage, CV_BGR2Lab );

	//	�����̓����v�Z����
	cvSub( currentLabImage, backgroundLabImage, differenceLabImage );
	cvPow( differenceLabImage, differenceLabImage, 2 );

	//	�������Ƃ̉摜�ɕ�������
	cvSplit( differenceLabImage, differenceLImage, differenceAImage, differenceBImage, NULL );

	cvCopy( differenceLImage, sqrDifferenceImage );
	cvAdd( differenceAImage, sqrDifferenceImage, sqrDifferenceImage );
	cvAdd( differenceBImage, sqrDifferenceImage, sqrDifferenceImage );

	//	臒l�������s��
	cvThreshold( sqrDifferenceImage, resultImage, THRESHOLD * THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	���������������
	cvReleaseImage( &currentLabImage );
	cvReleaseImage( &backgroundLabImage );
	cvReleaseImage( &differenceLabImage );
	cvReleaseImage( &differenceLImage );
	cvReleaseImage( &differenceAImage );
	cvReleaseImage( &differenceBImage );
	cvReleaseImage( &sqrDifferenceImage );
}

//
//	�O���[�X�P�[���ŕ]������
//
//	����:
//		currentImage    : ���݂̉摜�pIplImage
//		backgroundImage : �w�i�摜�pIplImage
//		resultImage     : ���ʉ摜�pIplImage
//
void grayScaleDifference( IplImage *currentImage, IplImage *backgroundImage, IplImage *resultImage ){

	//	�摜�𐶐�����
	IplImage *differenceImage = cvCreateImage( cvSize(currentImage->width, currentImage->height), IPL_DEPTH_8U, 3 );	//	�����摜�pIplImage

	//	���݂̔w�i�Ƃ̍��̐�Βl�𐬕����ƂɎ��
	cvAbsDiff( currentImage, backgroundImage, differenceImage );

	//	BGR����O���[�X�P�[���ɕϊ�����
	cvCvtColor( differenceImage, resultImage, CV_BGR2GRAY );

	//	�O���[�X�P�[������2�l�ɕϊ�����
	cvThreshold( resultImage, resultImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	//	���������������
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
	captureOn = CAPTURE_ON;				//	�w�i�������s���摜���X�V���邩�ǂ���
    differenceMode = COLOR_DIFFERENCE;	//	�����̌v�Z���[�h
	noiseMode = NOISE_KEEP;				//	�m�C�Y���������郂�[�h

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
		
		//�C���[�WRead
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

		//	�摜��1���L���v�`�����w�i�Ƃ��ĕۑ�����
		//backgroundImage = cvCloneImage( originalImage );

		//	���݂̉摜�Ƃ��Ă�1���m�ۂ���
		//currentImage = cvCloneImage( originalImage );

		//Key����Read
		if(m_keyIn.isNew()){
			m_keyIn.read();
			//Int�ɕϊ�
			key = (int)m_key.data;
		}
		
		//InPort�̉f���̎擾
		memcpy(originalImage->imageData,(void *)&(m_img_orig.pixels[0]),m_img_orig.pixels.length());

		if( captureOn != 0){
			if( currentImage != NULL){
				cvReleaseImage( &currentImage );
			}
			currentImage = cvCloneImage( originalImage );
		}
		
		//	���̌v�Z���@�̐؂�ւ�
		if( differenceMode == COLOR_DIFFERENCE ){	
			//	�������Ƃɕ]��������
			colorDifference( currentImage, backgroundImage, resultImage );
		} else if( differenceMode == LAB_DIFFERENCE ){	
			//	L*a*b*�ŋ�����]������
			labDifference( currentImage, backgroundImage, resultImage );
		} else if( differenceMode == GRAY_DIFFERENCE ){
			//	�O���[�X�P�[���ŕ]��������
			grayScaleDifference( currentImage, backgroundImage, resultImage );
		}
		
		//	�m�C�Y����
		if( noiseMode == NOISE_MORPHOLOGY ){
			cvErode( resultImage, resultImage );
			cvDilate( resultImage, resultImage );
		}else if ( noiseMode == NOISE_MEDIAN ){
			cvSmooth( resultImage, resultImage, CV_MEDIAN );
		}
		
		if( resultImage->origin == 0 ){
			//�@���オ���_�̏ꍇ
			//cvFlip( resultImage, resultImage, 0 );
		}
		
		cvMerge( resultImage, resultImage, resultImage, NULL, outputImage );

		// �摜�f�[�^�̃T�C�Y�擾
		double len1 = (currentImage->nChannels * currentImage->width * currentImage->height);
		double len2 = (outputImage->nChannels * outputImage->width * outputImage->height);
		double len3 = (backgroundImage->nChannels * backgroundImage->width * backgroundImage->height);

		m_img_curr.pixels.length(len1);
		m_img_resu.pixels.length(len2);
		m_img_back.pixels.length(len3);

		// �Y���̃C���[�W��MemCopy����
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

		//	�摜��\������
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
			printf( "�w�i�摜�X�V\n" );
		}else if(key == ' '){
			captureOn = 1 - captureOn;
			if(captureOn == CAPTURE_ON){
				printf("�f���擾�FON\n");
			}else if(captureOn == CAPTURE_OFF){
				printf("�f���擾�FOFF\n");
			}
		}else if(key == 'm'){
			differenceMode = differenceMode + 1;
			if( differenceMode > GRAY_DIFFERENCE ){
				differenceMode = COLOR_DIFFERENCE;
			}
			printf( "���̕]�����@: %s\n", differenceMethod[differenceMode] );
		}else if( key == 'n' ){ 
			//	'n'�L�[�������ꂽ��m�C�Y�������@��ύX����
			noiseMode = noiseMode + 1;
			if( noiseMode > NOISE_MEDIAN ){
				noiseMode = NOISE_KEEP;
			}
			printf( "�m�C�Y�������@: %s\n", noiseMethod[noiseMode] );
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


