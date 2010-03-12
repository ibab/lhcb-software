// $Id: HcalPIDmuAlg.cpp,v 1.6 2010-03-12 21:51:41 dgolubko Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2009/08/21 16:49:45  odescham
// implement generic context-dependent TES I/O
//
// Revision 1.4  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.3  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.2  2006/11/06 11:05:25  vegorych
//  New PID tuning for DC06
//
// Revision 1.1  2006/06/18 18:35:28  ibelyaev
//  the firstcommmit for DC06 branch
// 
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
DECLARE_ALGORITHM_FACTORY( HcalPIDmuAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
