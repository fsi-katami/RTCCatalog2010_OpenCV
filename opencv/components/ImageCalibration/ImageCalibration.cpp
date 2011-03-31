// -*- C++ -*-
/*!
 * @file  ImageCalibration.cpp
 * @brief ImageCalibration component
 * @date $Date$
 *
 * $Id$
 */

#include "ImageCalibration.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imagecalibration_spec[] =
  {
    "implementation_id", "ImageCalibration",
    "type_name",         "ImageCalibration",
    "description",       "ImageCalibration component",
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
    "conf.default.board_w", "11",
    "conf.default.board_h", "8",
	"conf.default.camera_Height", "-20",
    ""
  };
// </rtc-template>

//IplImage *inputImage_buff;
//IplImage *outputImage_buff;
//IplImage *tempImage_buff;

int m_board_w;
int m_board_h;
int g_temp_w = 0;
int g_temp_h = 0;

//CvPoint2D32f* corners = new CvPoint2D32f[11 * 8];
CvPoint2D32f objPts[4], imgPts[4];

CvSize board_sz;

char* renseParameters = new char[200];
char* internalParameter = new char[200];
char* externalParameter = new char[200];


/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageCalibration::ImageCalibration(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inputImageIn("inputImage", m_inputImage),
	m_keyIn("key", m_key),
    m_origImageOut("orignalImage", m_origImage),
	//m_calbImageOut("calbImage", m_calbImage),
    m_birdImageOut("birdImage", m_birdImage),
	m_internalParameterOut("internalParameter", m_internalParameter),
	m_externalParameterOut("externalParameter", m_externalParameter),
	m_renseParameterOut("renseParameter", m_renseParameter),

    // </rtc-template>
	dummy(0)
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  registerInPort("inputImage", m_inputImageIn);
  registerInPort("key", m_keyIn);
  
  // Set OutPort buffer
  registerOutPort("orignalImage", m_origImageOut);
  //registerOutPort("calbImage", m_calbImageOut);
  registerOutPort("birdImage", m_birdImageOut);
  registerOutPort("internalParameter", m_internalParameterOut);
  registerOutPort("externalParameter", m_externalParameterOut);
  registerOutPort("renseParameter", m_renseParameterOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

void saveRenseMatrix(CvMat *matrixRense){
	sprintf(renseParameters, "%lf\n%lf\n%lf\n%lf\n",
		cvmGet(matrixRense,0,0), 
		cvmGet(matrixRense,1,0), 
		cvmGet(matrixRense,2,0),
		cvmGet(matrixRense,3,0)
	);
	
}

void saveInternalParameterMatrix(CvMat *matrix){
	sprintf(internalParameter, 
		"%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
		cvmGet(matrix,0,0), 
		cvmGet(matrix,0,1), 
		cvmGet(matrix,0,2),
		cvmGet(matrix,1,0), 
		cvmGet(matrix,1,1), 
		cvmGet(matrix,1,2),
		cvmGet(matrix,2,0), 
		cvmGet(matrix,2,1), 
		cvmGet(matrix,2,2)
		
	);
}

void saveExternalParameterMatrix(CvMat *Matrix, CvMat *Vector){
	sprintf(externalParameter, 
		"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		cvmGet( Matrix, 0, 0),
		cvmGet( Matrix, 0, 1),
		cvmGet( Matrix, 0, 2),
		cvmGet( Vector, 0, 0),
		cvmGet( Matrix, 1, 0 ),
		cvmGet( Matrix, 1, 1 ),
		cvmGet( Matrix, 1, 2 ),
		cvmGet( Vector, 0, 1 ),
		cvmGet( Matrix, 2, 0 ),
		cvmGet( Matrix, 2, 1 ),
		cvmGet( Matrix, 2, 2 ),
		cvmGet( Vector, 0, 2 )
	);
}

//
//	�s�����ʂɕ\������
//
//	����:
//		disp   : �\�����鐔�l�̏���
//		matrix : �\������s��
//
void printMatrix( char *disp, CvMat *matrix ) {
    for ( int y=0; y < matrix->height; y++ ) {
        for ( int x=0; x < matrix->width; x++ ) {
            printf( disp, cvmGet( matrix, y, x ) ); 
        }
        printf( "\n" );
    }
    printf( "\n" );

}
//
//	�O���p�����[�^�s�����ʂɕ\������
//
//	����:
//		rotationMatrix    :  ��]�s��
//		translationVector :�@���i�x�N�g��
//
void printExtrinsicMatrix( CvMat *rotationMatrix, CvMat *translationVector ) {
	for ( int i = 0; i<3; i++ ) {
		printf(
			"%lf %lf %lf %lf\n",
			cvmGet( rotationMatrix, i, 0 ),
			cvmGet( rotationMatrix, i, 1 ),
			cvmGet( rotationMatrix, i, 2 ),
			cvmGet( translationVector, 0, i )
		);

	}

	
}

//
//	cvFindChessboardCorners�p�̃t���O�𐶐�����
//
int createFindChessboardCornersFlag() {
	int flag = 0;

	if ( ADAPTIVE_THRESH != 0 ) {
		flag = flag | CV_CALIB_CB_ADAPTIVE_THRESH;
	}
	if ( NORMALIZE_IMAGE != 0 ) {
		flag = flag | CV_CALIB_CB_NORMALIZE_IMAGE;
	}
	if ( FILTER_QUADS != 0 ) {
		flag = flag | CV_CALIB_CB_FILTER_QUADS;
	}

	return flag;
}

//
//	�R�[�i�[�����o����
//
//	����:
//      frameImage : �L���v�`���摜�pIplImage
//      grayImage  : �O���[�X�P�[���摜�pIplImage
//      corners    : �R�[�i�[�̈ʒu���i�[����ϐ�
//
//	�߂�l:
//		0   : �R�[�i�[�����ׂČ��o�ł��Ȃ������ꍇ
//		��0 : �R�[�i�[�����ׂČ��o���ꂽ�ꍇ
//
int findCorners( IplImage *frameImage, IplImage *grayImage, CvPoint2D32f *corners ) {
	int cornerCount;				//	���o�����R�[�i�[�̐�
	int findChessboardCornersFlag;	//	cvFindChessboardCorners�p�t���O
	int findFlag;					//	�R�[�i�[�����ׂČ��o�ł������̃t���O
	
	IplImage* m_image_binary;
	IplImage* m_set_image;
	
	m_image_binary     = cvCreateImage(cvSize(frameImage->width, frameImage->height), IPL_DEPTH_8U, 1);
	m_set_image     = cvCreateImage(cvSize(frameImage->width, frameImage->height), IPL_DEPTH_8U, 3);

	//	cvChessboardCorners�p�t���O�𐶐�����
	findChessboardCornersFlag = createFindChessboardCornersFlag();
	
	//�@�摜��BinaryImage�Ƃ��ĕϊ�����B
	//	�R�[�i�[�����o����
	cvCvtColor( frameImage, grayImage, CV_BGR2GRAY );

    //	�O���[�X�P�[������2�l�ɕϊ�����
    cvThreshold( grayImage, m_image_binary, 128, 255, CV_THRESH_BINARY );

    // Convert to 3channel image
    cvMerge(m_image_binary, m_image_binary, m_image_binary, NULL, m_set_image);

	findFlag=cvFindChessboardCorners(
		m_set_image,
		//m_set_image,
		//cvSize( CORNER_WIDTH, CORNER_HEIGHT ),
		board_sz,
		corners,
		&cornerCount,
		findChessboardCornersFlag
	);
	
	if( findFlag != 0 ) {
		//	�R�[�i�[�����ׂČ��o���ꂽ�ꍇ
		//	���o���ꂽ�R�[�i�[�̈ʒu���T�u�s�N�Z���P�ʂɂ���

		CvTermCriteria criteria={ CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_ITERATIONS, EPSILON };
		cvFindCornerSubPix(
			grayImage,
			corners,
			cornerCount,
			cvSize( SEARCH_WINDOW_HALF_WIDTH, SEARCH_WINDOW_HALF_HEIGHT ),
			cvSize( DEAD_REGION_HALF_WIDTH, DEAD_REGION_HALF_HEIGHT ), 
			cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_ITERATIONS, EPSILON )
		);
	}
		
	//	�R�[�i�[�̈ʒu��`��
	cvDrawChessboardCorners( frameImage, board_sz, corners, cornerCount, findFlag );
	
	cvReleaseImage(&m_set_image);
	cvReleaseImage(&m_image_binary);
	
	return findFlag;
}

/*!
 * @brief destructor
 */
ImageCalibration::~ImageCalibration()
{
}



RTC::ReturnCode_t ImageCalibration::onInitialize()
{
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("board_w", m_board_w, "11");
  bindParameter("board_h", m_board_h, "8");
  bindParameter("camera_Height", m_camera_Height, "-20");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageCalibration::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageCalibration::onActivated(RTC::UniqueId ec_id)
{	
	return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageCalibration::onDeactivated(RTC::UniqueId ec_id)
{

	return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageCalibration::onExecute(RTC::UniqueId ec_id)
{	
	
	board_sz = cvSize(m_board_w, m_board_h);
	
	//Calibration�p�^�[�����v�Z����B
	if (m_inputImageIn.isNew()) {

		m_inputImageIn.read();

		if(m_keyIn.isNew()){
			m_keyIn.read();
			key = (int)m_key.data;
		}
		
		if(g_temp_w != m_inputImage.width || g_temp_h != m_inputImage.height){
		
			inputImage_buff = cvCreateImage(cvSize(m_inputImage.width, m_inputImage.height), 8, 3);
			outputImage_buff = cvCreateImage(cvSize(m_inputImage.width, m_inputImage.height), 8, 3);
			tempImage_buff = cvCreateImage(cvSize(m_inputImage.width, m_inputImage.height), 8, 3);
			undistortionImage = cvCreateImage(cvSize(m_inputImage.width, m_inputImage.height), 8, 3);
			birds_image = cvCreateImage(cvSize(m_inputImage.width, m_inputImage.height), 8, 3);
			
			intrinsicMatrix = cvCreateMat(3,3,CV_64FC1);
			distortionCoefficient = cvCreateMat(4,1,CV_64FC1);
			
			captureCount = 0;
			findFlag = 0;

			mapx = cvCreateImage( cvSize(m_inputImage.width, m_inputImage.height), IPL_DEPTH_32F, 1);
			mapy = cvCreateImage( cvSize(m_inputImage.width, m_inputImage.height), IPL_DEPTH_32F, 1);

			corners = new CvPoint2D32f[m_board_w * m_board_h];
			
			g_temp_w = m_inputImage.width;
			g_temp_h = m_inputImage.height;
		
		}

		//Capture�J�n����B
		memcpy(inputImage_buff->imageData,(void *)&(m_inputImage.pixels[0]), m_inputImage.pixels.length());

//		tempImage_buff = cvCloneImage(inputImage_buff);
		//OutPort�ɏo�͂���B
		int len = inputImage_buff->nChannels * inputImage_buff->width * inputImage_buff->height;
		m_origImage.pixels.length(len);
		
		memcpy((void *)&(m_origImage.pixels[0]), inputImage_buff->imageData, len);
		m_origImage.width = inputImage_buff->width;
		m_origImage.height = inputImage_buff->height;

		m_origImageOut.write();
		
		//Capture�m�F�p��Window�̐���
		//cvShowImage("Capture", inputImage_buff);
		cvWaitKey(1);
		
		//SpaceBar�������ƃT���v���f��5�����B��
		if (key == ' ') {
			
			tempImage_buff = cvCloneImage(inputImage_buff);
			//�f���𐶐�����
			IplImage *grayImage = cvCreateImage(cvGetSize(tempImage_buff), 8, 1);

			//�s��̐���
			CvMat *worldCoordinates = cvCreateMat((m_board_w * m_board_h) * NUM_OF_BACKGROUND_FRAMES, 3, CV_64FC1); //���E���W�p�s��
			CvMat *imageCoordinates = cvCreateMat((m_board_w * m_board_h) * NUM_OF_BACKGROUND_FRAMES ,2, CV_64FC1); //�摜���W�p�s��
			CvMat *pointCounts = cvCreateMat(NUM_OF_BACKGROUND_FRAMES, 1, CV_32SC1); //�R�[�i�[���̍s��
			CvMat *rotationVectors = cvCreateMat(NUM_OF_BACKGROUND_FRAMES, 3, CV_64FC1); //��]�x�N�g��
			CvMat *translationVectors = cvCreateMat(NUM_OF_BACKGROUND_FRAMES, 3, CV_64FC1); 

			//���E���W��ݒ肷��
			for (int i = 0; i < NUM_OF_BACKGROUND_FRAMES; i++){
				for ( int j = 0; j < (m_board_w * m_board_h); j++) {
					cvSetReal2D(worldCoordinates, i * (m_board_w * m_board_h) + j, 0, (j % m_board_w) * UNIT);
					cvSetReal2D(worldCoordinates, i * (m_board_w * m_board_h) + j, 1, (j / m_board_w) * UNIT);
					cvSetReal2D(worldCoordinates, i * (m_board_w * m_board_h) + j, 2, 0.0);
				}
			}

			//�R�[�i�[����ݒ�
			for(int i = 0; i < NUM_OF_BACKGROUND_FRAMES; i++){
				cvSetReal2D(pointCounts, i, 0, (m_board_w * m_board_h));
			}
			
			//�R�[�i�[�����o����B
			findFlag = findCorners(tempImage_buff, grayImage, corners);

			if (findFlag != 0) {
			
				//�R�[�i�[�����ׂČ��o�����ꍇ
				//�f�����W��ݒ肷��B
				for (;;){
					for (int i = 0; i < (m_board_w * m_board_h); i++){
 						cvSetReal2D(imageCoordinates, captureCount * (m_board_w * m_board_h) + i, 0, corners[i].x);
						cvSetReal2D(imageCoordinates, captureCount * (m_board_w * m_board_h) + i, 1, corners[i].y);
					}
				
					captureCount++;    

					printf("%d���ڃL���v�`�����܂���\n", captureCount);

					if (captureCount == NUM_OF_BACKGROUND_FRAMES) {
						//�ݒ肵���񐔃`�F�b�N�p�^�[�����B�����ꍇ
						//�J�����p�����[�^�𐄒肷��B
						cvCalibrateCamera2(
							worldCoordinates,
							imageCoordinates,
							pointCounts,
							cvGetSize(inputImage_buff),
							intrinsicMatrix,
							distortionCoefficient,
							rotationVectors,
							translationVectors,
							CALIBRATE_CAMERA_FLAG
						);
						
						//����Text�Ƃ��ďo��
						printf("\n�����Y�c�݌W��\n");
						saveRenseMatrix(distortionCoefficient);
						printMatrix("%lf", distortionCoefficient);
						
						//m_renseParameter.data = renseParameters;
												
						printf("\n�����p�����[�^\n");
						saveInternalParameterMatrix(intrinsicMatrix);
						printMatrix("%lf ", intrinsicMatrix);

						//m_internalParameter.data = internalParameter;
						
						captureCount = 0;
						break;
						
					}
				}
			}

			if (findFlag != 0){
				InParameter = 1;
			}else if (findFlag == 0) {
				InParameter = 0;
			}
			
			//����������
			cvReleaseMat(&worldCoordinates);
			cvReleaseMat(&imageCoordinates);
			cvReleaseMat(&pointCounts);
			cvReleaseMat(&rotationVectors);
			cvReleaseMat(&translationVectors);
			cvReleaseImage(&grayImage);

		}
		g_temp_w = m_inputImage.width;
		g_temp_h = m_inputImage.height;

	}
	//�O���p�^�[�����擾
	if (key == ' ' && m_inputImageIn.isNew() && InParameter == 1) {

		//�s��̐���
		CvMat *worldCoordinates = cvCreateMat((m_board_w * m_board_h), 3, CV_64FC1); //���E���W�p�s��
		CvMat *imageCoordinates = cvCreateMat((m_board_w * m_board_h), 2, CV_64FC1); //�摜���W�p�s��
		CvMat *rotationVectors = cvCreateMat(1, 3, CV_64FC1); //��]�x�N�g��
		CvMat *rotationMatrix = cvCreateMat(3, 3, CV_64FC1); //��]�s��
		CvMat *translationVectors = cvCreateMat(1, 3, CV_64FC1); 

		//���E���W��ݒ肷��
		for (int i = 0; i < (m_board_w * m_board_h); i++){
			cvSetReal2D(worldCoordinates, i, 0, (i % m_board_w) * UNIT);
			cvSetReal2D(worldCoordinates, i, 1, (i / m_board_w) * UNIT);
			cvSetReal2D(worldCoordinates, i, 2, 0.0);
		}
	
		cvWaitKey( 1 );
	
		//	�X�y�[�X�L�[�������ꂽ��
		if ( findFlag != 0 ) {
			//	�R�[�i�[�����ׂČ��o���ꂽ�ꍇ
			//	�摜���W��ݒ肷��
			for ( int i = 0; i < (m_board_w * m_board_h); i++ ){
				cvSetReal2D( imageCoordinates, i, 0, corners[i].x);
				cvSetReal2D( imageCoordinates, i, 1, corners[i].y);
			}

			//	�O���p�����[�^�𐄒肷��
			cvFindExtrinsicCameraParams2(
				worldCoordinates,
				imageCoordinates,
				intrinsicMatrix,
				distortionCoefficient,
				rotationVectors,
				translationVectors
			);

			//	��]�x�N�g������]�s��ɕϊ�����
			cvRodrigues2( rotationVectors, rotationMatrix, NULL );

			printf( "\n�O���p�����[�^\n" );
			printExtrinsicMatrix( rotationMatrix, translationVectors );
			saveExternalParameterMatrix(rotationMatrix, translationVectors);

			m_externalParameter.data = CORBA::string_dup(externalParameter);
			m_renseParameter.data = CORBA::string_dup(renseParameters);
			m_internalParameter.data = CORBA::string_dup(internalParameter);
						
		}
		//�����������
		cvReleaseMat( &worldCoordinates );
		cvReleaseMat( &imageCoordinates );
		cvReleaseMat( &rotationVectors );
		cvReleaseMat( &rotationMatrix );
		cvReleaseMat( &translationVectors );
		
		//X,Y������
		cvInitUndistortMap(
			intrinsicMatrix,
			distortionCoefficient,
			mapx,
			mapy
		);
		//�O���p�����[�^�m�F�t���O
		outParameter = 1;
		key = 0;
				
	 }
	
	//�����O���p�����[�^�̏o�͂ɐ���������
	if (InParameter == 1 && outParameter == 1) {

		//	�����Y�c�݂�␳�����摜�𐶐�����
		cvUndistort2(
			inputImage_buff,
			undistortionImage,
			intrinsicMatrix,
			distortionCoefficient
		);

		//cvShowImage("�c�ݕ␳", undistortionImage);

		//OutPort�ɕ␳�f�����o�͂���B
		//int len = undistortionImage->nChannels * undistortionImage->width * undistortionImage->height;
		//m_calbImage.pixels.length(len);
		
		//�c�ݕ␳�f����OutPort�Ƃ��ă������R�s�[����B
		//memcpy((void *)&(m_calbImage.pixels[0]), undistortionImage->imageData, len);
		//m_calbImageOut.write();
		
		//���Ր}�̍��W�ݒ�
		objPts[0].x = 0;					objPts[0].y = 0;
		objPts[1].x = m_board_w-1;			objPts[1].y = 0;
		objPts[2].x = 0;					objPts[2].y = m_board_h-1;
		objPts[3].x = m_board_w-1;			objPts[3].y = m_board_h-1;
		
		//�擾����Corner��ݒ�
		imgPts[0] = corners[0];
		imgPts[1] = corners[m_board_w - 1];
		imgPts[2] = corners[(m_board_h - 1) * m_board_w];
		imgPts[3] = corners[(m_board_h - 1) * m_board_w + m_board_w - 1];
		
		//�w�肵��Corner�Ɂ����쐬����
		cvCircle(tempImage_buff, cvPointFrom32f(imgPts[0]), 9, CV_RGB(0,0,255), 3);
		cvCircle(tempImage_buff, cvPointFrom32f(imgPts[1]), 9, CV_RGB(0,255,0), 3);
		cvCircle(tempImage_buff, cvPointFrom32f(imgPts[2]), 9, CV_RGB(255,0,0), 3);
		cvCircle(tempImage_buff, cvPointFrom32f(imgPts[3]), 9, CV_RGB(255,255,0), 3);

		CvMat *H = cvCreateMat(3, 3, CV_32F);
		cvGetPerspectiveTransform(objPts, imgPts, H);
		
		//������ݒ肷��B
		CV_MAT_ELEM(*H, float, 2, 2) = m_camera_Height;
		
		//Warpping�����s
		cvWarpPerspective(inputImage_buff, birds_image, H, CV_INTER_LINEAR | CV_WARP_INVERSE_MAP | CV_WARP_FILL_OUTLIERS);
		
		//���Ր}��OutPort�ɏo�͂���B
		int len = birds_image->nChannels * birds_image->width * birds_image->height;
		m_birdImage.pixels.length(len);
		memcpy((void *)&(m_birdImage.pixels[0]), birds_image->imageData, len);

		m_birdImage.width = inputImage_buff->width;
		m_birdImage.height = inputImage_buff->height;

		m_birdImageOut.write();

		cvWaitKey(10);

		//cvShowImage("Bird_Eye", birds_image);
		cvReleaseMat(&H);

		g_temp_w = m_inputImage.width;
		g_temp_h = m_inputImage.height;

		key = 0;

	}

	//cvShowImage("Capture", inputImage_buff);
	
	if (InParameter == 1 && outParameter == 1) {	

		m_renseParameterOut.write();
		m_internalParameterOut.write();
		m_externalParameterOut.write();
	}

	if (g_temp_w != m_inputImage.width || g_temp_h != m_inputImage.height){
		
		if(intrinsicMatrix==NULL){
			cvReleaseMat(&intrinsicMatrix);
		}
		if(distortionCoefficient==NULL){
			cvReleaseMat(&distortionCoefficient);
		}
		
		if(mapx==NULL){
			cvReleaseImage(&mapx);
		}
		if(mapy==NULL){
			cvReleaseImage(&mapy);
		}
		if(inputImage_buff==NULL){
			cvReleaseImage(&inputImage_buff);
		}
		if(outputImage_buff==NULL){
			cvReleaseImage(&outputImage_buff);
		}
		if(tempImage_buff==NULL){
			cvReleaseImage(&tempImage_buff);
		}
		if(birds_image==NULL){
			cvReleaseImage(&birds_image);
		}
		if(undistortionImage==NULL){
			cvReleaseImage(&undistortionImage);
		}

		//g_temp_w = m_inputImage.width;
		//g_temp_h = m_inputImage.height;
		InParameter = 0;
		InParameter = 0;

		key = 0;
	}
	
	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageCalibration::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageCalibration::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ImageCalibrationInit(RTC::Manager* manager)
  {
    coil::Properties profile(imagecalibration_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageCalibration>,
                             RTC::Delete<ImageCalibration>);
  }
  
};


