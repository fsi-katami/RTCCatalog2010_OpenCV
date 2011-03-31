// -*- C++ -*-
/*!
 * @file  Scale.cpp
 * @brief Scale image component
 * @date $Date$
 *
 * $Id$
 */

#include "Scale.h"

// Module specification
// <rtc-template block="module_spec">
static const char* scale_spec[] =
  {
    "implementation_id", "Scale",
    "type_name",         "Scale",
    "description",       "Scale image component",
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
    "conf.default.output_scale_x",  "1.0",
    "conf.default.output_scale_y",  "1.0",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Scale::Scale(RTC::Manager* manager)
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
Scale::~Scale()
{
}



RTC::ReturnCode_t Scale::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("output_scale_x", m_scale_x,    "1.0");
  bindParameter("output_scale_y", m_scale_y,    "1.0");

  m_currentScaleX = 1.0;
  m_currentScaleY = 1.0;

  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Scale::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Scale::onActivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̊m��
    m_image_buff       = NULL;
    m_image_dest       = NULL;

    m_currentScaleX    = m_scale_x;
    m_currentScaleY    = m_scale_y;

    m_in_height        = 0;
    m_in_width         = 0;
    
    return RTC::RTC_OK;
}


RTC::ReturnCode_t Scale::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff != NULL)
        cvReleaseImage(&m_image_buff);
    
    if(m_image_dest != NULL)
        cvReleaseImage(&m_image_dest);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Scale::onExecute(RTC::UniqueId ec_id)
{
    // Common CV actions
    // �V�����f�[�^�̃`�F�b�N
    if (m_image_origIn.isNew()) 
    {
        // InPort�f�[�^�̓ǂݍ���
        m_image_origIn.read();

        // Anternative actions

        // �T�C�Y���ς�����Ƃ������Đ�������
        if(m_in_height != m_image_orig.height || m_in_width != m_image_orig.width)
        {
            printf("[onExecute] Size of input image is not match!\n");

            m_in_height = m_image_orig.height;
            m_in_width  = m_image_orig.width;
            
            if(m_image_buff != NULL)
                cvReleaseImage(&m_image_buff);

            // �T�C�Y�ϊ��̂���Temp�������[���悢����
            m_image_buff = cvCreateImage(cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3);
        }

        // InPort�̉摜�f�[�^��IplImage��imageData�ɃR�s�[
        memcpy(m_image_buff->imageData, (void *)&(m_image_orig.pixels[0]), m_image_orig.pixels.length());

	    // �g��䗦���X�V���ꂽ��o�̓C���[�W�p���������Ċm�ۂ���
        if(m_image_dest == NULL || m_currentScaleX != m_scale_x || m_currentScaleY != m_scale_y)
        {
            m_currentScaleX    = m_scale_x;
            m_currentScaleY    = m_scale_y;

            printf( "[onExecute] Sacle has been changed to (%f, %f)\n", m_scale_x, m_scale_y);
            printf( "[onExecute] Realloc memory for output-image by (%d, %d)\n", (int)(m_in_width  * m_currentScaleX), 
                                                                                 (int)(m_in_height * m_currentScaleY));

            // �����̃��������������
            if(m_image_dest != NULL)
                cvReleaseImage(&m_image_dest);
    	    m_image_dest = cvCreateImage(cvSize((int)(m_in_width  * m_currentScaleX), 
                                                (int)(m_in_height * m_currentScaleY)), IPL_DEPTH_8U, 3);
        }        

	    // �摜�̑傫����ϊ�����
	    cvResize( m_image_buff, m_image_dest, CV_INTER_LINEAR );

        // �摜�f�[�^�̃T�C�Y�擾
        int len = m_image_dest->nChannels * m_image_dest->width * m_image_dest->height;
                
        m_image_output.pixels.length(len);
        // ��ʂ̃T�C�Y��������
        m_image_output.width  = m_image_dest->width;
        m_image_output.height = m_image_dest->height;

        // ���]�����摜�f�[�^��OutPort�ɃR�s�[
        memcpy((void *)&(m_image_output.pixels[0]), m_image_dest->imageData,len);

        // ���]�����摜�f�[�^��OutPort����o�͂���B
        m_image_outputOut.write();
    }

    return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t Scale::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Scale::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ScaleInit(RTC::Manager* manager)
  {
    coil::Properties profile(scale_spec);
    manager->registerFactory(profile,
                             RTC::Create<Scale>,
                             RTC::Delete<Scale>);
  }
  
};


