#ifndef      __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__
#define      __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__  1

///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaKineAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000

//
class  GiGaKineAddress: public GenericAddress   
{
  ///  
public:
  ///
  GiGaKineAddress(  const CLID&        Clid          , 
		    const std::string& FileName      , 
		    const int          /* SeqInt */  , 
		    const std::string& ContainerName )
    : GenericAddress( GiGaKine_StorageType, Clid )
  {
    setDbName        ( FileName          );
    setContainerName ( ContainerName     );
    setObjectName    ( "GiGaKineObject" );
  };
  ///
  GiGaKineAddress(  const GenericLink& Link          ,
		    const std::string& FileName      , 
		    const std::string& ContainerName , 
		    const std::string& ObjName       )
    : GenericAddress( Link )
  {
    setDbName       ( FileName      );
    setContainerName( ContainerName );
    setObjectName   ( ObjName       );
  };
  ///
  GiGaKineAddress(  const GenericLinkBase & Link     )
    : GenericAddress(  Link ) 
  {};
  ///
  GiGaKineAddress(  const GenericLinkBase * Link     )
    : GenericAddress(  Link ) 
  {};
  /// Standard destructor
  virtual ~GiGaKineAddress(){};
  ///
};
///

#endif   //  __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__

