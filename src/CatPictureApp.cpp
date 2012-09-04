/**
*@file CatPictureApp.cpp
* Should display a picture
* 
* @author David Gayda
* @date 8/29/2012
* 
* @note This file is (c) 2012. It is licensed under the 
* CC BY 3.0 license (http://creativecomons.org/licenses/b/3.0/).
* which means you are free to use, share, and remix it as long as you
* give attribution. Commercial uses are allowed.
*/


#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);
	

private:
	Surface* mySurface_;
	gl::Texture* myTexture;
	//Surface* mySurfaceCopy;  //The Surface we work for blur
	int frame_number_;
	float r;
	float g;
	float b;
	void rectangle(uint8_t* dataArray, int row, int col, int width, int height);
	void rec(uint8_t* dataArray, int midx, int midy, int col, int row, Color8u color);
	void circle(uint8_t* dataArray, float midx, float midy, float radius, Color8u color);
	void CatPictureApp::distance(int midx, int midy, float radius);
	void CatPictureApp::line(uint8_t* dataArray, float row1, float col1, float row2, float col2, Color8u color);
	void CatPictureApp::tint(uint8_t* dataArray,int row, int col);
	void CatPictureApp::copyRecAndMove(uint8_t* dataArray, int row, int col, int row2, int col2);
	void CatPictureApp::blur(uint8_t* dataArray);
};


static const int AppWidth = 512;
static const int AppHeight = 512;
static const int TextureSize = 512;

void CatPictureApp::prepareSettings(Settings* settings) {
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}

//Draws rectangle from top left ((x,y) or (col,row)) coordinate with a specified width and height

//HOLLOW (To change use (x/y) >= (col+width/row+height)
void CatPictureApp::rectangle(uint8_t* dataArray, int row, int col, int width, int height) {
	Color8u color = Color8u(255,0,255);
	for (int x=col; x<=col+width; x++) { //col+width
		for (int y=row; y<=row+height; y++) { // row+height
			if (x == col+width|| x == col || y == row ||y == row+height) {
					dataArray[(3*(x+(y*512)))] = color.r;
					dataArray[(3*(x+(y*512)))+1] = color.g;
					dataArray[(3*(x+(y*512)))+2] = color.b;
			}
		}
	}
}

//Draws line from top left to bottom right... Change! Can't draw positive slope line
//
//FIX IT CAN'T HAVE POINT ONE START AT 0!!!!!!
void CatPictureApp::line(uint8_t* dataArray, float row1, float col1, float row2, float col2, Color8u color) {
	float rise = row2-row1;
	float run = col2-col1;
	//if ((row1>row2 && col1>col2 && run !=0) == false) {
	float slope = (rise/run);
	if (row2>row1 || col2>col1) {
		for (float x=col1; x<col2; x++) {
			for (float y=row1; y<row2; y++) {
				if (((col2-x)/(row2-y)) == slope) {
					dataArray[(int)(3*(x+(y*512)))] = color.r;
					dataArray[(int)(3*(x+(y*512)))+1] = color.g;
					dataArray[(int)(3*(x+(y*512)))+2] = color.b;
				}
				/*if(col1<col2 || row1<row2){
				int x=1;

int y=1;

while (x<=512 || y <=512) {

if(((y)/(x)) == slope) {

dataArray[(3*(x+(y*512)))] = color.r;

dataArray[(3*(x+(y*512)))+1] = color.g;

dataArray[(3*(x+(y*512)))+2] = color.b;

}

x++;

y++;

}

}

*/
			}
		}

//}

	}
}

