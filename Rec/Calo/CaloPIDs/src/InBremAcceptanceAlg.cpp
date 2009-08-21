// $Id: InBremAcceptanceAlg.cpp,v 1.5 2009-08-21 16:49:45 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revsion:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2009/08/05 17:35:33  ibelyaev
//  add CaloPIDs configurables
//
// Revision 1.3  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
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
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InBremAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InBremAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation("InBrem" , context() ) ) ;
    
    _setProperty ( "Tool"   , "InBremAcceptance/InBrem" ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
    //
  } 
  /// virtual proected destructor 
  virtual ~InBremAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InBremAcceptanceAlg() ;
  InBremAcceptanceAlg           ( const InBremAcceptanceAlg& );
  InBremAcceptanceAlg& operator=( const InBremAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InBremAcceptanceAlg );
// ============================================================================
// The END 
// ============================================================================
