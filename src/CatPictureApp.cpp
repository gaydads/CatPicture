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
	int frame_number_;
	float r;
	float g;
	float b;
	void rectangle(uint8_t* dataArray, int row, int col, int width, int height);
	void rec(uint8_t* dataArray, int midx, int midy, int col, int row, Color8u color);
	void circle(uint8_t* dataArray, float midx, float midy, float radius, Color8u color);
	void CatPictureApp::distance(int midx, int midy, float radius);
	void CatPictureApp::line(uint8_t* dataArray, int row1, int col1, int row2, int col2, Color8u color);
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
void CatPictureApp::line(uint8_t* dataArray, int row1, int col1, int row2, int col2, Color8u color) {
	int rise = row1-row2;
	int run = col1-col2;
	if ((row1>row2 && col1>col2 && run !=0) == false) {
	int slope = (rise/run);
	int x=col1;
	int y=row1;

	while (x<=col2 && y <=row2) {
			if(((y)/(x)) == slope) {
					dataArray[(3*(x+(y*512)))] = color.r;
					dataArray[(3*(x+(y*512)))+1] = color.g;
					dataArray[(3*(x+(y*512)))+2] = color.b;
			}
			x++;
			y++;
	}
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



void CatPictureApp::setup()
{
	frame_number_= 0;
	mySurface_ = new Surface(TextureSize,TextureSize,false);
	
}



void CatPictureApp::mouseDown( MouseEvent event )
{
}

void CatPictureApp::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	//Color8u fill = Color8u(255,255,255);

	//r = sin( getElapsedSeconds() ) * 0.5f +0.5f;
	r=1.0f;

	//Oscillates the colors
	g = cos( getElapsedSeconds()*0.5 ) * 0.35f + 0.5f;
	b = cos( getElapsedSeconds()*0.5 ) * 0.5f + 0.5f;
	
	//Two rectangles
	//rectangle(dataArray, 256, 256, 100, 100);

	//LINE
	line(dataArray, 100, 100, 200, 200, Color(0,0,b));

	//good recs
	//rec(dataArray, 256, 256, 256, 256, Color(0,0,b));
	//rec(dataArray, 256, 450, 256, 62, Color(0,g,0)); 

	circle(dataArray, 50, 462, 50, Color(2.0f,1.0f,1.0f));  
	//rectangle(dataArray, 412, 412, 100, 100);

	//Hopefully one day will produce a circle...
	//circle(dataArray, 50,50,50,Color(255,255,255));

	//writeImage("CAT!.jpg",*mySurface_);
}

void CatPictureApp::draw()
{

	gl::draw(*mySurface_);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
