
#ifndef LESTER_STREAMCANVASTOOLS_BUFFER_H
#define LESTER_STREAMCANVASTOOLS_BUFFER_H

#include "opengl/BufferInterface.h"
#include "opengl/Drawable.h"
#include "opengl/Canvas.h"

namespace Lester {
  namespace StreamCanvasTools {

    class Buffer : virtual public BufferInterface {
    public:
      virtual void clear() {
	boost::recursive_mutex::scoped_lock lock(m_drawables_mutex);
	m_drawables.clear();
      };
      virtual void add(boost::shared_ptr<Drawable> d) {
	boost::recursive_mutex::scoped_lock lock(m_drawables_mutex);
	m_drawables.push_back(d);
      };
      virtual void flushDrawablesToCanvas(Lester::Canvas * canvas) const {
	boost::recursive_mutex::scoped_lock lock(m_drawables_mutex);	
	for (Drawables::const_iterator it=m_drawables.begin();
	     it!=m_drawables.end();
	     ++it) {
	  (**it).drawOn(canvas);
	};
      };
    private:
      typedef std::list<boost::shared_ptr<Drawable> > Drawables;
      Drawables m_drawables;
      mutable boost::recursive_mutex m_drawables_mutex;
    public:
      Buffer() {
      };
      Buffer(const Buffer & other) :
	m_drawables(other.m_drawables) {
      };
      void operator=(const Buffer & other) {
	m_drawables = other.m_drawables;
      };
    };

  };
};

#endif
