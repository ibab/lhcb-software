///
#include <string>
#include <vector>
#include <list>
///
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IGiGaSvc.h"
#include "GaudiKernel/AlgFactory.h"
/// from Geant4 
#include "G4Event.hh"
/// local 
#include "GiGaMiscGetEventAlg.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
static const  AlgFactory<GiGaMiscGetEventAlg>                  s_Factory;
const        IAlgFactory&         GiGaMiscGetEventAlgFactory = s_Factory;
////////////////////////////////////////////////////////////////////////////////////////////////////
GiGaMiscGetEventAlg::GiGaMiscGetEventAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  ///
  : Algorithm( name , pSvcLocator) 
  ///
  , m_GiGaSvcName ( "GiGaSvc" )
  ///
  , m_gigaSvc     (  0        )
  ///
{};
//////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMiscGetEventAlg::initialize() 
{
  ///
  StatusCode st = serviceLocator()->service( "GiGaSvc", m_gigaSvc );
  if( st.isFailure() ) { return StatusCode::FAILURE; } 
  if( 0 == gigaSvc() ) { return StatusCode::FAILURE; } 
  ///
  gigaSvc()->addRef();
  ///
  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMiscGetEventAlg::execute() 
{
  ///
  G4Event* evt = 0 ;
  ///
  *gigaSvc() >> evt ; 
  ///
  return StatusCode::SUCCESS;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMiscGetEventAlg::finalize() 
{ return StatusCode::SUCCESS; }
////////////////////////////////////////////////////////////////////////////////////////////////////////






