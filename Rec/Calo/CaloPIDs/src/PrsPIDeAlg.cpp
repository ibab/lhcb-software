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


    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "PrsE" , context() ) ) ;
    _setProperty ( "Output" , CaloIdLocation( "PrsPIDe" , context() ) ) ;
    
    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );

    _setProperty ( "HistogramL"     , "DLL_Long" ) ;
    _setProperty ( "HistogramD"     , "DLL_Downstream" ) ;
    _setProperty ( "HistogramT"     , "DLL_Ttrack" ) ;
    _setProperty ( "ConditionName"  , "Conditions/ParticleID/Calo/PrsPIDe" ) ;

    _setProperty ( "HistogramL_THS" , "CaloPIDs/CALO/PRSPIDE/h3" ) ;
    _setProperty ( "HistogramD_THS" , "CaloPIDs/CALO/PRSPIDE/h5" ) ;
    _setProperty ( "HistogramT_THS" , "CaloPIDs/CALO/PRSPIDE/h6" ) ;

    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long       ,
                     LHCb::Track::Ttrack     ,
                     LHCb::Track::Downstream ) ) ;
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
DECLARE_ALGORITHM_FACTORY( PrsPIDeAlg )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
