// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:08  ibelyaev
/// the package restructurisation
///
// ============================================================================
#ifndef GIGACNV_GIGAGEOMADDRESS_H
#define GIGACNV_GIGAGEOMADDRESS_H  1
// ============================================================================

///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaGeomAddress: definition of artificial address
///                          for Input Geom GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000



///
class  GiGaGeomAddress: public GenericAddress   
{
  ///
public:
  ///
  GiGaGeomAddress(  const CLID&        Clid          , 
                    const std::string& FileName      , 
                    const int          /* SeqInt */  , 
                    const std::string& ContainerName ) ;
  ///
  GiGaGeomAddress(  const GenericLink& Link          ,
                    const std::string& FileName      , 
                    const std::string& ContainerName , 
                    const std::string& ObjName       );
  ///
  GiGaGeomAddress(  const GenericLinkBase & Link     );
  ///
  GiGaGeomAddress(  const GenericLinkBase * Link     );
  /// Standard destructor
  virtual ~GiGaGeomAddress();
  ///
};

// ============================================================================
#endif   //  __GIGA_GIGACONVERSION_GIGAGEOMADDRESS_H__
// ============================================================================
