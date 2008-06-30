// $Id: InBremAcceptanceAlg.cpp,v 1.3 2008-06-30 15:37:34 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.1  2006/06/18 18:35:28  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InBremAcceptanceAlg InBremAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
#include "GaudiKernel/System.h"
class InBremAcceptanceAlg : public InCaloAcceptanceAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<InBremAcceptanceAlg>;
protected:
  /// Standard protected constructor
  InBremAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {
     if( "HLT" == context() ){
      _setProperty ( "Output" , LHCb::CaloIdLocation::InBremHlt    ) ;
    }else{ 
      _setProperty ( "Output" , LHCb::CaloIdLocation::InBrem     ) ;
    }     
    _setProperty ( "Tool"   , "InBremAcceptance/InBrem:PUBLIC" ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
    //
  } ;
  /// virtual proected destructor 
  virtual ~InBremAcceptanceAlg() {} ; 
private:
  InBremAcceptanceAlg() ;
  InBremAcceptanceAlg           ( const InBremAcceptanceAlg& );
  InBremAcceptanceAlg& operator=( const InBremAcceptanceAlg& );
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InBremAcceptanceAlg );
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
