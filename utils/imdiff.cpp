/* imdiff.cpp - visual alignment of two images
 *
 * imdiff.cpp is a useful program that can be used for visual alignment of two images.
 * It is useful especially when comparing rectified stereo images. 
 *
 */

#include <stdio.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat im0, im1;
Mat im1t; // transformed image 1
Mat imd; // "difference" image
const char *win = "imdiff";
float dx = 26;
int dxval = dx+20;
float dy = 0;
float startx = 9999;
float starty = 9999;

void imdiff()
{
    float s = 1;
    Mat T1 = (Mat_<float>(2,3) << s, 0, dx, 0, s, dy); 
    warpAffine(im1, im1t, T1, im1.size());
    addWeighted(im0, 1, im1t, -1, 128, imd);
    char txt[100];
    sprintf(txt, "dx=%g  dy=%g", dx, dy);
    putText(imd, txt, Point(5, 10), FONT_HERSHEY_PLAIN, 0.8, Scalar(255, 255, 255));
    imshow(win, imd);
}

void changedx(int, void *)
{
    dx = dxval-20;
    imdiff();
}

static void onMouse( int event, int x, int y, int, void* )
{
    //printf("x=%d y=%d    ", x, y);
    //printf("dx=%g dy=%g\n", dx, dy);
    if (event == CV_EVENT_LBUTTONDOWN) {
	startx = x-dx;
	starty = y-dy;
    } else if (event == CV_EVENT_LBUTTONUP) {
	startx = 9999;
	starty = 9999;
	printf("calling imdiff: dx=%g dy=%g\n", dx, dy);
	waitKey(1);
	imdiff();
    } else if (event == CV_EVENT_MOUSEMOVE && startx < 9999) {
	dx = x - startx;
	dy = y - starty;
	imdiff();
    }
}

void help() {
  printf("j l - left right\n");
  printf(", . - left right small step\n");
  printf("i k - up down small step\n");
  printf("a s - original left right images");
  printf("space - reset dx, dy\n");
  printf("ESC, q - quit\n");
}

int main(int argc, char ** argv)
{
    if (argc < 3) {
	fprintf(stderr, "usage: %s im1 im2\n", argv[0]);
	exit(1);
    }

    im0 = imread(argv[1], 1);
    if (!im0.data) { 
	fprintf(stderr, "cannot read image %s\n", argv[1]); 
	exit(1); 
    }
    im1 = imread(argv[2], 1);
    if (!im1.data) { 
	fprintf(stderr, "cannot read image %s\n", argv[2]); 
	exit(1); 
    }

    // convert to gray
    //cvtColor(im0, im0bw, CV_RGB2GRAY );  
    //cvtColor(im1, im1bw, CV_RGB2GRAY );  

    namedWindow(win, CV_WINDOW_AUTOSIZE);
    createTrackbar("dx", win, &dxval, 100, changedx);
    setMouseCallback(win, onMouse);
    imdiff();

    float step = 0.25;
    while(1) {
	int c = waitKey(0);
	switch(c) {
	case 27:// ESC
	case 'q':
	    return 0;
	case 2424832: case 65361: case 'j': // left arrow, j
	    dx -= 1; imdiff(); break;
	case 2555904: case 65363: case 'l': // right arrow, l
	    dx += 1; imdiff(); break;
	case ',': // small step to the left
	    dx -= step; imdiff(); break;
	case '.': // small step to the right
	    dx += step; imdiff(); break;
	case 2490368: case 65362: case 'i': // up arrow, i
	    dy -= step; imdiff(); break; 
	case 2621440: case 65364: case 'k': // down arrow
	    dy += step; imdiff(); break;
	case ' ': // reset
	    dx = 0; dy = 0; imdiff(); break;
	case 'a': // show original left image
	    imshow(win, im0); break;
	case 's': // show original right image
	    imshow(win, im1t); break;
	case 'd': // difference
	    imdiff(); break;
	case 'h': case '?':
	  help(); break;
	default:
	    printf("key %d (%c %d) pressed\n", c, (char)c, (char)c);
	}
    }
    return 0;
}
