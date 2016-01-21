
// local
#include "Event/PackedCaloCluster.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void CaloClusterPacker::pack( const DataVector & clus,
                              PackedDataVector & pclus ) const
{
  if ( 0 == pclus.packingVersion()  )
  {
    pclus.data().reserve( clus.size() );
    for ( const Data * clu : clus )
    {
      // make a new packed object
      pclus.data().emplace_back( PackedData() );
      auto & pclu = pclus.data().back();

      // fill data

      // general
      pclu.key  = clu->key();
      pclu.type = (int)clu->type();
      pclu.seed = clu->seed().all();

      // position object
      pclu.pos_x = m_pack.position( clu->position().x() );
      pclu.pos_y = m_pack.position( clu->position().y() );
      pclu.pos_z = m_pack.position( clu->position().z() );
      pclu.pos_e = m_pack.energy  ( clu->position().e() );
      //
      pclu.pos_c0 = m_pack.position( clu->position().center()[0] );
      pclu.pos_c1 = m_pack.position( clu->position().center()[1] );
      //
      const auto err0 = safe_sqrt( clu->position().covariance()(0,0) );
      const auto err1 = safe_sqrt( clu->position().covariance()(1,1) );
      const auto err2 = safe_sqrt( clu->position().covariance()(2,2) );
      pclu.pos_cov00 = m_pack.position( err0 );
      pclu.pos_cov11 = m_pack.position( err1 );
      pclu.pos_cov22 = m_pack.energy  ( err2 );
      pclu.pos_cov10 = m_pack.fraction( clu->position().covariance()(1,0), err1*err0 );
      pclu.pos_cov20 = m_pack.fraction( clu->position().covariance()(2,0), err2*err0 );
      pclu.pos_cov21 = m_pack.fraction( clu->position().covariance()(2,1), err2*err1 );
      //
      const auto serr0 = safe_sqrt( clu->position().spread()(0,0) );
      const auto serr1 = safe_sqrt( clu->position().spread()(1,1) );
      pclu.pos_spread00 = m_pack.position( serr0 );
      pclu.pos_spread11 = m_pack.position( serr1 );
      pclu.pos_spread10 = m_pack.fraction( clu->position().spread()(1,0), serr1*serr0 );

      // entries
      pclu.firstEntry = pclus.entries().size();
      for ( const auto & En : clu->entries() )
      {
        pclus.entries().emplace_back( PackedCaloClusterEntry() );
        auto & pEnt = pclus.entries().back();
        if ( En.digit().target() )
        {
          pEnt.digit = m_pack.reference64( &pclus,
                                           En.digit()->parent(),
                                           En.digit()->key().all() );
        }
        pEnt.status = En.status();
        pEnt.fraction = m_pack.fraction( En.fraction() );

      }
      pclu.lastEntry  = pclus.entries().size();

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pclus.packingVersion();
    throw GaudiException( mess.str(), "CaloClusterPacker", StatusCode::FAILURE );
  }
}

void CaloClusterPacker::unpack( const PackedDataVector & pclus,
                                DataVector       & clus ) const
{
  if ( 0 == pclus.packingVersion()  )
  {
    clus.reserve( pclus.data().size() );
    for ( const auto & pclu : pclus.data() )
    {
      // make and save new clUster container, with original key
      auto * clu  = new Data();
      clus.insert( clu, pclu.key );

      // Fill data from packed object

      // general
      clu->setType( (LHCb::CaloCluster::Type)pclu.type );
      clu->setSeed( LHCb::CaloCellID(pclu.seed) );

      // position
      typedef LHCb::CaloPosition CaloP;
      //
      clu->position().setZ( m_pack.position(pclu.pos_z) );
      clu->position().setParameters( CaloP::Parameters( m_pack.position(pclu.pos_x),
                                                        m_pack.position(pclu.pos_y),
                                                        m_pack.energy  (pclu.pos_e) ) );
      //
      clu->position().setCenter( CaloP::Center( m_pack.position(pclu.pos_c0),
                                                m_pack.position(pclu.pos_c1) ) );
      //
      auto & cov = clu->position().covariance();
      const auto err0 = m_pack.position( pclu.pos_cov00 );
      const auto err1 = m_pack.position( pclu.pos_cov11 );
      const auto err2 = m_pack.energy  ( pclu.pos_cov22 );
      cov(0,0) = err0 * err0;
      cov(1,0) = err1 * err0 * m_pack.fraction( pclu.pos_cov10 );
      cov(1,1) = err1 * err1;
      cov(2,0) = err2 * err0 * m_pack.fraction( pclu.pos_cov20 );
      cov(2,1) = err2 * err1 * m_pack.fraction( pclu.pos_cov21 );
      cov(2,2) = err2 * err2;
      //
      auto & spr = clu->position().spread();
      const auto serr0 = m_pack.position( pclu.pos_spread00 );
      const auto serr1 = m_pack.position( pclu.pos_spread11 );
      spr(0,0) = serr0 * serr0;
      spr(1,0) = serr1 * serr0 * m_pack.fraction( pclu.pos_spread10 );
      spr(1,1) = serr1 * serr1;

      // entries
      for ( auto iE = pclu.firstEntry; iE < pclu.lastEntry; ++iE )
      {
        // get the packed entry
        const auto & pEnt = pclus.entries()[iE];
        // make a new unpacked one
        clu->entries().emplace_back( LHCb::CaloClusterEntry() );
        auto & ent = clu->entries().back();
        // Set data
        if ( -1 != pEnt.digit )
        {
          int hintID(0), key(0);
          if ( m_pack.hintAndKey64( pEnt.digit, &pclus, &clus, hintID, key ) )
          {
            ent.setDigit( LHCb::CaloClusterEntry::Digit( &clus, hintID, key ) );
          }
          else { parent().Error("Corrupt CaloCluster Digit SmartRef found").ignore(); }
        }
        ent.setStatus( pEnt.status );
        ent.setFraction( m_pack.fraction( pEnt.fraction ) );
      }

    }
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pclus.packingVersion();
    throw GaudiException( mess.str(), "CaloClusterPacker", StatusCode::FAILURE );
  }
}

StatusCode CaloClusterPacker::check( const DataVector & dataA,
                                     const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Loop over data containers together and compare
  auto iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    sc = sc && check( **iA, **iB );
  }

  // Return final status
  return sc;
}

