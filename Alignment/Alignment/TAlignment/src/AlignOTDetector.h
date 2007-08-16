// $Id: AlignOTDetector.h,v 1.3 2007-08-16 13:48:33 graven Exp $
#ifndef TALIGNMENT_ALIGNOTDETECTOR_H 
#define TALIGNMENT_ALIGNOTDETECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"
#include "AlignmentInterfaces/IAlignDetectorSelector.h"            // Interface

#include "AlignmentFunctions.h"

/** @class AlignOTDetector AlignOTDetector.h AlignOTDetector.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-02-07
 */

class AlignOTDetector : public GaudiTool, virtual public IAlignDetectorSelector {
public: 
  
  /// Standard constructor
  AlignOTDetector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~AlignOTDetector( ); ///< Destructor

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

inline const Elements& AlignOTDetector::getDetectorElements() const {
  return m_elements;
}

inline unsigned int AlignOTDetector::nElements() const {
  return m_elements.size();
}

inline const std::vector<std::vector<double> >& AlignOTDetector::constraints() const {
  return m_constraints;
}
#endif // TALIGNMENT_ALIGNOTDETECTOR_H
