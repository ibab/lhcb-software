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

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "BremChi2" , context() ) );
    _setProperty ( "Output" , CaloIdLocation( "BremPIDe" , context() ) );    

    _setProperty ( "nVlong" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nVvelo" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nVupstr" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    _setProperty ( "nMvelo" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    _setProperty ( "nMupstr" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );

    _setProperty ( "HistogramU"     , "DLL_Long" ) ;
    _setProperty ( "HistogramL"     , "DLL_Long" ) ;
    _setProperty ( "HistogramV"     , "DLL_Long" ) ;
    _setProperty ( "ConditionName"  , "Conditions/ParticleID/Calo/BremPIDe" ) ;


    _setProperty ( "HistogramU_THS" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    _setProperty ( "HistogramL_THS" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    _setProperty ( "HistogramV_THS" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    // track types 
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
DECLARE_ALGORITHM_FACTORY( BremPIDeAlg )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
