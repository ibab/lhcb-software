// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:08  ibelyaev
/// the package restructurisation
/// 
// ============================================================================
#ifndef  GIGACNV_GIGAHITSADDRESS_H
#define  GIGACNV_GIGAHITSADDRESS_H  1
// ============================================================================

///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaHitsAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000

///
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

// ============================================================================
#endif   //  __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
// ============================================================================
