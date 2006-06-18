// $Id: EcalPIDmuAlg.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
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
/** @class HcalPIDmuAlg  HcalPIDmuAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class HcalPIDmuAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<HcalPIDmuAlg> ;
protected:
  /// Standard protected constructor
  HcalPIDmuAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::HcalE  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::HcalPIDmu  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) ) ;
    setProperty ( "pScale" , Gaudi::Utils::toString( 25 * Gaudi::Units::GeV ) ) ;
    setProperty ( "Histogram" , "CaloPIDs/Calo/HcalPIDm/u/5" ) ;
  };
  /// virtual protected destructor 
  virtual ~HcalPIDmuAlg() {};
private:
  HcalPIDmuAlg () ;
  HcalPIDmuAlg           ( const  HcalPIDmuAlg& ) ;
  HcalPIDmuAlg& operator=( const  HcalPIDmuAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( HcalPIDmuAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
