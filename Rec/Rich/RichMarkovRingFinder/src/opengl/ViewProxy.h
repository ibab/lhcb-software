#ifndef LESTER_STREAMCANVASTOOLS_VIEWPROXY_H
#define LESTER_STREAMCANVASTOOLS_VIEWPROXY_H

#include "opengl/View.h"

namespace Lester {
  namespace StreamCanvasTools {

  class ViewProxy : public View {
  public:
    ViewProxy(View & view, boost::recursive_mutex & mutex) : 
      m_view(view),
      m_mutex(mutex) {
    };
    virtual void rotateView(const double theta) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.rotateView(theta);
    };
    virtual void rotateUp(const double theta) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.rotateUp(theta);
    };
    virtual void rotateLeft(const double theta) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.rotateLeft(theta);
    };
    virtual void moveUp(const double amount) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.moveUp(amount);
    };
    virtual void moveRight(const double amount) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.moveRight(amount);
    };
    virtual void moveForward(const double amount) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.moveForward(amount);
    };
    virtual void zoomIn(const double logZoomFactor) {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.zoomIn(logZoomFactor);
    };
    virtual void flushViewTo(Lester::Canvas * canvas, bool drawLookPoint=true) const {
      boost::recursive_mutex::scoped_lock lk(m_mutex);
      m_view.flushViewTo(canvas, drawLookPoint);
    };
  private:
    View & m_view;
    boost::recursive_mutex & m_mutex;
  };

  };
};

#endif
