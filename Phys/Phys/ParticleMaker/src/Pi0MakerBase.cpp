// $Id
// $Id: Pi0MakerBase.cpp,v 1.2 2010-04-14 12:42:39 odescham Exp $
// ============================================================================
// Include files
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
  , m_Id              (0)
  , m_Mass            (0)
  , m_point            ()
  , m_pointErr         ()
{
  // Filter
  declareProperty( "MassWindow"     , m_MassWin = 30. * Gaudi::Units::MeV);
  declareProperty( "PtCut"          , m_PtCut = 0. * Gaudi::Units::MeV);
  setProperty    ( "Particle"       , "pi0");
  declareProperty( "FirstPVasOrigin", m_setPV = false);
  // 
  //
  m_point = Gaudi::XYZPoint();
  m_pointErr = Gaudi::SymMatrix3x3();
  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
Pi0MakerBase::~Pi0MakerBase() {}
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
  m_Id   = (*partProp).particleID().pid();
  m_Mass = (*partProp).mass();
  if( m_setPV )info() << "Pi0 momentum is evaluated assuming its origin is the 1st primary vertex (when exist)" << endmsg;
  return StatusCode::SUCCESS ;
}
// ============================================================================

StatusCode Pi0MakerBase::finalize      ()
{

  double f = (m_count[0] == 0) ? 0. :  (float) m_count[1]/ (float) m_count[0];
  info() << " - Pi0MakerBase Summary -----" << endreq;
  info() << " Created : " << f << " " << m_pid << "per event" << endreq;
  info() << " --------------------------------" << endreq;
  // finalize the base class
  return ParticleMakerBase::finalize ();
}

