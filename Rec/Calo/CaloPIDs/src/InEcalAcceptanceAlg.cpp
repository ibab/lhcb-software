// $Id: InEcalAcceptanceAlg.cpp,v 1.1 2006-06-18 18:35:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InEcalAcceptanceAlg InEcalAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InEcalAcceptanceAlg : public InCaloAcceptanceAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<InEcalAcceptanceAlg>;
protected:
  /// Standard protected constructor
  InEcalAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {
    setProperty ( "Output" , LHCb::CaloIdLocation::InEcal ) ;
    setProperty ( "Tool"   , "InEcalAcceptance/InEcal:PUBLIC"     ) ;
    // track types:
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Downstream ,
                    LHCb::Track::Ttrack     ) ) ;
  } ;
  /// virtual proected destructor 
  virtual ~InEcalAcceptanceAlg() {} ; 
private:
  InEcalAcceptanceAlg() ;
  InEcalAcceptanceAlg           ( const InEcalAcceptanceAlg& );
  InEcalAcceptanceAlg& operator=( const InEcalAcceptanceAlg& );
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InEcalAcceptanceAlg );
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
