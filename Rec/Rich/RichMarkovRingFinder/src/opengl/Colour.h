
#ifndef LESTER_COLOUR_H
#define LESTER_COLOUR_H

#include "opengl/Colour.fwd"

#include "opengl/OpenGLRelatedDefinitions.h"
#include "opengl/Drawable.h"
#include "opengl/Canvas.h"

namespace Lester {

  class Colour : public StreamCanvasTools::Drawable {
  public:
    virtual void drawOn(Lester::Canvas * canvas) const {
      issue();
    };
    float getRed() const {
      return _r;
    };
    float getGreen() const {
      return _g;
    };
    float getBlue() const {
      return _b;
    };
  private:
    float _r;
    float _g;
    float _b;
  public:
    void changeTo(const float r,
                  const float g,
                  const float b) {
      _r=r;
      _g=g;
      _b=b;
    };
  public:
    static void issue(const float r,
		      const float g,
		      const float b) {
      glColor3f(r,g,b);
    };
    void issue() const {
#ifdef LESTER_HAVE_GL
      glColor3f(_r,_g,_b);
#endif
    };
    Colour(const float r,
           const float g,
           const float b) :
      _r(r),
      _g(g),
      _b(b) {
    };
  public:
    inline static const Colour kRainbow(double r) {
      if(r>1) {
	r=1;
      } else if (r<0) {
	r=0;
      };
      return kCyclicRainbow(r);
    };
    inline static const Colour kCyclicRainbow(double r) {
      const double red[2]={-0.5,-sqrt(3.)/2};
      const double green[2]={1.0,0};      
      const double blue[2]={-0.5,sqrt(3.)/2};
      const double theta = r*2.0*3.14159;
      const double x=cos(theta);
      const double y=sin(theta);
      const double rx=(red[0]-x);
      const double ry=(red[1]-y);
      const double gx=(green[0]-x);
      const double gy=(green[1]-y);
      const double bx=(blue[0]-x);
      const double by=(blue[1]-y);
      double rr=sqrt(rx*rx+ry*ry);
      double gg=sqrt(gx*gx+gy*gy);
      double bb=sqrt(rx*bx+by*by);
      const double mag=sqrt(rr*rr+gg*gg+bb*gg);
      double rrr=rr/mag;
      double ggg=gg/mag;
      double bbb=bb/mag;
      if (rrr>1) { rrr=1; };
      if (ggg>1) { ggg=1; };
      if (bbb>1) { bbb=1; };
      return Colour(rrr,ggg,bbb);
    };
    inline static const Colour kWhite() { return Colour(1,1,1); };
    inline static const Colour kBlack() { return Colour(0,0,0); };

    inline static const Colour kRed() { return Colour(1,0,0); };
    inline static const Colour kGreen() { return Colour(0,1,0); };
    inline static const Colour kBlue() { return Colour(0,0,1); };

    inline static const Colour kYellow() { return Colour(1,1,0); };
    inline static const Colour kMagenta() { return Colour(1,0,1); };
    inline static const Colour kCyan() { return Colour(0,1,1); };
  };
};

#endif
