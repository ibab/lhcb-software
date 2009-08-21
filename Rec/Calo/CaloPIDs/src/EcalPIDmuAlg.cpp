// $Id: EcalPIDmuAlg.cpp,v 1.5 2009-08-21 16:49:45 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.3  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.2  2006/11/06 11:05:24  vegorych
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
/** @class EcalPIDmuAlg  EcalPIDmuAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class EcalPIDmuAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<EcalPIDmuAlg> ;
protected:
  /// Standard protected constructor
  EcalPIDmuAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {


    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "EcalE"     , context() ) ) ;
    _setProperty ( "Output" , CaloIdLocation( "EcalPIDmu" , context() ) ) ;

    _setProperty ( "nVlong" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVdown" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nVTtrack" ,   Gaudi::Utils::toString( 5 * Gaudi::Units::GeV ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    _setProperty ( "nMdown" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    _setProperty ( "nMTtrack" , Gaudi::Utils::toString(  25 * Gaudi::Units::GeV ) );
    _setProperty ( "HistogramL" , "CaloPIDs/CALO/ECALPIDM/h3" ) ;
    _setProperty ( "HistogramD" , "CaloPIDs/CALO/ECALPIDM/h5" ) ;
    _setProperty ( "HistogramT" , "CaloPIDs/CALO/ECALPIDM/h6" ) ;
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
  };
  /// virtual protected destructor 
  virtual ~EcalPIDmuAlg() {};
private:
  EcalPIDmuAlg () ;
  EcalPIDmuAlg           ( const  EcalPIDmuAlg& ) ;
  EcalPIDmuAlg& operator=( const  EcalPIDmuAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( EcalPIDmuAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
