// $Id: $

// local
#include "Event/PackedProtoParticle.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

//-----------------------------------------------------------------------------

void
ProtoParticlePacker::pack( const DataVector & protos,
                           PackedDataVector & pprotos ) const
{
  pprotos.protos().reserve(protos.size());

  for ( LHCb::ProtoParticles::const_iterator itP = protos.begin();
        protos.end() != itP; ++itP )
  {
    pprotos.protos().push_back( LHCb::PackedProtoParticle() );
    LHCb::PackedProtoParticle & newPart = pprotos.protos().back();
    const LHCb::ProtoParticle * part = *itP;
    if ( !part ) continue;

    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << "Packing ProtoParticle " << part->key() << endmsg;

    newPart.key = part->key();
    if ( 0 != part->track() )
    {
      newPart.track = m_pack.reference( &pprotos, part->track()->parent(), part->track()->key() );
      if ( parent().msgLevel(MSG::VERBOSE) )
        parent().verbose() << " -> Track " << part->track() << " "
                           << part->track()->parent()->registry()->identifier() << " "
                           << newPart.track
                           << endmsg;
    }
    else
    {
      newPart.track = -1;
    }

    if ( 0 != part->richPID() )
    {
      newPart.richPID = m_pack.reference( &pprotos, part->richPID()->parent(), part->richPID()->key() );
      if ( parent().msgLevel(MSG::VERBOSE) )
        parent().verbose() << " -> RichPID " << part->richPID() << " "
                           << part->richPID()->parent()->registry()->identifier() << " "
                           << newPart.richPID
                           << endmsg;
    }
    else
    {
      newPart.richPID = -1;
    }

    if ( 0 != part->muonPID() )
    {
      newPart.muonPID = m_pack.reference( &pprotos, part->muonPID()->parent(), part->muonPID()->key() );
      if ( parent().msgLevel(MSG::VERBOSE) )
        parent().verbose() << " -> MuonPID " << part->muonPID() << " "
                           << part->muonPID()->parent()->registry()->identifier() << " "
                           << newPart.muonPID
                           << endmsg;
    }
    else
    {
      newPart.muonPID = -1;
    }

    //== Store the CaloHypos
    newPart.firstHypo = pprotos.refs().size();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = part->calo().begin();
          part->calo().end() != itO; ++itO )
    {
      pprotos.refs().push_back( m_pack.reference( &pprotos, (*itO)->parent(), (*itO)->key() ) );
      if ( parent().msgLevel(MSG::VERBOSE) )
        parent().verbose() << " -> CaloHypo " << *itO << " "
                           << (*itO)->parent()->registry()->identifier() << " "
                           << pprotos.refs().back()
                           << endmsg;
    }
    newPart.lastHypo = pprotos.refs().size();

    //== Handles the ExtraInfo
    newPart.firstExtra = pprotos.extras().size();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = part->extraInfo().begin();
          part->extraInfo().end() != itE; ++itE )
    {
      pprotos.extras().push_back( std::pair<int,int>((*itE).first,
                                                     m_pack.fltPacked((*itE).second)) );
    }
    newPart.lastExtra = pprotos.extras().size();

  }

}

void
ProtoParticlePacker::unpack( const PackedDataVector & pprotos,
                             DataVector             & protos ) const
{
  protos.reserve(pprotos.protos().size());

  for ( std::vector<LHCb::PackedProtoParticle>::const_iterator itS = pprotos.protos().begin();
        pprotos.protos().end() != itS; ++itS )
  {
    const LHCb::PackedProtoParticle& src = (*itS);

    LHCb::ProtoParticle* part = new LHCb::ProtoParticle( );
    protos.insert( part, src.key );

    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << "UnPacking ProtoParticle " << src.key << endmsg;

    int hintID(0), key(0);

    if ( -1 != src.track )
    {
      m_pack.hintAndKey( src.track, &pprotos, &protos, hintID, key );
      SmartRef<LHCb::Track> ref( &protos, hintID, key );
      part->setTrack( ref );
      if ( parent().msgLevel(MSG::VERBOSE) )
      {
        parent().verbose() << " -> Track PackedRef " << src.track << " hintID " << hintID << " key " << key
                           << " -> SmartRef " << part->track();
        if ( part->track() ) parent().verbose() << " " << part->track()->parent()->registry()->identifier();
        parent().verbose() << endmsg;
      }
    }

    if ( -1 != src.richPID )
    {
      m_pack.hintAndKey( src.richPID, &pprotos, &protos, hintID, key );
      SmartRef<LHCb::RichPID> ref( &protos, hintID, key );
      part->setRichPID( ref );
      if ( parent().msgLevel(MSG::VERBOSE) )
      {
        parent().verbose() << " -> RichPID PackedRef " << src.richPID << " hintID " << hintID << " key " << key
                           << " -> SmartRef " << part->richPID();
        if ( part->richPID() )
          parent().verbose() << " " << part->richPID()->parent()->registry()->identifier();
        parent().verbose() << endmsg;
      }
    }

    if ( -1 != src.muonPID )
    {
      m_pack.hintAndKey( src.muonPID, &pprotos, &protos, hintID, key );
      SmartRef<LHCb::MuonPID> ref( &protos, hintID, key );
      part->setMuonPID( ref );
      if ( parent().msgLevel(MSG::VERBOSE) )
      {
        parent().verbose() << " -> MuonPID PackedRef " << src.muonPID << " hintID " << hintID << " key " << key
                           << " -> SmartRef " << part->muonPID();
        if ( part->muonPID() )
          parent().verbose() << " " << part->muonPID()->parent()->registry()->identifier();
        parent().verbose() << endmsg;
      }
    }

    for ( int kk = src.firstHypo; src.lastHypo > kk; ++kk )
    {
      const int reference = *(pprotos.refs().begin()+kk);
      m_pack.hintAndKey( reference, &pprotos, &protos, hintID, key );
      SmartRef<LHCb::CaloHypo> ref( &protos, hintID, key );
      part->addToCalo( ref );
      if ( parent().msgLevel(MSG::VERBOSE) )
      {
        parent().verbose() << " -> CaloHypo PackedRef " << reference
                           << " hintID " << hintID << " key " << key
                           << " -> SmartRef " << ref << " "
                           << ref->parent()->registry()->identifier()
                           << endmsg;
      }
    }

    for ( int kk = src.firstExtra; src.lastExtra > kk; ++kk )
    {
      const std::pair<int,int>& info = *(pprotos.extras().begin()+kk);
      part->addInfo( info.first, m_pack.fltPacked( info.second ) );
    }

  }

}

