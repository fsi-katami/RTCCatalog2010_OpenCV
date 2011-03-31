// -*- C++ -*-
/*!
 * @file  Sepia.cpp
 * @brief Sepia component
 * @date $Date$
 *
 * $Id$
 */

#include "Sepia.h"

// Module specification
// <rtc-template block="module_spec">
static const char* sepia_spec[] =
  {
    "implementation_id", "Sepia",
    "type_name",         "Sepia",
    "description",       "Sepia component",
    "version",           "1.0.0",
    "vendor",            "VenderName",
    "category",          "AIST",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    "exec_cxt.periodic.rate", "1000.0",

        // Configuration variables
    "conf.default.image_hue",          "22",
    "conf.default.image_Saturation",   "90",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Sepia::Sepia(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_sepiaOut("sepia_image", m_image_sepia)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("sepia_image", m_image_sepiaOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Sepia::~Sepia()
{
}


RTC::ReturnCode_t Sepia::onInitialize()
{
    // <rtc-template block="bind_config">
    // Bind variables and configuration variable
    bindParameter("image_hue",         m_nHue,         "22");
    bindParameter("image_Saturation",  m_nSaturation,  "90");

    // </rtc-template>
    return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t Sepia::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Sepia::onActivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̊m��
    m_image_buff       = NULL;

	m_hsvImage         = NULL;

    m_hueImage         = NULL;
	m_saturationImage  = NULL;
	m_valueImage       = NULL;

	m_mergeImage       = NULL;
	m_destinationImage = NULL;

    m_in_height        = 0;
    m_in_width         = 0;
    
    return RTC::RTC_OK;
}


RTC::ReturnCode_t Sepia::onDeactivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̉��
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_hsvImage         != NULL)
        cvReleaseImage(&m_hsvImage);
    if(m_hueImage         != NULL)
        cvReleaseImage(&m_hueImage);
    if(m_saturationImage  != NULL)
        cvReleaseImage(&m_saturationImage);
    if(m_valueImage       != NULL)
        cvReleaseImage(&m_valueImage);
    if(m_mergeImage       != NULL)
        cvReleaseImage(&m_mergeImage);
    if(m_destinationImage != NULL)
        cvReleaseImage(&m_destinationImage);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Sepia::onExecute(RTC::UniqueId ec_id)
{
    // Common CV actions
    // �V�����f�[�^�̃`�F�b�N
    if (m_image_origIn.isNew()) 
    {
        // InPort�f�[�^�̓ǂݍ���
        m_image_origIn.read();

        // �T�C�Y���ς�����Ƃ������Đ�������
        if(m_in_height != m_image_orig.height || m_in_width != m_image_orig.width)
        {
            printf("[onExecute] Size of input image is not match!\n");

            m_in_height = m_image_orig.height;
            m_in_width  = m_image_orig.width;
            
            if(m_image_buff       != NULL)
                cvReleaseImage(&m_image_buff);
            if(m_hsvImage         != NULL)
                cvReleaseImage(&m_hsvImage);
            if(m_hueImage         != NULL)
                cvReleaseImage(&m_hueImage);
            if(m_saturationImage  != NULL)
                cvReleaseImage(&m_saturationImage);
            if(m_valueImage       != NULL)
                cvReleaseImage(&m_valueImage);
            if(m_mergeImage       != NULL)
                cvReleaseImage(&m_mergeImage);
            if(m_destinationImage != NULL)
                cvReleaseImage(&m_destinationImage);

            // �T�C�Y�ϊ��̂���Temp�������[���悢����
            m_image_buff       = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);

	        m_hsvImage         = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);

            m_hueImage         = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1);
	        m_saturationImage  = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1);
	        m_valueImage       = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1);

	        m_mergeImage       = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_destinationImage = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPort�̉摜�f�[�^��IplImage��imageData�ɃR�s�[
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative actions

        //	BGR����HSV�ɕϊ�����
        cvCvtColor(m_image_buff, m_hsvImage, CV_BGR2HSV);

        //	HSV�摜��H�AS�AV�摜�ɕ�����
        cvSplit(m_hsvImage, m_hueImage, m_saturationImage, m_valueImage, NULL); 

        //	H��S�̒l��ύX����
        cvSet(m_hueImage,        cvScalar( m_nHue ),        NULL);
        cvSet(m_saturationImage, cvScalar( m_nSaturation ), NULL);

        //	3�`�����l��������
        cvMerge(m_hueImage, m_saturationImage, m_valueImage, NULL, m_mergeImage);

        //	HSV����BGR�ɕϊ�����
        cvCvtColor(m_mergeImage, m_destinationImage, CV_HSV2BGR);

        // �摜�f�[�^�̃T�C�Y�擾
        int len = m_destinationImage->nChannels * m_destinationImage->width * m_destinationImage->height;
                
        // ��ʂ̃T�C�Y��������
        m_image_sepia.pixels.length(len);        
        m_image_sepia.width  = m_destinationImage->width;
        m_image_sepia.height = m_destinationImage->height;

        // ���]�����摜�f�[�^��OutPort�ɃR�s�[
        memcpy((void *)&(m_image_sepia.pixels[0]), m_destinationImage->imageData,len);

        // ���]�����摜�f�[�^��OutPort����o�͂���B
        m_image_sepiaOut.write();
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Sepia::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Sepia::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void SepiaInit(RTC::Manager* manager)
  {
    coil::Properties profile(sepia_spec);
    manager->registerFactory(profile,
                             RTC::Create<Sepia>,
                             RTC::Delete<Sepia>);
  }
  
};


