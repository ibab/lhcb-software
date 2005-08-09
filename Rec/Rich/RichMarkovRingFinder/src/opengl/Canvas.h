
#ifndef LESTER_OPENGL_CANVAS_H
#define LESTER_OPENGL_CANVAS_H

#include "opengl/Canvas.fwd"

#include "opengl/OpenGLRelatedDefinitions.h"

#include <string>
#include <cmath>
#include <iostream>
#include <unistd.h>

#include "X11/keysymdef.h"
#include "opengl/BasicCanvasInterface.h"
#include "opengl/Colour.h"

namespace Lester {

  //extern int snglBuf[];
  //extern int dblBuf[];

  class Canvas : public BasicCanvasInterface {
  public:
#ifdef LESTER_HAVE_GL
    typedef ::GLfloat   GLfloat;
    typedef ::GLboolean GLboolean;
#else
    typedef double GLfloat;
    typedef bool GLboolean;
#define GL_TRUE true
#define GL_FALSE false
#endif

  private:
    int snglBuf[4];// = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
    int dblBuf[5];// = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
    std::ostream * m_eps;
    double m_epsScaleFactor;
  public:

    class FailException : public std::exception {
	    const std::string m_mess;
    public:
      FailException(const std::string & s) : m_mess(s) {
      };
      const std::string & message() const { return m_mess; };
      const char * what() const throw () {
	return m_mess.c_str();
      };
      ~FailException() throw() {
      };
    };
  private:
    GLboolean       doubleBuffer ;
#ifdef LESTER_HAVE_GLX
    Display * dpy;
    Window    win;
    GLXContext cx;
#endif
    GLfloat cursor_x;
    GLfloat cursor_y;
    GLfloat eye_x;
    GLfloat eye_y;
    GLfloat eye_z;
    GLfloat look_x;
    GLfloat look_y;
    GLfloat look_z;
    GLfloat up_x;
    GLfloat up_y;
    GLfloat up_z;
    GLfloat eye_to_near_clip_dist;
    GLfloat eye_to_far_clip_dist;
    GLfloat m_fieldOfViewDegrees;
    GLfloat m_aspectRatio;
  private:
    inline void privateCd() {
#ifdef LESTER_HAVE_GLX
      glXMakeCurrent(dpy, win, cx);
#endif
    };
  private:
    inline void initCursor() {
      moveTo(0,0);
    };
  public:
    inline void resetGaze() {
      eye_x=0;
      eye_y=0;
      eye_z=1;
      look_x=0;
      look_y=0;
      look_z=0;
      up_x=0;
      up_y=1;
      up_z=0;
      m_fieldOfViewDegrees=40;
      m_aspectRatio=1;
      eye_to_near_clip_dist=0.5;
      eye_to_far_clip_dist=20;
      doAGluLookAt();
    };
  private:
    void doAGluLookAt() {
      privateCd();


      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(m_fieldOfViewDegrees, 
		     m_aspectRatio, 
		     eye_to_near_clip_dist, 
		     eye_to_far_clip_dist);


      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(eye_x, eye_y, eye_z, 
		look_x, look_y, look_z, 
		up_x, up_y, up_z);
    };
  public:
    inline void lookAt(const GLfloat x,
		       const GLfloat y,
		       const GLfloat z) {
      look_x=x;
      look_y=y;
      look_z=z;
      doAGluLookAt();
    };
    inline void moveEyeTo(const GLfloat x,
			  const GLfloat y,
			  const GLfloat z) {
      eye_x=x;
      eye_y=y;
      eye_z=z;
      doAGluLookAt();
    };
    inline void defineUp(const GLfloat x,
			 const GLfloat y,
			 const GLfloat z) {
      up_x=x;
      up_y=y;
      up_z=z;
      doAGluLookAt();
    };
    inline void setAspectRatio(const GLfloat a) {
      m_aspectRatio=a;
      doAGluLookAt();
    };
    inline void setFieldOfViewDegrees(const GLfloat f) {
      m_fieldOfViewDegrees=f;
      doAGluLookAt();
    };
  public:
    inline void moveTo(const GLfloat x, const GLfloat y) {
      cursor_x = x;
      cursor_y = y;
    };
  public:
    inline void point(const GLfloat x, const GLfloat y) {;
    privateCd();
#ifdef LESTER_HAVE_GL
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
#endif
    };
    inline void point3D(const GLfloat x,
			const GLfloat y,
			const GLfloat z) {;
    privateCd();
#ifdef LESTER_HAVE_GL
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);
    glEnd();
#endif
    };
    inline void arc(const GLfloat centreX, const GLfloat centreY, 
                    const GLfloat radius, const GLfloat fromTheta, const GLfloat deltaTheta, const int facets = 50) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glBegin(GL_LINE_STRIP);
      for (int i=0; i<=facets; ++i) {
        const float theta = fromTheta + static_cast<double>(i)/static_cast<double>(facets)*deltaTheta;
        const float x = centreX + radius*cos(theta);
        const float y = centreY + radius*sin(theta);
        glVertex2f(x,y);
      };
      glEnd();
#endif
    };

    inline void circle(const GLfloat centreX, const GLfloat centreY,
                       const GLfloat radius, const int facets = 50) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glBegin(GL_LINE_LOOP);
      const double two=2;
      const double pi=3.14159;
      for (int i=0; i<facets; ++i) {
        const float theta = static_cast<double>(i)/static_cast<double>(facets)*two*pi;
        const float x = centreX + radius*cos(theta);
        const float y = centreY + radius*sin(theta);
        glVertex2f(x,y);
      };
      glEnd();
      if (m_eps) {
	(*m_eps) << centreX*m_epsScaleFactor << " " << centreY*m_epsScaleFactor << " " << radius*m_epsScaleFactor << " 0 360 arc closepath stroke\n";
      };
