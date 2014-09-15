// ustrike.cpp : Defines the entry point for the console application.
//

// I-STRIKE I.P. CODE
// Abhijit Lal
// Nikhil Kaswan
// Udbhav Prasad


#include "stdafx.h"
#include <sys/types.h>
//#include <dirent.h>
#include <errno.h>
#include "cv.h"
#include "highgui.h"
#include "stdio.h"
#include "math.h"
#include "conio.h"
#include <time.h>

#define DUR 90
#define EDGES 17

//GLOBALS ------------
CvPoint vertices[12];
int valid[17];
int vindex[17][2] = {	{0,1}, {1,2}, {3,4}, {4,5}, {6,7}, {7,8}, {9,10}, {10,11},
{0,3}, {1,4}, {2,5}, {3,6}, {4,7}, {5,8}, {6,9}, {7,10}, {8,11}	};

CvSeq* contour;
CvSeq* contourLow;
CvPoint p1, p2, cent;
IplImage* img ;//= cvLoadImage("istrikex.jpg", 1);
CvCapture* capture = cvCreateCameraCapture(CV_CAP_DSHOW);
IplImage* thresh;
int path[12];
int npath = 0;
int orient[] = {0,0,0,0,0,0,0,0,0,0,0};
//END OF DECLARATIONS --

//FUNCTIONS
void setups() {

//	cvCircle(img, p1, 10, CV_RGB(255,0,0));
//	cvCircle(img, p2, 10, CV_RGB(255,0,0));
	cvCircle(img, cent, 10, CV_RGB(255,0,0));

	int wd = img->width;
	int ht = img->height;

	printf("HELLLOOOOOOOO   %d, %d\n", wd, ht);

	double dist[7];
	double min = ht;

	CvPoint pl1, pl2;
	pl1.x = (12.5/200)*wd;
	pl2.x = pl1.x;
	pl1.y = 0;
	pl2.y = ht;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[0] = abs(cent.x-pl1.x);

	pl1.x = (97.5/200)*wd;
	pl2.x = pl1.x;
	pl1.y = 0;
	pl2.y = ht;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[1] = abs(cent.x-pl1.x);

	pl1.x = (187.5/200)*wd;
	pl2.x = pl1.x;
	pl1.y = 0;
	pl2.y = ht;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[2] = abs(cent.x-pl1.x);

	pl1.y = (12.5/300)*ht;
	pl2.y = pl1.y;
	pl1.x = 0;
	pl2.x = wd;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[3] = abs(cent.x-pl1.x);

	pl1.y = (102.5/300)*ht;
	pl2.y = pl1.y;
	pl1.x = 0;
	pl2.x = wd;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[4] = abs(cent.y-pl1.y);

	pl1.y = (197.5/300)*ht;
	pl2.y = pl1.y;
	pl1.x = 0;
	pl2.x = wd;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[5] = abs(cent.y-pl1.y);

	pl1.y = (287.5/300)*ht;
	pl2.y = pl1.y;
	pl1.x = 0;
	pl2.x = wd;
	cvLine(img, pl1, pl2, CV_RGB(255,0,0));
	dist[6] = abs(cent.x-pl1.x);

	int line_num;
	for (int i=0; i<7; ++i) {
		if (min > dist[i]) {
			min = dist[i];
			line_num = i+1;
		}
	}

	vertices[0].x = (12.5/200)*wd;
	vertices[0].y = (12.5/300)*ht;

	vertices[1].x = (97.5/200)*wd;
	vertices[1].y = (12.5/300)*ht;

	vertices[2].x = (187.5/200)*wd;
	vertices[2].y = (12.5/300)*ht;

	vertices[3].x = (12.5/200)*wd;
	vertices[3].y = (102.5/300)*ht;

	vertices[4].x = (97.5/200)*wd;
	vertices[4].y = (102.5/300)*ht;

	vertices[5].x = (187.5/200)*wd;
	vertices[5].y = (102.5/300)*ht;

	vertices[6].x = (12.5/200)*wd;
	vertices[6].y = (197.5/300)*ht;

	vertices[7].x = (97.5/200)*wd;
	vertices[7].y = (197.5/300)*ht;

	vertices[8].x = (187.5/200)*wd;
	vertices[8].y = (197.5/300)*ht;

	vertices[9].x = (12.5/200)*wd;
	vertices[9].y = (287.5/300)*ht;

	vertices[10].x = (97.5/200)*wd;
	vertices[10].y = (287.5/300)*ht;

	vertices[11].x = (187.5/200)*wd;
	vertices[11].y = (287.5/300)*ht;

	for(int j=0; j<12; ++j) {
		cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
	}

	printf("%lf, %d\n", min, line_num);
}

