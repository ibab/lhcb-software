// Include files
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Particle.h"  
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "TupleToolConeIsolation.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolConeIsolation
//
// 2015-02-19 : Simone Bifani, Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TupleToolConeIsolation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolConeIsolation::TupleToolConeIsolation( const std::string &type,
						const std::string &name,
						const IInterface *parent )
: TupleToolBase ( type, name, parent )
{

  declareInterface<IParticleTupleTool>( this );

  declareProperty( "MinConeSize", m_minConeSize = 0.4,
                   "Set the minimum cone size (default = 0.4)" );
  declareProperty( "MaxConeSize", m_maxConeSize = 0.6,
                   "Set the maximum cone size (default = 0.6)" );
  declareProperty( "SizeStep", m_sizeStep = 0.1,
                   "Set the cone size step between two iterations (default = 0.1)" );

  declareProperty( "FillCharged", m_fillCharged = true,
                   "Flag to fill the charged cone (default = true)" );
  declareProperty( "ExtraParticlesLocation", m_extraParticlesLocation = "Phys/StdAllNoPIDsMuons/Particles",
                   "Set the type of particles that are considered in the charged cone (default = Phys/StdAllNoPIDsMuons/Particles)" );
  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks that are considered in the cone (default = 3)" );

  declareProperty( "FillNeutral", m_fillNeutral = true,
                   "Flag to fill the neutral cone (default = true)" );
  declareProperty( "ExtraPhotonsLocation", m_extraPhotonsLocation = "Phys/StdLooseAllPhotons/Particles",
                   "Set the type of particles that are considered in the neutral cone (default = Phys/StdLooseAllPhotons/Particles)" );

  declareProperty( "FillAsymmetry", m_fillAsymmetry = false,
		   "Flag to fill the asymmetry variables (default = false)" );
  declareProperty( "FillDeltas", m_fillDeltas = false,
		   "Flag to fill the delta variables (default = false)" );
  declareProperty( "FillIsolation", m_fillIsolation = true,
                   "Flag to fill the isolation variables (default = true)" );
  declareProperty( "FillMaxPt", m_fillMaxPt = true,
                   "Flag to fill the momentum of the max-pT object in the cone (default = true)" );
  declareProperty( "MaxPtParticlesLocation", m_maxPtParticleLocation = "Phys/StdAllLooseMuons/Particles",
                   "Set the type of max-pT particles that are considered in the charged cone (default = Phys/StdAllLooseMuons/Particles)" );

  declareProperty( "FillComponents", m_fillComponents = false,
                   "Flag to fill the components of all relevant variables (default = false)" );

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolConeIsolation::~TupleToolConeIsolation() {} 
//=============================================================================
// Initialization
//=============================================================================
StatusCode TupleToolConeIsolation::initialize() {

  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() )
    return sc;

  if ( !m_fillCharged && !m_fillNeutral ) {
    if ( msgLevel( MSG::FATAL ) )
      fatal() << "No cone to fill" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( m_minConeSize > m_maxConeSize) {
    if ( msgLevel( MSG::FATAL ) )
      fatal() << "Max conesize smaller than min conesize" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  if ( isVerbose() ) {
    m_fillCharged   = true;
    m_fillNeutral   = true;
    m_fillAsymmetry = true;
    m_fillDeltas    = true;
    m_fillIsolation = true;
    m_fillMaxPt     = true;
  }

  return StatusCode::SUCCESS;

}
//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode TupleToolConeIsolation::fill( const LHCb::Particle *top,
					 const LHCb::Particle *seed,
					 const std::string &head,
					 Tuples::Tuple &tuple ) {

  const std::string prefix = fullName( head );

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Fill" << endmsg;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay  according to the decay descriptor.

  // -- Clear the vector with the particles in the specific decay
  m_decayParticles.clear();
  
  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_decayParticles.push_back( top );

  // -- Save all particles that belong to the given decay in the vector m_decayParticles
  saveDecayParticles( top );

  // -- Check particle containers
  const LHCb::Particles *parts = get<LHCb::Particles>( m_extraParticlesLocation );
  if ( m_fillCharged )
    if ( parts->size() == 0 )
      if ( msgLevel( MSG::WARNING ) )
	warning() << "Could not retrieve extra-particles... Skipping" << endmsg;
  const LHCb::Particles *maxPts = get<LHCb::Particles>( m_maxPtParticleLocation );
  if ( m_fillMaxPt )
    if ( maxPts->size() == 0 )
      if ( msgLevel( MSG::WARNING ) )
	warning() << "Could not retrieve maxPt-particle... Skipping" << endmsg;
  const LHCb::Particles *photons = get<LHCb::Particles>( m_extraPhotonsLocation );
  if ( m_fillNeutral )
    if ( photons->size() == 0 )
      if ( msgLevel( MSG::WARNING ) )
	warning() << "Could not retrieve extra-photons... Skipping" << endmsg;

  bool test = true;
      
  if ( seed ) {     
    if ( msgLevel( MSG::VERBOSE ) )
      verbose() << "Start looping through different conesizes" << endmsg;
    
    // --  Loop over the different conesizes
    double coneSize = m_minConeSize;
    while ( coneSize <= m_maxConeSize ) {

      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "Filling variables with conesize " << coneSize << endmsg;

      // -- Convert the conesize to a string
      char coneNumber[4];
      sprintf( coneNumber, "%.2f", coneSize );
      std::string conesize( coneNumber );

      // -- Charged cone
      Gaudi::XYZVector ccP;
      if ( m_fillCharged ) {
	if ( parts->size() != 0 ) {
	  int ccMultiplicity = 0;
	  double ccScalarPt = 0.;
	  std::vector<double> ccMomentum;
	  std::vector<double> ccMaxPt;

	  StatusCode ccSc = ChargedCone( seed, parts, maxPts, coneSize, ccMultiplicity, ccScalarPt, ccMomentum, ccMaxPt );
	  if ( ccSc.isFailure() )
	    ccMultiplicity = -1;

	  ccP.SetX( ccMomentum[0] );
	  ccP.SetY( ccMomentum[1] );
	  ccP.SetZ( ccMomentum[2] );

	  double ccDeltaEta = seed->momentum().Eta() - ccP.Eta();
	  double ccDeltaPhi = fabs( seed->momentum().Phi() - ccP.Phi() );
	  if ( ccDeltaPhi > M_PI )
	    ccDeltaPhi = 2 * M_PI - ccDeltaPhi;

	  // -- Fill the tuple with the variables
	  test &= tuple->column( prefix + "_" + conesize + "_cc_mult", ccMultiplicity );
	  test &= tuple->column( prefix + "_" + conesize + "_cc_sPT",  ccScalarPt );
	  test &= tuple->column( prefix + "_" + conesize + "_cc_vPT",  sqrt( ccP.Perp2() ) );

	  if ( m_fillComponents ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PX", ccP.X() );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PY", ccP.Y() );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PZ", ccP.Z() );
	  }

	  if ( m_fillAsymmetry ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_asy_P",  ( seed->p()  - sqrt( ccP.Mag2() ) )  / (seed->p()  + sqrt( ccP.Mag2() ) ) );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PT", ( seed->pt() - sqrt( ccP.Perp2() ) ) / (seed->pt() + sqrt( ccP.Perp2() ) ) );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PX", ( seed->momentum().Px() - ccP.X() )  / (seed->momentum().Px() + ccP.X()) );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PY", ( seed->momentum().Py() - ccP.Y() )  / (seed->momentum().Py() + ccP.Y()) );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PZ", ( seed->momentum().Pz() - ccP.Z() )  / (seed->momentum().Pz() + ccP.Z()) );
	    }
	  }

	  if ( m_fillDeltas ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_deltaEta", ccDeltaEta );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_deltaPhi", ccDeltaPhi );
	  }

	  if ( m_fillIsolation )
	    test &= tuple->column( prefix + "_" + conesize + "_cc_IT", seed->pt() / sqrt( pow(seed->momentum().Px() + ccP.X(), 2) + pow(seed->momentum().Py() + ccP.Y(), 2) ) );

	  if ( m_fillMaxPt ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_Q",  (int) ccMaxPt[4] );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PT", sqrt( pow(ccMaxPt[0], 2) + pow(ccMaxPt[1], 2) ) );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PX", ccMaxPt[0] );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PY", ccMaxPt[1] );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PZ", ccMaxPt[2] );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PE", ccMaxPt[3] );
	    }
	  }
	}
	else {
	  test &= tuple->column( prefix + "_" + conesize + "_cc_mult", 0  );
	  test &= tuple->column( prefix + "_" + conesize + "_cc_sPT",  0. );
	  test &= tuple->column( prefix + "_" + conesize + "_cc_vPT",  0. );

	  if ( m_fillComponents ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PX", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PY", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_PZ", 0. );
	  }

	  if ( m_fillAsymmetry ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_asy_P",  1. );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PT", 1. );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PX", 1. );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PY", 1. );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_asy_PZ", 1. );
	    }
	  }

	  if ( m_fillDeltas ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_deltaEta", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_deltaPhi", 0. );
	  }

	  if ( m_fillIsolation )
	    test &= tuple->column( prefix + "_" + conesize + "_cc_IT", 1. );

	  if ( m_fillMaxPt ) {
	    test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_Q",  0  );
	    test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PT", 0. );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PX", 0. );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PY", 0. );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PZ", 0. );
	      test &= tuple->column( prefix + "_" + conesize + "_cc_maxPt_PE", 0. );
	    }
	  }
	}
      }

      // -- Neutral cone
      Gaudi::XYZVector ncP;
      if ( m_fillNeutral ) {
	if ( photons->size() != 0 ) {
	  int ncMultiplicity = 0;
	  double ncScalarPt = 0.;
	  std::vector<double> ncMomentum;
	  std::vector<double> ncMaxPt;

	  StatusCode ncSc = NeutralCone( seed, photons, coneSize, ncMultiplicity, ncScalarPt, ncMomentum, ncMaxPt );
	  if ( ncSc.isFailure() )
	    ncMultiplicity = -1;

	  ncP.SetX( ncMomentum[0] );
	  ncP.SetY( ncMomentum[1] );
	  ncP.SetZ( ncMomentum[2] );

	  double ncDeltaEta = seed->momentum().Eta() - ncP.Eta();
	  double ncDeltaPhi = fabs( seed->momentum().Phi() - ncP.Phi() );
	  if ( ncDeltaPhi > M_PI )
	    ncDeltaPhi = 2 * M_PI - ncDeltaPhi;

	  // -- Fill the tuple with the variables
	  test &= tuple->column( prefix + "_" + conesize + "_nc_mult", ncMultiplicity);
	  test &= tuple->column( prefix + "_" + conesize + "_nc_sPT",  ncScalarPt );
	  test &= tuple->column( prefix + "_" + conesize + "_nc_vPT",  sqrt( ncP.Perp2() ) );

	  if ( m_fillComponents ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PX", ncP.X() );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PY", ncP.Y() );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PZ", ncP.Z() );
	  }

	  if ( m_fillAsymmetry ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_asy_P",  ( seed->p()  - sqrt( ncP.Mag2() ) )  / (seed->p()  + sqrt( ncP.Mag2() ) ) );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PT", ( seed->pt() - sqrt( ncP.Perp2() ) ) / (seed->pt() + sqrt( ncP.Perp2() ) ) );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PX", ( seed->momentum().Px() - ncP.X() )  / (seed->momentum().Px() + ncP.X()) );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PY", ( seed->momentum().Py() - ncP.Y() )  / (seed->momentum().Py() + ncP.Y()) );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PZ", ( seed->momentum().Pz() - ncP.Z() )  / (seed->momentum().Pz() + ncP.Z()) );
	    }
	  }

	  if ( m_fillDeltas ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_deltaEta", ncDeltaEta );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_deltaPhi", ncDeltaPhi );
	  }

	  if ( m_fillIsolation )
	    test &= tuple->column( prefix + "_" + conesize + "_nc_IT", seed->pt() / sqrt( pow(seed->momentum().Px() + ncP.X(), 2) + pow(seed->momentum().Py() + ncP.Y(), 2) ) );

	  if ( m_fillMaxPt ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PT", sqrt( pow(ncMaxPt[0], 2) + pow(ncMaxPt[1], 2) ) );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PX", ncMaxPt[0] );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PY", ncMaxPt[1] );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PZ", ncMaxPt[2] );
	    }
	  }
	}
	else {
	  test &= tuple->column( prefix + "_" + conesize + "_nc_mult", 0  );
	  test &= tuple->column( prefix + "_" + conesize + "_nc_sPT",  0. );
	  test &= tuple->column( prefix + "_" + conesize + "_nc_vPT",  0. );

	  if ( m_fillComponents ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PX", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PY", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_PZ", 0. );
	  }

	  if ( m_fillAsymmetry ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_asy_P",  1. );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PT", 1. );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PX", 1. );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PY", 1. );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_asy_PZ", 1. );
	    }
	  }

	  if ( m_fillDeltas ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_deltaEta", 0. );
	    test &= tuple->column( prefix + "_" + conesize + "_nc_deltaPhi", 0. );
	  }

	  if ( m_fillIsolation )
	    test &= tuple->column( prefix + "_" + conesize + "_nc_IT", 1. );

	  if ( m_fillMaxPt ) {
	    test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PT", 0. );
	    if ( m_fillComponents ) {
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PX", 0. );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PY", 0. );
	      test &= tuple->column( prefix + "_" + conesize + "_nc_maxPt_PZ", 0. );
	    }
	  }
	}
      }

      if ( m_fillIsolation ) {
	if ( m_fillCharged && m_fillNeutral ) {
	  if ( ( parts->size() != 0 ) && ( photons->size() != 0 ) )
	    test &= tuple->column( prefix + "_" + conesize + "_IT", seed->momentum().Pt() / sqrt( pow(seed->momentum().Px() + ccP.X() + ncP.X(), 2) + pow(seed->momentum().Py() + ccP.Y() + ncP.Y(), 2) ) );
	  else
	    test &= tuple->column( prefix + "_" + conesize + "_IT", 1. );
	}
      }

      // -- Increase the counter with the stepsize
      coneSize += m_sizeStep;
    }
  } 
  else {
    if ( msgLevel( MSG::WARNING ) )
      warning() << "The seed particle is not valid. Skipping" << endmsg;

    return StatusCode::FAILURE;
  }

  if ( msgLevel( MSG::VERBOSE ) )
    verbose() << "Stop looping through different conesizes" << endmsg;

  return StatusCode( test );

}
//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void TupleToolConeIsolation::saveDecayParticles( const LHCb::Particle *top ) {

  // -- Get the daughters of the top particle
  const SmartRefVector< LHCb::Particle > daughters = top->daughters();
  
  // -- Fill all the daugthers in m_decayParticles
  for ( SmartRefVector< LHCb::Particle >::const_iterator ip = daughters.begin(); ip != daughters.end(); ++ip ) {
    
    // -- If the particle is stable, save it in the vector, or...
    if ( (*ip)->isBasicParticle() ) {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Filling particle with ID " << (*ip)->particleID().pid() << endmsg;
      m_decayParticles.push_back( (*ip) );
    }
    else {
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( (*ip) );
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Filling particle with ID " << (*ip)->particleID().pid() << endmsg;
      saveDecayParticles( (*ip) );
    }
  }

  return;
  
}
//=============================================================================
// Loop over all the tracks in the cone which do not belong to the desired decay
//=============================================================================
StatusCode TupleToolConeIsolation::ChargedCone( const LHCb::Particle  *seed,
						const LHCb::Particles *parts,
						const LHCb::Particles *maxPts,
						const double rcut,
						int &multiplicity,
						double &scalarPt,
						std::vector<double> &momentum,
						std::vector<double> &maxPt ) {

  // -- Initialize values
  multiplicity = 0;
  scalarPt = 0;
  double conePx  = 0;
  double conePy  = 0;
  double conePz  = 0;
  double maxPtPx = 0;
  double maxPtPy = 0;
  double maxPtPz = 0;
  double maxPtPe = 0;
  double maxPtQ  = 0;

  // -- Get the momentum of the seed particle
  Gaudi::XYZVector seedMomentum = seed->momentum().Vect();

  for ( LHCb::Particles::const_iterator ip = parts->begin(); ip != parts->end(); ++ip ) {
    const LHCb::Particle *particle = (*ip);

    const LHCb::ProtoParticle *proto = particle->proto();
    if ( proto ) {

      const LHCb::Track *track = proto->track();
      if ( track ) {

        // -- Check if the track belongs to the decay itself
        bool isInDecay = isTrackInDecay( track );
        if ( isInDecay )
          continue;

        Gaudi::XYZVector trackMomentum = track->momentum();

        // -- Calculate the difference in Eta and Phi between the seed particle and a track
        double deltaPhi = fabs( seedMomentum.Phi() - trackMomentum.Phi() );
        if ( deltaPhi > M_PI )
          deltaPhi = 2 * M_PI - deltaPhi;
        double deltaEta = seedMomentum.Eta() - trackMomentum.Eta();
        double deltaR   = sqrt( pow(deltaPhi, 2) + pow(deltaEta, 2) );
      
	if ( ( rcut == 0 ) || ( deltaR < rcut ) ) {
	  if ( track->type() == m_trackType ) {

            multiplicity++;

            // -- Calculate scalar information
            scalarPt += track->pt();

            // -- Calculate vector information
            conePx += trackMomentum.X();
            conePy += trackMomentum.Y();
            conePz += trackMomentum.Z();

	    // -- Find max-pT particle
	    if ( maxPts->size() != 0 ) {
	      if ( track->pt() > sqrt( pow(maxPtPx, 2) + pow(maxPtPy, 2) ) ) {

		for ( LHCb::Particles::const_iterator iPt = maxPts->begin(); iPt != maxPts->end(); ++iPt ) {
		  const LHCb::Particle *particlePt = (*iPt);

		  const LHCb::ProtoParticle *protoPt = particlePt->proto();
		  if ( protoPt ) {

		    if ( protoPt == proto ) {

		      const LHCb::Track *trackPt = protoPt->track();
		      if ( trackPt ) {
			maxPtPx = trackPt->momentum().X();
			maxPtPy = trackPt->momentum().Y();
			maxPtPz = trackPt->momentum().Z();
			maxPtPe = particlePt->momentum().E();
			maxPtQ  = trackPt->charge();
		      }

		      break;
		    }
		  }
		}
	      }
	    }
	    else {
	      maxPtPx = 0.;
	      maxPtPy = 0.;
	      maxPtPz = 0.;
	      maxPtPe = 0.;
	      maxPtQ  = 0;
	    }
	  }
	}
      }
    }
  }

  momentum.push_back( conePx );
  momentum.push_back( conePy );
  momentum.push_back( conePz );

  maxPt.push_back( maxPtPx );
  maxPt.push_back( maxPtPy );
  maxPt.push_back( maxPtPz );
  maxPt.push_back( maxPtPe );
  maxPt.push_back( maxPtQ );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
// Loop over all the photons in the cone
//=============================================================================
StatusCode TupleToolConeIsolation::NeutralCone( const LHCb::Particle  *seed,
						const LHCb::Particles *photons,
						const double rcut,
						int &multiplicity,
						double &scalarPt,
						std::vector<double> &momentum,
						std::vector<double> &maxPt ) {

  // -- Initialize values
  multiplicity = 0;
  scalarPt = 0;
  double conePx  = 0;
  double conePy  = 0;
  double conePz  = 0;
  double maxPtPx = 0;
  double maxPtPy = 0;
  double maxPtPz = 0;

  // -- Get the momentum of the seed particle
  Gaudi::XYZVector seedMomentum = seed->momentum().Vect();

  for ( LHCb::Particles::const_iterator ip = photons->begin(); ip != photons->end(); ++ip ) {
    const LHCb::Particle *photon = (*ip);

    // -- Get the momentum of the photon
    Gaudi::XYZVector photonMomentum = photon->momentum().Vect();

    // -- Calculate the difference in Eta and Phi between the seed particle and a photons
    double deltaPhi = fabs( seedMomentum.Phi() - photonMomentum.Phi() );
    if ( deltaPhi > M_PI )
      deltaPhi = 2 * M_PI - deltaPhi;
    double deltaEta = seedMomentum.Eta() - photonMomentum.Eta();
    double deltaR   = sqrt( pow(deltaPhi, 2) + pow(deltaEta, 2) );
      
    if ( ( rcut == 0 ) || ( deltaR < rcut ) ) {
      multiplicity++;

      // -- Calculate scalar information
      scalarPt += sqrt( photonMomentum.Perp2() );
            
      // -- Calculate vector information
      conePx += photonMomentum.X();
      conePy += photonMomentum.Y();
      conePz += photonMomentum.Z();

      // Extra Photon
      if ( sqrt( photonMomentum.Perp2() ) > sqrt( pow(maxPtPx, 2) + pow(maxPtPy, 2) ) ) {
	maxPtPx = photonMomentum.X();
	maxPtPy = photonMomentum.Y();
	maxPtPz = photonMomentum.Z();
      }
    }

  }

  momentum.push_back( conePx );
  momentum.push_back( conePy );
  momentum.push_back( conePz );

  maxPt.push_back( maxPtPx );
  maxPt.push_back( maxPtPy );
  maxPt.push_back( maxPtPz );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
// Check if the track is already in the decay
//=============================================================================
bool TupleToolConeIsolation::isTrackInDecay( const LHCb::Track *track ) {

  bool isInDecay = false;
  
  for ( std::vector<const LHCb::Particle*>::iterator ip = m_decayParticles.begin(); ip != m_decayParticles.end(); ++ip ) {  
    const LHCb::ProtoParticle *proto = (*ip)->proto();
    if ( proto ) {

      const LHCb::Track *myTrack = proto->track();
      if ( myTrack ) {

        if ( myTrack == track ) {
          if ( msgLevel( MSG::DEBUG ) )
            debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }

      }
    }
  }
  
  return isInDecay;

}
