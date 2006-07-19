// $Id: BremChi22ID.cpp,v 1.3 2006-07-19 07:32:57 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/06/22 15:40:39  ibelyaev
//  fix incinsistencied for 'Brem'-algorithms
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Include files 
// ============================================================================
// Local 
// ============================================================================
#include "CaloChi22ID.h"
#include "ToVector.h"
// ============================================================================
/** @class EcalChi22ID EcalChi22ID.h
 *  The preconfigured instance of class CaloChi22ID 
 *  @suthor Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class BremChi22ID: public CaloChi22ID 
{
  /// friend factory for instantiation 
  friend class AlgFactory<BremChi22ID>;
protected:
  /// Standard protected constructor
  BremChi22ID 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    :  CaloChi22ID ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::BremMatch ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::BremChi2      ) ;
    // @todo it must be in agrement with "Threshold" for BremMatchAlg 
    setProperty ( "CutOff" , "10000"  ) ; //  
    // track types:
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Velo     ,
                    LHCb::Track::Long     ,
                    LHCb::Track::Upstream ) ) ;
  } ;
  /// protected vertual destructor
  virtual ~BremChi22ID(){}; 
private:
  BremChi22ID () ;
  BremChi22ID           ( const BremChi22ID& ) ;
  BremChi22ID& operator=( const BremChi22ID& ) ;
} ;
// ============================================================================
//// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY ( BremChi22ID ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================