IplImage* Rotate(IplImage* image) //---------------------------------------------------CHANGE '0' to '1' to rotate clockwise.
{
	IplImage* rotated;
	rotated = cvCreateImage(cvSize(image->height, image->width), image->depth, image->nChannels);
	
	cvTranspose(image, rotated);
	cvFlip(rotated, rotated, 0);

	return rotated;
}
IplImage* Crop_Round2(IplImage* img)
{
	IplImage* cropped;
	CvRect rect= {64, 106, 521, 348}; 
	cvSetImageROI(img, rect);
	cropped = cvCreateImage(cvSize(521, 348), img->depth, img->nChannels);
	cvCopy(img, cropped);
	cvResetImageROI(img);
	
	return cropped;
}
int checkForSignals(int v) {

	IplImage* thr = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	
	CvScalar low =CV_RGB(180, 80, 80);
	CvScalar high = CV_RGB(240, 120, 120);
	//cvCvtColor(img, imgHSV, CV_BGR2HSV);
	cvCopy(img, imgHSV);
	cvInRangeS(imgHSV, low, high, thr);
	cvSmooth(thr, thr, CV_GAUSSIAN, 3, 0, 0, 0);

	cvErode(thr, thr, 0, 1);
	cvDilate(thr, thr, 0, 1);

	int ht = img->height;
	int wd = img->width;

	int signals[12] = {0,0,0,0,0,0,0,0,0,0,0};

	printf("In HERE\n");

	double xoff = 60.0;
	double yoff = 75.0;

	for (int j=0; j<6; ++j) {
		int flag = 0;
		int lr = ((vertices[path[j]].x-(xoff*wd/200.0))>0)?(xoff*wd/200.0):(10.0*wd/200.0);
		lr = vertices[path[j]].x - lr;
		int ur = ((vertices[path[j]].x+(xoff*wd/200.0))<wd)?(xoff*wd/200.0):(10.0*wd/200.0);
		ur = vertices[path[j]].x + ur;
		printf("%d, %d\n", lr, ur);

		int lc = (vertices[path[j]].y-(xoff*ht/300.0))>0?(xoff*ht/300.0):(10.0*ht/300.0);
		lc = vertices[path[j]].y - lc;
		int uc = (vertices[path[j]].y+(xoff*ht/300.0))<ht?(xoff*ht/300.0):(10.0*ht/300.0);
		uc = vertices[path[j]].y + uc;
		printf("%d, %d\n\n", lc, uc);

		cvCircle(img, cvPoint(lr, lc), 20, CV_RGB(0,0,255));
		cvCircle(img, cvPoint(ur, uc), 20, CV_RGB(0,255,0));

		for (int k=lr; k<ur; k++) {

			for (int l=lc; l<uc; l++) {

				if (cvGetReal2D(thr, l, k) != 0) {
					flag = 1;
					signals[path[j]] = 1;
					break;
				}
			}
			if (flag == 1) {
				break;
			}
		}
	}

	printf("OUtta HERE\n");

	if (signals[v] == 1)
		return 1;
	else return 0;
}

