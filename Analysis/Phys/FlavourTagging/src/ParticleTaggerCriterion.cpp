// $Id: ParticleTaggerCriterion.cpp,v 1.1.1.1 2002-05-23 23:25:51 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Units/SystemOfUnits.h"

// local
#include "ParticleTaggerCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleTaggerCriterion
//
// 14/03/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static const ToolFactory<ParticleTaggerCriterion> s_factory;
const IToolFactory& ParticleTaggerCriterionFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleTaggerCriterion::ParticleTaggerCriterion( const std::string &type,
                                                  const std::string &name,
                                                  const IInterface *parent )
  : AlgTool( type, name, parent ), m_ppSvc(0)
{
  declareInterface<IFilterCriterion>(this);

  declareProperty("ParticlePropertyService",
                  m_ppSvcName = "ParticlePropertySvc");
  declareProperty("ParticleName", m_PartName = "");
  declareProperty("MinConfLevel", m_CLMin = -1);
  declareProperty("MaxConfLevel", m_CLMax = -1);
  declareProperty("MinTransverseMomentum", m_PtMin = -1);
  declareProperty("MaxTransverseMomentum", m_PtMax = -1);
  declareProperty("MinMomentum", m_PMin = -1);
  declareProperty("MaxMomentum", m_PMax = -1);
  declareProperty("MinVeloZ", m_ZMin = -2*meter);
  declareProperty("MaxVeloZ", m_ZMax = 1*meter);
}

StatusCode ParticleTaggerCriterion::initialize()
{
  MsgStream log(msgSvc(), name());
  if( serviceLocator() )
    serviceLocator()->service(m_ppSvcName, m_ppSvc);
  if( !m_ppSvc )
  {
    log << MSG::FATAL << "ParticlePropertySvc named '" << m_ppSvcName
        << "' could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  ParticleProperty *prop = m_ppSvc->find(m_PartName);
  if( prop )
  {
    m_PartCode = abs(prop->jetsetID());
    return StatusCode::SUCCESS;
  }
  log << MSG::FATAL << "The particle named '" << m_PartName
      << "' could not be found by the ParticlePropertySvc." << endreq;
  return StatusCode::FAILURE;
}

//=============================================================================

bool ParticleTaggerCriterion::isSatisfied( const Particle * const &cpart )
{
  Particle *part = const_cast<Particle *>(cpart);
  return( (part->particleID().abspid() == m_PartCode) &&
  //return( (abs(part->particleID().id()) == m_PartCode) &&
          ((m_CLMin < 0) || (part->confLevel() > m_CLMin)) &&
          ((m_CLMax < 0) || (part->confLevel() < m_CLMax)) &&
          ((m_PtMin < 0) || (part->pt() > m_PtMin)) &&
          ((m_PtMax < 0) || (part->pt() < m_PtMax)) &&
          ((m_PMin  < 0) || (part->p() > m_PMin)) &&
          ((m_PMax  < 0) || (part->p() < m_PMax)) &&
          (m_ZMin < part->pointOnTrack().z()) &&
          (part->pointOnTrack().z() < m_ZMax) );
}

bool ParticleTaggerCriterion::operator()( const Particle * const &cpart )
{
  return isSatisfied(cpart);
}
