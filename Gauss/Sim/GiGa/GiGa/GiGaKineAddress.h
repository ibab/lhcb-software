#ifndef      __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__
#define      __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__  1



///
///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaKineAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000


class  GiGaKineAddress: public GenericAddress   
{
  
 public:
  
  GiGaKineAddress(  const CLID&        Clid          , 
		    const std::string& FileName      , 
		    const int          SeqInt        , 
		    const std::string& ContainerName ); 
  
  GiGaKineAddress(  const GenericLink& Link          ,
		    const std::string& FileName      , 
		    const std::string& ContainerName , 
		    const std::string& ObjName       );
  
  GiGaKineAddress(  const GenericLinkBase & Link     );
  GiGaKineAddress(  const GenericLinkBase * Link     );
  
  /// Standard destructor
  ~GiGaKineAddress(){};
  
};

#endif   //  __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__