float EuclideanDistance(CvPoint pt_A, CvPoint pt_B)
{
	float dist;
	float X, Y, Z;
	X = ((pt_A.x - pt_B.x)*(pt_A.x - pt_B.x));
	Y = ((pt_A.y - pt_B.y)*(pt_A.y - pt_B.y));
	Z = X+Y;
	dist = sqrt(Z);
	return dist;
}
void contours(IplImage* img) {
	int k;
	contour=0;
	contourLow=0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	
	IplImage* imgThreshed = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* eroded = cvCreateImage(cvSize(img->width, img->height), 8, 1); 
	IplImage* dilated = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* imgHSV = cvCreateImage(cvSize(img->width, img->height), 8, 3);
    
	cvCvtColor(img, imgHSV, CV_BGR2HSV);
	 
	cvInRangeS(imgHSV, cvScalar(90, 120, 100), cvScalar(120, 255, 255), imgThreshed);
	
	cvReleaseImage(&imgHSV);
    cvSmooth(imgThreshed, imgThreshed, CV_GAUSSIAN, 3, 0, 0, 0);
	CvScalar avg;
	CvScalar avgStd;
	cvAvgSdv(img, &avg, &avgStd, NULL);
	/*cvSmooth(img, img, CV_GAUSSIAN, 3, 0, 0, 0);
	
	cvDilate(img, img, 0, 1);
	cvErode(img, img, 0, 1);*/
	cvThreshold(imgThreshed, imgThreshed, (int)avg.val[0]-7*(int)(avgStd.val[0]/8), 255, CV_THRESH_BINARY);
	
	cvDilate(imgThreshed, dilated, NULL, 1);
	cvErode(dilated, eroded, NULL, 1);

	k=cvFindContours(eroded, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	if(k==0)
	{
		printf("No contours found for blue color");
		return;
	}
	contourLow = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 1, 1);

	CvPoint prev_pt1, prev_pt2;
	prev_pt1.x= 0; prev_pt1.y = 0;
	prev_pt2.x =0; prev_pt2.y = 0;
	CvPoint new_pt1, new_pt2;
	CvScalar color = CV_RGB( 255, 0, 0);

	for( ; contourLow != 0; contourLow = contourLow->h_next )
	{
		
		//We can draw the contour of object
		//cvDrawContours( imagen_color, contourLow, color, color, -1, 0, 8, cvPoint(0,0) );
		//Or detect bounding rect of contour	
		CvRect rect;
		float dist_prev, dist_new;
		
		rect=cvBoundingRect(contourLow, NULL);
		//printf("(%d, %d)", prev_pt1.x, prev_pt1.y);
		
		new_pt1.x = rect.x;
        new_pt2.x = (rect.x+rect.width);
        new_pt1.y = rect.y;
        new_pt2.y = (rect.y+rect.height);
		
		dist_prev = EuclideanDistance(prev_pt1, prev_pt2);
		dist_new = EuclideanDistance(new_pt1, new_pt2);
		
		if(dist_new > dist_prev)
		{
			prev_pt1.x = new_pt1.x;
			prev_pt1.y = new_pt1.y;
			prev_pt2.x = new_pt2.x;
			prev_pt2.y = new_pt2.y;
			continue;
		}
	}
	if (EuclideanDistance(prev_pt1, prev_pt2) > EuclideanDistance(new_pt1, new_pt2)) {
		p1 = prev_pt1;
		p2 = prev_pt2;
		cvRectangle(img, prev_pt1, prev_pt2, color, 1, 8, 0);
	}
	else {
		p1 = new_pt1;
		p2 = new_pt2;
		cvRectangle(img, new_pt1, new_pt2, color, 1, 8, 0);
	}
	double dx = abs(p1.y-p2.y);
	double dy = abs(p1.x-p2.x);

	cent.x = (p1.x+p2.x)/2;
	cent.y = (p1.y+p2.y)/2;
}

int pathfinder() {

	printf("START OF PATHFINDER\n");

	int cind = 9;
	CvPoint cvert = vertices[9];
	npath = 0;
	path[npath] = cind;

	int dvalid[17];
	for (int copy=0; copy<17; copy++) {
		dvalid[copy] = valid[copy];
	}

	while (!((cvert.x == vertices[2].x)&&(cvert.y == vertices[2].y))) {
//		printf("%d\n", cind);
		int nind = -1;
		int poss=0;
		int dind;
		int di = 100000;
		for (int i=0; i<17; ++i) {
			if ((vindex[i][0] == cind) && (dvalid[i] ==  0)) {
				nind = vindex[i][1];
				if (EuclideanDistance(vertices[nind], vertices[2]) < di) {
					di = EuclideanDistance(vertices[nind], vertices[2]);
					dind = nind;
				}
			}
			else if ((vindex[i][1] == cind) && (dvalid[i] == 0)) {
				nind = vindex[i][0];
				if (EuclideanDistance(vertices[nind], vertices[2]) < di) {
					di = EuclideanDistance(vertices[nind], vertices[2]);
					dind = nind;
				}
			}
		}

		nind = dind;

		if (nind == -1) {
			if (path[npath] < path[npath-1]) {
				for (int z=0; z<17; ++z) {
					if ((vindex[z][0] == path[npath]) && (vindex[z][1] == path[npath-1])) {
						valid[z] = 1;
						break;
					}
				}
			}
			else {
				for (int z=0; z<17; ++z) {
					if ((vindex[z][0] == path[npath-1]) && (vindex[z][1] == path[npath])) {
						valid[z] = 1;
						break;
					}
				}
			}
			return -1;
		}

//		printf("%d",nind);
		for (int j=0; j<17; ++j) {
			if (vindex[j][0] == cind) {
				dvalid[j] = 1;
			}
			else if (vindex[j][1] == cind){
				dvalid[j] = 1;
			}
		}
		npath++;
		path[npath] = nind;
		cind = nind;
		cvert = vertices[cind];
	//	for (int m=0; m<17; ++m) {
	//		printf("%d, %d,%d\n",valid[m], vindex[m][0], vindex[m][1]);
	//	}
	}

	for (int c=0; c<=npath; ++c) {
		printf("%d\n", path[c]);
	}	

	for (int a = 0; a < npath; ++a) {
		cvLine(img, vertices[path[a]], vertices[path[a+1]], CV_RGB(255,255,0), 3);
	}

	printf("END OF PATHFINDER\n");
	return 0;
}

