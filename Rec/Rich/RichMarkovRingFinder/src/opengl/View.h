#ifndef STREAMCANVASTOOLS_VIEW_H
#define STREAMCANVASTOOLS_VIEW_H

namespace Lester {
  namespace StreamCanvasTools {

    class View {
    public:
      virtual void rotateView(const double theta) = 0;
      virtual void rotateUp(const double theta) = 0;
      virtual void rotateLeft(const double theta) = 0;
      virtual void moveForward(const double amount) = 0;
      virtual void moveUp(const double amount) = 0;
      virtual void moveRight(const double amount) = 0;
      virtual void zoomIn(const double logZoomFactor) = 0;
      virtual void flushViewTo(Lester::Canvas * canvas, bool drawLookPoint=true) const = 0;
      virtual ~View() {};
    };
  };
};

#endif