StatusCode CaloClusterPacker::check( const Data & dataA,
                                     const Data & dataB ) const
{
  // assume OK from the start
  bool ok = true;

  // checker
  const DataPacking::DataChecks ch(parent());

  // checks here

  // key
  ok &= ch.compareInts( "key", dataA.key(), dataB.key() );
  // type
  ok &= ch.compareInts( "type", dataA.type(), dataB.type() );
  // seed
  ok &= ch.compareInts( "seed", dataA.seed().all(), dataB.seed().all() );

  // 'positions'
  ok &= ch.compareDoubles( "Position-X",
                           dataA.position().x(), dataB.position().x() );
  ok &= ch.compareDoubles( "Position-Y",
                           dataA.position().y(), dataB.position().y() );
  ok &= ch.compareDoubles( "Position-Z",
                           dataA.position().z(), dataB.position().z() );
  ok &= ch.compareEnergies( "Position-E",
                            dataA.position().e(), dataB.position().e() );
  ok &= ch.compareVectors( "Position-Center",
                           dataA.position().center(), dataB.position().center() );

  //   ok &= ch.compareMatrices<Gaudi::SymMatrix3x3,3,3>( "Position-Covariance",
  //                                                      dataA.position().covariance(),
  //                                                      dataB.position().covariance() );
  //   ok &= ch.compareMatrices<Gaudi::SymMatrix2x2,2,2>( "Position-Spread",
  //                                                      dataA.position().spread(),
  //                                                      dataB.position().spread() );

  // Same checks as in caloHypo for covariance and spread
  // -------------------------------------------------------------------------
  std::vector<double> oDiag, tDiag, oFrac, tFrac;

  oDiag.push_back( safe_sqrt(dataA.position().covariance()(0,0)) );
  oDiag.push_back( safe_sqrt(dataA.position().covariance()(1,1)) );
  oDiag.push_back( safe_sqrt(dataA.position().covariance()(2,2)) );
  oDiag.push_back( safe_sqrt(dataA.position().spread()(0,0)) );
  oDiag.push_back( safe_sqrt(dataA.position().spread()(1,1)) );

  tDiag.push_back( safe_sqrt(dataB.position().covariance()(0,0)) );
  tDiag.push_back( safe_sqrt(dataB.position().covariance()(1,1)) );
  tDiag.push_back( safe_sqrt(dataB.position().covariance()(2,2)) );
  tDiag.push_back( safe_sqrt(dataB.position().spread()(0,0)) );
  tDiag.push_back( safe_sqrt(dataB.position().spread()(1,1)) );

  if ( 5.e-5 < fabs( oDiag[0] - tDiag[0] ) ) ok = false;
  if ( 5.e-5 < fabs( oDiag[1] - tDiag[1] ) ) ok = false;
  if ( 5.e-3 < fabs( oDiag[2] - tDiag[2] ) ) ok = false;
  if ( 5.e-5 < fabs( oDiag[3] - tDiag[3] ) ) ok = false;
  if ( 5.e-5 < fabs( oDiag[4] - tDiag[4] ) ) ok = false;

  oFrac.push_back( dataA.position().covariance()(1,0) / oDiag[1] / oDiag[0] );
  oFrac.push_back( dataA.position().covariance()(2,0) / oDiag[2] / oDiag[0] );
  oFrac.push_back( dataA.position().covariance()(2,1) / oDiag[2] / oDiag[1] );
  oFrac.push_back( dataA.position().spread()(1,0)     / oDiag[3] / oDiag[4] );

  tFrac.push_back( dataB.position().covariance()(1,0) / tDiag[1] / tDiag[0] );
  tFrac.push_back( dataB.position().covariance()(2,0) / tDiag[2] / tDiag[0] );
  tFrac.push_back( dataB.position().covariance()(2,1) / tDiag[2] / tDiag[1] );
  tFrac.push_back( dataB.position().spread()(1,0)     / tDiag[3] / tDiag[4] );

  for ( unsigned int kk = 0; oFrac.size() > kk; ++kk )
  {
    if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) ok = false;
  }
  // -------------------------------------------------------------------------

  // Entries
  const bool entsSizeOK = dataA.entries().size() == dataB.entries().size();
  ok &= entsSizeOK;
  if ( entsSizeOK )
  {
    LHCb::CaloCluster::Entries::const_iterator iEA(dataA.entries().begin());
    LHCb::CaloCluster::Entries::const_iterator iEB(dataB.entries().begin());
    for ( ; iEA != dataA.entries().end() && iEB != dataB.entries().end();
          ++iEA, ++iEB )
    {
      ok &= (*iEA).digit()  == (*iEB).digit();
      ok &= (*iEA).status() == (*iEB).status();
      ok &= ch.compareDoubles( "Entry-Fraction",
                               (*iEA).fraction(), (*iEB).fraction() );
    }
  }

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.parent() && dataA.parent()->registry() ?
                              dataA.parent()->registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with CaloCluster data packing :-" << endmsg
                       << "  Original Cluster key=" << dataA.key() 
                       << " in '" << loc << "'" << endmsg
                       << dataA << endmsg
                       << "  Unpacked Cluster" << endmsg
                       << dataB << endmsg;
  }

  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}
