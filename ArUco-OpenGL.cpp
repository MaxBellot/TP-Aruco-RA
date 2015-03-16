//
//  ArUco-OpenGL.cpp
//  UserPerspectiveAR
//
//  Created by Jean-Marie Normand on 28/02/13.
//  Copyright (c) 2013 Centrale Innovation. All rights reserved.
//

#include <iostream>
#include "ArUco-OpenGL.h"
#include <cmath>

// Constructor
ArUco::ArUco(string intrinFileName, float markerSize) {
   // Initializing attributes
   m_IntrinsicFile= intrinFileName;
   m_MarkerSize   = markerSize;
   // read camera parameters if passed
   m_CameraParams.readFromXMLFile(intrinFileName);
   xBalle = 0;
   dir = 1;
}

// Destructor
ArUco::~ArUco() {}

// Detect marker and draw things
void ArUco::doWork(Mat inputImg) {
   m_InputImage   = inputImg;
   m_GlWindowSize = m_InputImage.size();
   resize(m_GlWindowSize.width, m_GlWindowSize.height);
}

// Draw axis function
void ArUco::drawAxis(float axisSize) {
   // X
   glColor3f (1,0,0);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(axisSize,0.0f, 0.0f); // ending point of the line
   glEnd( );
   
   // Y
   glColor3f (0,1,0);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(0.0f, axisSize, 0.0f); // ending point of the line
   glEnd( );
   
   // Z
   glColor3f (0,0,1);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(0.0f, 0.0f, axisSize); // ending point of the line
   glEnd( );
}

// GLUT functionnalities

