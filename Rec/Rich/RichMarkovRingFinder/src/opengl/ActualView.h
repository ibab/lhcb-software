#ifndef LESTER_STREAMCANVASTOOLS_ACTUALVIEW_H
#define LESTER_STREAMCANVASTOOLS_ACTUALVIEW_H

#include "opengl/View.h"

namespace Lester {
  namespace StreamCanvasTools {

  class ActualView : public View {
  public:
    ActualView() :
      m_lookAt(Hep3Vector(0,0,0)),
      m_eyeAt(Hep3Vector(0,0,+3)),
      m_up(Hep3Vector(0,1,0)),
      m_viewAngleDegrees(40) {
      m_up = m_up.unit();
    };
  public:
    void flushViewTo(Lester::Canvas * canvas, bool drawLookPoint=true) const {
      std::cout << "Telling canvas about the view coords: eye " << m_eyeAt << " look " << m_lookAt << std::endl;
      canvas->moveEyeTo(m_eyeAt.x(), m_eyeAt.y(), m_eyeAt.z());
      canvas->lookAt(m_lookAt.x(), m_lookAt.y(), m_lookAt.z());
      canvas->defineUp(m_up.x(), m_up.y(), m_up.z());
      canvas->setFieldOfViewDegrees(m_viewAngleDegrees);
      if (drawLookPoint) {
	const double small = 0.03;
	const double delta=(eyeToObj().mag())*m_viewAngleDegrees/180.*3.14159*small;
	canvas->line(m_lookAt.x()-delta,
		     m_lookAt.y(),
		     m_lookAt.z(),
		     m_lookAt.x()+delta,
		     m_lookAt.y(),
		     m_lookAt.z());
	canvas->line(m_lookAt.x(),
		     m_lookAt.y()-delta,
		     m_lookAt.z(),
		     m_lookAt.x(),
		     m_lookAt.y()+delta,
		     m_lookAt.z());
	canvas->line(m_lookAt.x(),
		     m_lookAt.y(),
		     m_lookAt.z()-delta,
		     m_lookAt.x(),
		     m_lookAt.y(),
		     m_lookAt.z()+delta);
      };
    };
    void translate(const Hep3Vector & vec) {
      m_eyeAt += vec;
      m_lookAt += vec;
    };
    virtual void moveUp(const double amount) {
      translate(    up()*eyeToObj().mag()*amount );
    };
    virtual void moveRight(const double amount) {
      translate( right()*eyeToObj().mag()*amount );
    };
    virtual void moveForward(const double amount) {
      translate(         eyeToObj()      *amount );
    };
    virtual void rotateView(const double theta) {
      m_up = (m_up.rotate(theta, eyeToObj())).unit();
    };
    virtual void rotateUp(const double theta) {
      const Hep3Vector originalRight=right();
      m_eyeAt = m_lookAt + objToEye().rotate(-theta, originalRight);
      m_up = (originalRight.cross(eyeToObj())).unit();
    };
    virtual void rotateLeft(const double theta) {
      m_eyeAt = m_lookAt + objToEye().rotate(-theta, up());
      // don't have to modify up !
    };
    virtual void zoomIn(const double logZoomFactor) {
      m_eyeAt = m_lookAt+(objToEye())*exp(-logZoomFactor);
    };
  private:
    Hep3Vector eyeToObj() const {
      return m_lookAt-m_eyeAt;
    };
    Hep3Vector objToEye() const {
      return m_eyeAt-m_lookAt;
    };
    Hep3Vector up() const {
      return m_up;
    };
    Hep3Vector right() const {
      return (eyeToObj().cross(up())).unit();
    };
  private:
    Hep3Vector m_lookAt;
    Hep3Vector m_eyeAt;
    Hep3Vector m_up;
    double m_viewAngleDegrees;
  };

  };
};

#endif
