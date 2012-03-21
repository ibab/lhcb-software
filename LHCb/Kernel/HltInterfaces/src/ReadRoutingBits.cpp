// $Id: ReadRoutingBits.cpp,v 1.5 2010-03-08 16:47:44 pkoppenb Exp $

#include "Kernel/ReadRoutingBits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReadRoutingBits
//
// 2009-11-06 : Patrick Koppenburg
//-----------------------------------------------------------------------------
namespace Hlt 
{
  //=========================================================================
  //  return bits if they are in a range
  //=========================================================================
  std::vector<unsigned int> firedRoutingBits(const LHCb::RawEvent* rawEvent,
                                             const std::vector<unsigned int>& relevantBits)
  {
    std::vector<unsigned int> yes ;
    boost::dynamic_bitset<unsigned int> x = readRoutingBits(rawEvent);
    for ( std::vector<unsigned int>::const_iterator j = relevantBits.begin();
          j != relevantBits.end() ; ++j ) 
    {
      if ( x[*j] ) yes.push_back(*j); // accepted
    }
    return yes ;
  }
  //=========================================================================
  //  return bits if they are in a list
  //=========================================================================
  std::vector<unsigned int> firedRoutingBits(const LHCb::RawEvent* rawEvent,
                                             const unsigned int relevantMin,
                                             const unsigned int relevantMax)
  {
    std::vector<unsigned int> yes ;
    boost::dynamic_bitset<unsigned int> x = readRoutingBits(rawEvent);
    //    std::cout << "Fired routing bits from " << relevantMin
    //             << " to " << relevantMax << " are : " ;
    for ( unsigned int j = relevantMin ; j <= relevantMax ; ++j ) 
    {
      if ( x[j] )
      {
        yes.push_back(j); // accepted
        //        std::cout << j << " (" << x[j] << ") "  ;
      }
    }
    //    std::cout << std::endl ;
    return yes ;
  }
  //=========================================================================
  //  read routing bits and return as bitset
  //=========================================================================
  boost::dynamic_bitset<unsigned int> readRoutingBits(const LHCb::RawEvent* rawEvent)
  {
    if ( !rawEvent )
    {
      throw GaudiException("Cannot find RawEvent","Hlt::readRoutingBits",StatusCode::FAILURE);
    }
    const unsigned int size = 3 ;
    
    const std::vector<LHCb::RawBank*>& banks =
      const_cast<LHCb::RawEvent*>(rawEvent)->banks(LHCb::RawBank::HltRoutingBits);
    //    std::cout << banks.size() << std::endl ;
    if ( banks.empty() ) 
    {
      throw GaudiException("There is no routing bits banks. Please make sure you run the trigger",
                           "Hlt::readRoutingBits", StatusCode::FAILURE);
    }
    if ( banks.size()>1 ) 
    {
      throw GaudiException("There are >1 routing bits banks. Please remove the original before re-running the trigger",
                           "Hlt::readRoutingBits", StatusCode::FAILURE);
    }
    if ( LHCb::RawBank::MagicPattern != banks.front()->magic() ) 
    {
      throw GaudiException("Wrong magic in HltRoutingBits","Hlt::readRoutingBits",StatusCode::FAILURE);
    }
    //    std::cout << banks.front()->size() << std::endl ;
    if ( banks.front()->size() != size*sizeof(unsigned int) ) 
    {
      throw GaudiException("RawBank wrong size","Hlt::readRoutingBits",StatusCode::FAILURE);
    }
    return boost::dynamic_bitset<unsigned int>(banks.front()->begin<unsigned int>(), banks.front()->end<unsigned int>());
  }
}