void imageproc() {

	printf("START OF IMPROC\n");
	IplImage* dummy_img_hsv=cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	cvCvtColor(img, dummy_img_hsv, CV_BGR2HSV);
	thresh=cvCreateImage(cvGetSize(img),8, 1);

	//cvAdaptiveThreshold(img1, thresh, 255, 0, 0, 71, 15);
	cvInRangeS(dummy_img_hsv, cvScalar(40, 130, 100), cvScalar(70, 255, 255), thresh);
	cvDilate(thresh,thresh,NULL,5);

	for (int m=0; m<8; ++m) {
		int k1 = vindex[m][0];
		int k2 = vindex[m][1];
//		printf("%d,%d\n", k1, k2);
//		printf("%d,%d\n", vertices[k1].y, vertices[k2].y);
		for (int n=vertices[k1].x; n<vertices[k2].x; ++n) {
			if (cvGetReal2D(thresh, vertices[k1].y, n)!=0) {
				valid[m] = 1;
				break;
			}
			else {
				valid[m] = 0;
			}
		}
//		printf("%d\n",valid[m]);
	}

	for (int m=8; m<17; ++m) {
		int k1 = vindex[m][0];
		int k2 = vindex[m][1];
		for (int n=vertices[k1].y; n<vertices[k2].y; ++n) {
			if (cvGetReal2D(thresh, n, vertices[k1].x)!=0) {
				valid[m] = 1;
				break;
			}
			else {
				valid[m] = 0;
			}
		}
//		printf("%d\n",valid[m]);
	}
/*
	for (int m=vertices[0].y; m<vertices[3].y; m++) {
		if (cvGetReal2D(thresh, m, vertices[0].x)!=0)
			printf("YES!\n");
		//printf("%lf",cvGetReal2D(thresh, vertices[5].x, m));
	}
*/
/*
	for (int m=0; m<17; ++m) {
		printf("%d, %d,%d\n",valid[m], vindex[m][0], vindex[m][1]);
	}
*/
	int tmp = -1;
	while (tmp == -1) {
		tmp = pathfinder();
	}

	printf("END OF IMPROC\n");
}



