// $Id: EcalChi22ID.cpp,v 1.3 2007-05-31 14:47:22 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// Local 
// ============================================================================
#include "CaloChi22ID.h"
#include "ToVector.h"
// ============================================================================
/** @class EcalChi22ID EcalChi22D.cpp
 *  The preconfigured instance of class CaloChi22ID 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class EcalChi22ID: public CaloChi22ID 
{
  /// friend factory for instantiation 
  friend class AlgFactory<EcalChi22ID>;
protected:
  /// Standard protected constructor
  EcalChi22ID 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    :  CaloChi22ID ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::ElectronMatch ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::EcalChi2      ) ;
    // @todo it must be in agrement with "Threshold" for ElectonMatchAlg 
    setProperty ( "CutOff" , "10000"  ) ; //  
    // track types:
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
  } ;
  /// protected vertual destructor
  virtual ~EcalChi22ID(){}; 
private:
  EcalChi22ID () ;
  EcalChi22ID           ( const EcalChi22ID& ) ;
  EcalChi22ID& operator=( const EcalChi22ID& ) ;
} ;
// ============================================================================
//// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY ( EcalChi22ID ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================







