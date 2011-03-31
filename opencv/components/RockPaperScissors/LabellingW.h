#ifndef LABELINGW_H
#define LABELINGW_H

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "Labeling.h"

#define Label LabelingBS
/*
Label *createLabeling(){
	return new LabelingBS();
}

int exec(Label *label,IplImage *target,IplImage *result,
		 const bool is_sort_region,int region_size_min){
	return label->Exec((unsigned char *)target->imageData,(short *)result->imageData,
		target->width,target->height,is_sort_region,region_size_min);
}

int getNumOfResultRegions(Label *label){
	return label->GetNumOfResultRegions();
}

void releaseLabeling(Label *label){
	delete label;
}
*/
#endif LABELINGW_H