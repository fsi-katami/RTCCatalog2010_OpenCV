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

//char *windowNameCapture = "Capture";					//	�L���v�`�������摜��\������E�B���h�E�̖��O
//char *windowNameForeground  = "Foreground Mask";		//	�O�i�}�X�N�摜��\������E�B���h�E�̖��O
//char *windowNameStillObjectMask  = "Still Object Mask";	//	�Î~���̃}�X�N�摜��\������E�B���h�E�̖��O
//char *windowNameBackground = "Background";				//	�w�i�摜��\������E�B���h�E�̖��O
//char *windowNameStillObject  = "Still Object";			//	�Î~���̂�\������E�B���h�E�̖��O
//char *windowNameCounter = "Counter";					//	�J�E���^��\������E�B���h�E�̖��O

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
			
			//	�摜�T�C�Y��ۑ�
			imageSize = cvSize(m_image_orig.width, m_image_orig.height);

			//	�摜�𐶐�����
			inputImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			backgroundAverageImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	�w�i�̕��ϒl�ۑ��pIplImage
			backgroundThresholdImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	�w�i��臒l�ۑ��pIplImage
			stillObjectAverageImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	�Î~���̂̕��ϒl�ۑ��pIplImage
			stillObjectThresholdImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	�Î~���̂�臒l�ۑ��pIplImage
			stillObjectCounterImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );		//	�Î~���̂̃J�E���^�pIplImage
			backgroundDifferenceImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );		//	�w�i�����摜�pIplImage
			stillObjectDifferenceImage = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );	//	�Î~���̍����摜�pIplIMage
			thresholdImage32 = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );				//	32bit��臒l�摜�pIplImage
			thresholdImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );					//	臒l�摜�pIplImage
			resultImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	���ʉ摜�pIplImage
			backgroundMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	�w�i�}�X�N�摜�pIplImage
			foregroundMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	�O�i�}�X�N�pIplImage
			stillObjectMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );			//	�Î~���̃}�X�N�pIplImage
			movingObjectMask = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );				//	�����̃}�X�N�pIplImage
			backgroundCopyMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );		//	�w�i�ɃR�s�[����ۂɎg�p����}�X�N�pIplImage
			tmpMaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	�e���|�����pIplImage
			tmp2MaskImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 1 );					//	�e���|�����pIplImage(����2)
			frameImage32 = cvCreateImage( imageSize, IPL_DEPTH_32F, 3 );					//	32bit�̃L���v�`�������摜�pIplImage
			backgroundImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );				//	�w�i�摜�pIplImage
			stillObjectImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3 );
			outputImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);

			foreGroundMaskBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectMaskBuff  = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			backGroundBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectImageBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
			stillObjectCounterBuff = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);

			memcpy(inputImage->imageData,(void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length());

			//	����������
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
		
			//	float 32bit�ɕϊ�����
			cvConvert( inputImage, frameImage32 );

			//	�w�i�Ƃ̍� /////////////////////////////////////////////////////////

			//	���݂̔w�i�Ƃ̍��̐�Βl�𐬕����ƂɎ��
			cvAbsDiff( frameImage32, backgroundAverageImage, backgroundDifferenceImage );

			//	臒l�̒l������
			cvAddWeighted( backgroundDifferenceImage, 1.0, backgroundThresholdImage, -THRESHOLD_COEFFICIENT, 0.0, thresholdImage32 );

			// thresholdImage �̗v�f��1�ł�0�ȏゾ������w�i�ł͂Ȃ�
			cvConvert( thresholdImage32, thresholdImage );
			cvCvtColor( thresholdImage, resultImage, CV_BGR2GRAY );
			cvThreshold( resultImage, backgroundMaskImage, MASK_THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY_INV );

			//	�w�i���Ƃ̍� /////////////////////////////////////////////////////

			//	���݂̔w�i���Ƃ̍��̐�Βl�𐬕����ƂɎ��
			cvAbsDiff( frameImage32, stillObjectAverageImage, stillObjectDifferenceImage );

			//	臒l�̒l������
			cvAddWeighted( stillObjectDifferenceImage, 1.0, stillObjectThresholdImage, -THRESHOLD_COEFFICIENT, 0.0, thresholdImage32 );

			//	thresholdImage �̗v�f��1�ł�0�ȏゾ������w�i���ł͂Ȃ�
			cvConvert( thresholdImage32, thresholdImage );
			cvCvtColor( thresholdImage, resultImage, CV_BGR2GRAY );
			cvThreshold( resultImage, stillObjectMaskImage, MASK_THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY_INV );
			
			//	�����܂łŁA
			//	backgroundDifferenceImage, backgroundMaskImage
			//	stillObjectDifferenceImage, stillObjectMaskImage
			//	�ɈӖ��̂���l������B

			//	�e������X�V���� /////////////////////////////////////////////////

			//	�w�i�ɓ�������ꍇ (backgroundMaskImage=1�̏ꍇ)
			cvRunningAvg( frameImage32, backgroundAverageImage, BACKGROUND_ALPHA, backgroundMaskImage );
			cvRunningAvg( backgroundDifferenceImage, backgroundThresholdImage, BACKGROUND_ALPHA, backgroundMaskImage );

			//	�w�i���ɓ�������ꍇ (backgroundMaskImage=0 && stillObjectMaskImage=1)
			cvNot( backgroundMaskImage, foregroundMaskImage );
			cvAnd( foregroundMaskImage, stillObjectMaskImage, tmpMaskImage );	//	�w�i���
			
			cvRunningAvg( frameImage32, stillObjectAverageImage, STILL_OBJECT_ALPHA, tmpMaskImage );
			cvRunningAvg( stillObjectDifferenceImage, stillObjectThresholdImage, STILL_OBJECT_ALPHA, tmpMaskImage );
						  
			//	�w�i���J�E���^�𑝂₷
			cvAddS( stillObjectCounterImage, cvScalar( 1 ), stillObjectCounterImage, tmpMaskImage );
			
			//	�J�E���^��臒l�ȏ�ɂȂ�����A�w�i����w�i�Ƃ��č̗p����
			cvThreshold( stillObjectCounterImage, tmp2MaskImage, STILL_OBJECT_TO_BACKGROUND, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
			cvAnd( tmpMaskImage, tmp2MaskImage, backgroundCopyMaskImage );
			cvCopy( stillObjectAverageImage, backgroundAverageImage, backgroundCopyMaskImage );
			cvCopy( stillObjectThresholdImage, backgroundThresholdImage, backgroundCopyMaskImage );

			//	���� backgroundCopyMaskImage �́A��Ŕw�i����0�ɏ���������ۂɎg�p
			cvSet( stillObjectCounterImage, cvScalarAll( 0 ), backgroundCopyMaskImage );

			//	�w�i���ł��Ȃ��A�w�i�ł��Ȃ��ꍇ
			//	(foregroundMaskImage = 1 && stillObjectMaskImage = 0)
			cvNot( stillObjectMaskImage, movingObjectMask );
			
			//	�J�E���^�����炷(�Z��,�ԃm�C�Y�Ή�)
			//	����́A�w�i�ɕ��ނ��ꂽ�s�N�Z���ɑ΂��Ă��s���B���Ȃ킿�A
			//	movingObjectMask == 1 || backgroundMaskImage == 1
			cvOr( backgroundMaskImage, movingObjectMask, tmpMaskImage );
			cvSubS( stillObjectCounterImage, cvScalarAll( NOT_STILL_DEC_STEP ), stillObjectCounterImage, tmpMaskImage );
			
			//	�J�E���^��0�ɂȂ�����w�i��������������
			cvNot( stillObjectCounterImage, tmp2MaskImage );	// tmp2 = 1 �Ȃ珉����

			//	����������l�̌���2��ނ���B
			//	(1)���݂̉摜�ŏ����� --- �w�i�ł��Ȃ��w�i���ł��Ȃ��ꍇ
			//	(2)�o�^�Ȃ���Ԃŏ����� --- �w�i�������͔w�i�����R�s�[�����ꍇ
			//	�����ł�(1)�ŏ��������Ă���
			cvOr( tmpMaskImage, backgroundCopyMaskImage, tmpMaskImage );
			cvAnd( tmpMaskImage, tmp2MaskImage, tmpMaskImage );

			cvCopy( frameImage32, stillObjectAverageImage, tmpMaskImage );
			cvSet( stillObjectThresholdImage, cvScalarAll( STILL_OBJECT_INITIAL_THRESHOLD ), tmpMaskImage );
			
			//	�m�C�Y����������
			cvSmooth( foregroundMaskImage, foregroundMaskImage, CV_MEDIAN );

			cvConvert( backgroundAverageImage, backgroundImage );
			cvConvert( stillObjectAverageImage, stillObjectImage );

			//	�L�[���͔���
			cvWaitKey( 1 );
			
			// �摜�f�[�^�̃T�C�Y�擾
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








