
// local
#include "Event/PackedParticle.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void ParticlePacker::pack( const Data & part,
                           PackedData & ppart,
                           PackedDataVector & pparts ) const
{
  if ( 1 == pparts.packingVersion() )
  {

    // Particle ID
    ppart.particleID = part.particleID().pid();

    // Mass and error
    ppart.measMass    = m_pack.mass( part.measuredMass()    );
    ppart.measMassErr = m_pack.mass( part.measuredMassErr() );

    // Lorentz vector
    ppart.lv_px = m_pack.energy( part.momentum().px() );
    ppart.lv_py = m_pack.energy( part.momentum().py() );
    ppart.lv_pz = m_pack.energy( part.momentum().pz() );
    ppart.lv_mass = (float) part.momentum().M() ;

    // reference point
    ppart.refx = m_pack.position( part.referencePoint().x() );
    ppart.refy = m_pack.position( part.referencePoint().y() );
    ppart.refz = m_pack.position( part.referencePoint().z() );

    // Mom Cov
    const auto merr00 = safe_sqrt( part.momCovMatrix()(0,0) );
    const auto merr11 = safe_sqrt( part.momCovMatrix()(1,1) );
    const auto merr22 = safe_sqrt( part.momCovMatrix()(2,2) );
    const auto merr33 = safe_sqrt( part.momCovMatrix()(3,3) );
    ppart.momCov00 = m_pack.energy( merr00 );
    ppart.momCov11 = m_pack.energy( merr11 );
    ppart.momCov22 = m_pack.energy( merr22 );
    ppart.momCov33 = m_pack.energy( merr33 );
    ppart.momCov10 = m_pack.fraction( part.momCovMatrix()(1,0), (merr11*merr00) );
    ppart.momCov20 = m_pack.fraction( part.momCovMatrix()(2,0), (merr22*merr00) );
    ppart.momCov21 = m_pack.fraction( part.momCovMatrix()(2,1), (merr22*merr11) );
    ppart.momCov30 = m_pack.fraction( part.momCovMatrix()(3,0), (merr33*merr00) );
    ppart.momCov31 = m_pack.fraction( part.momCovMatrix()(3,1), (merr33*merr11) );
    ppart.momCov32 = m_pack.fraction( part.momCovMatrix()(3,2), (merr33*merr22) );

    // Pos Cov
    const auto perr00 = safe_sqrt( part.posCovMatrix()(0,0) );
    const auto perr11 = safe_sqrt( part.posCovMatrix()(1,1) );
    const auto perr22 = safe_sqrt( part.posCovMatrix()(2,2) );
    ppart.posCov00 = m_pack.position( perr00 );
    ppart.posCov11 = m_pack.position( perr11 );
    ppart.posCov22 = m_pack.position( perr22 );
    ppart.posCov10 = m_pack.fraction( part.posCovMatrix()(1,0), (perr11*perr00) );
    ppart.posCov20 = m_pack.fraction( part.posCovMatrix()(2,0), (perr22*perr00) );
    ppart.posCov21 = m_pack.fraction( part.posCovMatrix()(2,1), (perr22*perr11) );

    // PosMom Cov
    ppart.pmCov00 = m_pack.fltPacked( part.posMomCovMatrix()(0,0) );
    ppart.pmCov01 = m_pack.fltPacked( part.posMomCovMatrix()(0,1) );
    ppart.pmCov02 = m_pack.fltPacked( part.posMomCovMatrix()(0,2) );
    ppart.pmCov10 = m_pack.fltPacked( part.posMomCovMatrix()(1,0) );
    ppart.pmCov11 = m_pack.fltPacked( part.posMomCovMatrix()(1,1) );
    ppart.pmCov12 = m_pack.fltPacked( part.posMomCovMatrix()(1,2) );
    ppart.pmCov20 = m_pack.fltPacked( part.posMomCovMatrix()(2,0) );
    ppart.pmCov21 = m_pack.fltPacked( part.posMomCovMatrix()(2,1) );
    ppart.pmCov22 = m_pack.fltPacked( part.posMomCovMatrix()(2,2) );
    ppart.pmCov30 = m_pack.fltPacked( part.posMomCovMatrix()(3,0) );
    ppart.pmCov31 = m_pack.fltPacked( part.posMomCovMatrix()(3,1) );
    ppart.pmCov32 = m_pack.fltPacked( part.posMomCovMatrix()(3,2) );

    // extra info
    ppart.firstExtra = pparts.extra().size();
    for ( const auto& E : part.extraInfo() )
    {
      pparts.extra().emplace_back( PackedDataVector::PackedExtraInfo(E.first,
                                                                     m_pack.fltPacked(E.second)) );
    }
    ppart.lastExtra = pparts.extra().size();

    // end vertex
    if ( part.endVertex() )
    {
      ppart.vertex = m_pack.reference64( &pparts,
                                         part.endVertex()->parent(),
                                         part.endVertex()->key() );
    }

    // protoparticle
    if ( part.proto() )
    {
      ppart.proto = m_pack.reference64( &pparts,
                                        part.proto()->parent(),
                                        part.proto()->key() );
    }

    // daughters
    ppart.firstDaughter = pparts.daughters().size();
    for ( const auto& P : part.daughters() )
    {
      if ( P.target() )
      {
        pparts.daughters().push_back( m_pack.reference64( &pparts,
                                                          P->parent(),
                                                          P->key() ) );
      }
    }
    ppart.lastDaughter = pparts.daughters().size();

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }

}

