/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:12  ibelyaev
/// the package restructurisation
/// 
//  ===========================================================================
#define GIGACNV_GIGAMISCGETEVENTALG_CPP
/// ===========================================================================
/// STL
#include <string>
#include <vector>
#include <list>
/// GaudiKernel 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
// GiGa 
#include "GiGa/IGiGaSvc.h"
/// from LHCbEvent 
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/MCVertex.h"
/// local 
#include "GiGaMiscGetEventAlg.h"


static const  AlgFactory<GiGaMiscGetEventAlg>         s_Factory;
const        IAlgFactory&GiGaMiscGetEventAlgFactory = s_Factory;

GiGaMiscGetEventAlg::GiGaMiscGetEventAlg(const std::string& name, 
                                         ISvcLocator* pSvcLocator) 
  ///
  : Algorithm( name , pSvcLocator) 
  ///
  , m_GiGaSvcName ( "GiGaSvc" )
  ///
  , m_gigaSvc     (  0        )
  ///
{};

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

StatusCode GiGaMiscGetEventAlg::execute() 
{
  ///
  {
    SmartDataPtr<MCParticleVector> 
      obj( eventSvc() , "/Event/G4/MCParticles" ) ;
    if( obj ) 
      { 
        std::cout <<  *obj << std::endl;  
	Stat stat( chronoSvc() , "#particles" , obj->size() ) ;
      } 
    else 
      { std::cout << " particles == 0 " << std::endl ; } 
    
  }
  ///
  {
    SmartDataPtr<MCVertexVector> 
      obj( eventSvc() , "/Event/G4/MCVertices" ) ;
    if( obj ) 
      { 
        std::cout <<  *obj << std::endl;  
	Stat stat( chronoSvc() , "#vertices" , obj->size() ) ;
      } 
    else 
      { std::cout << " vertices == 0 " << std::endl ; } 
  }
  ///
  return StatusCode::SUCCESS;
};

StatusCode GiGaMiscGetEventAlg::finalize() 
{ return StatusCode::SUCCESS; }







