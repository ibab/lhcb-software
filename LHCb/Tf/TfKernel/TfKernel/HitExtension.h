#ifndef HITEXTENSION_H 
#define HITEXTENSION_H 1

#include <stddef.h>

namespace Tf
{

  /** @class HitExtension
   *  Base class from which which algorithm specific hit classes can be derived.
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template <class Hit>
  class HitExtension
  {

  public:

    /// Constructor from pointer
    HitExtension( const Hit* hit = 0 ) : m_hit(hit) { }

    /// Constructor from reference
    HitExtension( const Hit& hit ) : m_hit(&hit) { }

    ~HitExtension() { }

    const Hit* hit()        { return m_hit; }
    const Hit* hit() const  { return m_hit; }

  protected:

    const Hit* m_hit;

  };

}

#endif // HITEXTENSION_H
