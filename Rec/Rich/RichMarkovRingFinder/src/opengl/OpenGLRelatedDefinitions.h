
#ifndef LESTER_OPENGLRELATEDDEFINITIONS_H
#define LESTER_OPENGLRELATEDDEFINITIONS_H

#ifndef WIN32
#define LESTER_HAVE_GL
#define LESTER_HAVE_GLX
#endif

#define XK_MISCELLANY

#ifdef LESTER_HAVE_GLX
#include <GL/glx.h>             /* this includes the necessary X headers */
#endif

#ifdef LESTER_HAVE_GL
#include <GL/glu.h>
#endif


#endif