//Draws Rectangle Based on x,y mid point and col(width) row(length)
	void CatPictureApp::rec(uint8_t* dataArray,int midx, int midy, int col, int row, Color8u color) {
	
		for(int y=midy-row; y<=midy+row; y++){
			for(int x=midx-col;x<=midx+col; x++){
			
				if(y < 0 == false || x < 0 == false) {
					//If inside the bounds
					if((x >= midx-col)||(x <= midx+col)||(y >= midy-row)||(y <= midy+row)) {
						//Row Major Order Formula
						int i = 3*(x + y*512);

						//Change the pixels!
						dataArray[i] = color.r;
						dataArray[i+1] = color.g;
						dataArray[i+2] = color.b;
					}
				}
			}
		}
	}

	void CatPictureApp::distance(int midx, int midy, float radius) {



	}

	void CatPictureApp::tint(uint8_t* dataArray, int row, int col) {
		for (float x=row; x<row+50; x++) {
			for (float y=col; y<col+50; y++) {
			dataArray[(int)(3*(x+(y*512)))] = dataArray[(int)(3*(x+(y*512)))]+200;
			dataArray[(int)(3*(x+(y*512)))+1] = dataArray[(int)(3*(x+(y*512)))+1]+200;
			dataArray[(int)(3*(x+(y*512)))+2] = dataArray[(int)(3*(x+(y*512)))+2]+200;
			}
		}

	}


	void CatPictureApp::blur(uint8_t* dataArray) {
		int j=0;
		uint8_t* dataArrayCopy = new uint8_t [512*512*3];
		for(int col=0; col<=512; col++){
			for(int row=0;row<=512; row++){
					
						int i = 3*(col + row*512);
						int tl = 3*((col-1) + ((row-1)*512));
						int t = 3*((col) + ((row-1)*512));
						int tr = 3*((col+1) + ((row-1)*512));
						int l = 3*((col-1) + ((row)*512));
						int c = 3*(col + (row*512));
						int r = 3*((col+1) + ((row)*512));
						int bl = 3*((col-1) + ((row+1)*512));
						int b = 3*((col) + ((row+1 )*512));
						int br = 3*((col+1) + ((row+1)*512));
						if (row == 0) {
							dataArrayCopy[i] = (dataArray[c] +dataArray[l] + dataArray[r] + dataArray[bl] + dataArray[b] + dataArray[br])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]
						+dataArray[l+1] + dataArray[r+1] + dataArray[bl+1] + dataArray[b+1] + dataArray[br+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]
						+dataArray[l+2] + dataArray[r+2] + dataArray[bl+2] + dataArray[b+2] + dataArray[br+2])/6;
						}
						else if (col==0) {
							dataArrayCopy[i] = (dataArray[c] + dataArray[t] + dataArray[tr]
						 + dataArray[r] + dataArray[b] + dataArray[br])/6;

						dataArrayCopy[i+1] = (dataArray[c+1] + dataArray[t+1] + dataArray[tr+1]
						+ dataArray[r+1] + dataArray[b+1] + dataArray[br+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2] + dataArray[t+2] + dataArray[tr+2]
						+ dataArray[r+2] + dataArray[b+2] + dataArray[br+2])/6;
						}

						else if(col == 512*3) {
						dataArrayCopy[i] = (dataArray[c]+dataArray[tl] + dataArray[t]
						+dataArray[l] + dataArray[bl] + dataArray[b])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]+dataArray[tl+1] + dataArray[t+1]
						+dataArray[l+1] + dataArray[bl+1] + dataArray[b+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]+dataArray[tl+2] + dataArray[t+2]
						+dataArray[l+2] + dataArray[bl+2] + dataArray[b+2])/6;
						}
						else if (row == 512*3) {
							dataArrayCopy[i] = (dataArray[c]+dataArray[tl] + dataArray[t] + dataArray[tr]
						+dataArray[l] + dataArray[r])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]+dataArray[tl+1] + dataArray[t+1] + dataArray[tr+1]
						+dataArray[l+1] + dataArray[r+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]+dataArray[tl+2] + dataArray[t+2] + dataArray[tr+2]
						+dataArray[l+2] + dataArray[r+2])/6;

						}

						else {
						dataArrayCopy[i] = (dataArray[c]+dataArray[tl] + dataArray[t] + dataArray[tr]
						+dataArray[l] + dataArray[r] + dataArray[bl] + dataArray[b] + dataArray[br])/9;

						dataArrayCopy[i+1] = (dataArray[c+1]+dataArray[tl+1] + dataArray[t+1] + dataArray[tr+1]
						+dataArray[l+1] + dataArray[r+1] + dataArray[bl+1] + dataArray[b+1] + dataArray[br+1])/9;
						
						dataArrayCopy[i+2] = (dataArray[c+2]+dataArray[tl+2] + dataArray[t+2] + dataArray[tr+2]
						+dataArray[l+2] + dataArray[r+2] + dataArray[bl+2] + dataArray[b+2] + dataArray[br+2])/9;
						}
			}
		}
		//dataArray = dataArrayCopy;
		for (int j=0; j<=512*512*3; j++) {

		dataArray[j] = dataArrayCopy[j];
		}
	}

	//Don't know how to draw circle...
	void CatPictureApp::circle(uint8_t* dataArray,float midx, float midy, float radius, Color8u color) {


	//for(int y=midy-radius; y<=midy+radius; y++){
		//for(int x=midx-radius;x<=midx+radius; x++){
		for(int y=0; y<=512; y++){
		for(int x=0;x<=512; x++){
			if(y < 0 || x < 0) continue;

			
			
				

			if ((pow(x-midy,2) + (pow(y-midx,2))) <= (pow(radius,2)))
			{

			int i = 3*(x + y*512);
			dataArray[i] = color.r;
			dataArray[i+1] = color.g;
			dataArray[i+2] = color.b;
			}
	}
	}
	}


	/*void CatPictureApp::copyRecAndMove(uint8_t* dataArray, int row, int col, int row2, int col2) {
	Color8u color = Color8u(255,0,255);
	for (int x=col; x<=col+50; x++) { //col+width
		for (int y=row; y<=row+50; y++) { // row+height
			if (x == col+50|| x == col || y == row ||y == row+50) {
				uint8_t* CopiedArray = (*mySurface_).getData();
					dataArray[(3*(x+(y*512)))] = color.r;
					dataArray[(3*(x+(y*512)))+1] = color.g;
					dataArray[(3*(x+(y*512)))+2] = color.b;
			}
		}
	}
}*/



