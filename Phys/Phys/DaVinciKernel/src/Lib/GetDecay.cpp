// $Id: GetDecay.cpp,v 1.2 2008-12-04 16:39:14 ibelyaev Exp $
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
Decays::Decay 
Decays::decay 
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{ 
  if ( 0 == decoder   ) { return Decay () ; }                // RETURN 
  //
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return Decay () ; }                // RETURN 
  Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return Decay () ; }                // RETURN
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
std::vector<Decays::Decay>
Decays::decays
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{
  if ( 0 == decoder   ) { return std::vector<Decay> () ; } // RETURN 
  //
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return std::vector<Decay> () ; } // RETURN 
  //
  Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return std::vector<Decay> () ; } // RETURN
  std::vector<Decay> result ( 1 , dec ) ;
  
  if ( decoder->is_cc()  )
  {
    Decay dec_cc ; 
    sc = decoder -> getDecay_cc ( dec_cc ) ;
    if ( sc.isFailure() ) { return std::vector<Decay> () ; } // RETURN 
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
std::vector<Decays::Decay>
Decays::decays
( const std::vector<std::string>& descriptors , 
  IDecodeSimpleDecayString*       decoder     ) 
{  
  if ( 0 == decoder ) { return std::vector<Decay> () ; }           // RETURN 
  //
  std::vector<Decay> result ;
  for ( std::vector<std::string>::const_iterator idesc = descriptors.begin() ;
        descriptors.end() != idesc ; ++idesc ) 
  {
    std::vector<Decay> res = decays ( *idesc , decoder ) ;
    if ( res.empty() ) { return std::vector<Decay> () ; }          // RETURN
    result.insert ( result.end() , res.begin() , res.end() ) ;
  }
  //
  return result ;  
}
// ============================================================================
// The END
// ============================================================================

