#ifndef      __GIGA_GIGACONVERSION_GIGATrajAddRESS_H__
#define      __GIGA_GIGACONVERSION_GIGATrajAddRESS_H__  1



///
///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaTrajAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000


class  GiGaTrajAddress: public GenericAddress   
{
  
 public:
  
  GiGaTrajAddress(  const CLID&        Clid          , 
		    const std::string& FileName      , 
		    const int          SeqInt        , 
		    const std::string& ContainerName ); 
  
  GiGaTrajAddress(  const GenericLink& Link          ,
		    const std::string& FileName      , 
		    const std::string& ContainerName , 
		    const std::string& ObjName       );
  
  GiGaTrajAddress(  const GenericLinkBase & Link     );
  GiGaTrajAddress(  const GenericLinkBase * Link     );
  
  /// Standard destructor
  ~GiGaTrajAddress(){};
  
};

#endif   //  __GIGA_GIGACONVERSION_GIGATrajAddRESS_H__

