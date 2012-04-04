
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
  pclus.data().reserve( clus.size() );
  if ( 0 == pclus.packingVersion()  )
  {
    for ( DataVector::const_iterator iD = clus.begin();
          iD != clus.end(); ++iD )
    {
      // Original object
      const Data & clu = **iD;

      // make a new packed object
      pclus.data().push_back( PackedData() );
      PackedData & pclu = pclus.data().back();

      // fill data

      // general
      pclu.key  = clu.key();
      pclu.type = (int)clu.type();
      pclu.seed = clu.seed().all();

      // position object
      pclu.pos_x = m_pack.position( clu.position().x() );
      pclu.pos_y = m_pack.position( clu.position().y() );
      pclu.pos_z = m_pack.position( clu.position().z() );
      pclu.pos_e = m_pack.position( clu.position().e() );
      //
      pclu.pos_c0 = m_pack.position( clu.position().center()[0] );
      pclu.pos_c1 = m_pack.position( clu.position().center()[1] );
      //
      const double err0 = std::sqrt( clu.position().covariance()(0,0) );
      const double err1 = std::sqrt( clu.position().covariance()(1,1) );
      const double err2 = std::sqrt( clu.position().covariance()(2,2) );
      pclu.pos_cov00 = m_pack.position( err0 );
      pclu.pos_cov11 = m_pack.position( err1 );
      pclu.pos_cov22 = m_pack.position( err2 );
      pclu.pos_cov10 = m_pack.fraction( clu.position().covariance()(1,0)/err1/err0 );
      pclu.pos_cov20 = m_pack.fraction( clu.position().covariance()(2,0)/err2/err0 );
      pclu.pos_cov21 = m_pack.fraction( clu.position().covariance()(2,1)/err2/err1 );
      //
      pclu.pos_spread00 = m_pack.position( clu.position().spread()(0,0) );
      pclu.pos_spread11 = m_pack.position( clu.position().spread()(1,1) );
      pclu.pos_spread10 = m_pack.position( clu.position().spread()(1,0) );

      // entries
      pclu.firstEntry = pclus.entries().size();
      for ( LHCb::CaloCluster::Entries::const_iterator iEn = clu.entries().begin();
            iEn != clu.entries().end(); ++iEn )
      {
        pclus.entries().push_back( PackedCaloClusterEntry() );
        PackedCaloClusterEntry & pEnt = pclus.entries().back();
        
        if ( NULL != (*iEn).digit().target() )
        {
          pEnt.digit = m_pack.reference64( &pclus,
                                           (*iEn).digit()->parent(),
                                           (*iEn).digit()->key().all() );
        }
        pEnt.status = (*iEn).status();
        pEnt.fraction = m_pack.fraction( (*iEn).fraction() );

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
  clus.reserve( pclus.data().size() );
  if ( 0 == pclus.packingVersion()  )
  {
    for ( PackedDataVector::ClusterVector::const_iterator iD = pclus.data().begin();
          iD != pclus.data().end(); ++iD )
    {
      const PackedData & pclu = *iD;
      // make and save new cluster in container, with original key
      Data * clu  = new Data();
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
                                                        m_pack.position(pclu.pos_e) ) );
      //
      clu->position().setCenter( CaloP::Center( m_pack.position(pclu.pos_c0),
                                                m_pack.position(pclu.pos_c1) ) );
      //
      CaloP::Covariance & cov = clu->position().covariance();
      const double err0 = m_pack.position( pclu.pos_cov00 );
      const double err1 = m_pack.position( pclu.pos_cov11 );
      const double err2 = m_pack.position( pclu.pos_cov22 );
      cov(0,0) = err0 * err0;
      cov(1,0) = err1 * err0 * m_pack.fraction( pclu.pos_cov10 );
      cov(1,1) = err1 * err1;
      cov(2,0) = err2 * err0 * m_pack.fraction( pclu.pos_cov20 );
      cov(2,1) = err2 * err1 * m_pack.fraction( pclu.pos_cov21 );
      cov(2,2) = err2 * err2;
      //
      CaloP::Spread & spread = clu->position().spread();
      spread(0,0) = m_pack.position( pclu.pos_spread00 );
      spread(1,1) = m_pack.position( pclu.pos_spread11 );
      spread(1,0) = m_pack.position( pclu.pos_spread10 );

      // entries
      for ( unsigned int iE = pclu.firstEntry; iE < pclu.lastEntry; ++iE )
      {
        // get the packed entry
        const PackedCaloClusterEntry & pEnt = pclus.entries()[iE];
        // make a new unpacked one
        clu->entries().push_back( LHCb::CaloClusterEntry() );
        LHCb::CaloClusterEntry & ent = clu->entries().back();
        // Set data
        if ( -1 != pEnt.digit )
        {
          int hintID(0), key(0);
          m_pack.hintAndKey64( pEnt.digit, &pclus, &clus, hintID, key );
          LHCb::CaloClusterEntry::Digit dig( &clus, hintID, key );
          ent.setDigit( dig );
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
                                     const DataVector & dataB,
                                     GaudiAlgorithm & parent ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent);

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;

    // checks here

    // key
    ok &= (*iA)->key()  == (*iB)->key();
    // type
    ok &= (*iA)->type() == (*iB)->type();
    // seed
    ok &= (*iA)->seed() == (*iB)->seed();

    // 'positions'
    ok &= ch.compareDoubles( "Position-X",
                             (*iA)->position().x(), (*iB)->position().x() );
    ok &= ch.compareDoubles( "Position-Y",
                             (*iA)->position().y(), (*iB)->position().y() );
    ok &= ch.compareDoubles( "Position-Z",
                             (*iA)->position().z(), (*iB)->position().z() );
    ok &= ch.compareDoubles( "Position-E",
                             (*iA)->position().e(), (*iB)->position().e() );
    ok &= ch.compareVectors( "Position-Center",
                             (*iA)->position().center(), (*iB)->position().center() );
    ok &= ch.compareMatrices<Gaudi::SymMatrix3x3,3,3>( "Position-Covariance",
                                                       (*iA)->position().covariance(), 
                                                       (*iB)->position().covariance() );
    ok &= ch.compareMatrices<Gaudi::SymMatrix2x2,2,2>( "Position-Spread",
                                                       (*iA)->position().spread(), 
                                                       (*iB)->position().spread() );

    // Entries
    const bool entsSizeOK = (*iA)->entries().size() == (*iB)->entries().size();
    ok &= entsSizeOK;
    if ( entsSizeOK )
    {
      LHCb::CaloCluster::Entries::const_iterator iEA((*iA)->entries().begin());
      LHCb::CaloCluster::Entries::const_iterator iEB((*iB)->entries().begin());
      for ( ; iEA != (*iA)->entries().end() && iEB != (*iB)->entries().end(); 
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
      parent.warning() << "Problem with CaloCluster data packing :-" << endmsg
                       << "  Original Cluster : " << **iA
                       << endmsg
                       << "  Unpacked Cluster : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
