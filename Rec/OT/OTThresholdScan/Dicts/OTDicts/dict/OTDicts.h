#ifndef DICT_OTDICTS_H
#define DICT_OTDICTS_H 1

#include <vector>
#include <GaudiKernel/Range.h>
#include <Event/Node.h>

#include <Event/OTLiteTime.h>

template class std::vector<const LHCb::Node*>;
template class Gaudi::Range_<std::vector<const LHCb::Node*> >;


// namespace 
// {
//   struct __Instantiations 
//   {
//     LHCb::OTLiteTimeContainer m_cnt ;
//   } ;  
// }

template class std::vector<LHCb::OTLiteTime>;


#endif // DICT_OTDICTS_H
