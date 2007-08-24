// $Id: InEcalAcceptanceAlg.cpp,v 1.2 2007-08-24 21:25:19 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/06/18 18:35:29  ibelyaev
//  the firstcommmit for DC06 branch
// 
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
    _setProperty ( "Output" , LHCb::CaloIdLocation::InEcal ) ;
    _setProperty ( "Tool"   , "InEcalAcceptance/InEcal:PUBLIC"     ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
