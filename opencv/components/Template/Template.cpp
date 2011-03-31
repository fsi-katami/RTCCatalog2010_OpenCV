// -*- C++ -*-
/*!
 * @file  Template.cpp
 * @brief Template image component
 * @date $Date$
 *
 * $Id$
 */

#include "Template.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* template_spec[] =
  {
    "implementation_id", "Template",
    "type_name",         "Template",
    "description",       "Template image component",
    "version",           "1.0.0",
    "vendor",            "VenderName",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",
    // Configuration variables
	"conf.default.image_path","",
	//"conf.default.image_path","template.bmp",
    ""
  };
// </rtc-template>
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Template::Template(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_templateOut("template", m_image_template),
	m_image_pictureOut("picture", m_image_picture)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("template", m_image_templateOut);
  registerOutPort("picture", m_image_pictureOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Template::~Template()
{
}



RTC::ReturnCode_t Template::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("image_path",m_img_path, "template.bmp");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Template::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Template::onActivated(RTC::UniqueId ec_id)
{

  // �Ώۉ摜�p�������̏�����
  templateID = -1;
  templateWidth = 0;
  templateHeight = 0;
  templateImage = NULL;
  templateGrayImage = NULL;
  templateBinaryImage = NULL;

  // �C���[�W�p�������̏�����
  imageBuff = NULL;
  sourceGrayImage = NULL;
  sourceBinaryImage = NULL;
  differenceMapImage = NULL;

  // OutPort�P�̉�ʃT�C�Y�̏�����
  m_image_template.width = 0;
  m_image_template.height = 0;

  len = 0;
  flag = 0;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Template::onDeactivated(RTC::UniqueId ec_id)
{

  if(imageBuff != NULL)
  {
    // �C���[�W�p�������̉��
	cvReleaseImage(&imageBuff);
	cvReleaseImage(&sourceGrayImage);
	cvReleaseImage(&sourceBinaryImage);
	cvReleaseImage(&differenceMapImage);
  }

  if( templateImage != NULL )
  {
	  cvReleaseImage(&templateImage);
	  cvReleaseImage(&templateGrayImage);
      cvReleaseImage(&templateBinaryImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Template::onExecute(RTC::UniqueId ec_id)
{
  // �V�����f�[�^�̃`�F�b�N
  if(m_image_origIn.isNew())
  {
	  // InPort�f�[�^�̓ǂݍ���
	  m_image_origIn.read();

	  // �Ώۉ摜��ǂݍ���
	  templateImage = cvLoadImage( m_img_path, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );

	  if( templateImage == NULL )
	  {
		  templateID = -1 ;
		  templateWidth = templateHeight = 0;
	  }

	  // �Ώۉ摜�`�F�b�N
	  // �Ώۉ摜��Path�Ƃ����������ꍇ�e���v���[�g�}�b�`���O���Ȃ��ē��͂��ꂽ�C���[�W�����̂܂܏o��
	  if( templateImage != NULL && templateID != templateImage->ID )
	  {
		  // �t���b�O�ݒ�(�������Ώۉ摜�����́j
		  flag = 1;
		  templateID = templateImage->ID;
		  templateWidth = templateImage->width;
		  templateHeight = templateImage->height;

		  if(templateGrayImage != NULL)
		  {
			  cvReleaseImage(&templateGrayImage);
			  cvReleaseImage(&templateBinaryImage);
		  }

		  // �Ώۉ摜�p�̃������m��
		  templateGrayImage = cvCreateImage( cvGetSize(templateImage), IPL_DEPTH_8U, 1 );
		  templateBinaryImage = cvCreateImage( cvGetSize(templateImage), IPL_DEPTH_8U, 1 );

		  // �Ώۉ摜ID�A�T�C�Y�o��
		  cout << "templateID : "<<templateID<<endl;
		  cout << "template - width :"<<templateWidth<<endl;
		  cout << "template - height :"<<templateHeight<<endl;

		  //  RGB����O���[�X�P�[���ɕϊ�����
		  cvCvtColor( templateImage, templateGrayImage, CV_RGB2GRAY );

		  //  �O���[�X�P�[������2�l�ɕϊ�����
		  cvThreshold( templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

		  // OutPort�Q�p�̉�ʃT�C�Y������
		  m_image_picture.width = templateImage->width;
		  m_image_picture.height = templateImage->height;
	  }

	  // InPort��OutPort�̉�ʃT�C�Y��������уC���[�W�p�������̊m��(�������Ώۉ摜��������differenceMapImage���ϊ������-�t���b�O�����Ĕ��f�j
	  if(m_image_orig.width != m_image_template.width || m_image_orig.height != m_image_template.height || flag == 1)
	  {
		  flag = 0;
		  m_image_template.width = m_image_orig.width;
		  m_image_template.height = m_image_orig.height;

		  // InPort�̃C���[�W�T�C�Y���ύX���ꂽ�ꍇ
		  if(imageBuff != NULL)
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&sourceGrayImage);
			  cvReleaseImage(&sourceBinaryImage);
			  cvReleaseImage(&differenceMapImage);
		  }
		  // �C���[�W�p�������̊m��
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  sourceGrayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  sourceBinaryImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  differenceMapImage = cvCreateImage( cvSize( m_image_orig.width - templateWidth + 1, m_image_orig.height - templateHeight + 1 ), IPL_DEPTH_32F, 1 );
	  }

	  // InPort�̉摜�f�[�^���R�s�[
	  memcpy( imageBuff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length() );

	  if( templateImage != NULL )
	  {
		  //  RGB����O���[�X�P�[���ɕϊ�����
		  cvCvtColor( imageBuff, sourceGrayImage, CV_RGB2GRAY );

		  //  �O���[�X�P�[������2�l�ɕϊ�����
		  cvThreshold( sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

		  //  �e���v���[�g�}�b�`���O���s��
		  cvMatchTemplate( sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF );

		  //  �e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
		  cvMinMaxLoc( differenceMapImage, NULL, NULL, &minLocation, NULL, NULL );

		  //  ��v����ꏊ�����摜�Ɏl�p�ŕ`��
		  cvRectangle(
			  imageBuff,
			  minLocation,
			  cvPoint( minLocation.x + templateImage->width, minLocation.y + templateImage->height ),
			  CV_RGB( 255, 0, 0 ),
			  LINE_THICKNESS,
			  LINE_TYPE,
			  SHIFT
		  );

		  // �摜�f�[�^�̃T�C�Y�擾
		  len = imageBuff->nChannels * imageBuff->width * imageBuff->height;
		  m_image_template.pixels.length(len);

		  //  ���]�����摜�f�[�^��OutPort�ɃR�s�[
		  memcpy( (void *)&(m_image_template.pixels[0]), imageBuff->imageData, len );
	  
		  //  ���]�����摜�f�[�^��OutPort����o��
		  m_image_templateOut.write();

		  //  �Ώۉ摜�f�[�^�̃T�C�Y�擾
		  len = templateImage->nChannels * templateImage->width * templateImage->height;
		  m_image_picture.pixels.length(len);

		  //  ���]�����Ώۉ摜�f�[�^��OutPort�ɃR�s�[

		  memcpy( (void *)&(m_image_picture.pixels[0]), templateImage->imageData, len );

		  //  ���]�����Ώۉ摜�f�[�^��OutPort����o��
		  m_image_pictureOut.write();

	  }else{

		  // �摜�f�[�^�̃T�C�Y�擾
		  len = imageBuff->nChannels * imageBuff->width * imageBuff->height;
		  m_image_template.pixels.length(len);

		  //  ���]�����摜�f�[�^��OutPort�ɃR�s�[
		  memcpy( (void *)&(m_image_template.pixels[0]), imageBuff->imageData, len );
	  
		  //  ���]�����摜�f�[�^��OutPort����o��
		  m_image_templateOut.write();

	  }

	  cvReleaseImage(&templateImage);
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Template::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Template::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void TemplateInit(RTC::Manager* manager)
  {
    coil::Properties profile(template_spec);
    manager->registerFactory(profile,
                             RTC::Create<Template>,
                             RTC::Delete<Template>);
  }
  
};


