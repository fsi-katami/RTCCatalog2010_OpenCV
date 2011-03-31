// -*- C++ -*-
/*!
 * @file  Hough.cpp
 * @brief Hough line component
 * @date $Date$
 *
 * $Id$
 */

#include "Hough.h"

// Module specification
// <rtc-template block="module_spec">
static const char* hough_spec[] =
  {
    "implementation_id", "Hough",
    "type_name",         "Hough",
    "description",       "Hough line component",
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
	"conf.default.canny_threshold1", "100",
	"conf.default.canny_threshold2", "100",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Hough::Hough(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("origin_image", m_image_orig),
    m_image_houghOut("houghline_image", m_image_hough)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("origin_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("houghline_image", m_image_houghOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Hough::~Hough()
{
}



RTC::ReturnCode_t Hough::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("canny_threshold1", canny_threshold1, "100");
  bindParameter("canny_threshold2", canny_threshold2, "100");
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Hough::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Hough::onActivated(RTC::UniqueId ec_id)
{
  //  �C���[�W�p�������̏�����
  imageBuff = NULL;
  grayImage = NULL;
  edgeImage = NULL;
  hough = NULL;
  houghImage = NULL;

  //  OutPort�̉�ʃT�C�Y�̏�����
  m_image_orig.width = 0;
  m_image_orig.height = 0;

  lines = NULL;
  len=0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Hough::onDeactivated(RTC::UniqueId ec_id)
{
  if(imageBuff != NULL)
  {
    // �C���[�W�p�������̉��
    cvReleaseImage(&imageBuff);
    cvReleaseImage(&grayImage);
    cvReleaseImage(&edgeImage);
    cvReleaseImage(&hough);
    cvReleaseImage(&houghImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Hough::onExecute(RTC::UniqueId ec_id)
{
	//  �V�����f�[�^�̃`�F�b�N
  if(m_image_origIn.isNew()){
	  //  InPort�f�[�^�̓ǂݍ���
	  m_image_origIn.read();

	  //  InPort��OutPort�̉�ʃT�C�Y��������уC���[�W�p�������̊m��
	  if(m_image_orig.width != m_image_hough.width || m_image_orig.height != m_image_hough.height)
	  {
		  m_image_hough.width = m_image_orig.width;
		  m_image_hough.height = m_image_orig.height;

		  //  InPort�̃C���[�W�T�C�Y���ύX���ꂽ�ꍇ
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&edgeImage);
			  cvReleaseImage(&hough);
			  cvReleaseImage(&houghImage);
		  }

		  //  �C���[�W�p�������̊m��
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  edgeImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  hough = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  houghImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
	  }

	  // InPort�̉�ʃf�[�^���R�s�[
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  //  RGB����O���[�X�P�[���ɕϊ�
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY );

	  //  �n�t�ϊ��ɕK�v�ȃ������̈�
	  CvMemStorage *storage = cvCreateMemStorage( 0 );

	  //�G�b�W���o���s��
	  cvCanny( grayImage, edgeImage, canny_threshold1, canny_threshold2, APERTURE_SIZE );

	  //�O���[�X�P�[������RGB�ɕϊ�����
      cvCvtColor( edgeImage, houghImage, CV_GRAY2RGB );

	  //�n�t�ϊ��ɂ�蒼���̒��o���s��
      lines = cvHoughLines2( edgeImage, storage, HOUGHLINE_METHOD, RHO, THETA, HOUGHLINE_THRESHOLD, HOUGHLINE_PARAM1, HOUGHLINE_PARAM2 );

	  //���o���ꂽ������`��
      for ( int i = 0; i < lines->total; i++ ) {
        CvPoint *line = ( CvPoint* )cvGetSeqElem( lines, i );
        cvLine( houghImage, line[0], line[1], CV_RGB( 255, 0, 0 ), LINE_THICKNESS, LINE_TYPE, SHIFT );
      }

	  //  �摜�f�[�^�̃T�C�Y�擾
	  len = houghImage->nChannels * houghImage->width * houghImage->height;
	  m_image_hough.pixels.length(len);

	  //  ���]�����摜�f�[�^��OutPort�ɃR�s�[
	  memcpy( (void *)&(m_image_hough.pixels[0]), houghImage->imageData, len );

	  //  ���]�����摜�f�[�^��OutPort����o��
	  m_image_houghOut.write();

	  //  �n�t�ϊ��Ɏg�p�������������
	  cvReleaseMemStorage(&storage);

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Hough::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Hough::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void HoughInit(RTC::Manager* manager)
  {
    coil::Properties profile(hough_spec);
    manager->registerFactory(profile,
                             RTC::Create<Hough>,
                             RTC::Delete<Hough>);
  }
  
};


