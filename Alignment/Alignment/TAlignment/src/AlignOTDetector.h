// $Id: AlignOTDetector.h,v 1.2 2007-07-20 17:24:33 janos Exp $
#ifndef TALIGNMENT_ALIGNOTDETECTOR_H 
#define TALIGNMENT_ALIGNOTDETECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/VectorMap.h"
#include "AlignmentInterfaces/IAlignDetectorSelector.h"            // Interface


/** @class AlignOTDetector AlignOTDetector.h AlignOTDetector.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-02-07
 */

class AlignOTDetector : public GaudiTool, virtual public IAlignDetectorSelector {
public: 
  
  typedef std::vector<DetectorElement*> Elements;
  typedef GaudiUtils::VectorMap<unsigned int, unsigned int> MapID;
  typedef std::pair<bool, unsigned int> BoolIndex;

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

  /** Virtual method to return an index associated to a detector element and whether it's
  * valid, i.e. a pair of a bool and int std::pair<bool, unsigned int>
  * @return a pair of a bool, (in)valid, and unsigned int, index.
  */ 
  BoolIndex index(const LHCb::LHCbID& anLHCbID) const;

protected:

private:
  /// Templated method to create indices 
  template <typename ID, typename ITER, typename FUNC>
  void createIndices(ITER i, ITER end, FUNC functor); 

  /// Templated method to return a bool, i.e. whether it belongs to 
  /// a to-be-aligned detector element, and the index for a given lchb id
  template <typename ID, typename MAP, typename FUNCCHECKDET, typename FUNCLHCB, typename FUNCSUBDET>
  BoolIndex getAssociatedIndex(const LHCb::LHCbID& anLHCbID, MAP map, FUNCCHECKDET funckCheckDet, 
                               FUNCLHCB funcLHCb, FUNCSUBDET funcSubDet) const;
   
  std::string       m_align;              ///< align what?
  Elements          m_elements;           ///< flat vector of pointers to detector elements
  std::vector<bool> m_elementFixed;       ///< Which elements are fixed 
  MapID             m_mapUniqueIDToIndex; ///< map id to index of element
};

inline const AlignOTDetector::Elements& AlignOTDetector::getDetectorElements() const {
  return m_elements;
}

inline unsigned int AlignOTDetector::nElements() const {
  return m_elements.size();
}

#endif // TALIGNMENT_ALIGNOTDETECTOR_H
