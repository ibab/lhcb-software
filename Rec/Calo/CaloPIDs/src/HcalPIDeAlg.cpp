// $Id: HcalPIDeAlg.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
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
/** @class HcalPIDeAlg  HcalPIDeAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class HcalPIDeAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<HcalPIDeAlg> ;
protected:
  /// Standard protected constructor
  HcalPIDeAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::HcalE  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::HcalPIDe  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString(  5 * Gaudi::Units::GeV ) );
    setProperty ( "pScale" , Gaudi::Utils::toString( 50 * Gaudi::Units::GeV ) );
    setProperty ( "Histogram" , "CaloPIDs/Calo/HcalPIDe/5" ) ;
  };
  /// virtual protected destructor 
  virtual ~HcalPIDeAlg() {};
private:
  HcalPIDeAlg () ;
  HcalPIDeAlg           ( const  HcalPIDeAlg& ) ;
  HcalPIDeAlg& operator=( const  HcalPIDeAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( HcalPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
