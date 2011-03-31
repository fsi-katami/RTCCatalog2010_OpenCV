// -*- C++ -*-
/*!
 * @file  Binarization.cpp
 * @brief Binarization image component
 * @date $Date$
 *
 * $Id$
 */

#include "Binarization.h"

#define THRESHOLD_MAX_VALUE	255	//	2�l���̍ۂɎg�p����ő�l

// Module specification
// <rtc-template block="module_spec">
static const char* binarization_spec[] =
  {
    "implementation_id", "Binarization",
    "type_name",         "Binarization",
    "description",       "Binarization image component",
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
    "conf.default.threshold_level", "128",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Binarization::Binarization(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_outputOut("output_image", m_image_output)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("output_image", m_image_outputOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Binarization::~Binarization()
{
}

IplImage *sourceImage;	//	���摜�pIplImage



RTC::ReturnCode_t Binarization::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("threshold_level", m_nThresholdLv, "128");

  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Binarization::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Binarization::onActivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̊m��
    m_image_buff       = NULL;
    m_image_binary     = NULL;
    m_image_gray       = NULL;
    m_image_dest       = NULL;

    m_in_height        = 0;
    m_in_width         = 0;

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Binarization::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_binary     != NULL)
        cvReleaseImage(&m_image_binary);
    if(m_image_gray       != NULL)
        cvReleaseImage(&m_image_gray);
    if(m_image_dest       != NULL)
        cvReleaseImage(&m_image_dest);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Binarization::onExecute(RTC::UniqueId ec_id)
{
    // Common CV process
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
            if(m_image_binary     != NULL)
                cvReleaseImage(&m_image_binary);
            if(m_image_gray       != NULL)
                cvReleaseImage(&m_image_gray);
            if(m_image_dest       != NULL)
                cvReleaseImage(&m_image_dest);


            // �T�C�Y�ϊ��̂���Temp�������[���悢����
	        m_image_buff   = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_image_binary = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1);
            m_image_gray   = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 1);
	        m_image_dest   = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPort�̉摜�f�[�^��IplImage��imageData�ɃR�s�[
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative process
       	//	BGR����O���[�X�P�[���ɕϊ�����
	    cvCvtColor( m_image_buff, m_image_gray, CV_BGR2GRAY );

	    //	�O���[�X�P�[������2�l�ɕϊ�����
	    cvThreshold( m_image_gray, m_image_binary, m_nThresholdLv, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );

        // Convert to 3channel image
        cvMerge(m_image_binary, m_image_binary, m_image_binary, NULL, m_image_dest);

        // Common process
        // �摜�f�[�^�̃T�C�Y�擾
        int len = m_image_dest->nChannels * m_image_dest->width * m_image_dest->height;
                
        // ��ʂ̃T�C�Y��������
        m_image_output.pixels.length(len);        
        m_image_output.width  = m_image_dest->width;
        m_image_output.height = m_image_dest->height;

        // ���]�����摜�f�[�^��OutPort�ɃR�s�[
        memcpy((void *)&(m_image_output.pixels[0]), m_image_dest->imageData,len);

        // ���]�����摜�f�[�^��OutPort����o�͂���B
        m_image_outputOut.write();

        //cvWaitKey( 0 );
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Binarization::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Binarization::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void BinarizationInit(RTC::Manager* manager)
  {
    coil::Properties profile(binarization_spec);
    manager->registerFactory(profile,
                             RTC::Create<Binarization>,
                             RTC::Delete<Binarization>);
  }
  
};


