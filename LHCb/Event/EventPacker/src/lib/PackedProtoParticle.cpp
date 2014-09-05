
// STL
#include <sstream>

// local
#include "Event/PackedProtoParticle.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Boost
#include <boost/numeric/conversion/bounds.hpp>

using namespace LHCb;

//-----------------------------------------------------------------------------

void
ProtoParticlePacker::pack( const Data & proto,
                           PackedData & pproto,
                           PackedDataVector & pprotos ) const
{
  if ( parent().msgLevel(MSG::VERBOSE) )
    parent().verbose() << "Packing ProtoParticle " << proto.key() << endmsg;

  if ( 0 != proto.track() )
  {
    pproto.track = m_pack.reference( &pprotos, proto.track()->parent(), proto.track()->key() );
    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << " -> Track " << proto.track() << " "
                         << proto.track()->parent()->registry()->identifier() << " "
                         << pproto.track
                         << endmsg;
  }
  else
  {
    pproto.track = -1;
  }

  if ( 0 != proto.richPID() )
  {
    pproto.richPID = m_pack.reference( &pprotos, proto.richPID()->parent(), proto.richPID()->key() );
    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << " -> RichPID " << proto.richPID() << " "
                         << proto.richPID()->parent()->registry()->identifier() << " "
                         << pproto.richPID
                         << endmsg;
  }
  else
  {
    pproto.richPID = -1;
  }

  if ( 0 != proto.muonPID() )
  {
    pproto.muonPID = m_pack.reference( &pprotos, proto.muonPID()->parent(), proto.muonPID()->key() );
    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << " -> MuonPID " << proto.muonPID() << " "
                         << proto.muonPID()->parent()->registry()->identifier() << " "
                         << pproto.muonPID
                         << endmsg;
  }
  else
  {
    pproto.muonPID = -1;
  }

  //== Store the CaloHypos
  pproto.firstHypo = pprotos.refs().size();
  for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = proto.calo().begin();
        proto.calo().end() != itO; ++itO )
  {
    pprotos.refs().push_back( m_pack.reference( &pprotos, (*itO)->parent(), (*itO)->key() ) );
    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << " -> CaloHypo " << *itO << " "
                         << (*itO)->parent()->registry()->identifier() << " "
                         << pprotos.refs().back()
                         << endmsg;
  }
  pproto.lastHypo = pprotos.refs().size();

  //== Handles the ExtraInfo
  pproto.firstExtra = pprotos.extras().size();
  const double high = boost::numeric::bounds<float>::highest();
  const double low  = boost::numeric::bounds<float>::lowest();
  for ( const auto einfo : proto.extraInfo() )
  {
    double info = einfo.second;
    if ( info > high || info < low )
    {
      std::ostringstream s;
      s << (LHCb::ProtoParticle::additionalInfo)einfo.first;
      parent().Warning( "ExtraInfo '" + s.str() +
                        "' out of floating point range. Truncating value." ).ignore();
      // if      ( info > high ) { info = high; }
      // else if ( info < low  ) { info = low;  }
    }
    pprotos.extras().push_back( std::make_pair(einfo.first,m_pack.fltPacked(info)) );
  }
  pproto.lastExtra = pprotos.extras().size();
}

void
ProtoParticlePacker::pack( const DataVector & protos,
                           PackedDataVector & pprotos ) const
{
  pprotos.protos().reserve(protos.size());
  for ( const LHCb::ProtoParticle * proto : protos )
  {
    if ( !proto ) continue;
    pprotos.protos().push_back( LHCb::PackedProtoParticle() );
    LHCb::PackedProtoParticle & pproto = pprotos.protos().back();
    // save the key
    pproto.key = proto->key();
    // fill the rest
    pack( *proto, pproto, pprotos );
  }
}

