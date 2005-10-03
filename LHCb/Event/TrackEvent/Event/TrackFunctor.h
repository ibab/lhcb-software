#ifndef TrackEvent_TrackFunctor_H
#define TrackEvent_TrackFunctor_H 1

// Include files
// -------------
#include <stdio.h>
#include <functional>
#include "Event/Track.h"

/** @namespace TrackFunctor
 *
 *  This namespace:
 * - contains functors (previously contained in associated classes)
 * - other small classes and things that do not fit (yet)
 *
 *  @author Jose A. Hernando
 *  @author Eduardo Rodrigues (adaptations)
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
// select T object close to z position
//=============================================================================
  template <class T>
  class closestToZ {
  private:
    double m_z0;
  public:
    explicit closestToZ( double z0 = 0.):m_z0(z0) {}
    bool operator()( const T* t1,
                     const T* t2 ) const
    {
      return ( (fabs(t1->z()-m_z0) < fabs(t2->z()-m_z0)) );
    }
  };

//=============================================================================
// select T object close to plane position
//=============================================================================
  template <class T>
  class closestToPlane {
  private:
    HepPlane3D m_plane;
  public:
    explicit closestToPlane(const HepPlane3D& plane):m_plane(plane) {}
    bool operator()( const T* t1,
                     const T* t2 ) const
    {
      double d1 = fabs(m_plane.distance(t1->position()));
      double d2 = fabs(m_plane.distance(t2->position()));
      return (d1 < d2);
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
      return (m_order)*t1->z() < (m_order)*t2->z();
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

  template <class T>
  class HasKey: public std::unary_function<T*, bool> {
  public:
    // A predicate (unary bool function):
    // example:
    // HasKey<Track> isBackward(&Track::checkFlag,TrackKeys::Backwards)
    // if (isBackward(track)) ...
    typedef bool (T::* ptr_memfun) (unsigned int) const;
  private:
    ptr_memfun m_pmf;
    unsigned int m_key;
  public:
    explicit HasKey(ptr_memfun check, unsigned int key ):
      m_pmf(check),m_key(key) {}
    bool operator()( const T* t ) const 
    {
      return (t ->* m_pmf) (m_key);
    }
  };  

  template <class T>
  void deleteFromList(std::vector<T*>& List, T* value) 
  {
    typename std::vector<T*>::iterator it;
    it = std::remove(List.begin(), List.end(), value );
    delete *it;
    List.erase( it, List.end() );
  }

  // Counts how many Measurements fulfill the predicate
  // Make a predicate e.g. using the HasKey template
  template <class T>
  unsigned int nMeasurements(const Track& track, T pred) 
  {
    const std::vector<Measurement*>& meas = track.measurements();
    return std::count_if(meas.begin(),meas.end(),pred);
  }

  // Counts how many LHCbIDs fulfill the predicate
  // Make a predicate e.g. using the HasKey template
  template <class T>
  unsigned int nLHCbIDs(const Track& track, T pred) 
    {
    const std::vector<LHCbID>& ids = track.lhcbIDs();
    return std::count_if(ids.begin(),ids.end(),pred);
  }
  
}

#endif   /// TrackEvent_TrackFunctor_H
