// $Id: RichRadIntersection.h,v 1.1 2006-03-01 14:53:01 papanest Exp $
#ifndef RICHDET_RICHRADINTERSECTION_H 
#define RICHDET_RICHRADINTERSECTION_H 1

// Include files
#include "Kernel/Point3DTypes.h"

class DeRichRadiator;

/** @class RichRadIntersection RichRadIntersection.h RichDet/RichRadIntersection.h
 *  
 *
 *  @author Antonis Papanestis
 *  @date   2006-02-28
 */
class RichRadIntersection {
public: 
  /// Standard constructor
  RichRadIntersection( ); 

  /// Constructor from two points and a pointer
  RichRadIntersection(Gaudi::XYZPoint entryPoint,
                      Gaudi::XYZPoint exitPoint,
                      const DeRichRadiator* rad ); 

  virtual ~RichRadIntersection( ); ///< Destructor

  /**
   * Set accessor to member m_entryPoint
   * @param entryPoint the new value for m_entryPoint
   */
  void setEntryPoint (Gaudi::XYZPoint entryPoint) {
    m_entryPoint = entryPoint;
  }

  /**
   * Get accessor to member m_entryPoint
   * @return the current value of m_entryPoint
   */
  inline Gaudi::XYZPoint entryPoint () {
    return m_entryPoint;
  }

  /**
   * Set accessor to member m_exitPoint
   * @param exitPoint the new value for m_exitPoint
   */
  void setExitPoint (Gaudi::XYZPoint exitPoint) {
    m_exitPoint = exitPoint;
  }

  /**
   * Get accessor to member m_exitPoint
   * @return the current value of m_exitPoint
   */
  inline Gaudi::XYZPoint exitPoint () {
    return m_exitPoint;
  }

  /**
   * Set accessor to member m_radiator
   * @param radiator the new value for m_radiator
   */
  void setRadiator (const DeRichRadiator* radiator) {
    m_radiator = radiator;
  }

  /**
   * Get accessor to member m_radiator
   * @return the current value of m_radiator
   */
  inline const DeRichRadiator* radiator () {
    return m_radiator;
  }


protected:

private:
  /// Entry point for radiator
  Gaudi::XYZPoint m_entryPoint;
  /// Exit point for radiator
  Gaudi::XYZPoint m_exitPoint;
  /// pointer to DeRichRadiator for this radiator
  const DeRichRadiator* m_radiator;
};
#endif // RICHDET_RICHRADINTERSECTION_H
