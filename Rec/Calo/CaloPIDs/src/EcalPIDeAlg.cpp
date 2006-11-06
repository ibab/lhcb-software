// $Id: EcalPIDeAlg.cpp,v 1.2 2006-11-06 11:05:24 vegorych Exp $
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
    setProperty ( "Input"  , LHCb::CaloIdLocation::EcalChi2  ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::EcalPIDe  ) ;
    setProperty ( "nVlong" ,   Gaudi::Utils::toString( 2500 ) );
    setProperty ( "nVdown" ,   Gaudi::Utils::toString( 2500 ) );
    setProperty ( "nVTtrack" , Gaudi::Utils::toString( 2500 ) );
    setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "HistogramL" , "CaloPIDs/CALO/ECALPIDE/h3" ) ;
    setProperty ( "HistogramD" , "CaloPIDs/CALO/ECALPIDE/h5" ) ;
    setProperty ( "HistogramT" , "CaloPIDs/CALO/ECALPIDE/h6" ) ;
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
DECLARE_ALGORITHM_FACTORY( EcalPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