void ParticlePacker::pack( const DataVector & parts,
                           PackedDataVector & pparts ) const
{
  pparts.data().reserve( parts.size() );

  for ( const Data * part : parts )
  {
    // Make a new packed data object and save
    pparts.data().emplace_back( PackedData() );
    auto & ppart = pparts.data().back();

    // fill ppart key from part
    ppart.key = part->key();

    // Pack all the physics information
    pack( *part, ppart, pparts );
  }

}

void ParticlePacker::unpack( const PackedData       & ppart,
                             Data                   & part,
                             const PackedDataVector & pparts,
                             DataVector             & parts ) const
{
  if ( 0 == pparts.packingVersion() ||
       1 == pparts.packingVersion() )
  {
    const bool isVZero = ( 0 == pparts.packingVersion() );

    // particle ID
    part.setParticleID( LHCb::ParticleID(ppart.particleID) );

    // Mass and error
    part.setMeasuredMass   ( m_pack.mass(ppart.measMass)    );
    part.setMeasuredMassErr( m_pack.mass(ppart.measMassErr) );

    // Lorentz momentum vector
    const auto pz = m_pack.energy( ppart.lv_pz );
    const auto px = ( isVZero ?
                        m_pack.slope ( ppart.lv_px ) * pz :
                        m_pack.energy( ppart.lv_px ) );
    const auto py = ( isVZero ?
                        m_pack.slope ( ppart.lv_py ) * pz :
                        m_pack.energy( ppart.lv_py ) );
    const auto mass = ppart.lv_mass;
    const auto E    = safe_sqrt( (px*px) + (py*py) + (pz*pz) + (mass*mass) );
    part.setMomentum( Gaudi::LorentzVector( px, py, pz, E ) );

    // reference point
    part.setReferencePoint( Gaudi::XYZPoint( m_pack.position(ppart.refx),
                                             m_pack.position(ppart.refy),
                                             m_pack.position(ppart.refz) ) );

    // Mom Cov
    auto & momCov = *(const_cast<Gaudi::SymMatrix4x4*>(&part.momCovMatrix()));
    const auto merr00 = ( isVZero ?
                            m_pack.slope ( ppart.momCov00 ) * px :
                            m_pack.energy( ppart.momCov00 ) );
    const auto merr11 = ( isVZero ?
                            m_pack.slope ( ppart.momCov11 ) * py :
                            m_pack.energy( ppart.momCov11 ) );
    const auto merr22 = m_pack.energy( ppart.momCov22 );
    const auto merr33 = m_pack.energy( ppart.momCov33 );
    momCov(0,0) = std::pow( merr00, 2 );
    momCov(1,1) = std::pow( merr11, 2 );
    momCov(2,2) = std::pow( merr22, 2 );
    momCov(3,3) = std::pow( merr33, 2 );
    momCov(1,0) = merr11*merr00 * m_pack.fraction( ppart.momCov10 );
    momCov(2,0) = merr22*merr00 * m_pack.fraction( ppart.momCov20 );
    momCov(2,1) = merr22*merr11 * m_pack.fraction( ppart.momCov21 );
    momCov(3,0) = merr33*merr00 * m_pack.fraction( ppart.momCov30 );
    momCov(3,1) = merr33*merr11 * m_pack.fraction( ppart.momCov31 );
    momCov(3,2) = merr33*merr22 * m_pack.fraction( ppart.momCov32 );

    // Pos Cov
    auto & posCov = *(const_cast<Gaudi::SymMatrix3x3*>(&part.posCovMatrix()));
    const auto perr00 = m_pack.position( ppart.posCov00 );
    const auto perr11 = m_pack.position( ppart.posCov11 );
    const auto perr22 = m_pack.position( ppart.posCov22 );
    posCov(0,0) = std::pow( perr00, 2 );
    posCov(1,1) = std::pow( perr11, 2 );
    posCov(2,2) = std::pow( perr22, 2 );
    posCov(1,0) = perr11*perr00 * m_pack.fraction( ppart.posCov10 );
    posCov(2,0) = perr22*perr00 * m_pack.fraction( ppart.posCov20 );
    posCov(2,1) = perr22*perr11 * m_pack.fraction( ppart.posCov21 );

    // Pos Mom Cov
    auto & pmCov = *(const_cast<Gaudi::Matrix4x3*>(&part.posMomCovMatrix()));
    pmCov(0,0) = m_pack.fltPacked( ppart.pmCov00 );
    pmCov(0,1) = m_pack.fltPacked( ppart.pmCov01 );
    pmCov(0,2) = m_pack.fltPacked( ppart.pmCov02 );
    pmCov(1,0) = m_pack.fltPacked( ppart.pmCov10 );
    pmCov(1,1) = m_pack.fltPacked( ppart.pmCov11 );
    pmCov(1,2) = m_pack.fltPacked( ppart.pmCov12 );
    pmCov(2,0) = m_pack.fltPacked( ppart.pmCov20 );
    pmCov(2,1) = m_pack.fltPacked( ppart.pmCov21 );
    pmCov(2,2) = m_pack.fltPacked( ppart.pmCov22 );
    pmCov(3,0) = m_pack.fltPacked( ppart.pmCov30 );
    pmCov(3,1) = m_pack.fltPacked( ppart.pmCov31 );
    pmCov(3,2) = m_pack.fltPacked( ppart.pmCov32 );

    // extra info
    for ( auto iE = ppart.firstExtra; iE < ppart.lastExtra; ++iE )
    {
      const PackedDataVector::PackedExtraInfo& pInfo = pparts.extra()[iE];
      part.addInfo( pInfo.first, m_pack.fltPacked(pInfo.second) );
    }

    // end vertex
    if ( -1 != ppart.vertex )
    {
      int hintID(0), key(0);
      if ( m_pack.hintAndKey64( ppart.vertex, &pparts, &parts, hintID, key ) )
      {
        SmartRef<LHCb::Vertex> ref(&parts,hintID,key);
        part.setEndVertex( ref );
      }
      else { parent().Error("Corrupt Particle Vertex SmartRef found").ignore(); }
    }

    // protoparticle
    if ( -1 != ppart.proto )
    {
      int hintID(0), key(0);
      if ( m_pack.hintAndKey64( ppart.proto, &pparts, &parts, hintID, key ) )
      {
        SmartRef<LHCb::ProtoParticle> ref(&parts,hintID,key);
        part.setProto( ref );
      }
      else { parent().Error("Corrupt Particle ProtoParticle SmartRef found").ignore(); }
    }

    // daughters
    for ( auto iiD = ppart.firstDaughter; iiD < ppart.lastDaughter; ++iiD )
    {
      int hintID(0), key(0);
      if ( m_pack.hintAndKey64( pparts.daughters()[iiD],
                                &pparts, &parts, hintID, key ) )
      {
        SmartRef<LHCb::Particle> ref(&parts,hintID,key);
        part.addToDaughters( ref );
      }
      else { parent().Error("Corrupt Particle Daughter Particle SmartRef found").ignore(); }
    }

  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }
}

