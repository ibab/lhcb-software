// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/GiGaCnv/GiGaKineAddress.h,v 1.2 2001-04-23 17:21:38 ibelyaev Exp $ 
#ifndef      GIGA_GIGAKINEADDRESS_H
#define      GIGA_GIGAKINEADDRESS_H  1

///
#include "GaudiKernel/GenericAddress.h"


/**      @class GiGaKineAddress GiGaKineAddress.h GiGaCnv/GigaKineAddress.h
	 
	 definition of artificial address for Input Kine GiGa store 

	 @author Vanya Belyaev 
	 @date   9 Aug 2000
*/

class  GiGaKineAddress: public GenericAddress   
{
  ///  
public:
  ///
  GiGaKineAddress(  const CLID&        Clid          , 
                    const std::string& FileName      , 
                    const int          /* SeqInt */  , 
                    const std::string& ContainerName );
  ///
  GiGaKineAddress(  const GenericLink& Link          ,
                    const std::string& FileName      , 
                    const std::string& ContainerName , 
                    const std::string& ObjName       );
  ///
  GiGaKineAddress(  const GenericLinkBase & Link     );
  ///
  GiGaKineAddress(  const GenericLinkBase * Link     );
  /// Standard destructor
  virtual ~GiGaKineAddress();
  ///
};
///

#endif   //  __GIGA_GIGACONVERSION_GIGAKINEADDRESS_H__

