// -*- C++ -*-
/*!
 * @file  Histogram.cpp
 * @brief Histogram image component
 * @date $Date$
 *
 * $Id$
 */

#include "Histogram.h"
#include <iostream>
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* histogram_spec[] =
  {
    "implementation_id", "Histogram",
    "type_name",         "Histogram",
    "description",       "Histogram image component",
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
    "conf.default.brightness", "100",
    "conf.default.contrast", "100",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Histogram::Histogram(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_image_origIn("original_image", m_image_orig),
	m_image_histogramImageOut("histogram_image", m_image_histogramImage),
    m_image_histogramOut("histogram", m_image_histogram)

    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("original_image", m_image_origIn);

  // Set OutPort buffer
  registerOutPort("histogram_image", m_image_histogramImageOut);
  
  // Set OutPort buffer
  registerOutPort("histogram", m_image_histogramOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

/*!
 * @brief destructor
 */
Histogram::~Histogram()
{
}



RTC::ReturnCode_t Histogram::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("brightness", m_brightness, "100");
  bindParameter("contrast", m_contrast, "100");

  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Histogram::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Histogram::onActivated(RTC::UniqueId ec_id)
{
  //  �C���[�W�p�������̏�����
  imageBuff = NULL;
  grayImage = NULL;
  destinationImage = NULL;
  histogramImage = NULL;
  histogramBarImage = NULL;

  //  OutPort��ʃT�C�Y�̏�����
  m_image_histogram.width = 0;
  m_image_histogram.height = 0;
  m_image_histogramImage.width = 0;
  m_image_histogramImage.height = 0;

  //  �q�X�g�O�����ɕ`�悳���c�_�̐�
  histogramSize = 128;
  //  �q�X�g�O�����͈̔�
  range_0[0] = 0;
  range_0[1] = 256;

  //  �q�X�g�O�����e�����͈̔͂������z��̃|�C���^
  ranges[0] = range_0 ;

  //  �q�X�g�O�����𐶐�
  histogram = cvCreateHist( DIMENSIONS, &histogramSize, CV_HIST_ARRAY, ranges, UNIFORM );

  //  �s��𐶐�
  lookUpTableMatrix = cvCreateMatHeader( 1, 256, CV_8UC1 );

  //  �Z�x�Ή��s��ɔZ�x�Ή��\���Z�b�g
  cvSetData( lookUpTableMatrix, lookUpTable, NULL );

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Histogram::onDeactivated(RTC::UniqueId ec_id)
{
  if( imageBuff != NULL )
  {
	  //  �C���[�W�p�������̉��
	  cvReleaseImage(&imageBuff);
	  cvReleaseImage(&grayImage);
	  cvReleaseImage(&destinationImage);
	  cvReleaseImage(&histogramImage);
	  cvReleaseImage(&histogramBarImage);
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Histogram::onExecute(RTC::UniqueId ec_id)
{
	// �V�����f�[�^�̃`�F�b�N
  if(m_image_origIn.isNew()){
	  //  InPort�f�[�^�̓ǂݍ���
	  m_image_origIn.read();

	  //  InPort��OutPort�̉�ʃT�C�Y��������уC���[�W�p�������m��
	  if(m_image_orig.width != m_image_histogram.width || m_image_orig.height != m_image_histogram.height)
	  {
		  m_image_histogram.width = m_image_histogramImage.width = m_image_orig.width;
		  m_image_histogram.height = m_image_histogramImage.height = m_image_orig.height;

		  //  InPort�̃C���[�W�T�C�Y���ύX���ꂽ�ꍇ
		  if( imageBuff != NULL )
		  {
			  cvReleaseImage(&imageBuff);
			  cvReleaseImage(&grayImage);
			  cvReleaseImage(&destinationImage);
			  cvReleaseImage(&histogramImage);
			  cvReleaseImage(&histogramBarImage);
		  }

		  //  �C���[�W�p�������̊m��
		  imageBuff = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  grayImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  destinationImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 1 );
		  histogramImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );
		  histogramBarImage = cvCreateImage( cvSize(m_image_orig.width, m_image_orig.height), IPL_DEPTH_8U, 3 );

		  //	�q�X�g�O�����̏c�_�̉������v�Z����
		  bin_w = cvRound( ( double )histogramBarImage->width / histogramSize );
	  }

	  //  InPort�̉�ʃf�[�^���R�s�[
	  memcpy(imageBuff->imageData,(void *)&(m_image_orig.pixels[0]),m_image_orig.pixels.length());

	  //  RGB����O���[�X�P�[���ɕϊ�
	  cvCvtColor( imageBuff, grayImage, CV_RGB2GRAY);
	  
	  int brightness = m_brightness - TRACKBAR_MAX_VALUE / 2;	//	�P�x�l
      int contrast = m_contrast - TRACKBAR_MAX_VALUE / 2;		//	�R���g���X�g

	  if ( contrast > 0 ) {
        double delta = 127.0 * contrast / 100.0;
        double a = 255.0 / ( 255.0 - delta * 2 );
        double b = a * ( brightness - delta );
        for (int i = 0; i < 256; i++ ){
			//	�ϊ���̊K�������߂�
            int v = cvRound( a * i + b );
			if( v < 0 ){
                v = 0;
			}
			if( v > 255 ){
                v = 255;
			}
            lookUpTable[i] = ( unsigned char )v;
        }
	   } else {
        double delta = -128.0 * contrast / 100.0;
        double a = (256.0 - delta * 2.0) / 255.0;
        double b = a * brightness + delta;
        for(int i = 0; i < 256; i++ ){
            int v = cvRound( a * i + b);
			if( v < 0 ){
                v = 0;
			}
			if( v > 255 ){
                v = 255;
			}
            lookUpTable[i] = ( unsigned char )v;
        }
      }
	  
	  //  �Z�x�Ή��s���p�����Z�x�K���ϊ����s��
	  cvLUT( grayImage, destinationImage, lookUpTableMatrix );

	  //  �O���[�X�P�[������RGB�ɕϊ�
	  cvCvtColor( destinationImage, histogramImage, CV_GRAY2RGB );

	  //  �摜�f�[�^�̃T�C�Y�擾
	  int len = histogramImage->nChannels * histogramImage->width * histogramImage->height;
	  m_image_histogramImage.pixels.length(len);

	  //  �ϓ]�����摜�f�[�^��OutPort�ɃR�s�[
	  memcpy((void *)&(m_image_histogramImage.pixels[0]), histogramImage->imageData,len);

	  //  �ϓ]�����摜�f�[�^��OutPort����o��
	  m_image_histogramImageOut.write();

	  //  �摜�̃q�X�g�O�������v�Z����
	  cvCalcHist( &destinationImage, histogram, ACCUMULATE, NULL );

	  float max_value = 0;
	  //  �q�X�g�O�����l�̍ő�l�𓾂�
      cvGetMinMaxHistValue( histogram, NULL, &max_value, NULL, NULL );

	  //  �q�X�g�O�������ő�l�ɂ���Đ��K������
	  cvConvertScale( histogram->bins, histogram->bins, 
		( ( double )histogramBarImage->height ) / max_value, SCALE_SHIFT );

	  //	�q�X�g�O�����摜�𔒂ŏ���������
	  cvSet( histogramBarImage, cvScalarAll( 255 ), NULL );

	  //	�q�X�g�O�����̏c�_��`�悷��
	  for ( int i = 0; i < histogramSize; i++ ) {
          cvRectangle(
		  	histogramBarImage,
		  	cvPoint( i * bin_w, histogramBarImage->height ),
			cvPoint( ( i + 1 ) * bin_w,histogramBarImage->height - cvRound( cvGetReal1D( histogram->bins, i) ) ),
			cvScalarAll( 0 ),
			LINE_THICKNESS,
			LINE_TYPE,
			SHIFT
		  );
	  }

	  //  �摜�f�[�^�̃T�C�Y�擾
	  len = histogramBarImage->nChannels * histogramBarImage->width * histogramBarImage->height;
	  m_image_histogram.pixels.length(len);

	  //  �ϓ]�����摜�f�[�^��OutPort�ɃR�s�[
	  memcpy((void *)&(m_image_histogram.pixels[0]), histogramBarImage->imageData,len);

	  //  �ϓ]�����摜�f�[�^��OutPort����o��
	  m_image_histogramOut.write();

  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Histogram::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Histogram::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void HistogramInit(RTC::Manager* manager)
  {
    coil::Properties profile(histogram_spec);
    manager->registerFactory(profile,
                             RTC::Create<Histogram>,
                             RTC::Delete<Histogram>);
  }
  
};


