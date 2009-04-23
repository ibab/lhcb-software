// $Id
// $Id: Pi0MakerBase.cpp,v 1.1 2009-04-23 10:39:31 pkoppenb Exp $
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h" 
// local
#include "Pi0MakerBase.h"

/** @file 
 *  
 *  Implementation file for class : Pi0MakerBase
 *
 *  @author P.Koppenburg 2009-03-23
 *
 */
// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
Pi0MakerBase::Pi0MakerBase
( const std::string& name,ISvcLocator* pSvcLocator  )
  : ParticleMakerBase           ( name, pSvcLocator ) 
    , m_point            () 
    , m_pointErr         ()
{
  // Filter 
  declareProperty( "MassWindow"     , m_MassWin = 30. * Gaudi::Units::MeV);
  declareProperty( "PtCut"          , m_PtCut = 0. * Gaudi::Units::MeV);
  // new default
  declareProperty( "Particle"       , m_pid = "pi0");
  //
  m_point = Gaudi::XYZPoint();
  m_pointErr = Gaudi::SymMatrix3x3();
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
Pi0MakerBase::~Pi0MakerBase() {};
// ============================================================================

StatusCode Pi0MakerBase::initialize    ()
{
  // initialize the base class
  StatusCode sc = ParticleMakerBase::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize ParticleMakerBase",sc);}

  const LHCb::ParticleProperty* partProp;
  partProp  = ppSvc()->find( m_pid );
  if( 0 == partProp){
    Error("Requested particle '" + m_pid + "' is unknown").ignore();
    return StatusCode::FAILURE;
  }
  m_Id      = (*partProp).particleID().pid();
  m_Mass = (*partProp).mass();
  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

StatusCode Pi0MakerBase::finalize      ()
{
  info() << " - Pi0MakerBase Summary -----" << endreq;
  info() << " Created : " << (float) m_count[1]/m_count[0] << " Resolved " << m_pid << "per event" << endreq;
  info() << " --------------------------------" << endreq;
  // finalize the base class
  return ParticleMakerBase::finalize ();
};

