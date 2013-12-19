//-------------------------------------------------------------------------------------------
//   Qtgl for graphics class
//-------------------------------------------------------------------------------------------

#include "glwidget.h"



const double torad = M_PI/180.0;
const double lim=0.5;
GLfloat vertices[4][2] = {{100.0, 100.0}, {400.0, 100.0}, {400.0, 400.0}, {100.0, 400.0}}; //  vertex coords
GLubyte indices[] = {0,1,1,2,2,3,3,0};

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      mClickLocationX(0),//mouseclick
      mClickLocationY(0)
{
    startup();
}

GLWidget::~GLWidget()
{    

}

void GLWidget::startup()
{
    winw=width();  // width returns width of window
    winh=height();
    button = 0;
    cerr << "Glwidget\n";
    version=MYVERSION;
    xangle= yangle= zangle=0.0;
    scale = 1.5;
    object =0; // in this version no display list
    xfrom=yfrom=zfrom=5.0;
    xto=yto=zto=0;
    filled=false;
    angleOfX = atan(1);//initialize the angle between radius(camera) and x axis
    angleOfY = atan(5/sqrt(50));
    radius = sqrt(75);//orginal radius calculated by using formula sqrt(x^2+y^2+z^2)
    pointnumber = 1;//initialize view to point
    xlook = zlook = 0.0;
    ylook=1.0;
    testview=true;
    statenumber=0;
}

void GLWidget::clear()
{
     updateGL();
}

void GLWidget::initializeGL()
{
    int i;
    QImage buf(256, 256, QImage::Format_RGB32);

GLfloat whiteDir[4] = {2.0, 2.0, 2.0, 1.0};
GLfloat whiteAmb[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightPos[4] = {30.0, 30.0, 30.0, 1.0};

//glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteAmb);

glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteDir);
glMaterialfv(GL_FRONT, GL_SPECULAR, whiteDir);
glMaterialf(GL_FRONT, GL_SHININESS, 20.0);

glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDir);		// enable diffuse
glLightfv(GL_LIGHT0, GL_SPECULAR, whiteDir);	// enable specular
glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

glShadeModel( GL_SMOOTH );

/* Set up the textures
 comented out for this simple version
for (i=0; i<6; i++) {
makeSpots(i, &buf);
tex[i] = QGLWidget::convertToGLFormat( buf );  // flipped 32bit RGBA
}

glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
glEnable( GL_TEXTURE_2D );
*/

// Set up various other stuff
glClearColor( 0.5, 1.0, 0.75, 0.0 ); // Let OpenGL clear to black
glEnable( GL_CULL_FACE );  	// don't need Z testing for convex objects
glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

// No display list in this simple version
//object = makeDice( );	// Generate an OpenGL display list
}

void GLWidget::redraw()
{
        updateGL();
}

void GLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glLoadIdentity();
    gluLookAt(xfrom,yfrom,zfrom, xto, yto, zto, xlook, ylook, zlook);
   // glTranslatef( 0.0, 0.0, -10.0 );
   // glScalef( scale, scale, scale );

    glRotatef( xangle, 1.0, 0.0, 0.0 );
    glRotatef( yangle, 0.0, 1.0, 0.0 );
    glRotatef( zangle, 0.0, 0.0, 1.0 );

   // glCallList( object );   no display list this version just make the cube
    makeDice( );
    makeGround();
    drawPoints();



}

void GLWidget::makeGround(){
    glLineWidth(0.5);
    glColor3f(0,0,1);
    glBegin(GL_LINES);

    for( double i =-4; i< 4;i=i+0.5){
        glVertex3f(-4,-0.8,i);
        glVertex3f(4,-0.8,i);
        glVertex3f(i,-0.8,-4);
        glVertex3f(i,-0.8,4);
    }
    glEnd();

    glLineWidth(3);
    glColor3f(255,192,203);
    //draw x,y,z axis
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(2,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,2,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,2);

    glEnd();
}

/* 2D */
void GLWidget::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    if(testview){
        glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 1500.0 );
    }
    else{
        glOrtho(-4.0,4.0,-4,4,0.0,1000.0);
    }
    glMatrixMode( GL_MODELVIEW );
}

void GLWidget::about()
{
    QString vnum;
    QString mess, notes;
    QString title="QtOpenGl-2 ";

    vnum.setNum (version );
    mess="Simple OpenGl and Qt by Brian Wyvill Release Version: ";
    mess = mess+vnum;
    notes = "\n\n News: No News.";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}

void GLWidget::help()
{
    QString vnum;
    QString mess, notes;
    QString title="Cube ";

    vnum.setNum ( version );
    mess="Simple Interface to openGl and Qt by Brian Wyvill Release Version: ";
    mess = mess+vnum;
    notes = "\nThis version driven from the GLwidget. \n \
            Draws a cube, allows crude camera control and Euler angle rotations \n   ";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}




