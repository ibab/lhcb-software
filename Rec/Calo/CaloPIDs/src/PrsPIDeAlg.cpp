// $Id: PrsPIDeAlg.cpp,v 1.2 2006-11-06 11:05:25 vegorych Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
    setProperty ( "Input"  , LHCb::CaloIdLocation::PrsE     ) ;
    setProperty ( "Output" , LHCb::CaloIdLocation::PrsPIDe  ) ;
    setProperty ( "nVlong" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    setProperty ( "nVdown" ,   Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    setProperty ( "nVTtrack" , Gaudi::Utils::toString( 100 * Gaudi::Units::MeV ) );
    setProperty ( "nMlong" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMdown" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "nMTtrack" , Gaudi::Utils::toString(  100 * Gaudi::Units::GeV ) );
    setProperty ( "HistogramL" , "CaloPIDs/CALO/PRSPIDE/h3" ) ;
    setProperty ( "HistogramD" , "CaloPIDs/CALO/PRSPIDE/h5" ) ;
    setProperty ( "HistogramT" , "CaloPIDs/CALO/PRSPIDE/h6" ) ;
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
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