void ParticlePacker::unpack( const PackedDataVector & pparts,
                             DataVector       & parts ) const
{
  parts.reserve( pparts.data().size() );
  for ( const PackedData & ppart : pparts.data() )
  {
    // make and save new pid in container
    Data * part = new Data();
    parts.insert( part, ppart.key );
    // Fill data from packed object
    unpack( ppart, *part, pparts, parts );
  }
}

StatusCode ParticlePacker::check( const DataVector & dataA,
                                  const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    sc = sc && check( **iA, **iB );
  }

  // Return final status
  return sc;
}

StatusCode ParticlePacker::check( const Data & dataA,
                                  const Data & dataB ) const
{
  // assume OK from the start
  bool ok = true;

  // checker
  const DataPacking::DataChecks ch(parent());

  // checks here

  // key
  ok &= ch.compareInts( "Key", dataA.key(), dataB.key() );

  // PID
  ok &= ch.compareInts( "PID", dataA.particleID(), dataB.particleID() );

  // Mass
  ok &= ch.compareEnergies( "MeasuredMass",
                            dataA.measuredMass(), dataB.measuredMass() );

  ok &= ch.compareEnergies( "MeasuredMassError",
                            dataA.measuredMassErr(), dataB.measuredMassErr() );

  // momentum
  ok &= ch.compareLorentzVectors( "Momentum",
                                  dataA.momentum(), dataB.momentum() );

  // reference position
  ok &= ch.comparePoints( "ReferencePoint",
                          dataA.referencePoint(), dataB.referencePoint() );

  // Mom Cov
  const boost::array<double,4> tolDiagMomCov = {{ 5.0e-3, 5.0e-3, 5.0e-3, 5.0e-3 }};
  ok &= ch.compareCovMatrices<Gaudi::SymMatrix4x4,4>( "MomCov",
                                                      dataA.momCovMatrix(),
                                                      dataB.momCovMatrix(),
                                                      tolDiagMomCov, 2.0e-5 );

  // Pos Cov
  const boost::array<double,3> tolDiagPosCov = {{ 5.0e-3, 5.0e-3, 5.0e-3 }};
  ok &= ch.compareCovMatrices<Gaudi::SymMatrix3x3,3>( "PosCov",
                                                      dataA.posCovMatrix(),
                                                      dataB.posCovMatrix(),
                                                      tolDiagPosCov, 2.0e-5 );

  // PosMom Cov
  ok &= ch.compareMatrices<Gaudi::Matrix4x3,4,3>( "PosMomCov",
                                                  dataA.posMomCovMatrix(),
                                                  dataB.posMomCovMatrix() );

  // Extra info
  const bool extraSizeOK = dataA.extraInfo().size() == dataB.extraInfo().size();
  ok &= extraSizeOK;
  if ( extraSizeOK )
  {
    LHCb::Particle::ExtraInfo::const_iterator iEA = dataA.extraInfo().begin();
    LHCb::Particle::ExtraInfo::const_iterator iEB = dataB.extraInfo().begin();
    for ( ; iEA != dataA.extraInfo().end() && iEB != dataB.extraInfo().end();
          ++iEA, ++iEB )
    {
      std::ostringstream mess;
      mess << "ExtraInfo:" << (LHCb::Particle::additionalInfo)iEA->first;
      const bool keyOK   = iEA->first == iEB->first;
      if ( !keyOK ) parent().warning() << mess.str() << " Different Keys" << endmsg;
      ok &= keyOK;
      const double relTol = 1.0e-3;
      double tol = relTol * fabs(iEA->second);
      if ( tol < relTol ) tol = relTol;
      const bool valueOK = ch.compareDoubles( mess.str(),
                                              iEA->second, iEB->second,
                                              tol );
      ok &= valueOK;
    }
  }
  else
  {
    parent().warning() << "ExtraInfo has different sizes" << endmsg;
  }

  // end vertex
  ok &= ch.comparePointers( "EndVertex", dataA.endVertex(), dataB.endVertex() );

  // proto particle
  ok &= ch.comparePointers( "ProtoParticle", dataA.proto(), dataB.proto() );

  // daughters
  const bool dauSizeOK = dataA.daughters().size() == dataB.daughters().size();
  ok &= dauSizeOK;
  if ( dauSizeOK )
  {
    SmartRefVector<LHCb::Particle>::const_iterator iDA = dataA.daughters().begin();
    SmartRefVector<LHCb::Particle>::const_iterator iDB = dataB.daughters().begin();
    for ( ; iDA != dataA.daughters().end() && iDB != dataB.daughters().end();
          ++iDA, ++iDB )
    {
      ok &= ch.comparePointers( "Daughters", &**iDA, &**iDB );
    }
  }
  else
  {
    parent().warning() << "Daughters different sizes" << endmsg;
  }

  // force printout for tests
  //ok = false;
  // If comparison not OK, print full information
  if ( !ok )
  {
    const std::string loc = ( dataA.parent() && dataA.parent()->registry() ?
                              dataA.parent()->registry()->identifier() : "Not in TES" );
    parent().warning() << "Problem with Particle data packing :-" << endmsg
                       << "  Original Particle key=" << dataA.key()
                       << " in '" << loc << "'" << endmsg
                       << dataA << endmsg
                       << "  Unpacked Particle" << endmsg
                       << dataB << endmsg;
  }

  return ( ok ? StatusCode::SUCCESS : StatusCode::FAILURE );
}