void writechar(char c, int ori) {

	printf("WC\n");

	static int i=0;
//	printf("%d\n",i);
	//printf("%d\n", path[i+1]);

	FILE* port = fopen( "COM3:", "wb" );
	fprintf(port, "%c", c);
	fclose(port);

//	printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
	
	if (c=='d') {
		printf("ONE\n");
		for (int j=0; j<20; ++j) {
//			printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
				Sleep(DUR);
				IplImage* frame=cvRetrieveFrame(capture);
				IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
				cvCopy(frame, dummy);
				img = Rotate(Crop_Round2(dummy));
				cvReleaseImage(&dummy);

				for(int j=0; j<12; ++j) {
					cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
				}

				contours(img);
				cvNamedWindow("IMG");
				cvShowImage("IMG", img);
				cvWaitKey(10);
		}
	}
	else if (c=='a') {
		printf("TWO\n");
		for (int j=0; j<20; ++j) {
//			printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
				Sleep(DUR);
				IplImage* frame=cvRetrieveFrame(capture);
				IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
				cvCopy(frame, dummy);
				img = Rotate(Crop_Round2(dummy));
				cvReleaseImage(&dummy);

				for(int j=0; j<12; ++j) {
					cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
				}

				contours(img);
				cvNamedWindow("IMG");
				cvShowImage("IMG", img);
				cvWaitKey(10);
		}
	}
	else if ((ori == 1)||(ori==-1)) {
		printf("THREE\n");
		while (abs(cent.y-vertices[path[i+1]].y) > 20) {
//			printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
			if (cvGrabFrame(capture))
			{
				IplImage* frame=cvRetrieveFrame(capture);
				IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
				cvCopy(frame, dummy);
				img = Rotate(Crop_Round2(dummy));
				cvReleaseImage(&dummy);

				for(int j=0; j<12; ++j) {
					cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
				}

				contours(img);
				cvNamedWindow("IMG");
				cvShowImage("IMG", img);
				cvWaitKey(10);
				//printf("%d\n", checkForSignals(path[i+1]));
				//printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);

				while(checkForSignals(path[i+1]) == 1) {
					IplImage* frame=cvRetrieveFrame(capture);
					IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
					cvCopy(frame, dummy);
					img = Rotate(Crop_Round2(dummy));
					cvReleaseImage(&dummy);

					for(int j=0; j<12; ++j) {
						cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
					}

					double val;
					int ht = img->height;
					int wd = img->width;
					val = (double)EuclideanDistance(vertices[path[i]], vertices[path[i+1]]);
					//printf("%d, %d\n", path[i], path[i+1]);
					contours(img);

					//printf("FAR1\n");
					//printf("%lf\n", EuclideanDistance(cent, vertices[path[i+1]]));
					//printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
					printf("%d, %d", (vertices[path[i]].x+vertices[path[i+1]].x)/2, (vertices[path[i]].y+vertices[path[i+1]].y)/2);
					cvCircle(img, cvPoint((vertices[path[i]].x+vertices[path[i+1]].x)/2,(vertices[path[i]].y+vertices[path[i+1]].y)/2), 10, CV_RGB(255,255,0));
					printf("%d, %d", cent.x, cent.y);
					if (EuclideanDistance(cent, vertices[path[i+1]]) < val/2) {

						char q='q';
						FILE* fp = fopen( "COM3:", "wb" );
						fprintf(fp, "%c", q);
						fclose(fp);
					}
				}
				if (checkForSignals(path[i+1]) == 0) {
					int w='w';
					FILE* fp = fopen( "COM3:", "wb" );
					fprintf(fp, "%c", w);
					fclose(fp);
				}

				cvNamedWindow("IMG");
				cvShowImage("IMG", img);
				cvWaitKey(10);
			}
		}
		i++;
		printf("END WC\n");
	}
	else if ((ori == 2)||(ori==-2)) {
		printf("FOUR\n");
		while (abs(cent.x-vertices[path[i+1]].x) > 20) {
//			printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
			if (cvGrabFrame(capture))
			{
				IplImage* frame=cvRetrieveFrame(capture);
				IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
				cvCopy(frame, dummy);
				img = Rotate(Crop_Round2(dummy));
				cvReleaseImage(&dummy);
				for(int j=0; j<12; ++j) {
						cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
					}

				contours(img);
				//printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
				//printf("%d\n", checkForSignals(path[i+1]));
				while(checkForSignals(path[i+1]) == 1) {
					IplImage* frame=cvRetrieveFrame(capture);
					IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
					cvCopy(frame, dummy);
					img = Rotate(Crop_Round2(dummy));
					cvReleaseImage(&dummy);

					for(int j=0; j<12; ++j) {
						cvCircle(img, vertices[j], 10, CV_RGB(255,0,0));
					}

					double val;
					int ht = img->height;
					int wd = img->width;
					val = (double)EuclideanDistance(vertices[path[i]], vertices[path[i+1]]);
					cvCircle(img, cvPoint((vertices[path[i]].x+vertices[path[i+1]].x)/2,(vertices[path[i]].y+vertices[path[i+1]].y)/2), 10, CV_RGB(255,255,0));
					//printf("%d, %d\n", path[i], path[i+1]);
					contours(img);
					cvNamedWindow("IMG");
					cvShowImage("IMG", img);
					cvWaitKey(10);

					//printf("%lf\n", EuclideanDistance(cent, vertices[path[i+1]]));
					//printf("FAR2\n");
					//printf("%d, %d, %d, %d\n", cent.x, cent.y, vertices[path[i+1]].x, vertices[path[i+1]].y);
					printf("%d, %d\n", (vertices[path[i]].x+vertices[path[i+1]].x)/2, (vertices[path[i]].y+vertices[path[i+1]].y)/2);
					printf("%d, %d\n", cent.x, cent.y);
					if (EuclideanDistance(cent, vertices[path[i+1]]) < val/2) {

						char q='q';
						FILE* fp = fopen( "COM3:", "wb" );
						fprintf(fp, "%c", q);
						fclose(fp);
					}
				}
				if (checkForSignals(path[i+1]) == 0) {
					int w='w';
					FILE* fp = fopen( "COM3:", "wb" );
					fprintf(fp, "%c", w);
					fclose(fp);
				}

				cvNamedWindow("IMG");
				cvShowImage("IMG", img);
				cvWaitKey(10);
			}
		}
		i++;
	}
}