StatusCode
ProtoParticlePacker::check( const DataVector & dataA,
                            const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( dataA.size() != dataB.size() )
  {
    parent().err() << "Old ProtoParticle size " << dataA.size()
                   << " differs form Test " << dataB.size()
                   << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::ProtoParticles::const_iterator itOld  = dataA.begin();
  LHCb::ProtoParticles::const_iterator itTest = dataB.begin();

  while ( dataA.end() != itOld )
  {
    LHCb::ProtoParticle* oPart = (*itOld++);
    LHCb::ProtoParticle* tPart = (*itTest++);
    if ( oPart->key() != tPart->key() )
    {
      parent().warning() << "Wrong key : old " <<  oPart->key() << " test " << tPart->key() << endmsg;
    }

    bool isOK = true;

    const LHCb::Track* oTrack = oPart->track();   // resolve to a pointer...
    const LHCb::Track* tTrack = tPart->track();
    if ( oTrack != tTrack ) isOK = false;

    const LHCb::RichPID* oRichPID = oPart->richPID();   // resolve to a pointer...
    const LHCb::RichPID* tRichPID = tPart->richPID();
    if ( oRichPID != tRichPID ) isOK = false;

    const LHCb::MuonPID* oMuonPID = oPart->muonPID();   // resolve to a pointer...
    const LHCb::MuonPID* tMuonPID = tPart->muonPID();
    if ( oMuonPID != tMuonPID ) isOK = false;

    unsigned int kk;
    std::vector<const LHCb::CaloHypo*> oHypo, tHypo;
    SmartRefVector<LHCb::CaloHypo>::const_iterator itH;
    for ( itH = oPart->calo().begin(); oPart->calo().end() != itH; ++itH ) {
      const LHCb::CaloHypo* z = *itH;
      oHypo.push_back( z );
    }
    for ( itH = tPart->calo().begin(); tPart->calo().end() != itH; ++itH ) {
      const LHCb::CaloHypo* z = *itH;
      tHypo.push_back( z );
    }
    if ( oHypo.size() != tHypo.size() ) isOK = false;
    for ( kk = 0; oHypo.size() > kk ; ++kk ) {
      if ( oHypo[kk] != tHypo[kk] ) isOK = false;
    }

    LHCb::ProtoParticle::ExtraInfo oExtra = oPart->extraInfo();
    LHCb::ProtoParticle::ExtraInfo tExtra = tPart->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::ProtoParticle::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::ProtoParticle::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }

    if ( !isOK || MSG::DEBUG >= parent().msgLevel() ) 
    {
      parent().info() << "===== ProtoParticle key " << oPart->key() << endmsg;
      parent().info() << format( "Old   track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                                 oTrack, oRichPID, oMuonPID, oHypo.size(), oExtra.size() )
                      << endmsg;
      parent().info() << format( "Test  track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                                 tTrack, tRichPID, tMuonPID, tHypo.size(), tExtra.size() )
                      << endmsg;
      for ( kk = 0 ; oHypo.size() != kk ; ++kk )
      {
        parent().info() << format( "   old CaloHypo %8x   new %8x", oHypo[kk], tHypo[kk] )  << endmsg;
      }
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) 
      {
        parent().info() << format( "   old Extra %5d %12.4f     new %5d %12.4f",
                                   (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second )
                        << endmsg;
      }
    }
  }

  return sc;
}
