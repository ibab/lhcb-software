// $Id: Particle2MCLinksAsct.cpp,v 1.4 2004-05-03 13:34:17 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgTool.h"
// local
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

// from event model
#include "Event/ProtoParticle.h"

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
    msg << MSG::VERBOSE << "    This associator is direct, without Relations table" << endreq;
    // Create the temporary table
    m_table = new Table;
    sc = toolSvc()->retrieveTool( "ProtoParticle2MCAsct", "ChargedPP2MCAsct", m_pChargedAsct) ;
    if( sc.isSuccess() ) {
      sc = toolSvc()->retrieveTool( "ProtoParticle2MCAsct", "NeutralPP2MCAsct", m_pNeutralAsct) ;
    }
  } else {
    m_hasTable = true;
    m_table = 0;
  }
  return sc;
}

StatusCode Particle2MCLinksAsct::finalize() {
  if( m_table) delete m_table;
  //  if( m_pChargedAsct ) m_pChargedAsct->release();
  //  if( m_pNeutralAsct ) m_pNeutralAsct->release();
  return StatusCode::SUCCESS;
}

StatusCode Particle2MCLinksAsct::handle(){
  if( m_table ) m_table->clear();
  return StatusCode::SUCCESS;
}
//=============================================================================
// Interface implementation
//=============================================================================
bool Particle2MCLinksAsct::tableExists() const{
  return (!m_hasTable && (NULL != m_table) &&
          m_pChargedAsct->tableExists() &&
          m_pNeutralAsct->tableExists() )
  || Asct::tableExists();
};

MCsFromParticleLinks Particle2MCLinksAsct::rangeFrom(const From& part) const
{
  MsgStream msg(msgSvc(), name());
  if( m_hasTable ) return Asct::rangeFrom( part );
  ProtoParticle2MCAsct::IAsct* protoAsct = part->charge() ? m_pChargedAsct : m_pNeutralAsct;
  // Local implementation...
  if( protoAsct->tableExists() ) {
    msg << MSG::VERBOSE << "    Particle " << part->key();
    const ProtoParticle* protoPart = dynamic_cast<const  ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      msg << " from ProtoParticle " << protoPart->key() << endreq;
      MCsFromProtoParticle range = protoAsct->rangeFrom( protoPart );
      for( MCsFromProtoParticleIterator it=range.begin(); range.end() != it; it++) {
        m_table->relate( part, it->to(), it->weight());
      }
      return m_table->relations( part );
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  }
  return MCsFromParticleLinks();
}

MCsFromParticleLinks Particle2MCLinksAsct::rangeWithLowCutFrom(const From& part, double cut) const
{
  if( m_hasTable ) return Asct::rangeWithLowCutFrom( part, cut );
  MsgStream  msg( msgSvc(), name() );
  // Local implementation...
  ProtoParticle2MCAsct::IAsct* protoAsct = part->charge() ? m_pChargedAsct : m_pNeutralAsct;
  if( protoAsct->tableExists() ) {
    msg << MSG::VERBOSE << "    Particle " << part->key();
    const ProtoParticle* protoPart = dynamic_cast<const ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      msg << " from ProtoParticle " << protoPart->key() << endreq;
      MCsFromProtoParticle range =  protoAsct->rangeWithLowCutFrom( protoPart, cut );
      for( MCsFromProtoParticleIterator it=range.begin(); range.end() != it; it++) {
        m_table->relate( part, it->to(), it->weight());
      }
      return m_table->relations( part );
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  }
  return MCsFromParticleLinks();
}

MCsFromParticleLinks Particle2MCLinksAsct::rangeWithHighCutFrom(const From& part, double cut) const
{
  if( m_hasTable ) return Asct::rangeWithHighCutFrom( part, cut );
  MsgStream  msg( msgSvc(), name() );
  // Local implementation...
  ProtoParticle2MCAsct::IAsct* protoAsct = part->charge() ? m_pChargedAsct : m_pNeutralAsct;
  if( protoAsct->tableExists() ) {
    msg << MSG::VERBOSE << "    Particle " << part->key();
    const ProtoParticle* protoPart = dynamic_cast<const ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      msg << " from ProtoParticle " << protoPart->key() << endreq;
      MCsFromProtoParticle range =  protoAsct->rangeWithHighCutFrom( protoPart, cut );
      for( MCsFromProtoParticleIterator it=range.begin(); range.end() != it; it++) {
        m_table->relate( part, it->to(), it->weight());
      }
      return m_table->relations( part );
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  }
  return MCsFromParticleLinks();
}

Particle2MCLinksAsct::To Particle2MCLinksAsct::associatedFrom(const From& part) const
{
  if( m_hasTable ) return Asct::associatedFrom( part );
  MsgStream  msg( msgSvc(), name() );
  // Local implementation...
  ProtoParticle2MCAsct::IAsct* protoAsct = part->charge() ? m_pChargedAsct : m_pNeutralAsct;
  if( protoAsct->tableExists() ) {
    msg << MSG::VERBOSE << "    Particle " << part->key();
    const ProtoParticle* protoPart = dynamic_cast<const ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      msg << " from ProtoParticle " << protoPart->key() << endreq;
      return protoAsct->associatedFrom( protoPart );
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  }
  return To();
}

Particle2MCLinksAsct::To Particle2MCLinksAsct::associatedFrom(const From& part, double& weight) const
{
  if( m_hasTable ) return Asct::associatedFrom( part, weight );
  MsgStream  msg( msgSvc(), name() );
  // Local implementation...
  ProtoParticle2MCAsct::IAsct* protoAsct = part->charge() ? m_pChargedAsct : m_pNeutralAsct;
  if( protoAsct->tableExists() ) {
    msg << MSG::VERBOSE << "    Particle " << part->key();
    const ProtoParticle* protoPart = dynamic_cast<const ProtoParticle*>( part->origin() ) ;
    if( protoPart ) {
      msg << " from ProtoParticle " << protoPart->key() << endreq;
      return protoAsct->associatedFrom( protoPart, weight );
    } else {
      msg << " not from a ProtoParticle" << endreq;
    }
  }
  return To();
}

//=============================================================================