void orientation() {
	char w='w', a='a', s='s', d='d', q='q';
	// +1 = up ; -1 = down ; +2 = right ; -2 = left
	int hor[8][2] = {	{0,1}, {1,2}, {3,4}, {4,5}, {6,7}, {7,8}, {9,10}, {10,11}	};
	int ver[9][2] = {	{0,3}, {1,4}, {2,5}, {3,6}, {4,7}, {5,8}, {6,9}, {7,10}, {8,11}	};

	printf("\n");
	orient[0] = +1;
	
	for (int cnt=0; cnt<npath; ++cnt) {
		for (int vc=0; vc<9; vc++) {
			int one = (path[cnt] < path[cnt+1] ? path[cnt]:path[cnt+1]);
			int two = (path[cnt] < path[cnt+1] ? path[cnt+1]:path[cnt]);
			if ((ver[vc][0] == one) && (ver[vc][1] == two)) {
				if(path[cnt]<path[cnt+1])
					orient[cnt] = -1;
				else
					orient[cnt] = +1;
			}
		}
		for (int hc=0; hc<8; hc++) {
			int one = (path[cnt] < path[cnt+1] ? path[cnt]:path[cnt+1]);
			int two = (path[cnt] < path[cnt+1] ? path[cnt+1]:path[cnt]);
			if ((hor[hc][0] == one) && (hor[hc][1] == two)) {
				if(path[cnt]<path[cnt+1])
					orient[cnt] = +2;
				else
					orient[cnt] = -2;
			}
		}
	}

	for (int h=0; h<npath; h++) {
		if (h==0) {
			switch(orient[h]) {
				case 2: //printf("Take a Right\n");
						writechar(d,orient[h]);
				case 1:	//printf("Go Straight\n");
						writechar(w,orient[h]);
						break;
			}
		} else {
			switch(orient[h]) {
				case 1: switch(orient[h-1]) {
							case 2: //printf("Take a Left\n");
									writechar(a,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case -2: //printf("Take a Right\n");
									writechar(d,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case 1: //printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
						}
						break;
				case 2: switch(orient[h-1]) {
							case -1: //printf("Take a Left\n");
									writechar(a,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case 1: //printf("Take a Right\n");
									writechar(d,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case 2: //printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
						}
						break;
				case -1: switch(orient[h-1]) {
							case -2: //printf("Take a Left\n");
									writechar(a,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case 2: //printf("Take a Right\n");
									writechar(d,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case -1: //printf("Go Straight\n");
									break;
						 }
						 break;
				 case -2: switch(orient[h-1]) {
							case 1: //printf("Take a Left\n");
									writechar(a,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case -1: //printf("Take a Right\n");
									writechar(d,orient[h]);
									//printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
							case -2: //printf("Go Straight\n");
									writechar(w,orient[h]);
									break;
						  }
						  break;
			}
		}
	}
	writechar(q,-1);
}
void disp_clean() {	
	cvNamedWindow("Road", 0);
	cvShowImage("Road", img);
	cvNamedWindow("LOL", 0);
	cvShowImage("LOL", thresh);
	//cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&img);
	cvReleaseImage(&thresh);
}




//END OF FUNCTIONS

using namespace std;

int main()
{
	system("MODE COM3: BAUD=4800 PARITY=n DATA=8 STOP=1");
	
	int i=0;
	while(i<2) {
		if (cvGrabFrame(capture)) {
			IplImage* frame = cvRetrieveFrame(capture);
			IplImage* dummy = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
			cvCopy(frame, dummy);
			img = Rotate(Crop_Round2(dummy));
			cvReleaseImage(&dummy);

			//cvNamedWindow("show", 1);
			//cvShowImage("show", img);
			//cvWaitKey();
			contours(img);
			setups();
			imageproc();
			//orientation();
			disp_clean();
			++i;
		}
	}

	printf("B4 ORI\n");
	orientation();
	printf("AFTER ORI\n");

	getch();
	return 0;
}