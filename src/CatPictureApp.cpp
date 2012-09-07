/**
*@file CatPictureApp.cpp
* Should display a picture
* 
* @author David Gayda
* @date 9/5/2012
* 
* @note This file is (c) 2012. It is licensed under the 
* CC BY 3.0 license (http://creativecomons.org/licenses/b/3.0/).
* which means you are free to use, share, and remix it as long as you
* give attribution. Commercial uses are allowed.
*/


#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "Resources.h"
#include <deque>


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
	Surface* mySurface_; //The Surface object whose pixel array we will modify
	gl::Texture* myTexture; //The Texture object that we use to display our Surface in the window
	int count_for_mouse;
	int mouse_row;
	int mouse_col;
	int frame_count;
	float r;
	float g;
	float b;
	void rectangle(uint8_t* dataArray, int row, int col, int width, int height);
	void rec(uint8_t* dataArray, int midx, int midy, int col, int row, Color8u color);
	void circle(uint8_t* dataArray, float midx, float midy, float radius, Color8u color);
	void CatPictureApp::line(uint8_t* dataArray, float row1, float col1, float row2, float col2, Color8u color);
	void CatPictureApp::tint(uint8_t* dataArray,int row, int col);
	void CatPictureApp::copyMoveUp(uint8_t* dataArray, int row, int col);
	void CatPictureApp::blur(uint8_t* dataArray);
	void CatPictureApp::horizontalLine(uint8_t* dataArray, int row, int col, int width);
	//deque used to monitor x,y coordinate clicks
	// Lucy:"This is really a great way I have never seen!"
	deque<int> tints;
	deque<int> copyMove;
	uint8_t* dataArrayCopyMove;
};

// Lucy:"I believe it is also smart to use varialbes to manager size of window. If you need to change size of the window, it is very convenient. My hard code is really hard for changing."
//Width and height of the screen
static const int AppWidth = 512;
static const int AppHeight = 512;
static const int TextureSize = 512; //Must be the next power of 2 bigger or equal to app dimensions

void CatPictureApp::prepareSettings(Settings* settings) {
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}


	/**
	 * Creates a hollow rectangle based on top left corner and a specified width and height.
	 *
	 * This satisfies the "rectangle" requirement, goal A.1
	 */

// Lucy:"I really love the idea that setting background color and then draw something on it."
void CatPictureApp::rectangle(uint8_t* dataArray, int row, int col, int width, int height) {
	Color8u color = Color8u(255,0,255);
	for (int x=col; x<=col+width; x++) { //col+width   *HOLLOW (To fill use <=)
		for (int y=row; y<=row+height; y++) { // row+height  *HOLLOW (To fill use <=)
			if (x == col+width|| x == col || y == row ||y == row+height) {
					dataArray[(3*(x+(y*512)))] = color.r;
					dataArray[(3*(x+(y*512)))+1] = color.g;
					dataArray[(3*(x+(y*512)))+2] = color.b;
			}
		}
	}
}

/**
	 * Creates a straight horizontal line (left to right) based on given row col and width
	 *
	 * This satisfies the "line" requirement, goal A.3
	 */
// Lucy: "Here is my suggestion: If you only assign color in this method, you may not be able to change color of this line easily when you need. So probably passing a paremeter called 'Color8u color' as usual is a better way?"
void CatPictureApp::horizontalLine(uint8_t* dataArray, int row, int col, int width) {
	Color8u color = Color8u(255,0,255);
	for (int x=col; x<=col+width; x++) {
		if (x<= width) {
			dataArray[(3*(x+(row*512)))] = color.r;
			dataArray[(3*(x+(row*512)))+1] = color.g;
			dataArray[(3*(x+(row*512)))+2] = color.b;
		}
	}


}


 
	/**
	 * Creates only a downward sloping line from two points
	 *
	 * 
	 */
//Fix Bugs... 45 degrees work...
// Lucy: "Do you really need this method in your program? This time I could not see the mountain you would like to draw."
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
				
			}
		}

	}
}

/**
	 * Creates a filled rectangle based on x and y mid points and a specified width and height.
	 *
	 * This satisfies the "rectangle" requirement, goal A.3
	 */
