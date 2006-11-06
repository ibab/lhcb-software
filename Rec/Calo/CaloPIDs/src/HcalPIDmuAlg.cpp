// $Id: HcalPIDmuAlg.cpp,v 1.2 2006-11-06 11:05:25 vegorych Exp $
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
    setProperty ( "Input"  , LHCb::CaloIdLocation::HcalE  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::HcalPIDmu  ) ;
    setProperty ( "nVlong" ,   Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    setProperty ( "nVdown" ,   Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    setProperty ( "nVTtrack" , Gaudi::Utils::toString( 10 * Gaudi::Units::GeV ) );
    setProperty ( "nMlong" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    setProperty ( "nMdown" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    setProperty ( "nMTtrack" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    setProperty ( "HistogramL" , "CaloPIDs/CALO/HCALPIDM/h3" ) ;
    setProperty ( "HistogramD" , "CaloPIDs/CALO/HCALPIDM/h5" ) ;
    setProperty ( "HistogramT" , "CaloPIDs/CALO/HCALPIDM/h6" ) ;
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
