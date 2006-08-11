
#include "Rich.h"
#include "EventDescription.h"
#include "Data.h"
#include "Utils/LogarithmicTools.h"
#include "Constants.h"

#ifdef LESTER_USE_GRAPHICS
void Lester::Rich::draw(BasicCanvasInterface & canvas,
			const EventDescription & rp,
			const Data & d,
			const bool showTruth) {
  d.draw(canvas, showTruth);
  rp.draw(canvas);
};
#endif
