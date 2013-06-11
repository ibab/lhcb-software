#ifndef DECAYTREEFITTER_ERRORCODE_H
#define DECAYTREEFITTER_ERRORCODE_H

#include <iostream>

namespace DecayTreeFitter
{

  class ErrCode
  {
  public:
    enum Status {success=0,
		 pocafailure=1,
		 baddistance=2,
		 inversionerror=4,
		 badsetup=8,
		 divergingconstraint=16,
		 slowdivergingfit=32,
		 fastdivergingfit=64,
		 filtererror=128} ;

    ErrCode() : _flag(success) {}

    ErrCode(Status flag) : _flag(flag) {}

    const ErrCode& operator|=(const ErrCode& rhs) {
      _flag |= rhs._flag ; return *this ; }
    
    bool operator==(const ErrCode& rhs) const {
      return _flag == rhs._flag ; }
    
    bool operator==(const ErrCode::Status& rhs) const {
      return *this == ErrCode(rhs) ; }
    
    void reset() { _flag = success ; }
    bool failure() const { return _flag!=success ; }
    unsigned int flag() const { return _flag ; }
    
  private:
    unsigned int _flag ;
  } ;

  std::ostream& operator<<(std::ostream& os, const ErrCode& code) ;

}

#endif
