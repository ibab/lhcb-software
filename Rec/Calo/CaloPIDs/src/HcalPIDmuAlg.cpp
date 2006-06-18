// $Id: HcalPIDmuAlg.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
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
/** @class EcalPIDmuAlg  EcalPIDmuAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class EcalPIDmuAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<EcalPIDmuAlg> ;
protected:
  /// Standard protected constructor
  EcalPIDmuAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::EcalE  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::EcalPIDmu  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString(  5 * Gaudi::Units::GeV ) ) ;
    setProperty ( "pScale" , Gaudi::Utils::toString( 25 * Gaudi::Units::GeV ) ) ;
    setProperty ( "Histogram" , "CaloPIDs/Calo/EcalPIDm/u/5" ) ;
  };
  /// virtual protected destructor 
  virtual ~EcalPIDmuAlg() {};
private:
  EcalPIDmuAlg () ;
  EcalPIDmuAlg           ( const  EcalPIDmuAlg& ) ;
  EcalPIDmuAlg& operator=( const  EcalPIDmuAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( EcalPIDmuAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