void GLWidget::initLight()
{
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 128.0 };
   //   GLfloat light_position[] = { 0.3, 0.3, 0.8, 0.0 };
   GLfloat light_position[] = { 0.2, 0.2, 0.9, 0.0 };
   GLfloat light_ambient[] = { 0.05, 0.05, 0.05, 1.0 };
//   GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat light_specular[] = { 0.99, 0.99, 0.99, 1.0 };
   GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };

//   glClearColor(1.0, 1.0, 0.9, 1.0);
     glShadeModel (GL_SMOOTH);
   //   glShadeModel (GL_FLAT);

// MATERIAL
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

// LIGHT0
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   //   glEnable(GL_DEPTH_TEST);
   //   glDisable(GL_LIGHTING);

}

GLuint GLWidget::makeDice( )
{
    GLuint list;
    float w = 0.8;

    //  list = glGenLists( 1 );
    //   glNewList( list, GL_COMPILE );   no display list this version
    // one
    drawFace(0,  w);

    // six
    glPushMatrix();
    glRotatef( 180.0, 1.0, 0.0, 0.0 );
    drawFace(5, w);
    glPopMatrix();

    // four on left
    glPushMatrix();
    glRotatef( -90.0, 0.0, 1.0, 0.0 );
    drawFace(3, w);
    glPopMatrix();

    // three on right
    glPushMatrix();
    glRotatef( 90.0, 0.0, 1.0, 0.0 );
    drawFace(2, w);
    glPopMatrix();

    // two
    glPushMatrix();
    glRotatef( 90.0, 1.0, 0.0, 0.0 );
    drawFace(1, w);
    glPopMatrix();

    // five
    glPushMatrix();
    glRotatef( -90.0, 1.0, 0.0, 0.0 );
    drawFace(4, w);
    glPopMatrix();



  //  glEndList();

  //  return list;
    return 0;
}

void GLWidget::drawFace(int tim, float w)
{
    // this version no texturing
 //   glTexImage2D( GL_TEXTURE_2D, 0, 3, tex[tim].width(), tex[tim].height(), 0,
           //       GL_RGBA, GL_UNSIGNED_BYTE, tex[tim].bits() );
     glLineWidth(3);
     glColor3f(0,0,0);
    if (filled) glBegin( GL_POLYGON ); else glBegin( GL_LINE_LOOP );
   // glTexCoord2f(0.0, 0.0);
    glVertex3f(  -w,  -w, w );
   // glTexCoord2f(0.0, 1.0);
    glVertex3f(   w,  -w, w );
    //glTexCoord2f(1.0, 1.0);
    glVertex3f(   w,   w, w );
    //glTexCoord2f(1.0, 0.0);
    glVertex3f(  -w,   w, w );
    glEnd();

}


void GLWidget::makeSpots(int tim, QImage *buf)
{
  int r=255, g=0, b=0;
  int rad=25;
  int w,h,i,j;

  w=buf->width();
  h=buf->height();

  // set red
  for (i=0; i<buf->width(); i++)
    for (j=0; j<buf->height(); j++)
      buf->setPixel(i,j, qRgb(r, g, b));

  switch(tim) {
  case 0: // value 1
    drawCircle(rad, w/2, h/2, buf);
    break;

  case 1: // value 2
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;


  case 2: // value 3
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/2, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;

  case 3: // value 4
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, h/4, buf);
    break;

  case 4: // value 5
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, 3*h/4, buf);
    drawCircle(rad, w/2, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, h/4, buf);
    break;

  case 5: // value 6
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, h/2, buf);
    drawCircle(rad, w/4, 3*h/4, buf);

    drawCircle(rad, 3*w/4, h/4, buf);
    drawCircle(rad, 3*w/4, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;

  default: std::cerr << " big oopsy \n";
    break;

  }
}

void GLWidget::drawCircle(int radius, int xcen, int ycen,  QImage *buf)
{
  int i,j,r2;

  r2=radius*radius;

  for(i=xcen-radius; i<xcen+radius; i++)
    for(j=ycen-radius; j<ycen+radius; j++) {
      if  ( (i-xcen)*(i-xcen) + (j-ycen)*(j-ycen) < r2)
        buf->setPixel(i,j,qRgb(255, 255, 255));
   }
}

// communication with the window widget
void GLWidget::rotx(int a)
{
        xangle =  (double)a;
//	std::cerr << " x angle "<<xangle<<"\n";
        updateGL();
}
void GLWidget::roty(int a)
{
        yangle =  (double)a;
        updateGL();
}

void GLWidget::rotz(int a)
{
        zangle =  (double)a;
        updateGL();
}


void GLWidget::setxFrom(int a)
{
    xfrom=a;
    updateGL();
}

