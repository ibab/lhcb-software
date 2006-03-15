// $Id: SiDataFunctor.h,v 1.1 2006-03-15 09:07:03 mneedham Exp $
#ifndef _SiDataFunctor_H_
#define _SiDataFunctor_H_

#include <functional>
#include <numeric>


//
// This File contains the declaration of STDataFunctor namespace 
//   - dumping ground for things that don't fit.
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 

namespace SiDataFunctor {

// functors 
template <typename TYPE>
  class CompareByChannel
    : public std::binary_function<const TYPE, const typename TYPE::chan_type, bool> {

  public:

    typedef typename TYPE::chan_type comp_type;

    inline bool operator() (const TYPE& obj,const comp_type& testID) const{
      return (testID > obj.channelID());
    }

    
    inline bool operator() (const comp_type& testID, const TYPE& obj ) const{
      return ( obj.channelID() > testID);
    }

}; 


};


#endif // _SiDataFunctor_H_