//Draws Rectangle Based on x,y mid point and col(width) row(length)
	void CatPictureApp::rec(uint8_t* dataArray,int midx, int midy, int col, int row, Color8u color) {
		for(int y=midy-row; y<=midy+row; y++){
			for(int x=midx-col;x<=midx+col; x++){
				if(y < 0 == false || x < 0 == false) {
					//Check for bounds
					if((x >= midx-col)||(x <= midx+col)||(y >= midy-row)||(y <= midy+row)) {
						int i = 3*(x + y*512);
						//Fill in the rectangle
						dataArray[i] = color.r;
						dataArray[i+1] = color.g;
						dataArray[i+2] = color.b;
					}
				}
			}
		}
	}

	/**
	 * Applies a circular colored tint to a specified circular area.
	 *
	 * This satisfies the "Tint" requirement, goal A.6
	 */
	// Lucy:"I always love 'tint' method! That is really intersting!"
	void CatPictureApp::tint(uint8_t* dataArray, int row, int col) {
			for(int y=0; y<=512; y++){
		for(int x=0;x<=512; x++){
			if ((pow((float)x-row,2) + (pow((float)y-col,2))) <= (pow(50.0f,2)))
			{
			dataArray[(int)(3*(x+(y*512)))] = dataArray[(int)(3*(x+(y*512)))]+200;
			dataArray[(int)(3*(x+(y*512)))+1] = dataArray[(int)(3*(x+(y*512)))+1]+200;
			dataArray[(int)(3*(x+(y*512)))+2] = dataArray[(int)(3*(x+(y*512)))+2]+200;
			}
		}
			}
	}

	/**
	 * Applies a blur to the entire image.
	 *
	 * This satisfies the "blur" requirement, goal B.1
	 */
	// Lucy: "It is probably not a good way to use hard code to represent width and height of window. If you need to change them, then...thousands of changes. Probably using variables is a better way."
	void CatPictureApp::blur(uint8_t* dataArray) {
		int j=0;
		uint8_t* dataArrayCopy = new uint8_t [512*512*3];
		for(int col=0; col<=512; col++){
			for(int row=0;row<=512; row++){
					
				//Find 9 pixel locations
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

						//if cuts off the top 3 pixels
						if (row == 0) {
							dataArrayCopy[i] = (dataArray[c] +dataArray[l] + dataArray[r] + dataArray[bl] + dataArray[b] + dataArray[br])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]
						+dataArray[l+1] + dataArray[r+1] + dataArray[bl+1] + dataArray[b+1] + dataArray[br+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]
						+dataArray[l+2] + dataArray[r+2] + dataArray[bl+2] + dataArray[b+2] + dataArray[br+2])/6;
						}
						//if cuts off left pixels
						else if (col==0) {
							dataArrayCopy[i] = (dataArray[c] + dataArray[t] + dataArray[tr]
						 + dataArray[r] + dataArray[b] + dataArray[br])/6;

						dataArrayCopy[i+1] = (dataArray[c+1] + dataArray[t+1] + dataArray[tr+1]
						+ dataArray[r+1] + dataArray[b+1] + dataArray[br+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2] + dataArray[t+2] + dataArray[tr+2]
						+ dataArray[r+2] + dataArray[b+2] + dataArray[br+2])/6;
						}

						//if cuts off right pixels
						else if(col == 512*3) {
						dataArrayCopy[i] = (dataArray[c]+dataArray[tl] + dataArray[t]
						+dataArray[l] + dataArray[bl] + dataArray[b])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]+dataArray[tl+1] + dataArray[t+1]
						+dataArray[l+1] + dataArray[bl+1] + dataArray[b+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]+dataArray[tl+2] + dataArray[t+2]
						+dataArray[l+2] + dataArray[bl+2] + dataArray[b+2])/6;
						}

						//if cuts off bottom pixels
						else if (row == 512*3) {
							dataArrayCopy[i] = (dataArray[c]+dataArray[tl] + dataArray[t] + dataArray[tr]
						+dataArray[l] + dataArray[r])/6;

						dataArrayCopy[i+1] = (dataArray[c+1]+dataArray[tl+1] + dataArray[t+1] + dataArray[tr+1]
						+dataArray[l+1] + dataArray[r+1])/6;
						
						dataArrayCopy[i+2] = (dataArray[c+2]+dataArray[tl+2] + dataArray[t+2] + dataArray[tr+2]
						+dataArray[l+2] + dataArray[r+2])/6;

						}

						//if 9 pixels to be blurred are present
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
		for (int j=0; j<=512*512*3; j++) {
		dataArray[j] = dataArrayCopy[j];
		}
	}

	/**
	 * Draws a filled circle based on midpoints and radius
	 *
	 * This satisfies the "circle" requirement, goal A.2
	 */
	void CatPictureApp::circle(uint8_t* dataArray,float midx, float midy, float radius, Color8u color) {
		for(int y=0; y<=512; y++){
		for(int x=0;x<=512; x++){
			// Lucy: "if y=0 and y++ and x=0 and x++ then x and y will never be negative. So this line probably will never work?"
			if(y < 0 || x < 0) continue;

			//If correct distance away from mid points
			if ((pow(x-midy,2) + (pow(y-midx,2))) <= (pow(radius,2))) {
			int i = 3*(x + y*512);
			dataArray[i] = color.r;
			dataArray[i+1] = color.g;
			dataArray[i+2] = color.b;
			}
		}
		}
	}

	//In process of being a copy maker
	/*void CatPictureApp::copyMoveUp(uint8_t* dataArray, int row, int col) {
		dataArrayCopyMove = new uint8_t [512*512*3];
		 Color8u color = Color(1.0f,1.0f,0.5f);
		for(int y=0; y<=512; y++){
			for(int x=0;x<=512; x++){
				int i = 3*(x + y*512);
						dataArrayCopyMove[i] = dataArray[i];
						dataArrayCopyMove[i+1] = dataArray[i+1];
						dataArrayCopyMove[i+2] = dataArray[i+2];
			}
		}
		int z = 156;
		int w = 156;
		for(int y=row-100; y<=row+100; y++){
			for(int x=col-100; x<=col+100; x++){
				int j = 3*(z + w*512);
				int i = 3*(col + row*512);
				if(y < 0 == false || x < 0 == false) {
				if((x >= col-100)||(x <= col+100)||(y >= row-100)||(y <= row+100)) {
					dataArrayCopyMove[i] = dataArray[j]; /////////////////
					dataArrayCopyMove[i+1] = dataArray[j+1];
					dataArrayCopyMove[i+2] = dataArray[j+2];
				}
				}
				z++;
			}
			w++;
		}

		for(int y=256-100; y<=256+100; y++){
			for(int x=256-100; x<=256+100; x++){
			int i = 3*(x + (y)*512);
			//int iMove = 3*((100)+((1)*512));
			if(y < 0 == false || x < 0 == false) {
			if((x >= 256-100)||(x <= 256+100)||(y >= 256-100)||(y <= 256+100)) {
				dataArray[i] = dataArrayCopyMove[i];
				dataArray[i+1] = dataArrayCopyMove[i+1];
				dataArray[i+2] = dataArrayCopyMove[i+2];
					//	dataArray[i] = dataArrayCopyMove[i];
						//dataArray[i+1] = dataArrayCopyMove[i+1];
						//dataArray[i+2] = dataArrayCopyMove[i+2];
			}
			}
			}
			}


		/*for(int y=0; y<=512; y++){
			for(int x=0;x<=512; x++){
				if(y < 0 == false || x < 0 == false) {
					//If inside the bounds
					if((x >= col-50)||(x <= col+50)||(y >= row-50)||(y <= row+50)) {
						//Row Major Order Formula
						int i = 3*(x + y*512);
						dataArrayCopyMove[i] = dataArray[i];
						dataArrayCopyMove[i+1] = dataArray[i+1];
						dataArrayCopyMove[i+2] = dataArray[i+2];
					}
				}
			}
			for(int y=row-50; y<=row+50; y++){
			for(int x=col-50;x<=col+50; x++){
			int i = 3*(col + row*512);
			int iMove = 3*((col+156)+((row+156)*512));
						dataArray[iMove] = dataArrayCopyMove[i];
						dataArray[iMove+1] =dataArrayCopyMove[i+1];
						dataArray[iMove+2] =dataArrayCopyMove[i+2];
			}
			}
	
	}
	}*/


	// Sets up for the draw update methods
