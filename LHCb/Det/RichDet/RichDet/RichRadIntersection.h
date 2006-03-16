// $Id: RichRadIntersection.h,v 1.2 2006-03-16 16:09:50 jonrob Exp $
#ifndef RICHDET_RICHRADINTERSECTION_H 
#define RICHDET_RICHRADINTERSECTION_H 1

// Include files
#include "Kernel/Point3DTypes.h"

class DeRichRadiator;

/** @class RichRadIntersection RichRadIntersection.h RichDet/RichRadIntersection.h
 *  
 *  RICH Radiator intersection information
 *  
 *  @author Antonis Papanestis
 *  @date   2006-02-28
 */

class RichRadIntersection 
{

public: 

  /// Standard constructor
  RichRadIntersection( ) : m_radiator ( NULL ) { }

  /// Constructor from two points and a pointer
  RichRadIntersection( const Gaudi::XYZPoint & entryPoint,
                       const Gaudi::XYZPoint & exitPoint,
                       const DeRichRadiator * rad ) :
    m_entryPoint ( entryPoint ),
    m_exitPoint  ( exitPoint  ),
    m_radiator   ( rad        ) { }
  
  ~RichRadIntersection( ) { } ///< Destructor

  /**
   * Set the entry point to the radiator
   * @param entryPoint the new entry point to the radiator
   */
  inline void setEntryPoint ( const Gaudi::XYZPoint & entryPoint ) 
  {
    m_entryPoint = entryPoint;
  }

  /**
   * Get the entry point to the radiator
   * @return the current value of the entry point to the radiator
   */
  inline const Gaudi::XYZPoint & entryPoint () const
  {
    return m_entryPoint;
  }

  /**
   * Set the exit point of the radiator
   * @param exitPoint the new exit point of the radiator
   */
  inline void setExitPoint ( const Gaudi::XYZPoint & exitPoint ) 
  {
    m_exitPoint = exitPoint;
  }

  /**
   * Get the exit point to the radiator
   * @return the current value of the exit point to the radiator
   */
  inline const Gaudi::XYZPoint & exitPoint () const 
  {
    return m_exitPoint;
  }
  
  /**
   * Set accessor to member m_radiator
   * @param radiator the new value for m_radiator
   */
  inline void setRadiator ( const DeRichRadiator* radiator ) 
  {
    m_radiator = radiator;
  }

  /**
   * Get accessor to member m_radiator
   * @return the current value of m_radiator
   */
  inline const DeRichRadiator* radiator () const
  {
    return m_radiator;
  }

private:

  /// Entry point for radiator
  Gaudi::XYZPoint m_entryPoint;

  /// Exit point for radiator
  Gaudi::XYZPoint m_exitPoint;

  /// pointer to DeRichRadiator for this radiator
  const DeRichRadiator* m_radiator;

};

#endif // RICHDET_RICHRADINTERSECTION_H