void GLWidget::setyFrom(int a)
{
    yfrom=a;
    updateGL();
}
void GLWidget::setzFrom(int a)
{
    zfrom=a;
    updateGL();
}
//change the viewpoint(camera), there are six choices
void GLWidget::viewPoint(){
    pointnumber++;
    switch(pointnumber){
    case 0:
        xto = 0;
        yto = 0;
        zto = 0;
        break;
    case 1:
        xto = 0.8;
        yto = 0.8;
        zto = 0.8;
        break;
    case 2:
        xto = 0.8;
        yto = -0.8;
        zto = 0;
        break;
    case 3:
        xto = -0.8;
        yto = 0;
        zto = 0;
        break;
    case 4:
        xto = 0;
        yto = 0.8;
        zto = 0.8;
        break;
    case 5:
        xto = 0.8;
        yto = 0;
        zto = 0;
        break;

    default:
        pointnumber = 0;
        viewPoint();//recall the function
        break;
    }


}

void GLWidget::drawPoints(){
    glPointSize(10.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for(int i=0;i<pointlistX.size();i++){
        glVertex3f(pointlistX[i],pointlistY[i],pointlistZ[i]);

    }
    glEnd( );

}
// mouse routines for camera control to be implemented
void GLWidget::mousePressEvent( QMouseEvent *e )

{


 /*   if (df->getPan()) dopan(e->x(), height()-e->y() , true);
    else {*/
     button =  e->buttons();
    if(testview){
     // left button held and mouse moves left-right controls azimuth angle and up-down controls elevation.
        if (button == Qt::LeftButton){
            mClickLocationX = e->x();//test the mouse click location
            mClickLocationY = e->y();
        }
        //right button could control the position of the camera along the line from the camera to the "View To" point
        else if(button == Qt::RightButton){
            mClickLocationX = e->x();
            mClickLocationY = e->y();
        }
        //midButton for change viewpoint
        else if(button == Qt::MidButton){
            viewPoint();
        }
        updateGL();
        }
    else{
        if(button == Qt::RightButton){
          switch(statenumber){
            case 1:
              pointlistX.append((e->x()-283)/71.25);
              pointlistY.append(0);
              pointlistZ.append((e->y()-252)/62.5);
              break;
            case 2:
              pointlistX.append((e->x()-283)/71.25);
              pointlistY.append(-(e->y()-252)/62.5);
              pointlistZ.append(0);
              break;
            case 3:
              pointlistX.append(0);
              pointlistY.append(-(e->y()-252)/62.5);
              pointlistZ.append(-(e->x()-283)/71.25);
              break;
            default:

              break;
            }
        }
        updateGL();
    }
}

void GLWidget::mouseReleaseEvent( QMouseEvent *e)
{

    updateGL();
}


void GLWidget::mouseMoveEvent ( QMouseEvent *e )
{

    button =  e->buttons();
    int mouseX = e->x();//test mouse location (X) after movement
    int mouseY = e->y();
    if(testview){
        if ( button == Qt::LeftButton){
            angleOfY = angleOfY + (mouseY - mClickLocationY)/50.0;
            angleOfX = angleOfX + (mouseX - mClickLocationX)/50.0;
            yfrom = sin(angleOfY) * radius;
            xfrom = (cos(angleOfY ) * radius) * cos(angleOfX);
            zfrom = (cos(angleOfY ) * radius) * sin(angleOfX);
            mClickLocationX = mouseX;
            mClickLocationY = mouseY;
            updateGL();

        }else if(button == Qt::RightButton)
        {
            //not changing the angle
            radius = radius+(mClickLocationY - mouseY)/50.0;
            yfrom = sin(angleOfY) * radius;
            xfrom = (cos(angleOfY ) * radius) * cos(angleOfX);
            zfrom = (cos(angleOfY ) * radius) * sin(angleOfX);
            mClickLocationX = mouseX;
            mClickLocationY = mouseY;
            updateGL();

        }
    }
    else{



        updateGL();
    }
    //update mouse click location

}


void GLWidget::setFilled(bool a)
{
    filled=a;
    updateGL();
}
void GLWidget::topview(){
    statenumber=1;
    testview=false;
    xlook = 0.0;
    ylook = 0.0;
    zlook = -1.0;
    xfrom = 0.0;
    yfrom = 10.0;
    zfrom = 0.0;
    resizeGL(794,603);
    updateGL();

}
void GLWidget::frontview(){
    statenumber=2;
    testview=false;
    xlook=0.0;
    ylook=1.0;
    zlook=0.0;
    xfrom = 0.0;
    yfrom = 0.0;
    zfrom = 10.0;
    resizeGL(794,603);
    updateGL();
}
void GLWidget::rightview(){
    statenumber=3;
    testview=false;
    xlook=0.0;
    ylook=1.0;
    zlook=0.0;
    xfrom = 10.0;
    yfrom = 0.0;
    zfrom = 0.0;
    resizeGL(794,603);
    updateGL();
}
void GLWidget::presPective(){
    statenumber=4;
    testview=true;
    xlook=0.0;
    ylook=1.0;
    zlook=0.0;
    xfrom = 5.0;
    yfrom = 5.0;
    zfrom = 5.0;
    resizeGL(794,603);
    updateGL();
}
