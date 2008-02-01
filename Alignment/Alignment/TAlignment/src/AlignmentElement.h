// $Id: AlignmentElement.h,v 1.8 2008-02-01 09:09:32 wouter Exp $
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

// LOcal
#include "AlDofMask.h"
#include "AlParameters.h"

/** @class AlignmentElement AlignmentElement.h
 *
 * The alignment element represents a single alignable object. 
 *
 * About reference frames (WDH)
 * ----------------------------
 * There are 3 relevant reference frames in the alignment:
 *
 * - the global frame: this is the LHCb frame. 
 *
 * - the 'database-frame': this is the frame in which the
 * delta-alignment is applied. Thie includes all previous
 * delta-alignments. We don't need to worry about the definition of
 * this frame ,because Juan has provided the transformation functions
 * to translate a delta-transform of the globalframe into the
 * 'data-base' frame.
 *
 * - the 'alignment-frame': this is the frame in which the derivatives
 * to the delta-alignment are calculated. So, the result of the
 * alignment procedure is a delta-alignment in the alignment-frame. We
 * transform to the globalframe by using the 'alignmentFrame'
 * transform. Currently, this is just the translation with the pivot
 * point. In the very near future, we would like to include the
 * stereoangle rotation.
 *
 *  @author Jan Amoraal
 *  @date   2007-08-15
 */

class AlignmentElement {

public:

  /** Construct an alignment element from a detector element and an index.
   *  The index is needed for bookkeeping purposes, i.e. where to get/put
   *  the derivatives in H and rho. Also a  vector of booleans representing 
   *  which dofs to align for, Tx, Ty, Tz, Rx, Ry, and Rz. Default is all dofs
   */
  AlignmentElement(const DetectorElement* element, const unsigned int index, 
                   const std::vector<bool>& dofMask = std::vector<bool>(6, true));

  /** Construct an alignment element from a group, i.e. vector, of
   *  detector elements and an index. The index is needed for bookkeeping
   *  purposes, i.e. where to get/put the derivatives in H and rho. Also a 
   *  vector of booleans representing which dofs to align for, Tx, Ty, Tz, 
   *  Rx, Ry, and Rz. Default is all dofs
   */
  AlignmentElement(const std::vector<const DetectorElement*>& elements, const unsigned int index, 
                   const std::vector<bool>& dofMask = std::vector<bool>(6, true));

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

  /**
   *  return number of dofs we want to align for
   */
  unsigned int nDOFs() const { return m_dofMask.nActive() ; }

  const AlDofMask& dofMask() const { return m_dofMask ; }
    
  /** Method to update the parameters of the detector element. 
   * The delta translations and rotations are in the alignment frame (see above).
   */
  StatusCode updateGeometry(const AlParameters& parameters) const ;


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

  const Gaudi::Transform3D& alignmentFrame() const { return m_alignmentFrame; }
 private:

  typedef std::vector<const DetectorElement*>::const_iterator ElemIter;

  void validDetectorElement(const DetectorElement* element) const;

  void setPivotPoint();

  double average(double n) const { return n/double(m_elements.size()); };

 private:

  mutable std::vector<const DetectorElement*>  m_elements;         ///< Vector of pointers to detector elements
  unsigned int                                 m_index;            ///< Index. Needed for bookkeeping
  AlDofMask                                    m_dofMask;          ///< d.o.f's we want to align for
  Gaudi::XYZPoint                              m_pivot;            ///< Pivot point
  Gaudi::Transform3D                           m_alignmentFrame;   ///> Frame in which delta-derivatives are calculated (will replace pivot point)
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
