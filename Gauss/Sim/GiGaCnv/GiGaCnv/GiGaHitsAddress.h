// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/GiGaCnv/GiGaHitsAddress.h,v 1.2 2001-04-23 17:21:38 ibelyaev Exp $ 
#ifndef     GIGA_GIGAHitsAddRESS_H
#define     GIGA_GIGAHitsAddRESS_H  1

///
#include "GaudiKernel/GenericAddress.h"

/**     @class GiGaHitsAddress GiGaHitsAddress.h GiGaCnv/GiGaHitsAddress.h 
	
	definition of artificial address  for hits GiGa store 
	
	@author Vanya Belyaev 
	@date   9 Aug 2000
*/


class  GiGaHitsAddress: public GenericAddress   
{
  ///
public:
  ///  
  GiGaHitsAddress(  const CLID&        Clid          , 
                    const std::string& FileName      , 
                    const int          /* SeqInt */  , 
                    const std::string& ContainerName );
  ///
  GiGaHitsAddress(  const GenericLink& Link          ,
                    const std::string& FileName      , 
                    const std::string& ContainerName , 
                    const std::string& ObjName       );
  ///
  GiGaHitsAddress(  const GenericLinkBase & Link     );
  ///
  GiGaHitsAddress(  const GenericLinkBase * Link     );
  /// Standard destructor
  virtual ~GiGaHitsAddress();
  ///
};
///

#endif   //  __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
