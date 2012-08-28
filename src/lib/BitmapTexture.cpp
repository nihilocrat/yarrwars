// ------------------------------------------------------------
// BitmapTexture.cpp
// ------------------------------------------------------------
// Functions to convert BMP resources to OpenGL textures
// Modified from:
// Using Bitmaps for OpenGL Texture Mapping in Microsoft Visual Studio
// (c) 2002 Rachel Grey
// Available at http://www.cityintherain.com/howtotex.html


// HEADERS -----------------------------------------------------
#include "..\\glut.h"
#include "BitmapTexture.h"


// DEFINITIONS -------------------------------------------------
#define NUM_TEXTURES 9 // maximum textures our program will use


// VARIABLES ---------------------------------------------------
HBITMAP TexHand[NUM_TEXTURES];	// handle to bitmap structure
BITMAP TexBMP[NUM_TEXTURES];	// storage for bitmap info
int CurrTexLoad = 0;			// which texture are we currently loading
int Filter = 0;					// type of smoothing for texture mapping
bool drawtextures = true;		// are we drawing textures or not?


// FUNCTIONS ---------------------------------------------------
void LoadBitmapTexture(int id) 
{	

	// to load resources from our .exe file, we need a handle to that file - 
	// get it here; use it in the LoadImage command below
	HINSTANCE theInstance; // handle to the program
	theInstance = GetModuleHandle(NULL);

	
	// bonehead error checking
	if (CurrTexLoad >= NUM_TEXTURES) {
		MessageBox(NULL,"Trying to load too many textures!","ERROR!",MB_OK);
		return;
	}

	// set handle to null
	TexHand[CurrTexLoad] = NULL;

	// Get a handle to the bitmap resource
	TexHand[CurrTexLoad] = (HBITMAP) ::LoadImage(theInstance, 
		MAKEINTRESOURCE(id), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	//Transfer info about the bitmap into our bitmap structure in memory
	TexBMP[CurrTexLoad];
	::GetObject(TexHand[CurrTexLoad], sizeof(TexBMP[CurrTexLoad]), &TexBMP[CurrTexLoad]);


	// Now we'll bind each texture to its resource ID (specified in resource.h)
	  // set the ID we're using - we'll be able to request the texture again later with this ID number
	glBindTexture(GL_TEXTURE_2D,id); 
	// Set basic parameters for this texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  // convert the BITMAP to a GL-usable format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TexBMP[CurrTexLoad].bmWidth, TexBMP[CurrTexLoad].bmHeight,
		0, GL_BGR_EXT, GL_UNSIGNED_BYTE, TexBMP[CurrTexLoad].bmBits); 


	CurrTexLoad++;

}


void SetTextureParameters() {
 
	//tell OpenGL to ignore padding at ends of rows
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Set the texture to tile
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// decide whether we're using the biliniear smoothing or not
	if (Filter == TF_NONE) 
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

}


void SetCurrentTexture(int tex) { // set the current drawing texture
	glBindTexture(GL_TEXTURE_2D,tex);
}


void DestroyBitmaps() { // gets rid of all textures - run at end of program to avoid memory leak
	for (int i=0;i<NUM_TEXTURES;i++) {
		DeleteObject((HGDIOBJ) TexHand[i]);  
		DeleteObject(&TexBMP[i]);
	}
}
