#ifndef      __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
#define      __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__  1



///
///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaHitsAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000


class  GiGaHitsAddress: public GenericAddress   
{
  
 public:
  
  GiGaHitsAddress(  const CLID&        Clid          , 
		    const std::string& FileName      , 
		    const int          SeqInt        , 
		    const std::string& ContainerName ); 
  
  GiGaHitsAddress(  const GenericLink& Link          ,
		    const std::string& FileName      , 
		    const std::string& ContainerName , 
		    const std::string& ObjName       );
  
  GiGaHitsAddress(  const GenericLinkBase & Link     );
  GiGaHitsAddress(  const GenericLinkBase * Link     );
  
  /// Standard destructor
  ~GiGaHitsAddress(){};
  
};

#endif   //  __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
