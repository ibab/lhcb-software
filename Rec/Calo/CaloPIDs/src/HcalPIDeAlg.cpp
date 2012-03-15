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


    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "HcalE"    , context() ) ) ;
    _setProperty ( "Output" , CaloIdLocation( "HcalPIDe" , context() ) ) ;    

    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );

    _setProperty ( "HistogramL"     , "DLL_Long" ) ;
    _setProperty ( "HistogramD"     , "DLL_Downstream" ) ;
    _setProperty ( "HistogramT"     , "DLL_Ttrack" ) ;
    _setProperty ( "ConditionName"  , "Conditions/ParticleID/Calo/HcalPIDe" ) ;


    _setProperty ( "HistogramL_THS" , "CaloPIDs/CALO/HCALPIDE/h3" ) ;
    _setProperty ( "HistogramD_THS" , "CaloPIDs/CALO/HCALPIDE/h5" ) ;
    _setProperty ( "HistogramT_THS" , "CaloPIDs/CALO/HCALPIDE/h6" ) ;
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
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
DECLARE_ALGORITHM_FACTORY( HcalPIDeAlg )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
