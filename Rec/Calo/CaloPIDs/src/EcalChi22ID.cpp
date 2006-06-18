// $Id: EcalChi22ID.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Local 
// ============================================================================
#include "CaloChi22ID.h"
// ============================================================================
/** @class EcalChi22ID EcalChi22ID.h
 *  The preconfigured instance of class CaloChi22ID 
 *  @suthor Vanya BELYAEV ibelyaev@physics.syr.edu
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







