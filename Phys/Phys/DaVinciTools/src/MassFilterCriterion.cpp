// $Id: MassFilterCriterion.cpp,v 1.1 2003-04-29 18:38:54 gcorti Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Event/ParticleID.h"

// local
#include "MassFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MassFilterCriterion
//
// 22/06/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MassFilterCriterion>          s_factory ;
const        IToolFactory& MassFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MassFilterCriterion::MassFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "Window", m_massWindow = 1000000. );  
  declareProperty( "UpperWindow", m_upperMassWindow = 1000000. );  
  declareProperty( "LowerWindow", m_lowerMassWindow = 1000000. );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode MassFilterCriterion::initialize() {

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>>   MassFilterCriterion::initialize() " 
      << endreq;
  log << MSG::DEBUG << ">>>   Cuts are " << endmsg;
  log << MSG::DEBUG << ">>>   MassWindow       " << m_massWindow << endmsg;    
  log << MSG::DEBUG << ">>>   UpperMassWindow       " << m_massWindow << endmsg;    
  log << MSG::DEBUG << ">>>   LowerMassWindow       " << m_massWindow << endmsg;    
  // get pointer to ParticlePropertySvc
  m_ppSvc = 0;
  StatusCode sc = service( "ParticlePropertySvc", m_ppSvc, true );
  return sc;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool MassFilterCriterion::isSatisfied( const Particle* const & part ) {

  const ParticleID& id = part->particleID();
  const ParticleProperty *pp = m_ppSvc->findByStdHepID(id.pid());
  double d = part->mass() - pp->mass() ;
#ifdef WIN32
  return d >0 ?  d < std::_MIN(m_massWindow,m_upperMassWindow)
              : -d < std::_MIN(m_massWindow,m_lowerMassWindow);
#else
  return d >0 ?  d < std::min(m_massWindow,m_upperMassWindow)
              : -d < std::min(m_massWindow,m_lowerMassWindow);
#endif
}

//=============================================================================