// Drawing function
void ArUco::drawScene() {

	typedef struct {
   float x;
   float y;
   float z;
   float r;
   float g;
   float b;
   //float a;
} vertex;


	// Création des sommets du cube
vertex cube[8]={
   {-0.5f,-0.5f, 0.5f,1.0f,0.0f,0.0f},
   {-0.5f, 0.5f, 0.2f,0.0f,1.0f,0.0f},
   { 0.5f, 0.5f, 0.5f,0.0f,0.0f,1.0f},
   { 0.5f,-0.5f, 0.5f,1.0f,1.0f,1.0f},
   {-0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f},
   {-0.5f, 0.5f,-0.5f,0.0f,1.0f,0.0f},
   { 0.5f, 1.0f,-0.5f,0.0f,0.0f,1.0f},
   { 0.5f,-0.5f,-0.5f,1.0f,1.0f,1.0f}
};

// Création des faces du cube
int face[6][4] ={
   {0,1,2,3},
   {7,6,2,3},//{3,2,6,7},
   {4,5,6,7},
   {0,1,5,4},
   {1,5,6,2},
   {0,4,7,3}

};


   // If we do not have an image we don't do anyhting
   if (m_ResizedImage.rows==0)
      return;
   
   // Setting up  OpenGL matrices
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, m_GlWindowSize.width, 0, m_GlWindowSize.height, -1.0, 1.0);
   glViewport(0, 0, m_GlWindowSize.width , m_GlWindowSize.height);
   glDisable(GL_TEXTURE_2D);
   glPixelZoom( 1, -1);
   
   //////glRasterPos3f( 0, m_GlWindowSize.height  - 0.5f, -1.0f );
   glRasterPos3f(0, m_GlWindowSize.height, -1.0f);
   
   glDrawPixels (m_GlWindowSize.width, m_GlWindowSize.height, GL_RGB, GL_UNSIGNED_BYTE, m_ResizedImage.ptr(0));
   
   // Enabling depth test
   glEnable(GL_DEPTH_TEST);
   
   // Set the appropriate projection matrix so that rendering is done 
   // in an environment like the real camera (without distorsion)
   glMatrixMode(GL_PROJECTION);
   double proj_matrix[16];
   //m_CameraParams.glGetProjectionMatrix(m_InputImage.size(),m_GlWindowSize,proj_matrix,0.05,10);
   m_CameraParams.glGetProjectionMatrix(m_ResizedImage.size(),m_GlWindowSize,proj_matrix,0.05,10);
   glLoadIdentity();
   glLoadMatrixd(proj_matrix);
   
   // Debug : outputting projection matrix
   /*
    std::cout<<"Projection Matrix"<<std::endl;
   for(int i=0;i<4;i++) {
      std::cout<<"| ";
      for(int j=0;j<4;j++) {
         std::cout<<proj_matrix[j*4+i]<<"\t\t\t";
      }
      std::cout<<" |"<<std::endl;
   }
    */
    // -------------------------------------------------------------------
    // we can start drawing!
    // -------------------------------------------------------------------

   double modelview_matrix[16];
   std::cout << "Number of markers: " << m_Markers.size() << std::endl;
   // For each detected marker



   for (unsigned int m=0;m<m_Markers.size();m++)
   {
      m_Markers[m].glGetModelViewMatrix(modelview_matrix);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glLoadMatrixd(modelview_matrix);
      
      // Disabling light if it is on
      GLboolean lightOn = false;
      glGetBooleanv(GL_LIGHTING, &lightOn);
      if(lightOn) {
         glDisable(GL_LIGHTING);
      }
      
      // Drawing axis
      drawAxis(m_MarkerSize);
      
      // Drawing a cube
	  Vec3f positionCube1, positionCube2, positionSphere, p;
	  
	  if (m_Markers[m].id == 22)
	  {
		  glColor3f(1,0,0);
		  glTranslatef(0, m_MarkerSize/2,0);
		  glPushMatrix();
          glutSolidCube( m_MarkerSize );

			GLfloat matrix[16]; 
			glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
			positionCube1 = Vec3f (matrix[12],matrix[13],matrix[14]);
	  } else  if ( m_Markers[m].id == 102) {
			glColor3f(1,0.4f,0.4f);
			glTranslatef(0, m_MarkerSize/2,0);
			glPushMatrix();
			glutSolidCube( m_MarkerSize );
			GLfloat matrix[16]; 
			glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
			positionCube2 = Vec3f (matrix[12],matrix[13],matrix[14]);

	  } else {
			glColor3f(0.0f,0.0f,1.0f);
			
			GLfloat matrix[16]; 
			glGetFloatv (GL_MODELVIEW_MATRIX, matrix);



			/*p = Vec3f(matrix[0]*positionCube1[0] + matrix[4]*positionCube1[1] + matrix[8]*positionCube1[2] + matrix[12], 
				matrix[1]*positionCube1[0] + matrix[5]*positionCube1[1]+ matrix[9]*positionCube1[2] + matrix[13], 
				matrix[2]*positionCube1[0] + matrix[6]*positionCube1[1] + matrix[10]*positionCube1[2] + matrix[14] );*/

			glTranslatef(xBalle, m_MarkerSize/2, 0);
			glPushMatrix();
			glutSolidSphere(m_MarkerSize/10, 16, 16);

			xBalle = xBalle + dir*0.005;

			//GLfloat matrix[16]; 
			//glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
			//positionSphere = Vec3f (matrix[12],matrix[13],matrix[14]);

			//p = Vec3f(matrix[0]*positionCube1[0] + matrix[4]*positionCube1[1] + matrix[8]*positionCube1[2] + matrix[12], matrix[1]*positionCube1[0] + matrix[5]*positionCube1[1]+ matrix[9]*positionCube1[2] + matrix[13], matrix[2]*positionCube1[0] + matrix[6]*positionCube1[1] + matrix[10]*positionCube1[2] + matrix[14] );
	  }
	  
	 // if ( pow(positionSphere[0]-positionCube1[0],2)+ pow(positionSphere[1]-positionCube1[1],2) + pow(positionSphere[2]-positionCube1[2],2) < .05)
		//	dir = - 1;
	  /*if ( p[0]!=0 && (p[0] - xBalle < 0.000001))
		  cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa "<< p[0] << endl;
		  dir = -1;*/
      

	

	  /*// Dessin d'un cube colore
		// face par face
	for(int i=0;i<6;i++){
		  glBegin(GL_POLYGON);
		  for(int j=0;j<4;j++){
			 glColor3f(cube[face[i][j]].r, cube[face[i][j]].g, cube[face[i][j]].b);
			 glVertex3f(cube[face[i][j]].x*m_MarkerSize, cube[face[i][j]].y*m_MarkerSize, cube[face[i][j]].z*m_MarkerSize);
      }
      glEnd();
	  

   }*/

      //glutSolidTeapot( m_MarkerSize );
      // Re-enabling light if it is on
      if(lightOn) {
         glEnable(GL_LIGHTING);
      }
      
      glPopMatrix();
   }

     // -------------------------------------------------------------------
   // Disabling depth test to display the video
    // -------------------------------------------------------------------

   glDisable(GL_DEPTH_TEST);
   
   m_pixels.create(m_GlWindowSize.height , m_GlWindowSize.width, CV_8UC3);
   //use fast 4-byte alignment (default anyway) if possible
   glPixelStorei(GL_PACK_ALIGNMENT, (m_pixels.step & 3) ? 1 : 4);
   //set length of one complete row in destination data (doesn't need to equal img.cols)
   glPixelStorei(GL_PACK_ROW_LENGTH, m_pixels.step/m_pixels.elemSize());
   // Reading back the pixels
   glReadPixels(0, 0, m_GlWindowSize.width , m_GlWindowSize.height, GL_RGB, GL_UNSIGNED_BYTE, m_pixels.data);
   // Flip the pixels since OpenCV stores top to bottom and OpenGL from bottom to top
   cv::flip(m_pixels, m_pixels, 0);
}

