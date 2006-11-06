// $Id: HcalPIDeAlg.cpp,v 1.2 2006-11-06 11:05:25 vegorych Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
    setProperty ( "Input"  , LHCb::CaloIdLocation::HcalE  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::HcalPIDe  ) ;
    setProperty ( "nVlong" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    setProperty ( "nVdown" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    setProperty ( "nVTtrack" , Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "HistogramL" , "CaloPIDs/CALO/HCALPIDE/h3" ) ;
    setProperty ( "HistogramD" , "CaloPIDs/CALO/HCALPIDE/h5" ) ;
    setProperty ( "HistogramT" , "CaloPIDs/CALO/HCALPIDE/h6" ) ;
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