void CatPictureApp::setup()
{
	frame_count=0;
	count_for_mouse = 0;
	dataArrayCopyMove = new uint8_t [50*50*3];
	mySurface_ = new Surface(TextureSize,TextureSize,false);
	myTexture = new gl::Texture(*mySurface_);
}


	/**
	 * Works with tint method to place a tint circle based on
	 * the users mouse click location
	 *
	 * This satisfies the "Mouse Interaction" requirement, goal E.6
	 */
// Lucy: "I like this way because now we can tint on space with color. In my solution I do not need this one but in your program it is very crucial."
void CatPictureApp::mouseDown( MouseEvent event )
{
	tints.push_back((int)event.getX());
	tints.push_back((int)event.getY());

}

void CatPictureApp::update()
{
	frame_count++;
	uint8_t* dataArray = (*mySurface_).getData();
	r=1.0f;

	//Oscillates the colors grean to dark green and blue to black
	g = cos( getElapsedSeconds()*0.5 ) * 0.35f + 0.5f;
	b = cos( getElapsedSeconds()*0.5 ) * 0.5f + 0.5f;
	//Oscillates Yellow to White
	Color8u yellowToWhite = Color( 1.0f, 1.0f, cos( getElapsedSeconds()*0.5 ) * -0.5f + 0.5f);
	//Draws the sky and the grass
	rec(dataArray, 256, 256, 256, 256, Color(0,0,b));
	rec(dataArray, 256, 450, 256, 62, Color(0,g,0));
	//Draws a line in the sky
	line(dataArray, 50.0f, 50.0f,100.0f,100.0f, Color(1.0f,1.0f,1.0f));

	//Draws the sun/moon guy
	circle(dataArray, 50, 462, 50, yellowToWhite);
	circle(dataArray, 30, 462-20,10,Color(255,255,255));
	circle(dataArray, 30, 462+20,10,Color(255,255,255));
	rec(dataArray, 462, 30,10,2,Color(255,255,255));
	rec(dataArray, 462, 60,20,4,Color(255,255,255));

	//Draws two horizontal lines at the bottom of the screen
	horizontalLine(dataArray, 500,0,512);
	horizontalLine(dataArray, 505,0,512);

	/**
	* Uses the update draw loop to count frames and perform fun annimation
	*
	* This satisfies the "Annimation" requirement, goal E.5
	*
	**/
	//It's gon rain!
	// Lucy: "I love this creative idea!!! But does it represent rain or cloud?"
	if (frame_count<30) {
	circle(dataArray, 10+frame_count*2,10+frame_count*3,10+frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 20+frame_count*2,10+frame_count*3,10+frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 20+frame_count*2,20+frame_count*3,10+frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 50-frame_count*2,200+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 50-frame_count*2,230+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 50-frame_count*2,260+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 50-frame_count*2,290+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 80-frame_count*2,215+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 80-frame_count*2,245+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));
	circle(dataArray, 80-frame_count*2,270+frame_count*3,30-frame_count,Color(cos(0.5f),cos(0.5f),cos(0.5f)));

	rec(dataArray,200+frame_count*3,130+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,220+frame_count*3,140+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,230+frame_count*3,130+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,250+frame_count*3,140+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,270+frame_count*3,130+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,245+frame_count*3,150+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	rec(dataArray,265+frame_count*3,160+frame_count*4,1,20,Color(0.0f,0.0f,1.0f));
	}
	else {
		frame_count =0;
	}

	//Blurs the surface images
	blur(dataArray);
	
	//Uses deque to determine if mouse has been clicked
		while (tints.size() > 0) {

			//Lucy: "I should check this part really careful for learning because I still do not understand how to do it. Thank you!"

			// initialize random seed:
			//srand ( time(NULL) );
			//generate random number
			//int random = rand() % 2 + 1;

			count_for_mouse = 1;
			mouse_row = tints[0];
			tints.pop_front();
			mouse_col = tints[0];
			tints.pop_front();
		}

		//If mouse is clicked, tint at mouse click
		if (count_for_mouse == 1) {
			mouse_row = mouse_row;
			mouse_col = mouse_col;
			tint(dataArray, mouse_row, mouse_col);
			//count_for_mouse = 0;
		}
		
		//Unimplemented copy click
		/*if (count_for_mouse == 2) {
			mouse_row = mouse_row;
			mouse_col = mouse_col;
			copyMoveUp(dataArray, mouse_row, mouse_col);
			//count_for_mouse = 0;
		}*/
		//copyMoveUp(dataArray,256,256);

	(*myTexture).update(*mySurface_,(*mySurface_).getBounds());
}

//Draws the image
void CatPictureApp::draw()
{
	gl::draw(*myTexture);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