#endif
    };
  public:
    inline void triangle(const GLfloat x1, const GLfloat y1,
			 const GLfloat x2, const GLfloat y2,
			 const GLfloat x3, const GLfloat y3,
			 const bool filled=false) {
      privateCd();
#ifdef LESTER_HAVE_GL
      if (filled) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      } else {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      };
      glBegin(GL_POLYGON);
      glVertex2f(x1,y1);
      glVertex2f(x2,y2);
      glVertex2f(x3,y3);
      glEnd();
#endif
      moveTo(x3,y3);
    }; 

  public:
    inline void triangle3D(const GLfloat x1, const GLfloat y1, const GLfloat z1,
			   const GLfloat x2, const GLfloat y2, const GLfloat z2,
			   const GLfloat x3, const GLfloat y3, const GLfloat z3,
			   const bool filled=false,
			   const bool lines=true) {
      privateCd();
#ifdef LESTER_HAVE_GL
      /*
	GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  // Red diffuse light. 
      GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  // Infinite light location. 
      glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
      glEnable(GL_LIGHT0);
      glEnable(GL_LIGHTING);
      */
     
      /* Use depth buffering for hidden surface elimination. */
      glEnable(GL_DEPTH_TEST);
   
      if (filled && !lines) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glEnd();
      } else if (filled && lines) {

	glDisable (GL_LIGHTING);
 	glEnable (GL_POLYGON_OFFSET_EXT);
	//glPolygonOffsetEXT (1., 1./(float)0x10000);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glEnd();


	glEnable (GL_LIGHTING);
	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (1., 1.);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glEnd();


	
      } else if (!filled && lines) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glEnd();
      } else if (!filled && !lines) {
	// do nothing!
      };
      glBegin(GL_POLYGON);
      glVertex3f(x1, y1, z1);
      glVertex3f(x2, y2, z2);
      glVertex3f(x3, y3, z3);
      glEnd();
#endif
      moveTo(x3,y3);
    }; 

  public:
    inline void colour(const Colour & col) {
      col.issue();
      if (m_eps) {
	(*m_eps) << col.getRed() << " " << col.getGreen() << " " << col.getBlue() << " setrgbcolor\n";
      };
    };
  public:
    inline void line(const GLfloat x1, const GLfloat y1,
                     const GLfloat x2, const GLfloat y2) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glBegin(GL_LINES);
      glVertex2f(x1,y1);
      glVertex2f(x2,y2);
      glEnd();
#endif
      if (m_eps) {
	(*m_eps) << x1*m_epsScaleFactor << " " << y1*m_epsScaleFactor << " newpath moveto " << x2*m_epsScaleFactor << " " << y2*m_epsScaleFactor << " lineto stroke\n";
      };
      moveTo(x2,y2);
    };
    inline void line(const GLfloat x1, const GLfloat y1, const GLfloat z1,
                     const GLfloat x2, const GLfloat y2, const GLfloat z2) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glBegin(GL_LINES);
      glVertex3f(x1,y1,z1);
      glVertex3f(x2,y2,z2);
      glEnd();
