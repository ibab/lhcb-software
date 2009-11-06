// $Id: ReadRoutingBits.cpp,v 1.1 2009-11-06 13:22:03 pkoppenb Exp $
// Include files 

#include "Kernel/ReadRoutingBits.h"

// local
#include "Kernel/ReadRoutingBits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReadRoutingBits
//
// 2009-11-06 : Patrick Koppenburg
//-----------------------------------------------------------------------------
namespace Hlt {
//=========================================================================
//  return bits if they are in a range
//=========================================================================
  std::vector<unsigned int> firedRoutingBits(LHCb::RawEvent* rawEvent, 
                                             const std::vector<unsigned int> relevantBits){
    std::vector<unsigned int> yes ;
    boost::dynamic_bitset<unsigned int> x = readRoutingBits(rawEvent);
    for ( std::vector<unsigned int>::const_iterator j = relevantBits.begin() ; j != relevantBits.end() ; ++j){
      //      if (msgLevel(MSG::VERBOSE)) verbose()
      if ( x[*j] ) yes.push_back(*j); // accepted
    }
    return yes ;
  };
//=========================================================================
//  return bits if they are in a list
//=========================================================================
  std::vector<unsigned int> firedRoutingBits(LHCb::RawEvent* rawEvent, 
                                             const unsigned int relevantMin, 
                                             const unsigned int relevantMax){
    std::vector<unsigned int> yes ;
    boost::dynamic_bitset<unsigned int> x = readRoutingBits(rawEvent);
    for ( unsigned int j = relevantMin ; j <= relevantMax ; ++j){
      if ( x[j] ) yes.push_back(j); // accepted
    }
    return yes ;
  };
//=========================================================================
//  read routing bits and return as bitset
//=========================================================================
  boost::dynamic_bitset<unsigned int> readRoutingBits(LHCb::RawEvent* rawEvent){
    if (!rawEvent){
      throw GaudiException("Cannot find RawEvent","Hlt::readRoutingBits",StatusCode::FAILURE);
    }
    const unsigned int size = 3 ;
    const unsigned int byte = 8 ;
    const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::HltRoutingBits);
    if (banks.size()!=1) 
      throw GaudiException("Cannot find RawBank","Hlt::readRoutingBits",StatusCode::FAILURE);
    if (banks.front()->size()!=size*sizeof(unsigned int)) {
      throw GaudiException("RawBank wrong size","Hlt::readRoutingBits",StatusCode::FAILURE);
    }
    const unsigned int *data = banks.front()->data();
    boost::dynamic_bitset<unsigned int> x(byte*sizeof(unsigned int), data[0]);
    x.append(data[1]);
    x.append(data[2]);
    return x ;
  }
  
}
