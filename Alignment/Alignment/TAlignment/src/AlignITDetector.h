// $Id: AlignITDetector.h,v 1.1 2007-10-15 16:24:49 lnicolas Exp $
#ifndef TALIGNMENT_ALIGNITDETECTOR_H 
#define TALIGNMENT_ALIGNITDETECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"
#include "AlignmentInterfaces/IAlignDetectorSelector.h"            // Interface

#include "AlignmentFunctions.h"

/** @class AlignITDetector AlignITDetector.h AlignITDetector.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-02-07
 */

class AlignITDetector : public GaudiTool, virtual public IAlignDetectorSelector {
public: 
  
  /// Standard constructor
  AlignITDetector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~AlignITDetector( ); ///< Destructor

  StatusCode initialize();

  /** Virtual method to get detector elements to align
  * @return const reference to flat vector of pointers to detector elements, 
  * i.e. std::vector<DetectorElement*>
  */
  const Elements& getDetectorElements() const;
  
  /** Virtual method to return the number of elements to align
  * @return number of elements of type unsigned int
  */
  unsigned int nElements() const;

  const std::vector<std::vector<double> >& constraints() const;
  

  /** Virtual method to return an index associated to a detector element and whether it's
  * valid, i.e. a pair of a bool and int std::pair<bool, unsigned int>
  * @return a pair of a bool, (in)valid, and unsigned int, index.
  */ 
  BoolIndex index(const LHCb::LHCbID& anLHCbID) const;

private:
  std::string       m_align;              ///< align what?
  Elements          m_elements;           ///< flat vector of pointers to detector elements
  std::vector<bool> m_fixed;              ///< Which elements are fixed 
  IndexMap          m_indexMap;           ///< map id to index of element
  std::vector<std::vector<double> > m_constraints;      ///< Vector of constraints f + f0
};

inline const Elements& AlignITDetector::getDetectorElements() const {
  return m_elements;
}

inline unsigned int AlignITDetector::nElements() const {
  return m_elements.size();
}

inline const std::vector<std::vector<double> >& AlignITDetector::constraints() const {
  return m_constraints;
}
#endif // TALIGNMENT_ALIGNITDETECTOR_H
