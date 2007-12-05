// $Id: AlignmentElement.h,v 1.4 2007-12-05 15:51:11 janos Exp $
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

  /** Construct an alignment element from a group, i.e. vector, of
   *  detector elements and an index. The index is needed for bookkeeping
   *  purposes, i.e. where to get/put the derivatives in H and rho
   */
  AlignmentElement(const std::vector<const DetectorElement*>& elements, const unsigned int index);


 public:

  /** Method to get the detector elements
   */
  //const std::vector<const DetectorElement*> elements() const;

  /** Method to get the name of the detector(s) element
   */
  const std::string name() const;

  /** Method to get the index
   */
  unsigned int index() const;

  /** Method to get the pivot point. The derivatives are taken w.r.t. this point
   */
  const Gaudi::XYZPoint& pivotXYZPoint() const;

  /** Method to get the pivot point as a vector of doubles
  */
  const std::vector<double> pivot() const;

  /** Method to get the delta translations as a vector of doubles
   */
  const std::vector<double> deltaTranslations() const;

  /** Method to get the delta rotations as a vector of doubles
   */
  const std::vector<double> deltaRotations() const;

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
  // const Gaudi::Transform3D localDeltaMatrix() const;

  /** Get local centre of element. Note: local centre != pivot point
   *
   */
  const Gaudi::XYZPoint localCentre() const;

  /** operator ==. Is this AlignmentElement constructed from this DetectorElement?
   */
  bool operator==(const DetectorElement* rhs) const;

  /** Output stream method
   */
  std::ostream& fillStream(std::ostream& s) const;


 private:

  typedef std::vector<const DetectorElement*>::const_iterator ElemIter;

  void validDetectorElement(const DetectorElement* element) const;

  void setPivotPoint();

  double average(double n) const { return n/double(m_elements.size()); };

 private:

  mutable std::vector<const DetectorElement*>  m_elements; ///< Vector of pointers to detector elements
  unsigned int                         m_index;    ///< Index. Needed for bookkeeping
  Gaudi::XYZPoint                      m_pivot;    ///< Pivot point

};

inline std::ostream& operator<<(std::ostream& lhs, const AlignmentElement& element) {
  return element.fillStream(lhs);
}

//inline const std::vector<const DetectorElement*> AlignmentElement::elements() const {return m_elements; }

inline unsigned int AlignmentElement::index() const { return m_index; }

inline const Gaudi::XYZPoint& AlignmentElement::pivotXYZPoint() const { return m_pivot; }

inline const std::vector<double> AlignmentElement::pivot() const {
  return boost::assign::list_of(m_pivot.x())(m_pivot.y())(m_pivot.z());
}

#endif // ALIGNMENTELEMENT_H
