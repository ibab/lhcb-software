// $Id: MassDifferenceFilterCriterion.cpp,v 1.4 2004-08-12 12:33:52 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

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
  : GaudiTool ( type, name , parent ),
    m_massOffset(0),
    m_parID(0),
    m_dauID(0)
 {

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

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;

  IParticlePropertySvc* ppSvc = 0; 
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service" << endmsg;
    return sc;
  }

  ParticleProperty* mother = ppSvc->find( m_motherName );
  if (mother==0) {
    fatal() << "    Unable to locate Particle Property for "  << m_motherName << endmsg;
    return StatusCode::FAILURE;
  }
  m_parID = abs(mother->jetsetID());

  ParticleProperty* daughter = ppSvc->find( m_daughterName );
  if (daughter==0) {
    fatal() << "    Unable to locate Particle Property for "  << m_daughterName << endmsg;
    return StatusCode::FAILURE;
  }
  m_dauID = abs(daughter->jetsetID());

  m_massOffset = mother->mass()-daughter->mass();

  debug() << ">>>   MassDifferenceFilterCriterion::initialize() " 
      << endmsg;
  debug() << ">>>   Cuts are " << endmsg;
  debug() << ">>>   MotherName       " << m_motherName << endmsg;    
  debug() << ">>>   DaughterName     " << m_daughterName << endmsg;    
  debug() << ">>>   MassWindow       " << m_massWindow << endmsg;    
  debug() << ">>>   (computed) MassOffset " << m_massOffset << endmsg;    
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
      err() << " # of " << m_daughterName << " daughters = " << p.size() << endmsg;
      return false;
   }
   
   double massDifference = part->mass()-p[0]->mass();
   verbose() << "Mass of mother " << part->mass() << " - dauhter " << p[0]->mass() 
             << " = " << massDifference << endreq ;
   return fabs(massDifference-m_massOffset) < m_massWindow;
}
