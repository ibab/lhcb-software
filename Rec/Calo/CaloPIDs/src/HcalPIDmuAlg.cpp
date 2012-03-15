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



    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "HcalE"     , context() ) ) ;
    _setProperty ( "Output" , CaloIdLocation( "HcalPIDmu" , context() ) ) ;

    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );

    _setProperty ( "HistogramL"     , "DLL_Long" ) ;
    _setProperty ( "HistogramD"     , "DLL_Downstream" ) ;
    _setProperty ( "HistogramT"     , "DLL_Ttrack" ) ;
    _setProperty ( "ConditionName"  , "Conditions/ParticleID/Calo/HcalPIDmu" ) ;

    _setProperty ( "HistogramL_THS" , "CaloPIDs/CALO/HCALPIDM/h3" ) ;
    _setProperty ( "HistogramD_THS" , "CaloPIDs/CALO/HCALPIDM/h5" ) ;
    _setProperty ( "HistogramT_THS" , "CaloPIDs/CALO/HCALPIDM/h6" ) ;
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
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
DECLARE_ALGORITHM_FACTORY( HcalPIDmuAlg )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
