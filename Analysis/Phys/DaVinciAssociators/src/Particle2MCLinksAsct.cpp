// $Id: Particle2MCLinksAsct.cpp,v 1.9 2004-08-03 15:32:59 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// from event model
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

// local
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCLinksAsct
//
// 07/02/2003 : Philippe Charpentier
//-----------------------------------------------------------------------------

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCLinksAsct::initialize() {

  MsgStream msg(msgSvc(), name());
  Asct::initialize();
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  StatusCode sc = StatusCode::SUCCESS;
  if( location() == "" ) {
    m_hasTable = false;
    msg << MSG::VERBOSE 
        << "    This associator is direct, without Relations table" 
        << endreq;
    // Create the temporary table
    m_table = new Table;
    const Algorithm* myAlg = 
      dynamic_cast<const Algorithm*>( this->parent());
    std::vector<std::string> chargedPPLocation;
    std::vector<std::string> neutralPPLocation;
    chargedPPLocation.push_back( ProtoParticleLocation::Charged );
    chargedPPLocation.push_back( ProtoParticleLocation::Upstream );
    neutralPPLocation.push_back( ProtoParticleLocation::Neutrals );
    // Create a helper class for each type of Protoparticles
    m_chargedLink = new Object2MCLink( myAlg, Particle2MCMethod::ChargedPP,
                                         chargedPPLocation);
    m_neutralLink = new Object2MCLink( myAlg, Particle2MCMethod::NeutralPP,
                                         neutralPPLocation);
  } else {
    m_hasTable = true;
    m_table = 0;
  }
  return sc;
}

StatusCode Particle2MCLinksAsct::finalize() {
  if( m_table) delete m_table;
  if( NULL != m_chargedLink ) delete m_chargedLink;
  if( NULL != m_neutralLink ) delete m_neutralLink;
  m_chargedLink = NULL;
  m_neutralLink = NULL;
  return StatusCode::SUCCESS;
}

void Particle2MCLinksAsct::handle(const Incident& incident){
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "Entering handle()" << endreq;
  if( m_table ) m_table->clear();
  AssociatorWeighted<Particle,MCParticle,double>::handle(incident);
}
//=============================================================================
// Interface implementation
//=============================================================================
bool Particle2MCLinksAsct::tableExists() const{
  return m_hasTable ? Asct::tableExists() : NULL != m_table;
  
};

MCsFromParticleLinks Particle2MCLinksAsct::rangeFrom(const From& part) const
{
  if( m_hasTable ) return Asct::rangeFrom( part );

  insertRange( part );
  return m_table->relations(part);
}

MCsFromParticleLinks 
Particle2MCLinksAsct::rangeWithLowCutFrom(const From& part, 
                                          double cut) const
{
  if( m_hasTable ) return Asct::rangeWithLowCutFrom( part, cut );

  insertRange( part );
  return m_table->relations(part, cut, true);
}

MCsFromParticleLinks 
Particle2MCLinksAsct::rangeWithHighCutFrom(const From& part, 
                                           double cut) const
{
  if( m_hasTable ) return Asct::rangeWithHighCutFrom( part, cut );

  insertRange( part );
  return m_table->relations(part, cut, false);
}

Particle2MCLinksAsct::To 
Particle2MCLinksAsct::associatedFrom(const From& part) const
{
  if( m_hasTable ) return Asct::associatedFrom( part );

  insertRange( part );
  MCsFromParticleLinks r = m_table->relations(part);
  if( r.empty() ) {
    return To();
  }
  return r.begin()->to();
}

Particle2MCLinksAsct::To 
Particle2MCLinksAsct::associatedFrom(const From& part, 
                                     double& weight) const
{
  if( m_hasTable ) return Asct::associatedFrom( part, weight );

  insertRange( part );
  MCsFromParticleLinks r = m_table->relations(part);
  if( r.empty() ) {
    weight = 0.;
    return To();
  }
  weight = r.begin()->weight();
  return r.begin()->to();
}

void
Particle2MCLinksAsct::insertRange( const From& part ) const
{
  MCsFromParticleLinks r = m_table->relations(part);
  MsgStream  msg( msgSvc(), name() );
  if( r.empty()) {
    Object2MCLink* link = 
      part->charge() ? m_chargedLink : m_neutralLink;
    // Local implementation...
    msg << MSG::VERBOSE << "    " 
        << (part->charge() ? "Charged" : "Neutral")
        << " particle " << objectName(part);
    const ProtoParticle* protoPart = 
      dynamic_cast<const  ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      MCParticle* mcPart = link->first(protoPart);
      msg << " from ProtoParticle " << protoPart->key()
          << (NULL == mcPart ? " not " : " ")
          << "associated with MCparticles" ;
      while( mcPart ) {
        msg << " - " << mcPart->key();
        m_table->relate( part, mcPart, link->weight());
        mcPart = link->next();
      }
      msg << endreq;
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  } else {
    msg << MSG::VERBOSE << "    Range already defined for Particle "
        << objectName(part) << endreq;
  }
}

//=============================================================================
