// $Id: InSpdAcceptanceAlg.cpp,v 1.1 2006-06-18 18:35:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InSpdAcceptanceAlg InSpdAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InSpdAcceptanceAlg : public InCaloAcceptanceAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<InSpdAcceptanceAlg>;
protected:
  /// Standard protected constructor
  InSpdAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {
    setProperty ( "Output" , LHCb::CaloIdLocation::InSpd    ) ;
    setProperty ( "Tool"   , "InSpdAcceptance/InSpd:PUBLIC" ) ;
    // track types:
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Downstream ,
                    LHCb::Track::Ttrack     ) ) ;
  } ;
  /// virtual proected destructor 
  virtual ~InSpdAcceptanceAlg() {} ; 
private:
  InSpdAcceptanceAlg() ;
  InSpdAcceptanceAlg           ( const InSpdAcceptanceAlg& );
  InSpdAcceptanceAlg& operator=( const InSpdAcceptanceAlg& );
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InSpdAcceptanceAlg );
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
