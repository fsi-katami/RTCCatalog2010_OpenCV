// -*- C++ -*-
/*!
 * @file  Perspective.cpp
 * @brief Perspective image component
 * @date $Date$
 *
 * $Id$
 */

#include "Perspective.h"

// Module specification
// <rtc-template block="module_spec">
static const char* perspective_spec[] =
  {
    "implementation_id", "Perspective",
    "type_name",         "Perspective",
    "description",       "Perspective image component",
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
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Perspective::Perspective(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
    m_image_outOut("out_image", m_image_out)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);
  
  // Set OutPort buffer
  registerOutPort("out_image", m_image_outOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Perspective::~Perspective()
{
}


RTC::ReturnCode_t Perspective::onInitialize()
{
    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Perspective::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Perspective::onActivated(RTC::UniqueId ec_id)
{
    // �C���[�W�p�������̊m��
    m_image_buff       = NULL;
    m_image_dest       = NULL;

    m_in_height  = 0;
    m_in_width   = 0;

    //	�s��𐶐�����
	m_perspectiveMatrix = cvCreateMat( 3, 3, CV_32FC1);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Perspective::onDeactivated(RTC::UniqueId ec_id)
{
    if(m_image_buff       != NULL)
        cvReleaseImage(&m_image_buff);
    if(m_image_dest         != NULL)
        cvReleaseImage(&m_image_dest);

    cvReleaseMat(&m_perspectiveMatrix);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Perspective::onExecute(RTC::UniqueId ec_id)
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
            if(m_image_dest         != NULL)
                cvReleaseImage(&m_image_dest);


            // �T�C�Y�ϊ��̂���Temp�������[���悢����
	        m_image_buff = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
	        m_image_dest = cvCreateImage(cvSize(m_in_width, m_in_height), IPL_DEPTH_8U, 3);
        }

        // InPort�̉摜�f�[�^��IplImage��imageData�ɃR�s�[
        memcpy(m_image_buff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

        // Anternative actions

	    CvPoint2D32f original[4];	//	�ϊ��O���W
	    CvPoint2D32f translate[4];	//	�ϊ�����W

	    //	�ϊ��O�̍��W��ݒ肷��
	    original[0] = cvPoint2D32f( 0, 0 );
	    original[1] = cvPoint2D32f( m_image_buff->width, 0 );
	    original[2] = cvPoint2D32f( 0, m_image_buff->height );
	    original[3] = cvPoint2D32f( m_image_buff->width, m_image_buff->height );

	    //	�ϊ���̍��W��ݒ肷��
	    translate[0] = cvPoint2D32f( m_image_buff->width / 5 * 1, m_image_buff->height / 5 * 2 );
	    translate[1] = cvPoint2D32f( m_image_buff->width / 5 * 4, m_image_buff->height / 5 * 2 );
	    translate[2] = cvPoint2D32f(                           0, m_image_buff->height / 5 * 4 );
	    translate[3] = cvPoint2D32f( m_image_buff->width        , m_image_buff->height / 5 * 4 );

	    //	�ϊ��s������߂�
	    cvGetPerspectiveTransform( original, translate, m_perspectiveMatrix );

	    //	�ϊ��s��𔽉f������
	    cvWarpPerspective( m_image_buff, m_image_dest, m_perspectiveMatrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll( 0 ) );

        // �摜�f�[�^�̃T�C�Y�擾
        int len = m_image_dest->nChannels * m_image_dest->width * m_image_dest->height;
                
        // ��ʂ̃T�C�Y��������
        m_image_out.pixels.length(len);        
        m_image_out.width  = m_image_dest->width;
        m_image_out.height = m_image_dest->height;

        // ���]�����摜�f�[�^��OutPort�ɃR�s�[
        memcpy((void *)&(m_image_out.pixels[0]), m_image_dest->imageData,len);

        // ���]�����摜�f�[�^��OutPort����o�͂���B
        m_image_outOut.write();
    }

    return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Perspective::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Perspective::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void PerspectiveInit(RTC::Manager* manager)
  {
    coil::Properties profile(perspective_spec);
    manager->registerFactory(profile,
                             RTC::Create<Perspective>,
                             RTC::Delete<Perspective>);
  }
  
};


