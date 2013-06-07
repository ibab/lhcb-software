
//-----------------------------------------------------------------------------
/** @file HitExtension.h
 *
 *  Header file for track find class Tf::HitExtension
 *
 *  CVS Log :-
 *  $Id: HitExtension.h,v 1.3 2007-08-20 11:07:07 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_HITEXTENSION_H 
#define TFKERNEL_HITEXTENSION_H 1

#include <stddef.h>

namespace Tf
{

  /** @class HitExtension
   *  Base class from which which algorithm specific hit classes can be derived.
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   **/

  template <class Hit>
  class HitExtension
  {

  public:

    /// Constructor from pointer to a Hit
    HitExtension( const Hit* hit = 0 ) : m_hit(hit)  { }

    /// Constructor from reference to a Hit
    HitExtension( const Hit& hit     ) : m_hit(&hit) { }

    /// Destructor
    ~HitExtension() { }

    /// Access to the underlying common Hit object
    inline const Hit* hit()        { return m_hit; }

    /// Access to the underlying common Hit object
    inline const Hit* hit() const  { return m_hit; }

  protected:

    const Hit* m_hit; ///< Pointer to the underlying hit object

  };

}

#endif // TFKERNEL_HITEXTENSION_H
