// $Id: PrsPIDeAlg.cpp,v 1.1 2006-06-18 18:35:29 ibelyaev Exp $
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
/** @class PrsPIDeAlg  PrsPIDeAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class PrsPIDeAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<PrsPIDeAlg> ;
protected:
  /// Standard protected constructor
  PrsPIDeAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::PrsE     ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::PrsPIDe  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    setProperty ( "pScale" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    setProperty ( "Histogram" , "CaloPIDs/Calo/PrsPIDe/5" ) ;
  };
  /// virtual protected destructor 
  virtual ~PrsPIDeAlg() {};
private:
  PrsPIDeAlg () ;
  PrsPIDeAlg           ( const  PrsPIDeAlg& ) ;
  PrsPIDeAlg& operator=( const  PrsPIDeAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( PrsPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
