// $Id: GetDecay.cpp,v 1.1 2008-05-29 13:51:45 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/GetDecay.h"
// ============================================================================
/*  create decay object from the descriptor
 *  @attention only "main" decay is created!
 *  @param descriptor the decay descriptor 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
LHCb::Decay DaVinci::decay 
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{ 
  if ( 0 == decoder   ) { return LHCb::Decay () ; }                // RETURN 
  //
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return LHCb::Decay () ; }                // RETURN 
  LHCb::Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return LHCb::Decay () ; }                // RETURN
  //
  return dec ;
}
// ============================================================================
/*  create all decay objects from the descriptor
 *  @param descriptor the decay descriptor 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
LHCb::Decays DaVinci::decays
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{
  if ( 0 == decoder   ) { return LHCb::Decays () ; }              // RETURN 
  //
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return LHCb::Decays () ; }              // RETURN 
  //
  LHCb::Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return LHCb::Decays () ; }              // RETURN
  LHCb::Decays result ( 1 , dec ) ;
  
  if ( decoder->is_cc()  )
  {
    LHCb::Decay dec_cc ; 
    sc = decoder -> getDecay_cc ( dec_cc ) ;
    if ( sc.isFailure() ) { return LHCb::Decays () ; }            // RETURN 
    result.push_back ( dec_cc ) ;
  }
  //
  return result ;
}
// ============================================================================
/* create all decay objects from the descriptor
 *  @param descriptors the decay descriptors 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
LHCb::Decays DaVinci::decays
( const std::vector<std::string>& descriptors , 
  IDecodeSimpleDecayString*       decoder     ) 
{  
  if ( 0 == decoder ) { return LHCb::Decays () ; }                   // RETURN 
  //
  LHCb::Decays result ;
  for ( std::vector<std::string>::const_iterator idesc = descriptors.begin() ;
        descriptors.end() != idesc ; ++idesc ) 
  {
    LHCb::Decays res = decays ( *idesc , decoder ) ;
    if ( res.empty() ) { return LHCb::Decays () ; }                  // RETURN
    result.insert ( result.end() , res.begin() , res.end() ) ;
  }
  //
  return result ;  
}
// ============================================================================
// The END
// ============================================================================

