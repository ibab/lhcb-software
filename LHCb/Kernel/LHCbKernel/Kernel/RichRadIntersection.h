
//-----------------------------------------------------------------------------
/** @file RichRadIntersection.h
 *
 *  Header file for RICH utility class : RichRadIntersection
 *
 *  CVS Log :-
 *  $Id: RichRadIntersection.h,v 1.1 2006-10-20 12:53:50 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2006-02-28
 */
//-----------------------------------------------------------------------------

#ifndef KERNEL_RichRadIntersection_H
#define KERNEL_RichRadIntersection_H 1

// STL
#include <functional>

// Include files
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// DetDesc
class DeRichRadiator;
//#include "RichDet/DeRichRadiator.h"

/** @class RichRadIntersection RichRadIntersection.h Kernel/RichRadIntersection.h
 *
 *  RICH Radiator intersection information.
 *
 *  Contains the entry and exit points of a single intersection with a radiator
 *  volume, together with the momentum vector at the intersection points, and
 *  a pointer to the intersected radiator volume.
 *
 *  @author Antonis Papanestis
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2006-02-28
 */

class RichRadIntersection
{

public:

  /// Definition of a vector of intersections
  typedef std::vector<RichRadIntersection> Vector;

public:

  /// Standard constructor
  RichRadIntersection( ) : m_radiator ( NULL ) { }

  /// Constructor from entry and exit points and momentum vectors, and a pointer to a DeRichRadiator
  RichRadIntersection( const Gaudi::XYZPoint  & entryPoint, ///< The radiator entry point
                       const Gaudi::XYZVector & entryVect,  ///< The direction vector at the entry point
                       const Gaudi::XYZPoint  & exitPoint,  ///< The radiator exit point
                       const Gaudi::XYZVector & exitVect,   ///< The direction vector at the exit point
                       const DeRichRadiator   * rad         ///< Pointer to the intersected radiator volume
                       ) :
    m_entryPoint ( entryPoint ),
    m_entryVect  ( entryVect  ),
    m_exitPoint  ( exitPoint  ),
    m_exitVect   ( exitVect   ),
    m_radiator   ( rad        ) { }

  ~RichRadIntersection( ) { } ///< Destructor

  /// Set the entry point to the radiator
  inline void setEntryPoint ( const Gaudi::XYZPoint & entryPoint )
  {
    m_entryPoint = entryPoint;
  }

  /// Get the entry point to the radiator
  inline const Gaudi::XYZPoint & entryPoint () const
  {
    return m_entryPoint;
  }

  /// Set the entry momentum vector to the radiator
  inline void setEntryMomentum ( const Gaudi::XYZVector & entryV )
  {
    m_entryVect = entryV;
  }

  /// Get the entry point to the radiator
  inline const Gaudi::XYZVector & entryMomentum () const
  {
    return m_entryVect;
  }

  /// Set the exit point of the radiator
  inline void setExitPoint ( const Gaudi::XYZPoint & exitPoint )
  {
    m_exitPoint = exitPoint;
  }

  /// Get the exit point to the radiator
  inline const Gaudi::XYZPoint & exitPoint () const
  {
    return m_exitPoint;
  }

  /// Set the exit momentum vector to the radiator
  inline void setExitMomentum ( const Gaudi::XYZVector & exitV )
  {
    m_exitVect = exitV;
  }

  /// Get the entry point to the radiator
  inline const Gaudi::XYZVector & exitMomentum () const
  {
    return m_exitVect;
  }

  /// Set pointer to associated radiator detector element
  inline void setRadiator ( const DeRichRadiator* radiator )
  {
    m_radiator = radiator;
  }

  /// Get pointer to associated radiator detector element
  inline const DeRichRadiator* radiator () const
  {
    return m_radiator;
  }

  /// Returns the path length (squared) in the given radiator
  inline double pathLength2() const
  {
    return (exitPoint()-entryPoint()).Mag2();
  }

  /// Returns the path length (squared) in the given radiator
  inline double pathLength() const
  {
    return sqrt( pathLength2() );
  }

  /// Overload output to ostream
  friend inline std::ostream& operator << ( std::ostream& os, 
                                            const RichRadIntersection & intersect )
  { 
    os << "[" 
      //<< " " << intersect.radiator()->radiatorID()
       << " Entry: Point=" << intersect.entryPoint() 
       << " Dir=" << intersect.entryMomentum() 
       << " Exit: Point=" << intersect.exitPoint()
       << " Dir=" << intersect.exitMomentum()
       << " ]";
    return os;
  }
  
private:

  /// Entry point for radiator
  Gaudi::XYZPoint m_entryPoint;

  /// Momentum (direction) vector at the entry point
  Gaudi::XYZVector m_entryVect;

  /// Exit point for radiator
  Gaudi::XYZPoint m_exitPoint;

  /// Momentum (direction) vector at the entry point
  Gaudi::XYZVector m_exitVect;

  /// pointer to DeRichRadiator for this radiator
  const DeRichRadiator * m_radiator;

public:

  /** @class Sorter RichRadIntersection.h Kernel/RichRadIntersection.h
   *
   *  Utility class to sort the Radiator Intersections
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   14/10/2006
   */
  class Sorter
  {
  private: // utility classes

    /// Functor to sort RichSmartIDs by Rich then panel numbers
    class SortByZ
      : std::binary_function< const RichRadIntersection, const RichRadIntersection, bool >
    {

    public:

      /** Sort operator for the RichRadIntersection
       *
       *  Sorts into order of increasing z position
       *
       *  @param i1 First RichRadIntersection
       *  @param i2 Second RichRadIntersection
       *
       *  @return bool indicating if i1 should be listed before i2
       */
      inline bool operator() ( const RichRadIntersection i1, const RichRadIntersection i2 ) const
      {
        return ( i1.entryPoint().z() < i2.entryPoint().z() );
      }

    };

  public:

    /// Constructor
    Sorter() {};

    /// Destructor
    ~Sorter() {};

    /// Sort the vector of intersections into order increasing Z position
    inline static void sortByZ( RichRadIntersection::Vector & intersections )
    {
      std::sort( intersections.begin(), intersections.end(), RichRadIntersection::Sorter::SortByZ() );
    }

  };

};



#endif // KERNEL_RichRadIntersection_H