void
ProtoParticlePacker::unpack( const PackedData       & pproto,
                             Data                   & proto,
                             const PackedDataVector & pprotos,
                             DataVector             & protos ) const
{
  if ( parent().msgLevel(MSG::VERBOSE) )
    parent().verbose() << "UnPacking ProtoParticle " << pproto.key << endmsg;

  int hintID(0), key(0);

  if ( -1 != pproto.track )
  {
    m_pack.hintAndKey( pproto.track, &pprotos, &protos, hintID, key );
    SmartRef<LHCb::Track> ref( &protos, hintID, key );
    proto.setTrack( ref );
    if ( parent().msgLevel(MSG::VERBOSE) )
    {
      parent().verbose() << " -> Track PackedRef " << pproto.track
                         << " hintID " << hintID << " key " << key
                         << " -> SmartRef " << proto.track();
      if ( proto.track() ) parent().verbose() << " " << proto.track()->parent()->registry()->identifier();
      parent().verbose() << endmsg;
    }
  }

  if ( -1 != pproto.richPID )
  {
    m_pack.hintAndKey( pproto.richPID, &pprotos, &protos, hintID, key );
    SmartRef<LHCb::RichPID> ref( &protos, hintID, key );
    proto.setRichPID( ref );
    if ( parent().msgLevel(MSG::VERBOSE) )
    {
      parent().verbose() << " -> RichPID PackedRef " << pproto.richPID
                         << " hintID " << hintID << " key " << key
                         << " -> SmartRef " << proto.richPID();
      if ( proto.richPID() )
        parent().verbose() << " " << proto.richPID()->parent()->registry()->identifier();
      parent().verbose() << endmsg;
    }
  }

  if ( -1 != pproto.muonPID )
  {
    m_pack.hintAndKey( pproto.muonPID, &pprotos, &protos, hintID, key );
    SmartRef<LHCb::MuonPID> ref( &protos, hintID, key );
    proto.setMuonPID( ref );
    if ( parent().msgLevel(MSG::VERBOSE) )
    {
      parent().verbose() << " -> MuonPID PackedRef " << pproto.muonPID << " hintID " << hintID << " key " << key
                         << " -> SmartRef " << proto.muonPID();
      if ( proto.muonPID() )
        parent().verbose() << " " << proto.muonPID()->parent()->registry()->identifier();
      parent().verbose() << endmsg;
    }
  }

  for ( int kk = pproto.firstHypo; pproto.lastHypo > kk; ++kk )
  {
    const int reference = *(pprotos.refs().begin()+kk);
    m_pack.hintAndKey( reference, &pprotos, &protos, hintID, key );
    SmartRef<LHCb::CaloHypo> ref( &protos, hintID, key );
    proto.addToCalo( ref );
    if ( parent().msgLevel(MSG::VERBOSE) )
    {
      parent().verbose() << " -> CaloHypo PackedRef " << reference
                         << " hintID " << hintID << " key " << key
                         << " -> SmartRef " << ref << " "
                         << ref->parent()->registry()->identifier()
                         << endmsg;
    }
  }

  for ( int kk = pproto.firstExtra; pproto.lastExtra > kk; ++kk )
  {
    const std::pair<int,int>& info = *(pprotos.extras().begin()+kk);
    proto.addInfo( info.first, m_pack.fltPacked( info.second ) );
  }

}

void
ProtoParticlePacker::unpack( const PackedDataVector & pprotos,
                             DataVector             & protos ) const
{
  protos.reserve(pprotos.protos().size());
  for ( const LHCb::PackedProtoParticle& pproto : pprotos.protos() )
  {
    LHCb::ProtoParticle * part = new LHCb::ProtoParticle( );
    protos.insert( part, pproto.key );
    unpack( pproto, *part, pprotos, protos );
  }
}

StatusCode ProtoParticlePacker::check( const Data & dataA,
                                       const Data & dataB ) const
{

  if ( dataA.key() != dataB.key() )
  {
    parent().warning() << "Wrong key : old " <<  dataA.key() << " test " << dataB.key() << endmsg;
  }

  bool isOK = true;

  const LHCb::Track* oTrack = dataA.track();   // resolve to a pointer...
  const LHCb::Track* tTrack = dataB.track();
  if ( oTrack != tTrack ) isOK = false;

  const LHCb::RichPID* oRichPID = dataA.richPID();   // resolve to a pointer...
  const LHCb::RichPID* tRichPID = dataB.richPID();
  if ( oRichPID != tRichPID ) isOK = false;

  const LHCb::MuonPID* oMuonPID = dataA.muonPID();   // resolve to a pointer...
  const LHCb::MuonPID* tMuonPID = dataB.muonPID();
  if ( oMuonPID != tMuonPID ) isOK = false;

  unsigned int kk;
  std::vector<const LHCb::CaloHypo*> oHypo, tHypo;
  SmartRefVector<LHCb::CaloHypo>::const_iterator itH;
  for ( itH = dataA.calo().begin(); dataA.calo().end() != itH; ++itH ) {
    const LHCb::CaloHypo* z = *itH;
    oHypo.push_back( z );
  }
  for ( itH = dataB.calo().begin(); dataB.calo().end() != itH; ++itH ) {
    const LHCb::CaloHypo* z = *itH;
    tHypo.push_back( z );
  }
  if ( oHypo.size() != tHypo.size() ) isOK = false;
  for ( kk = 0; oHypo.size() > kk ; ++kk ) {
    if ( oHypo[kk] != tHypo[kk] ) isOK = false;
  }

  LHCb::ProtoParticle::ExtraInfo oExtra = dataA.extraInfo();
  LHCb::ProtoParticle::ExtraInfo tExtra = dataB.extraInfo();
  if ( oExtra.size() != tExtra.size() ) isOK = false;
  LHCb::ProtoParticle::ExtraInfo::const_iterator oIt = oExtra.begin();
  LHCb::ProtoParticle::ExtraInfo::const_iterator tIt = tExtra.begin();
  for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
    if ( (*oIt).first != (*tIt).first ) isOK = false;
    if ( ( (*tIt).second == 0 && (*tIt).second != (*oIt).second ) ||
         ( (*tIt).second != 0 && 1.e-7 < fabs( ((*tIt).second - (*oIt).second ) / (*tIt).second ) ) ) isOK = false;
  }

  if ( !isOK || MSG::DEBUG >= parent().msgLevel() )
  {
    parent().info() << "===== ProtoParticle key " << dataA.key() << endmsg;
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

  return ( isOK ? StatusCode::SUCCESS : StatusCode::FAILURE );
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
    sc = sc && check( *oPart, *tPart );
  }

  return sc;
}
