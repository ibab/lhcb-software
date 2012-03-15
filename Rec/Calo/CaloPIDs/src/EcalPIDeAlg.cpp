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

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "EcalChi2" , context() ) ) ;
    _setProperty ( "Output" , CaloIdLocation( "EcalPIDe" , context() ) ) ;
    
    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 2500 ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 2500 ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 2500 ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );

    _setProperty ( "HistogramL"     , "DLL_Long" ) ;
    _setProperty ( "HistogramD"     , "DLL_Downstream" ) ;
    _setProperty ( "HistogramT"     , "DLL_Ttrack" ) ;
    _setProperty ( "ConditionName"  , "Conditions/ParticleID/Calo/EcalPIDe" ) ;


    _setProperty ( "HistogramL_THS" , "CaloPIDs/CALO/ECALPIDE/h3" ) ;
    _setProperty ( "HistogramD_THS" , "CaloPIDs/CALO/ECALPIDE/h5" ) ;
    _setProperty ( "HistogramT_THS" , "CaloPIDs/CALO/ECALPIDE/h6" ) ;
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
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
DECLARE_ALGORITHM_FACTORY( EcalPIDeAlg )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
