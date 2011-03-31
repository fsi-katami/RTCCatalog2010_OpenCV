// -*- C++ -*-
/*!
 * @file  Translate.cpp
 * @brief Translate image component
 * @date $Date$
 *
 * $Id$
 */

#include "Translate.h"

// Module specification
// <rtc-template block="module_spec">
static const char* Translate_spec[] =
  {
    "implementation_id", "Translate",
    "type_name",         "Translate",
    "description",       "Translate image component",
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
    "conf.default.Translate_x", "50",
    "conf.default.Translate_y", "50",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Translate::Translate(RTC::Manager* manager)
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
Translate::~Translate()
{
}



RTC::ReturnCode_t Translate::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("Translate_x", m_nTransX, "50");
  bindParameter("Translate_y", m_nTransY, "50");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Translate::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Translate::onActivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̊m��
    m_image_buff       = NULL;
    m_image_dest       = NULL;

    m_in_height = 0;
    m_in_width  = 0;

    //	�s��𐶐�����
	m_transformMatrix = cvCreateMat( 2, 3, CV_32FC1);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Translate::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_dest       != NULL)
        cvReleaseImage(&m_image_dest);

    cvReleaseMat(&m_transformMatrix);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Translate::onExecute(RTC::UniqueId ec_id)
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
            if(m_image_dest       != NULL)
                cvReleaseImage(&m_image_dest);


            // �T�C�Y�ϊ��̂���Temp�������[���悢����
	        m_image_buff = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_image_dest = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPort�̉摜�f�[�^��IplImage��imageData�ɃR�s�[
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative process
	    CvPoint2D32f original[3];	//	�ϊ��O���W
	    CvPoint2D32f Translate[3];	//	�ϊ�����W

	    //�ϊ��O�̍��W��ݒ肷��
	    original[0] = cvPoint2D32f( 0, 0 );
	    original[1] = cvPoint2D32f( m_image_buff->width, 0 );
	    original[2] = cvPoint2D32f( 0, m_image_buff->height );

	    //�ϊ���̍��W��ݒ肷��
	    Translate[0] = cvPoint2D32f( m_nTransX,                       m_nTransY );
	    Translate[1] = cvPoint2D32f( m_nTransX + m_image_buff->width, m_nTransY );
	    Translate[2] = cvPoint2D32f( m_nTransX,                       m_nTransY + m_image_buff->height );

	    //	�ϊ��s������߂�
	    cvGetAffineTransform( original, Translate, m_transformMatrix );

	    //	�ϊ��s��𔽉f������
	    cvWarpAffine( m_image_buff, m_image_dest, m_transformMatrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll( 0 ) );

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
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Translate::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Translate::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void TranslateInit(RTC::Manager* manager)
  {
    coil::Properties profile(Translate_spec);
    manager->registerFactory(profile,
                             RTC::Create<Translate>,
                             RTC::Delete<Translate>);
  }
  
};


