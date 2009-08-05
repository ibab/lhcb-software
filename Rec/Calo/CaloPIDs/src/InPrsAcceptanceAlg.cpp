// $Id: InPrsAcceptanceAlg.cpp,v 1.4 2009-08-05 17:35:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.2  2007/08/24 21:25:19  odescham
// fix uncheck. StatusCodes
//
// Revision 1.1  2006/06/18 18:35:29  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InPrsAcceptanceAlg InPrsAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InPrsAcceptanceAlg : public InCaloAcceptanceAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InPrsAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InPrsAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {
    if( "HLT" == context() )
    { _setProperty ( "Output" , LHCb::CaloIdLocation::InPrsHlt  ) ; }
    else
    { _setProperty ( "Output" , LHCb::CaloIdLocation::InPrs     ) ; }
    //
    _setProperty ( "Tool"   , "InPrsAcceptance/InPrs" ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long       ,
                     LHCb::Track::Downstream ,
                     LHCb::Track::Ttrack     ) ) ;
  } 
  /// virtual proected destructor 
  virtual ~InPrsAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InPrsAcceptanceAlg() ;
  InPrsAcceptanceAlg           ( const InPrsAcceptanceAlg& );
  InPrsAcceptanceAlg& operator=( const InPrsAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InPrsAcceptanceAlg );
// ============================================================================
// The END 
// ============================================================================
