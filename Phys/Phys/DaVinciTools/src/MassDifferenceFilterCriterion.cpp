// $Id: MassDifferenceFilterCriterion.cpp,v 1.1 2002-11-14 12:35:35 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "Event/ParticleID.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "MassDifferenceFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MassDifferenceFilterCriterion
//
// 13/11/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MassDifferenceFilterCriterion>          s_factory ;
const        IToolFactory& MassDifferenceFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MassDifferenceFilterCriterion::MassDifferenceFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "Window", m_massWindow = 100000. );  
  declareProperty( "DaughterName", m_daughterName);
  declareProperty( "MotherName", m_motherName);
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode MassDifferenceFilterCriterion::initialize() {

  MsgStream          log( msgSvc(), name() );
  IParticlePropertySvc* ppSvc = 0; 
  StatusCode sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endmsg;
    return sc;
  }

  ParticleProperty* mother = ppSvc->find( m_motherName );
  if (mother==0) {
    log << MSG::FATAL << "    Unable to locate Particle Property for "  << m_motherName
        << endmsg;
    return StatusCode::FAILURE;
  }
  m_parID = abs(mother->jetsetID());

  ParticleProperty* daughter = ppSvc->find( m_daughterName );
  if (daughter==0) {
    log << MSG::FATAL << "    Unable to locate Particle Property for "  << m_daughterName
        << endmsg;
    return StatusCode::FAILURE;
  }
  m_dauID = abs(daughter->jetsetID());

  m_massOffset = mother->mass()-daughter->mass();

  log << MSG::DEBUG << ">>>   MassDifferenceFilterCriterion::initialize() " 
      << endmsg;
  log << MSG::DEBUG << ">>>   Cuts are " << endmsg;
  log << MSG::DEBUG << ">>>   MotherName       " << m_motherName << endmsg;    
  log << MSG::DEBUG << ">>>   DaughterName     " << m_daughterName << endmsg;    
  log << MSG::DEBUG << ">>>   MassWindow       " << m_massWindow << endmsg;    
  log << MSG::DEBUG << ">>>   (computed) MassOffset " << m_massOffset << endmsg;    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool MassDifferenceFilterCriterion::isSatisfied( const Particle* const & part ) {

   if (part->particleID().abspid()!=m_parID) return false;

   const Vertex* v = part->endVertex();
   if (v==0) return false;

   ParticleVector p;
   const SmartRefVector<Particle>& pv = v->products();
   for (SmartRefVector<Particle>::const_iterator i=pv.begin();i!=pv.end();++i) {
     int pid = (*i)->particleID().abspid();
     if (pid==m_dauID)  p.push_back(const_cast<Particle*>(i->target()));
   }
   if (p.size()!=1) {
      MsgStream          log( msgSvc(), name() );
      log << MSG::ERROR << " # of " << m_daughterName << " daughters = " << p.size() << endmsg;
      return false;
   }
   
   double massDifference = part->mass()-p[0]->mass();
   return fabs(massDifference) < m_massWindow;
}
