//Bilinear Interpolation,  Junkai Qiu
//code enviornment :  Visual Studio 2012, opencv 2.4.6

#include <highgui.h>
#include <math.h>
#include<Windows.h>

static float ScaleX = 1.0;    //scale factor in horizontal direction
static float ScaleY = 1.0;    //scale factor in vertical direction
static bool flag = true;      //depend whether amplify or shrink
IplImage * srcImage;          //source image -- lena.jpg
IplImage * rstImage;          //output image

void Bilinear(IplImage *  src, IplImage *  rst);
void WINAPI OnTimer(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);

void Bilinear(IplImage *  src, IplImage* rst){
	IplImage * rstImage2=NULL;  //state a second pointer for memory recycle
	if(rst!= NULL)
		rstImage2=rst;      //rst is always the pointer I operate

	rst = cvCreateImage(cvSize(srcImage->width*ScaleX, srcImage->height*ScaleY),srcImage->depth,srcImage->nChannels); //create a new image
	//Acquire widths and heights of images
	int srcWidth = src->width;
	int srcHeight = src->height;
	int rstWidth = rst->width;
	int rstHeight = rst->height;

	//Ratioes of width and height
	const float tx = (srcWidth-1.0f)/(rstWidth-1.0f);
	const float ty = (srcHeight-1.0f)/(rstHeight-1.0f);

	CvPoint2D32f upos; //Position(float) of source image pixel
	CvPoint3D32f f1;
	CvPoint3D32f f2;

	for (int j=0; j < rstHeight-1; j++){
		for (int i=0; i < rstWidth-1; i++){
			upos.x = i*tx;
			upos.y = j*ty;
			int icol = (int)upos.x;
			int irow = (int)upos.y;

			CvScalar p1, p2, p3, p4, s;
			p1 = cvGet2D(src, irow, icol);  
			p2 = cvGet2D(src, irow, icol+1);
			p3 = cvGet2D(src, irow+1, icol);
			p4 = cvGet2D(src, irow+1, icol+1);
			//horizontal direction
			f1.x = (float) p1.val[0]*(1-abs(upos.x-icol)) + (float) p2.val[0]*(upos.x-icol);
			f1.y = (float) p1.val[1]*(1-abs(upos.x-icol)) + (float) p2.val[1]*(upos.x-icol);
			f1.z = (float) p1.val[2]*(1-abs(upos.x-icol)) + (float) p2.val[2]*(upos.x-icol);

			f2.x = (float) p3.val[0]*(1-abs(upos.x-icol)) + (float) p4.val[0]*(upos.x-icol);
			f2.y = (float) p3.val[1]*(1-abs(upos.x-icol)) + (float) p4.val[1]*(upos.x-icol);
			f2.z = (float) p3.val[2]*(1-abs(upos.x-icol)) + (float) p4.val[2]*(upos.x-icol);
			//vertical direction
			s.val[0] = f1.x*(1-abs(upos.y-irow)) + f2.x*(abs(upos.y-irow));
			s.val[1] = f1.y*(1-abs(upos.y-irow)) + f2.y*(abs(upos.y-irow));
			s.val[2] = f1.z*(1-abs(upos.y-irow)) + f2.z*(abs(upos.y-irow));
			cvSet2D(rst, j, i, s);
		}
		//Don't forget the last column
		cvSet2D(rst, j, rstWidth-1, cvGet2D(rst, j, rstWidth-2));
	}
	//Add the last row
	for(int i = 0; i<rstWidth; i++){
		cvSet2D(rst, rstHeight-1, i, cvGet2D(rst, rstHeight-2, i));
	}
	cvShowImage("zoom",rst);  //show the new image
	if(rstImage2!=NULL)
		delete rstImage2;   //retrieve memory
}

void WINAPI OnTimer(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{
	//The zoom range is from 1.0 to 1.5
	if (ScaleX >= 1.5 || ScaleY >= 1.5)   
		flag = false;
	else if(ScaleX <= 1 || ScaleY<=1)
		flag = true;

	if(true == flag)
		ScaleX= ScaleY +=0.1;
	else
		ScaleX = ScaleY -= 0.1;	
	Bilinear(srcImage,rstImage);
}

int main() 
{
	srcImage = cvLoadImage("lena.jpg");   //get the original image data
	if(NULL == srcImage)
	{
		printf("the source image doesn't exit\n");
		system("pause");
		return 0;
	}
	SetTimer(NULL,0,30,OnTimer);    //register timer function
	cvShowImage("Original Image", srcImage);  //original image
	cvWaitKey(0);
	return 1;
}