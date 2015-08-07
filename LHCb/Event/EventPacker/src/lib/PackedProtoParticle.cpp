
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
  // packing version
  const char ver = pprotos.packingVersion();
  if ( 1 == ver || 0 == ver )
  {

    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << "Packing ProtoParticle " << proto.key() << endmsg;

    if ( 0 != proto.track() )
    {
      pproto.track = ( UNLIKELY( 0==ver ) ?
                       m_pack.reference32( &pprotos, proto.track()->parent(), proto.track()->key() ) :
                       m_pack.reference64( &pprotos, proto.track()->parent(), proto.track()->key() ) );
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
      pproto.richPID = ( UNLIKELY( 0==ver ) ?
                         m_pack.reference32( &pprotos, proto.richPID()->parent(), proto.richPID()->key() ) :
                         m_pack.reference64( &pprotos, proto.richPID()->parent(), proto.richPID()->key() ) );
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
      pproto.muonPID = ( UNLIKELY( 0==ver ) ?
                         m_pack.reference32( &pprotos, proto.muonPID()->parent(), proto.muonPID()->key() ) :
                         m_pack.reference64( &pprotos, proto.muonPID()->parent(), proto.muonPID()->key() ) );
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
    for ( const SmartRef<LHCb::CaloHypo>& caloH : proto.calo() )
    {
      pprotos.refs().push_back( UNLIKELY( 0==ver ) ?
                                m_pack.reference32( &pprotos, caloH->parent(), caloH->key() ) :
                                m_pack.reference64( &pprotos, caloH->parent(), caloH->key() ) );
      if ( parent().msgLevel(MSG::VERBOSE) )
        parent().verbose() << " -> CaloHypo " << *caloH << " "
                           << caloH->parent()->registry()->identifier() << " "
                           << pprotos.refs().back()
                           << endmsg;
    }
    pproto.lastHypo = pprotos.refs().size();

    //== Handles the ExtraInfo
    pproto.firstExtra = pprotos.extras().size();
    const double high = boost::numeric::bounds<float>::highest();
    const double low  = boost::numeric::bounds<float>::lowest();
    for ( const auto& einfo : proto.extraInfo() )
    {
      const double& info = einfo.second;
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
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "ProtoParticlePacker", StatusCode::FAILURE );
  }

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
  // packing version
  const char ver = pprotos.packingVersion();
  if ( 1 == ver || 0 == ver )
  {

    if ( parent().msgLevel(MSG::VERBOSE) )
      parent().verbose() << "UnPacking ProtoParticle " << pproto.key << endmsg;

    int hintID(0), key(0);

    if ( -1 != pproto.track )
    {
      if ( ( 0!=ver && m_pack.hintAndKey64(pproto.track,&pprotos,&protos,hintID,key) ) ||
           ( 0==ver && m_pack.hintAndKey32(pproto.track,&pprotos,&protos,hintID,key) ) )
      {
        SmartRef<LHCb::Track> ref( &protos, hintID, key );
        proto.setTrack( ref );
        if ( parent().msgLevel(MSG::VERBOSE) )
        {
          parent().verbose() << " -> Track PackedRef " << pproto.track
                             << " hintID " << hintID << " key " << key
                             << " -> SmartRef " << proto.track();
          if ( proto.track() )
            parent().verbose() << " " << proto.track()->parent()->registry()->identifier();
          parent().verbose() << endmsg;
        }
      }
      else
      {
        parent().Error( "Corrupt ProtoParticle Track SmartRef detected." ).ignore();
      }
    }

    if ( -1 != pproto.richPID )
    {
      if ( ( 0!=ver && m_pack.hintAndKey64(pproto.richPID,&pprotos,&protos,hintID,key) ) ||
           ( 0==ver && m_pack.hintAndKey32(pproto.richPID,&pprotos,&protos,hintID,key) ) )
      {
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
      else
      {
        parent().Error( "Corrupt ProtoParticle RichPID SmartRef detected." ).ignore();
      }
    }

    if ( -1 != pproto.muonPID )
    {
      if ( ( 0!=ver && m_pack.hintAndKey64(pproto.muonPID,&pprotos,&protos,hintID,key) ) ||
           ( 0==ver && m_pack.hintAndKey32(pproto.muonPID,&pprotos,&protos,hintID,key) ) )
      {
        SmartRef<LHCb::MuonPID> ref( &protos, hintID, key );
        proto.setMuonPID( ref );
        if ( parent().msgLevel(MSG::VERBOSE) )
        {
          parent().verbose() << " -> MuonPID PackedRef " << pproto.muonPID
                             << " hintID " << hintID << " key " << key
                             << " -> SmartRef " << proto.muonPID();
          if ( proto.muonPID() )
            parent().verbose() << " " << proto.muonPID()->parent()->registry()->identifier();
          parent().verbose() << endmsg;
        }
      }
      else
      {
        parent().Error( "Corrupt ProtoParticle MuonPID SmartRef detected." ).ignore();
      }
    }

    for ( int kk = pproto.firstHypo; pproto.lastHypo > kk; ++kk )
    {
      const long long reference = *(pprotos.refs().begin()+kk);
      if ( ( 0!=ver && m_pack.hintAndKey64(reference,&pprotos,&protos,hintID,key) ) ||
           ( 0==ver && m_pack.hintAndKey32(reference,&pprotos,&protos,hintID,key) ) )
      {
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
      else
      {
        parent().Error( "Corrupt ProtoParticle CaloHypo SmartRef detected." ).ignore();
      }
    }

    for ( int kk = pproto.firstExtra; pproto.lastExtra > kk; ++kk )
    {
      const std::pair<int,int>& info = *(pprotos.extras().begin()+kk);
      proto.addInfo( info.first, m_pack.fltPacked( info.second ) );
    }

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)ver;
    throw GaudiException( mess.str(), "ProtoParticlePacker", StatusCode::FAILURE );
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

  // check referenced objects
  if ( dataA.track()   != dataB.track()   ) isOK = false;
  if ( dataA.richPID() != dataB.richPID() ) isOK = false;
  if ( dataA.muonPID() != dataB.muonPID() ) isOK = false;

  // calo hypos
  if ( dataA.calo().size() != dataB.calo().size() ) isOK = false;
  if ( isOK )
  {
    for ( auto iC = std::make_pair(dataA.calo().begin(),dataB.calo().begin());
          iC.first != dataA.calo().end() && iC.second != dataB.calo().end();
          ++iC.first, ++iC.second )
    {
      if ( iC.first != iC.second ) isOK = false;
    }
  }

  // extra info
  if ( dataA.extraInfo().size() != dataB.extraInfo().size() ) isOK = false;
  if ( isOK )
  {
    for ( auto iE = std::make_pair(dataA.extraInfo().begin(),dataB.extraInfo().begin());
          iE.first != dataA.extraInfo().end() && iE.second != dataA.extraInfo().end();
          ++iE.first, ++iE.second )
    {
      if ( iE.first->first != iE.second->first ) isOK = false;
      if ( isOK )
      {
        if ( ( iE.second->second == 0 && iE.second->second != iE.first->second ) ||
             ( iE.second->second != 0 && 
               1.e-7 < fabs( ( iE.second->second - iE.first->second ) / iE.second->second ) ) ) isOK = false;
      }
    }
  }

  if ( !isOK || MSG::DEBUG >= parent().msgLevel() )
  {
    parent().info() << "===== ProtoParticle key " << dataA.key() << endmsg;
    parent().info() << format( "Old   track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                               dataA.track(), dataA.richPID(), dataA.muonPID(), 
                               dataA.calo().size(), dataA.extraInfo().size() )
                    << endmsg;
    parent().info() << format( "Test  track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                               dataB.track(), dataB.richPID(), dataB.muonPID(), 
                               dataB.calo().size(), dataA.extraInfo().size() )
                    << endmsg;
    for ( auto iC = std::make_pair(dataA.calo().begin(),dataB.calo().begin());
          iC.first != dataA.calo().end() && iC.second != dataB.calo().end();
          ++iC.first, ++iC.second )
    {
      parent().info() << format( "   old CaloHypo %8x   new %8x", iC.first, iC.second )  << endmsg;
    }
    for ( auto iE = std::make_pair(dataA.extraInfo().begin(),dataB.extraInfo().begin());
          iE.first != dataA.extraInfo().end() && iE.second != dataA.extraInfo().end();
          ++iE.first, ++iE.second )
    {
      parent().info() << format( "   old Extra %5d %12.4f     new %5d %12.4f",
                                 iE.first->first, iE.first->second, iE.second->first, iE.second->second )
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
