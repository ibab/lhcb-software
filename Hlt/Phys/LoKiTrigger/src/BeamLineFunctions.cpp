// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartDataPtr.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Constants.h"
#include "LoKi/BeamLineFunctions.h"
// ============================================================================
/** @file 
 *  Collection of "beam-line"-related functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 *
 *                    $Revision:$
 *  Last Modification $Date:$ 
 *                 by $Author:$ 
 */
// ============================================================================
// Constructor from velo-detector name 
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho ( const std::string& name ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_velo    ( name ) 
  , m_veloDet (      ) 
{
  //
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  Assert ( 0 != svc      , "Unable to reach LoKi Service!"  ) ;
  SmartIF<ISvcLocator> loc ( svc ) ;
  Assert ( loc.isValid() , "Unable to reach ServiceLocator" ) ;
  //
  SmartIF<IDataProviderSvc> det = 
    loc->service<IDataProviderSvc>( "DetectorDataSvc" , true ) ;
  //
  Assert ( det.isValid() , "Unable to reach Detector Service" ) ;
  //
  SmartDataPtr<DeVelo> veloDet (  det , name ) ;
  Assert ( !(!veloDet) , "Unable to locate DETECTOR='" + name + "'" ) ;
  //
  m_veloDet = veloDet ;
}  
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::BeamSpotRho::~BeamSpotRho(){}
// ============================================================================
// MANDATOTY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::BeamSpotRho*
LoKi::Vertices::BeamSpotRho::clone() const 
{ return new LoKi::Vertices::BeamSpotRho ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Vertices::BeamSpotRho::result_type 
LoKi::Vertices::BeamSpotRho::operator() 
  ( LoKi::Vertices::BeamSpotRho::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("invalid argument, return InvalidDistance");
    return LoKi::Constants::InvalidDistance ;
  }
  //
  Assert ( !(!m_veloDet) , "Invalid Detector!" ) ;
  //
  Warning  ( "Not yet implemented, return +100" ) ;
  //
  //
  //
  return +100 ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Vertices::BeamSpotRho::fillStream( std::ostream& s ) const 
{ 
  std::string _name = "";
  if ( DeVeloLocation::Default != m_velo ) { _name = "'" + m_velo + "'" ; }
  return s << "VX_BEAMSPOTRHO(" << _name << ")" ;
}








// ============================================================================
// The END 
// ============================================================================


