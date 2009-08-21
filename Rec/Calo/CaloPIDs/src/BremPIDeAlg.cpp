// $Id: BremPIDeAlg.cpp,v 1.7 2009-08-21 16:49:45 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.5  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.4  2007/05/26 17:13:45  vegorych
// new PIDs from DC06 and BremPID added
//
// Revision 1.3  2006/11/06 11:05:24  vegorych
//  New PID tuning for DC06
//
// Revision 1.2  2006/06/22 15:40:39  ibelyaev
//  fix incinsistencied for 'Brem'-algorithms
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloID2DLL.h" 
// ============================================================================
/** @class BremPIDeAlg  BremPIDeAlg.cpp 
 *  The preconfigured instance of class CaloID2DLL 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-18
 */
// ============================================================================
class BremPIDeAlg : public CaloID2DLL 
{
  // friend factory for instantiation 
  friend class AlgFactory<BremPIDeAlg> ;
protected:
  /// Standard protected constructor
  BremPIDeAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloID2DLL ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Input"  , CaloIdLocation( "BremChi2" , context() ) );
    _setProperty ( "Output" , CaloIdLocation( "BremPIDe" , context() ) );    

    _setProperty ( "nVlong" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nVvelo" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nVupstr" , Gaudi::Utils::toString( 200 ) );
    _setProperty ( "nMlong" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    _setProperty ( "nMvelo" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    _setProperty ( "nMupstr" , Gaudi::Utils::toString(  50 * Gaudi::Units::GeV ) );
    _setProperty ( "HistogramU" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    _setProperty ( "HistogramL" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    _setProperty ( "HistogramV" , "CaloPIDs/CALO/BREMPIDE/h3" ) ;
    // track types 
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
  };
  /// virtual protected destructor 
  virtual ~BremPIDeAlg() {};
private:
  BremPIDeAlg () ;
  BremPIDeAlg           ( const  BremPIDeAlg& ) ;
  BremPIDeAlg& operator=( const  BremPIDeAlg& ) ;
} ;    
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( BremPIDeAlg );
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