// Idle function
void ArUco::idle(Mat newImage) {
   // Getting new image
   m_InputImage = newImage.clone();
   
   // Do that here ?
   resize(m_InputImage.size().width, m_InputImage.size().height);
   
   // Undistort image based on distorsion parameters
   m_UndInputImage.create(m_InputImage.size(),CV_8UC3);
   
   //transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
   cv::cvtColor(m_InputImage,m_InputImage,CV_BGR2RGB);
   
   //remove distorion in image
   // Jim commented next line and added the clone line
   //cv::undistort(m_InputImage,m_UndInputImage, m_CameraParams.CameraMatrix, m_CameraParams.Distorsion);
   m_UndInputImage = m_InputImage.clone();
   
   //detect markers
   m_PPDetector.detect(m_UndInputImage, m_Markers, m_CameraParams.CameraMatrix, Mat(), m_MarkerSize);
   //m_PPDetector.detect(m_UndInputImage, m_Markers, m_CameraParams, m_MarkerSize);
   
   //resize the image to the size of the GL window
   cv::resize(m_UndInputImage,m_ResizedImage,m_GlWindowSize);
}

// Resize function
void ArUco::resize(GLsizei iWidth, GLsizei iHeight) {
   m_GlWindowSize=Size(iWidth,iHeight);
   
   //not all sizes are allowed. OpenCv images have padding at the end of each line in these that are not aligned to 4 bytes
   if (iWidth*3%4!=0) {
      iWidth+=iWidth*3%4;//resize to avoid padding
      resize(iWidth, m_GlWindowSize.height);
   }
   else {
      //resize the image to the size of the GL window
      //if (m_UndInputImage.rows!=0)
         //cv::resize(m_UndInputImage, m_ResizedImage, m_GlWindowSize);
   }
}


// Jim
cv::Mat ArUco::getPixels() {
   return m_pixels.clone();
}

// Test using ArUco to display a 3D cube in OpenCV
void ArUco::draw3DCube(cv::Mat img, int markerInd) {
   if(m_Markers.size() > markerInd) {
      aruco::CvDrawingUtils::draw3dCube(img, m_Markers[markerInd], m_CameraParams); 
   }
}

void ArUco::draw3DAxis(cv::Mat img, int markerInd) {
   if(m_Markers.size() > markerInd) {
      aruco::CvDrawingUtils::draw3dAxis(img, m_Markers[markerInd], m_CameraParams); 
   }
   
}
