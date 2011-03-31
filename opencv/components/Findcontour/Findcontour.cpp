// -*- C++ -*-
/*!
 * @file  Findcontour.cpp
 * @brief Findcontour component
 * @date $Date$
 *
 * $Id$
 */

#include "Findcontour.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* findcontour_spec[] =
  {
    "implementation_id", "Findcontour",
    "type_name",         "Findcontour",
    "description",       "Findcontour component",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Findcontour::Findcontour(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_contourOut("contour_image", m_image_contour)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("contour_image", m_image_contourOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Findcontour::~Findcontour()
{
}


/*
RTC::ReturnCode_t Findcontour::onInitialize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Findcontour::onActivated(RTC::UniqueId ec_id)
{
  //  �C���[�W�p�������̏�����
  imageBuff = NULL;
  grayImage = NULL;
  binaryImage = NULL;
  contourImage = NULL;

  //  OutPort��ʃT�C�Y�̏�����
  m_image_contour.width = 0;
  m_image_contour.height = 0;

  find_contour = NULL;
  red = CV_RGB( 255, 0, 0 );

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Findcontour::onDeactivated(RTC::UniqueId ec_id)
{
  if(imageBuff != NULL )
  {
    //  �C���[�W�p�������̉��
	cvReleaseImage(&imageBuff);
	cvReleaseImage(&grayImage);
    cvReleaseImage(&binaryImage);
    cvReleaseImage(&contourImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Findcontour::onExecute(RTC::UniqueId ec_id)
{
	//  �V�����f�[�^�̃`�F�b�N
  if(m_image_origIn.isNew()){
	  //  InPort�f�[�^�̓ǂݍ���
	  m_image_origIn.read();

	  //  InPort��OutPort�̉�ʃT�C�Y��������уC���[�W�p�������m��
	  if( m_image_orig.width != m_image_contour.width || m_image_orig.height != m_image_contour.height)
	  {
		  m_image_contour.width = m_image_orig.width;
		  m_image_contour.height = m_image_orig.height;

		  //  InPort�̃C���[�W�T�C�Y���ύX���ꂽ�ꍇ
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&binaryImage);
			  cvReleaseImage(&contourImage);
		  }

		  //  �C���[�W�p�������̊m��
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  binaryImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1);
		  contourImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3);
	  }

	  //  InPort�̉�ʃf�[�^���R�s�[
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );
	  memcpy( contourImage->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  //  RGB����O���[�X�P�[���ɕϊ�
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY);

	  //  �O���[�X�P�[������2�l�ɕϊ�����
	  cvThreshold( grayImage, binaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

	  //  ���o���ꂽ�֊s��ۑ�����̈� 
	  CvMemStorage* storage = cvCreateMemStorage( 0 );
	
	  //  2�l�摜���̗֊s�������A���̐���Ԃ�
	  find_contour_num = cvFindContours( 
		binaryImage,			//	���͉摜(�W�r�b�g�V���O���`�����l���j
		storage,				//	���o���ꂽ�֊s��ۑ�����̈�
		&find_contour,			//	��ԊO���̗֊s�ւ̃|�C���^�ւ̃|�C���^
		sizeof( CvContour ),	//	�V�[�P���X�w�b�_�̃T�C�Y
		CV_RETR_LIST,			//	���o���[�h 
		CV_CHAIN_APPROX_NONE,	//	�����@
		cvPoint( 0, 0 )			//	�I�t�Z�b�g
	  );

	  cvDrawContours( 
		contourImage,			//	�֊s��`�悷��摜
		find_contour,			//	�ŏ��̗֊s�ւ̃|�C���^
		red,					//	�O���֊s���̐F
		red,					//	�����֊s���i���j�̐F
		CONTOUR_MAX_LEVEL,		//	�`�悳���֊s�̍ő僌�x��
		LINE_THICKNESS,			//	�`�悳���֊s���̑���
		LINE_TYPE,				//	���̎��
		cvPoint( 0, 0 )			//	�I�t�Z�b�g
	  );

	  //  �摜�f�[�^�̃T�C�Y�擾
	  int len = contourImage->nChannels * contourImage->width * contourImage->height;
	  m_image_contour.pixels.length(len);

	  //  �ϓ]�����摜�f�[�^��OutPort�ɃR�s�[
	  memcpy((void *)&(m_image_contour.pixels[0]), contourImage->imageData, len);

	  //  �ϓ]�����摜�f�[�^��OutPort����o��
	  m_image_contourOut.write();

	  //  ���o���ꂽ�֊s�����
	  cvReleaseMemStorage( &storage );

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Findcontour::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Findcontour::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void FindcontourInit(RTC::Manager* manager)
  {
    coil::Properties profile(findcontour_spec);
    manager->registerFactory(profile,
                             RTC::Create<Findcontour>,
                             RTC::Delete<Findcontour>);
  }
  
};