void CatPictureApp::setup()
{
	frame_number_= 0;
	mySurface_ = new Surface(TextureSize,TextureSize,false);
	myTexture = new gl::Texture(*mySurface_);
	//dataArrayCopy = (*mySurface_).getData();



	
}



void CatPictureApp::mouseDown( MouseEvent event )
{
	//Satisfies E.6, though it is debatable whether or not this is an "interesting" interaction
	tint((*mySurface_).getData(), event.getX(), event.getY());
}

void CatPictureApp::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	//uint8_t* dataArrayCopy = new uint8_t[512*512*3];

	//for(int i = 0; i<=512*512*3; i++)
	//{
		//dataArrayCopy[i] = dataArray[i];
	//}

	//dataArrayCopy = (*mySurface_).getData();
	//Color8u fill = Color8u(255,255,255);
	//r = sin( getElapsedSeconds() ) * 0.5f +0.5f;
	r=1.0f;
	//Oscillates the colors
	g = cos( getElapsedSeconds()*0.5 ) * 0.35f + 0.5f;
	b = cos( getElapsedSeconds()*0.5 ) * 0.5f + 0.5f;
	Color8u yellowToWhite = Color( 1.0f, 1.0f, cos( getElapsedSeconds()*0.5 ) * -0.5f + 0.5f);
	
	//YES
	//rec(dataArray, 256, 256, 256, 256, Color(0,0,b));
	rec(dataArray, 256, 450, 256, 62, Color(0,g,0));
	rec(dataArray, 100, 100, 100, 100, Color(255,255,255));
	line(dataArray, 100.0f, 100.0f,200.0f,200.0f, Color(1.0f,1.0f,1.0f));
	circle(dataArray, 50, 462, 50, yellowToWhite);
	tint(dataArray,100,100);
	//tint(dataArray,256,256);
	blur(dataArray);
	//TO HERE
	//rectangle(dataArray, 412, 412, 100, 100);
	//Hopefully one day will produce a circle...
	//circle(dataArray, 50,50,50,Color(255,255,255));
	//writeImage("CAT!.jpg",*mySurface_);
	///IDK!
	
	rectangle(dataArray,0,0,100,100);
	(*myTexture).update(*mySurface_,(*mySurface_).getBounds());
}

void CatPictureApp::draw()
{
	//gl::draw(*mySurface_);
	//lets try...
	gl::draw(*myTexture);
	//gl::draw(*mySurface_2);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
