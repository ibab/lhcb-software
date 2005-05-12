#ifndef TrackEvent_TrackFunctor_H
#define TrackEvent_TrackFunctor_H 1

// Include files
// -------------
#include <functional>
#include "Event/State.h"
#include "Event/Track.h"

/** @namespace TrackFunctor
 *
 *  This namespace:
 * - contains functors (previously contained in associated classes)
 * - other small classes and things that do not fit (yet)
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-05
 *
 * @author Rutger van der Eijk
 *  created Thu Jun 20 14:47:34 2002
 *
 */

namespace TrackFunctor
{

//=============================================================================
// Class to test if the z of a class T is < than a certain value
//=============================================================================
  template <class T>
  class less_z: public std::unary_function<T*, bool> {
  private:
    double m_z;
  public:
    explicit less_z( double z ):m_z(z) {}
    bool operator()( const T* t ) const 
    {
      return t -> z() < m_z;
    }
  };
  
//=============================================================================
// Class to test if the z of a class T is > than a certain value
//=============================================================================
  template <class T>
  class greater_z: public std::unary_function<T*, bool> {
  private:
    double m_z;
  public:
    explicit greater_z( double z ):m_z(z) {}
    bool operator()( const T* t ) const 
    {
      return t -> z() > m_z;
    }
  };
  
//=============================================================================
// Class for sorting class T by z in order (+1/-1)
//=============================================================================
  template <class T>
  class orderByZ {
  private:
    int m_order;
  public:
    explicit orderByZ( int order = +1):m_order(order) {}
    bool operator()( const T* t1,
                     const T* t2 ) const
    {
      return (m_order)*t1->z() > (m_order)*t2->z();
    }
  };

//=============================================================================
// Class for sorting class T by increasing z
//=============================================================================
  template <class T>
  class increasingByZ {
  public:
    bool operator()( const T* t1,
                     const T* t2 ) const
    {
      return t1->z() < t2->z();
    }
  };

//=============================================================================
// Class for sorting class T by decreasing z
//=============================================================================
  template <class T>
  class decreasingByZ {
  public:
    bool operator()( const T* t1,
                     const T* t2 ) const
    {
      return t1->z() > t2->z();
    }
  };
  
}

#endif   /// TrackEvent_TrackFunctor_H
