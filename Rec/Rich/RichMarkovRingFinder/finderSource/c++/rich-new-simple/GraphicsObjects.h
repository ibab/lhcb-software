
#ifdef LESTER_USE_GRAPHICS

#ifndef LESTER_GRAPHICSOBJECTS_H
#define LESTER_GRAPHICSOBJECTS_H

// fwd dec
#include "GraphicsObjects.fwd" 

// includes
#include <iostream>
#include "opengl/Canvas.h"
#include "opengl/WarpableCanvas.h"
#include "opengl/Colour.h"
#include "opengl/BasicCanvasInterface.h"

// declaration
class GraphicsObjects {
// GraphicsObjects() {};
// GraphicsObjects(const GraphicsObjects & other) {};
// GraphicsObjects & operator=(const GraphicsObjects & other) { return *this; };
// virtual ~GraphicsObjects() {};
public:
  static Lester::Canvas * globalCanvas;
  static Lester::Canvas * globalCanvas2;
  static WarpableCanvas * wc;
  static WarpableCanvas * wc2;
};

#endif

#endif
