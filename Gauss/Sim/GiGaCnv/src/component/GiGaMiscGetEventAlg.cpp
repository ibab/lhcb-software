// $Id: GiGaMiscGetEventAlg.cpp,v 1.9 2002-05-01 18:33:18 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//  ===========================================================================
#define GIGACNV_GIGAMISCGETEVENTALG_CPP
// ============================================================================
/// STL
#include <string>
#include <vector>
#include <list>
/// GaudiKernel 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/IGiGaSvc.h"
/// from LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
/// local 
#include "GiGaMiscGetEventAlg.h"


// ============================================================================
/** @file GiGaMiscGetEventAlg.cpp
 *  
 *  implementation of class GiGaMiscGetEvent 
 *
 *  @author Vany aBelyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** mandatory factory stuff
 */
// ============================================================================
static const  AlgFactory<GiGaMiscGetEventAlg>         s_Factory;
const        IAlgFactory&GiGaMiscGetEventAlgFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param name name of teh algorithm 
 *  @param SvcLoc pointer to service locator 
 */
// ============================================================================
GiGaMiscGetEventAlg::GiGaMiscGetEventAlg(const std::string& name, 
                                         ISvcLocator* pSvcLocator) 
  ///
  : Algorithm( name , pSvcLocator) 
  , m_particles   ( MCParticleLocation::Default )
  , m_vertices    ( MCVertexLocation::Default   )
{ 
  declareProperty( "Particles" , m_particles  ); 
  declareProperty( "Vertices"  , m_vertices   ); 
};

// ============================================================================
/** destructor 
 */
// ============================================================================
GiGaMiscGetEventAlg::~GiGaMiscGetEventAlg(){};

// ============================================================================
/** the standard Algorithm initialization method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode GiGaMiscGetEventAlg::initialize() 
{ return StatusCode::SUCCESS; };

// ============================================================================
/** the standard Algorithm execution method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode GiGaMiscGetEventAlg::execute() 
{
  ///
  typedef MCParticles Particles ;
  typedef MCVertices  Vertices  ;
  ///
  MsgStream log( msgSvc() , name() ) ;
  if( !m_particles.empty() )
    {
      SmartDataPtr<Particles> obj( eventSvc() , m_particles ) ;
      if( obj ) 
        {
          log << MSG::INFO
              << "Number of extracted particles '"
              << m_particles << "' \t"
              << obj->size() 
              << endreq ;
          Stat stat( chronoSvc() , "#particles" , obj->size() ) ; 
        } 
      else 
        { 
          log << MSG::ERROR 
              << " Could not extract 'Particles' from '"
              << m_particles << "'" 
              << endreq ;
          ///
          return StatusCode::FAILURE;
        } 
    }
  ///
  if( !m_vertices.empty() )
    {
      SmartDataPtr<Vertices> obj( eventSvc() , m_vertices ) ;
      if( obj ) 
        { 
          log << MSG::INFO
              << "Number of extracted vertices  '"
              << m_vertices << "'  \t" 
              << obj->size() 
              << endreq ;
          Stat stat( chronoSvc() , "#vertices" , obj->size() ) ; 
        } 
      else 
        { 
          log << MSG::ERROR 
              << " Could not extract 'Vertices' from '"
              << m_vertices << "' \t" 
              << endreq ;
          ///
          return StatusCode::FAILURE;
        } 
    }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** the standard Algorithm finalization method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode GiGaMiscGetEventAlg::finalize()
{ return StatusCode::SUCCESS; }

// ============================================================================
// The End 
// ============================================================================








