// $Id: HcalPIDeAlg.cpp,v 1.3 2007-08-24 21:25:18 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
    _setProperty ( "Input"  , LHCb::CaloIdLocation::HcalE  ) ;
    _setProperty ( "Output" , LHCb::CaloIdLocation::HcalPIDe  ) ;
    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "HistogramL" , "CaloPIDs/CALO/HCALPIDE/h3" ) ;
    _setProperty ( "HistogramD" , "CaloPIDs/CALO/HCALPIDE/h5" ) ;
    _setProperty ( "HistogramT" , "CaloPIDs/CALO/HCALPIDE/h6" ) ;
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
DECLARE_ALGORITHM_FACTORY( HcalPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
