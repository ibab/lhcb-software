// $Id: BremPIDeAlg.cpp,v 1.2 2006-06-22 15:40:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloID2DLL.h" 
// ============================================================================
/** @class BremPIDeAlg  BremPIDeAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class BremPIDeAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<BremPIDeAlg> ;
protected:
  /// Standard protected constructor
  BremPIDeAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {
    setProperty ( "Input"  , LHCb::CaloIdLocation::BremChi2  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::BremPIDe  ) ;
    setProperty ( "vScale" , Gaudi::Utils::toString( 200 ) );
    setProperty ( "pScale" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    setProperty ( "Histogram" , "CaloPIDs/Calo/BremPIDe/5" ) ;
    // track types 
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Velo     ,
                    LHCb::Track::Long     ,
                    LHCb::Track::Upstream ) ) ;
  };
  /// virtual protected destructor 
  virtual ~BremPIDeAlg() {};
private:
  BremPIDeAlg () ;
  BremPIDeAlg           ( const  BremPIDeAlg& ) ;
  BremPIDeAlg& operator=( const  BremPIDeAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( BremPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