#endif
      moveTo(x2,y2);
    };
  public:
    inline void box(const GLfloat x1, const GLfloat y1,
		    const GLfloat x2, const GLfloat y2
		    ,bool filled=false
		    ) {
      //std::cout << "box " << x1<<" " <<y1<<" " << x2<< " " <<y2<< std::endl;
      privateCd();
#ifdef LESTER_HAVE_GL
      if (!filled) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1,y1);
	glVertex2f(x2,y1);
	glVertex2f(x2,y2);
	glVertex2f(x1,y2);
	glEnd();
      } else {
	if (filled) {
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	};
	glBegin(GL_POLYGON);
	glVertex2f(x1,y1);
	glVertex2f(x2,y1);
	glVertex2f(x2,y2);
	glVertex2f(x1,y2);
	glEnd();
      };
#endif
    };
  public:
    inline void lineTo(const GLfloat x, const GLfloat y) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glBegin(GL_LINES);
      glVertex2f(cursor_x,cursor_y);
      glVertex2f(x,y);
      glEnd();
#endif
      moveTo(x,y);
    };
  public:
    inline void update() {
      privateCd();
      if(doubleBuffer) {
#ifdef LESTER_HAVE_GLX
        glXSwapBuffers(dpy, win); /* buffer swap does implicit glFlush */
#endif
      } else {
#ifdef LESTER_HAVE_GL
        glFlush(); /* explicit flush for single buffered case */
#endif
      };
    };
  public:
    inline void clear() {
      privateCd();
#ifdef LESTER_HAVE_GL
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    };
  public:
    inline void setOrthoProjection(const GLfloat left=-1,
                                   const GLfloat bottom=-1,
                                   const GLfloat right=1,
                                   const GLfloat top=1,
				   const GLfloat near=1000,
				   const GLfloat far=-1000) {
      privateCd();
#ifdef LESTER_HAVE_GL
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      //glOrtho describes a matrix that produces a parallel projection. (left, bottom, -near) and (right, top, -near) specify the points on the near clipping plane that are mapped to the lower left and upper right corners of the window, respectively, assuming that the eye is located at (0, 0, 0). -far specifies the location of the far clipping plane. Both near and far can be either positive or negative. 
      glOrtho(
              left,
              right,
              bottom,
              top,
              near, 
              far
	      );
     
      writeEpsHeader(left,bottom,right,top);
      
#endif
    };
    inline void setPerspectiveProjection(const GLdouble left=-1,
					 const GLdouble bottom=-1,
					 const GLdouble right=1,
					 const GLdouble top=1,
					 const GLdouble positiveDistanceToNearPlane=0.1,
					 const GLdouble positiveDistanceToFarPlane=1) {
      privateCd();
      assert(false);
#ifdef LESTER_HAVE_GL
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      //glFrustum describes a perspective matrix that produces a perspective projection. (left,bottom,-near) and (right,top,-near) specify the points on the near clipping plane that are mapped to the lower left and upper right corners of the window, respectively, assuming that the eye is located at (0, 0, 0). -far specifies the location of the far clipping plane. Both near and far must be positive.
      glFrustum(
		left,
		right,
		bottom,
		top,
		positiveDistanceToNearPlane,
		positiveDistanceToFarPlane);
#endif
    };
    inline void translateCamera(const GLfloat x,
				const GLfloat y,
				const GLfloat z) {
      privateCd();
      assert(false);
#ifdef LESTER_HAVE_GL
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(-x,-y,-z); // translatef actually translates objects, so this is why we negate the inputs!
#endif
    };
  private:
    inline void resetModelView() {
#ifdef LESTER_HAVE_GL
      glMatrixMode(GL_MODELVIEW);
      /* reset modelview matrix to the identity matrix */
      glLoadIdentity();
      /* move the camera back three units */
      //glTranslatef(0.0, 0.0, -3.0);
      /* rotate by X, Y, and Z angles */
      // glRotatef(xAngle, 0.1, 0.0, 0.0);
      //glRotatef(yAngle, 0.0, 0.1, 0.0);
      //glRotatef(zAngle, 0.0, 0.0, 1.0);
#endif
    };
  private:
    char * getDisplayFromArgs(int argc, char **argv) {
      for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "-display") {
          if ((i+1) >= argc) {
            // don't complain
          } else {
            return argv[i+1];
          }
        };
      };
      return 0;
    };
  private:
    void redraw(void) {
#ifdef LESTER_HAVE_GL
      static GLboolean   displayListInited = GL_FALSE;

      if (displayListInited) {
        /* if display list already exists, just execute it */
        glCallList(1);
      } else {
        /* otherwise compile and execute to create the display list */
        glNewList(1, GL_COMPILE_AND_EXECUTE);
        if (true) {
          resetModelView();
        }
        clear();
        /* front face */
        glBegin(GL_QUADS);
        glColor3f(0.0, 0.7, 0.1);       /* green */
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glVertex3f(-1.0, -1.0, 1.0);
        /* back face */
        glColor3f(0.9, 1.0, 0.0);       /* yellow */
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(1.0, -1.0, -1.0);
        glVertex3f(-1.0, -1.0, -1.0);
        /* top side face */
        glColor3f(0.2, 0.2, 1.0);       /* blue */
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(-1.0, 1.0, -1.0);
        /* bottom side face */
        glColor3f(0.7, 0.0, 0.1);       /* red */
        glVertex3f(-1.0, -1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glVertex3f(1.0, -1.0, -1.0);
        glVertex3f(-1.0, -1.0, -1.0);
        glEnd();
        glEndList();
        displayListInited = GL_TRUE;
      }
#endif
    }
  public:
    void resetAllGLX() {
      privateCd();
      resetGaze(); // sets model view to default value

      //setOrthoProjection();
#ifdef LESTER_HAVE_GL
      glClearColor(1.0, 1.0, 1.0, 1.0);
#endif
      Colour::kBlack().issue();
      clear();
    };
  private:
    static void onceOnlyConstructor() {
      static bool first=true;
      if (first) {
	first=false;

      };
    };
  private:
    void writeEpsTrailerAndClose() {
      if (m_eps) {
	(*m_eps) << "%%EOF\n" <<std::flush;
	m_eps = 0;
      };
    };
    void writeEpsHeader(const double left, const double bot,
			const double right, const double top) const {
      if (m_eps) {
	std::ostream & os = *m_eps;

	os << "%!PS-Adobe-2.0 EPSF-2.0\n" 
	   << "%%BoundingBox: " << left*m_epsScaleFactor << " " << bot*m_epsScaleFactor << " " << right*m_epsScaleFactor << " " << top*m_epsScaleFactor << "\n";
	const double hypot=sqrt((right-left)*(right-left)+(top-bot)*(top-bot));
	os << hypot*m_epsScaleFactor/1000. << " setlinewidth\n";
	//os << " 0    0    0.10  0 360 arc closepath stroke" << std::endl;
	//os << " 0.15 0.0  0.08  0 360 arc closepath stroke" << std::endl;
	//os << " 0.15 0.15 0.06  0 360 arc closepath stroke" << std::endl;
      };
    };
  public:
    void sendEpsTo(std::ostream * os, const double epsScaleFactor=1) {
      writeEpsTrailerAndClose();
      m_eps=os;
      m_epsScaleFactor=epsScaleFactor;
    };
  public:
    Canvas(int argc, char **argv,
           const std::string & title,
           const int width, const int height,
           const int xpos=-1, const int ypos=-1) :
      m_eps(0),
      doubleBuffer( GL_TRUE ) {
      onceOnlyConstructor();

      //std::cerr << "Making a canvas called " << title << " of width " << width << std::endl;
#ifdef LESTER_HAVE_GL
#ifdef LESTER_HAVE_GLX
      snglBuf[0] = GLX_RGBA;
      snglBuf[1] = GLX_DEPTH_SIZE;
      snglBuf[2] = 16;
      snglBuf[3] = None;

      dblBuf[0] = GLX_RGBA;
      dblBuf[1] = GLX_DEPTH_SIZE;
      dblBuf[2] = 16;
      dblBuf[3] = GLX_DOUBLEBUFFER;
      dblBuf[4] = None;

      XVisualInfo    *vi;
      Colormap        cmap;
      XSetWindowAttributes swa;
      int             dummy;

      /*** (1) open a connection to the X server ***/

      dpy = XOpenDisplay(getDisplayFromArgs(argc,argv));
      //std::cerr << "Will use display " << dpy << std::endl;
      if (dpy == NULL) {
        throw FailException("could not open display");
      };
      /*** (2) make sure OpenGL's GLX extension supported ***/

      if(!glXQueryExtension(dpy, &dummy, &dummy)) {
        throw FailException("X server has no OpenGL GLX extension");
      };
      /*** (3) find an appropriate visual ***/

      /* find an OpenGL-capable RGB visual with depth buffer */
      vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
      if (vi == NULL) {
        vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
        if (vi == NULL) {
          throw FailException("no RGB visual with depth buffer");
        };
        doubleBuffer = GL_FALSE;
      }
      //if(vi->class != TrueColor) fatalError("TrueColor visual required for this program");


      /*** (4) create an OpenGL rendering context  ***/

      /* create an OpenGL rendering context */
      cx = glXCreateContext(dpy, vi, /* no sharing of display lists */ None,
                            /* direct rendering if possible */ GL_TRUE);
      if (cx == NULL) {
        throw FailException("could not create rendering context");
      };
      /*** (5) create an X window with the selected visual ***/

      /* create an X colormap since probably not using default visual */
      cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
      swa.colormap = cmap;
      swa.border_pixel = 0;
      swa.event_mask = //ExposureMask | StructureNotifyMask |
	KeyPressMask | ButtonPressMask | ExposureMask ;
      win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height, 0, vi->depth,
                          InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
      XSetStandardProperties(dpy, win, title.c_str(), title.c_str(), None, argv, argc, NULL);

      /*** (6) bind the rendering context to the window ***/

      glXMakeCurrent(dpy, win, cx);

      /*** (7) request the X window to be displayed on the screen ***/

      XMapWindow(dpy, win);

      if (xpos>=0 && ypos>=0) {
        XMoveWindow(dpy, win, xpos, ypos);
      };

      /*** (8) configure the OpenGL context for rendering ***/

      glEnable(GL_DEPTH_TEST); /* enable depth buffering */
      glDepthFunc(GL_LESS
		  );    /* pedantic, GL_LESS is the default */
      glClearDepth(1.0);       /* pedantic, 1.0 is the default */
      glViewport(0, 0, width, height); /* pedantic, full window size is default viewport anyway */


      // sets my lester cursor:
      initCursor();

      // prepare for drawing:
      resetAllGLX();
#endif
#endif
    }


  protected:

    /// The following method is called whenever a key is pressed over the window.
    virtual void keyPress(const KeySym c) {
    };

  protected:
    bool process_input() {
      XEvent event;
      bool redraw = false;
      static int mstate, omx, omy, mx, my;
      
      while(XPending(dpy)) {
	char buf[31];
	KeySym keysym;
	XNextEvent(dpy, &event);
	std::cout << "Something is happening..." << std::endl;
	switch(event.type) {
	case Expose:
	  std::cout << "Expose" << std::endl;
	  redraw = true;
	  break;
	case ConfigureNotify:
	  std::cout << "ConfigureNotify" << std::endl;
	  //glViewport(0, 0, event.xconfigure.width,
	  //	     event.xconfigure.height);
	  redraw = true;
	  break;
	case KeyPress:
	  std::cout << "KeyPress" << std::endl;
	  (void) XLookupString(&event.xkey, buf, sizeof(buf),
			       &keysym, NULL);
	  {
	    char f=keysym;
	    keyPress(keysym);
	    std::cout << "keysym is [" << f << "] which has number " << keysym << std::endl;
	  };
	  switch (keysym) {
	  case XK_Escape:
	    std::cout << "Exiting with success!!!" << std::endl;
	    exit(EXIT_SUCCESS);
	  default:
	    break;
	  }
	case KeyRelease:
	  std::cout << "KeyRelease" << std::endl;
	  (void) XLookupString(&event.xkey, buf, sizeof(buf),
			       &keysym, NULL);
	  {
	    char f=keysym;
	    std::cout << "keysym is [" << f << "] which has number " << keysym << std::endl;
	  };
	  switch (keysym) {
	  case XK_Escape:
	    std::cout << "Exiting with success!!!" << std::endl;
	    exit(EXIT_SUCCESS);
	  default:
	    break;
	  }
	case ButtonPress:
	  std::cout << "ButtonPress" << std::endl;
	  if (event.xbutton.button == Button2) {
	    mstate |= 2;
	    mx = event.xbutton.x;
	    my = event.xbutton.y;
	  } else if (event.xbutton.button == Button1) {
	    mstate |= 1;
	    mx = event.xbutton.x;
	    my = event.xbutton.y;
	  }
	  break;
	case ButtonRelease:
	  std::cout << "ButtonRelease" << std::endl;
	  if (event.xbutton.button == Button2)
	    mstate &= ~2;
	  else if (event.xbutton.button == Button1)
	    mstate &= ~1;
	  break;
	case MotionNotify:
	  std::cout << "MotionNotify" << std::endl;
	  if (mstate) {
	    omx = mx;
	    omy = my;
	    mx = event.xbutton.x;
	    my = event.xbutton.y;
	    //redraw = true;
	  }
	  break;
	default:
	  std::cout << "Either found no input waiting, or only unrecognised input" << std::endl;
	  break;
	}
      };// while (XPending(dpy));
      //std::cout << "leaving process_input" << std::endl;
      return redraw;
    }
  public:
    virtual ~Canvas() {};
    
  }; // end of class

  
} // end of namespace Lester


#endif
