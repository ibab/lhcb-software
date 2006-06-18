// $Id: EcalPIDeAlg.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloID2DLL.h" 
// ============================================================================
/** @class EcalPIDeAlg  EcalPIDeAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class EcalPIDeAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<EcalPIDeAlg> ;
protected:
  /// Standard protected constructor
  EcalPIDeAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::EcalChi2  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::EcalPIDe  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString( 100 ) );
    setProperty ( "pScale" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    setProperty ( "Histogram" , "CaloPIDs/Calo/EcalPIDe/5" ) ;
  };
  /// virtual protected destructor 
  virtual ~EcalPIDeAlg() {};
private:
  EcalPIDeAlg () ;
  EcalPIDeAlg           ( const  EcalPIDeAlg& ) ;
  EcalPIDeAlg& operator=( const  EcalPIDeAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( EcalPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
