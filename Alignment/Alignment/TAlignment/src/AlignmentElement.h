// $Id: AlignmentElement.h,v 1.3 2007-11-15 11:12:03 janos Exp $
#ifndef TALIGNMENT_ALIGNMENTELEMENT_H 
#define TALIGNMENT_ALIGNMENTELEMENT_H 1

// Include files
// from STD
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/Point3DTypes.h"

// from DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/AlignmentCondition.h"

// from BOOST
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"

/** @class AlignmentElement AlignmentElement.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-08-15
 */

class AlignmentElement {

 public:

  /** Construct an alignment element from a detector element and an index. 
   *  The index is needed for bookkeeping purposes, i.e. where to get/put
   *  the derivatives in H and rho
   */
  AlignmentElement(const DetectorElement* element, const unsigned int index);

 public:
  
  /** Method to get the detector element
   */
  const DetectorElement* element() const;
  
  /** Method to get the name of the detector element
   */
  const std::string name() const;
  
  /** Method to get the index
   */
  unsigned int index() const;

  /** Method to get the pivot point. The derivatives are taken w.r.t. this point
   */
  const Gaudi::XYZPoint& pivotXYZPoint() const;

  /** Method to get the delta translations as a vector of doubles
   */
  const std::vector<double>& deltaTranslations() const;

  /** Method to get the delta rotations as a vector of doubles
   */
  const std::vector<double>& deltaRotations() const;

  /** Method to get the pivot point as a vector of doubles
   */
  const std::vector<double> pivot() const;

  /** Method to set the local deltas of the detector element.
   * The delta translations and rotations are in the local frame
   */
  StatusCode setLocalDeltaParams(const std::vector<double>& localDeltaT, 
                                 const std::vector<double>& localDeltaR) const;

  /** Method to set the local delta transformation matrix (from global to local)
   * The delta translations and rotations are in the global frame
   */
  StatusCode setLocalDeltaMatrix(const std::vector<double>& globalDeltaT, 
                                 const std::vector<double>& globalDeltaR) const;

  /** Method to get the local delta transformation matrix (from global to local)
   *
   */
  const Gaudi::Transform3D localDeltaMatrix() const;
  

  const Gaudi::XYZPoint localCentre() const;
  
  /** operator ==. Is this AlignmentElement constructed from this DetectorElement?  
   */
  bool operator==(const DetectorElement* rhs) const;

  /** Output stream method
   */
  std::ostream& fillStream(std::ostream& s) const;
  
  
 private:
  
  void grabPivotPoint();
    
 private:
  
  const DetectorElement*               m_element; /// Pointer to detector element
  unsigned int                         m_index;   /// Index. Needed for bookkeeping
  Gaudi::XYZPoint                      m_pivot;   /// Pivot point

};

inline std::ostream& operator<<(std::ostream& lhs, const AlignmentElement& element) { 
  return element.fillStream(lhs);
}

inline const DetectorElement* AlignmentElement::element() const {return m_element; }
  
inline unsigned int AlignmentElement::index() const { return m_index; }

inline bool AlignmentElement::operator==(const DetectorElement* rhs) const {return m_element == rhs; }

inline const Gaudi::XYZPoint& AlignmentElement::pivotXYZPoint() const { return m_pivot; }

inline const std::vector<double> AlignmentElement::pivot() const {
  
  return boost::assign::list_of(m_pivot.x())(m_pivot.y())(m_pivot.z());
}

inline StatusCode AlignmentElement::setLocalDeltaParams(const std::vector<double>& localDeltaT, 
                                                        const std::vector<double>& localDeltaR) const {  
  StatusCode sc = const_cast<IGeometryInfo*>(m_element->geometry())->ownToOffNominalParams(localDeltaT, localDeltaR, pivot());
  
  return sc;
}

inline StatusCode AlignmentElement::setLocalDeltaMatrix(const std::vector<double>& globalDeltaT, 
                                                        const std::vector<double>& globalDeltaR) const {
  /// Get transformation matrix to go from old delta to new delta
  const Gaudi::Transform3D globalDeltaMatrix = DetDesc::localToGlobalTransformation(globalDeltaT, 
                                                                                    globalDeltaR, 
                                                                                    this->pivot());
  /// Transform global deltas to new local deltas (takes into account current local delta!!)
  const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix(m_element, globalDeltaMatrix);
  /// Update position of detector elements
  StatusCode sc = const_cast<IGeometryInfo*>(m_element->geometry())->ownToOffNominalMatrix(localDeltaMatrix);
  
  return sc;
}

inline const Gaudi::Transform3D AlignmentElement::localDeltaMatrix() const {
  return m_element->geometry()->ownToOffNominalMatrix();  
}

inline const Gaudi::XYZPoint AlignmentElement::localCentre() const {
  return m_element->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
}
#endif // ALIGNMENTELEMENT_H
