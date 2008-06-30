// $Id: PrsPIDeAlg.cpp,v 1.4 2008-06-30 15:37:34 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2007/08/24 21:25:19  odescham
// fix uncheck. StatusCodes
//
// Revision 1.2  2006/11/06 11:05:25  vegorych
//  New PID tuning for DC06
//
// Revision 1.1  2006/06/18 18:35:29  ibelyaev
//  the firstcommmit for DC06 branch
// 
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
    if("HLT"==context()){
      _setProperty ( "Input"  , LHCb::CaloIdLocation::PrsEHlt     ) ;
      _setProperty ( "Output" , LHCb::CaloIdLocation::PrsPIDeHlt  ) ;
    }else{
      _setProperty ( "Input"  , LHCb::CaloIdLocation::PrsE     ) ;
      _setProperty ( "Output" , LHCb::CaloIdLocation::PrsPIDe  ) ;
    }
    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nVTtrack" , Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    _setProperty ( "HistogramL" , "CaloPIDs/CALO/PRSPIDE/h3" ) ;
    _setProperty ( "HistogramD" , "CaloPIDs/CALO/PRSPIDE/h5" ) ;
    _setProperty ( "HistogramT" , "CaloPIDs/CALO/PRSPIDE/h6" ) ;
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
DECLARE_ALGORITHM_FACTORY( PrsPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
