//#ifndef TALIGNMENT_ALIGNMENTFUNCTIONS_H 
//#define TALIGNMENT_ALIGNMENTFUNCTIONS_H 1

#include <vector>
#include <utility>
#include "GaudiKernel/VectorMap.h"
#include "Kernel/LHCbID.h"
#include "boost/lambda/lambda.hpp"

/// Anonymous namespace
namespace {
  
  /// Some handy typedefs
  typedef std::vector<DetectorElement*>                     Elements;
  typedef std::vector<bool>                                 Fixed;
  typedef GaudiUtils::VectorMap<unsigned int, unsigned int> IndexMap;
  typedef std::pair<bool, unsigned int>                     BoolIndex;

  /// f(x) = x
  template <typename T> const T& id(const T& t) { return t; }
  
  template <typename ITER      , 
            typename ACTION1   , 
            typename ACTION2>
  const IndexMap createIndices(ITER       i               ,      ///< begin iterator of element container
                               ITER       end             ,      ///< end iterator of element container
                               const Fixed&      fixed    ,      ///< Which elements are fixed    
                               ACTION1    chanIDfromDetElem,      ///< Get channel id from LHCb id
                               ACTION2    elemIDfromChanID,      ///< Get element id from channel id
                               Elements&  elements           ) { ///< Reference to vector of elements to be aligned
    IndexMap indices;
    for (unsigned predicate = 0u, index = 0u; i != end; ++i) {
      /// Note: ++not++ does not apply here.
      if (!fixed.empty() && fixed[predicate++]) continue;
      elements.push_back((*i));
      typedef typename ACTION1::template sig<boost::lambda::tuple<ACTION1,typename ITER::value_type> >::type ID;
      ID chanID = chanIDfromDetElem((*i));
      /// Note: ++not++ does not apply here.
      indices.insert(elemIDfromChanID(chanID), index++);
    }
    return indices;
  }

  template <typename MAP      , 
            typename PREDICATE, 
            typename ACTION1  , 
            typename ACTION2>
  BoolIndex getAssociatedIndex(const LHCb::LHCbID& anLHCbID        ,  ///< an LHCb id
                               const MAP&          indexMap      ,  ///< Reference to map of element to index
                               PREDICATE           isSubDet        ,  ///< Check sub-detetor type of LHCbID 
                               ACTION1             chanIDfromLHCbID,  ///< Get channel id from LHCb id
                               ACTION2             elemIDfromChanID) {///< Get element id from channel id
      if (!isSubDet(anLHCbID)) return std::make_pair(false, 0u);
      typedef typename ACTION1::template sig<boost::lambda::tuple<ACTION1,LHCb::LHCbID> >::type ID;
      ID aChannel = chanIDfromLHCbID(anLHCbID);
      typename MAP::iterator iter = indexMap.find(elemIDfromChanID(aChannel)); 
      return iter != indexMap.end() ? std::make_pair(true, iter->second) : std::make_pair(false, 0u); 
  }
}
