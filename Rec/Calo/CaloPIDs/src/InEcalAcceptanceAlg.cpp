// $Id: InEcalAcceptanceAlg.cpp,v 1.5 2009-08-21 16:49:45 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2009/08/05 17:35:34  ibelyaev
//  add CaloPIDs configurables
//
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
/** @class InEcalAcceptanceAlg InEcalAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InEcalAcceptanceAlg : public InCaloAcceptanceAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InEcalAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InEcalAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation("InEcal" , context() ) ) ;

    _setProperty ( "Tool"   , "InEcalAcceptance/InEcal"     ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long       ,
                     LHCb::Track::Downstream ,
                     LHCb::Track::Ttrack     ) ) ;
  } 
  /// virtual proected destructor 
  virtual ~InEcalAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InEcalAcceptanceAlg() ;
  InEcalAcceptanceAlg           ( const InEcalAcceptanceAlg& );
  InEcalAcceptanceAlg& operator=( const InEcalAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InEcalAcceptanceAlg );
// ============================================================================
// The END 
// ============================================================================
