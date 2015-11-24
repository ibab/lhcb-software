#include "Event/Particle.h"
#include "Event/RelatedInfoMap.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/RelatedInfoNamed.h"

// local
#include "RelInfoConeIsolation.h"

#include <string>
//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoConeIsolation
//
// 2015-02-19 : Simone Bifani, Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoConeIsolation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoConeIsolation::RelInfoConeIsolation( const std::string &type,
						const std::string &name,
						const IInterface *parent )
: GaudiTool ( type, name , parent )
{

  declareInterface<IRelatedInfoTool>(this);

  declareProperty( "Variables", m_variables,
                   "List of variables to store (store all if empty)");
  declareProperty( "ConeSize", m_coneSize = 0.4,
                   "Set the cone size (default = 0.4)" );

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

  declareProperty( "FillMaxPt", m_fillMaxPt = true,
                   "Flag to fill the neutral cone (default = true)" );
  declareProperty( "MaxPtParticlesLocation", m_maxPtParticleLocation = "Phys/StdAllLooseMuons/Particles",
                   "Set the type of max-pT particles that are considered in the charged cone (default = Phys/StdAllLooseMuons/Particles)" );

}
//=============================================================================
// Destructor
//=============================================================================
RelInfoConeIsolation::~RelInfoConeIsolation() {} 
//=============================================================================
// Initialization
//=============================================================================
StatusCode RelInfoConeIsolation::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    return sc;

  if ( !m_fillCharged && !m_fillNeutral ) {
    if ( msgLevel( MSG::FATAL ) )
      fatal() << "No cone to fill" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize" << endmsg;

  if ( m_variables.empty() )
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << "List of variables empty, adding all" << endmsg;
    if (m_fillCharged) { 
      m_keys.push_back( RelatedInfoNamed::CC_ANGLE );
      m_keys.push_back( RelatedInfoNamed::CC_MULT );
      m_keys.push_back( RelatedInfoNamed::CC_SPT );
      m_keys.push_back( RelatedInfoNamed::CC_VPT );
      m_keys.push_back( RelatedInfoNamed::CC_PX );
      m_keys.push_back( RelatedInfoNamed::CC_PY );
      m_keys.push_back( RelatedInfoNamed::CC_PZ );
      m_keys.push_back( RelatedInfoNamed::CC_PASYM );
      m_keys.push_back( RelatedInfoNamed::CC_PTASYM );
      m_keys.push_back( RelatedInfoNamed::CC_PXASYM );
      m_keys.push_back( RelatedInfoNamed::CC_PYASYM );
      m_keys.push_back( RelatedInfoNamed::CC_PZASYM );
      m_keys.push_back( RelatedInfoNamed::CC_DELTAETA );
      m_keys.push_back( RelatedInfoNamed::CC_DELTAPHI );
      m_keys.push_back( RelatedInfoNamed::CC_IT );
      if (m_fillMaxPt) {
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_Q );
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_PT );
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_PX );
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_PY );
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_PZ );
        m_keys.push_back( RelatedInfoNamed::CC_MAXPT_PE );
      }
    }
    if (m_fillNeutral) {
      m_keys.push_back( RelatedInfoNamed::NC_ANGLE );
      m_keys.push_back( RelatedInfoNamed::NC_MULT );
      m_keys.push_back( RelatedInfoNamed::NC_SPT );
      m_keys.push_back( RelatedInfoNamed::NC_VPT );
      m_keys.push_back( RelatedInfoNamed::NC_PX );
      m_keys.push_back( RelatedInfoNamed::NC_PY );
      m_keys.push_back( RelatedInfoNamed::NC_PZ );
      m_keys.push_back( RelatedInfoNamed::NC_PASYM );
      m_keys.push_back( RelatedInfoNamed::NC_PTASYM );
      m_keys.push_back( RelatedInfoNamed::NC_PXASYM );
      m_keys.push_back( RelatedInfoNamed::NC_PYASYM );
      m_keys.push_back( RelatedInfoNamed::NC_PZASYM );
      m_keys.push_back( RelatedInfoNamed::NC_DELTAETA );
      m_keys.push_back( RelatedInfoNamed::NC_DELTAPHI );
      m_keys.push_back( RelatedInfoNamed::NC_IT );
      if (m_fillMaxPt) {
        m_keys.push_back( RelatedInfoNamed::NC_MAXPT_PT );
        m_keys.push_back( RelatedInfoNamed::NC_MAXPT_PX );
        m_keys.push_back( RelatedInfoNamed::NC_MAXPT_PY );
        m_keys.push_back( RelatedInfoNamed::NC_MAXPT_PZ );
      }
    }
    if (m_fillCharged && m_fillNeutral) {
      m_keys.push_back( RelatedInfoNamed::CCNC_IT );
    }
  } else {

    for ( const auto& var : m_variables )
    {
      short int key = RelatedInfoNamed::indexByName( var );
      if (key != RelatedInfoNamed::UNKNOWN)
      {
        m_keys.push_back( key );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Adding variable " << var << ", key = " << key << endmsg;
      }
      else
      {
        warning() << "Unknown variable " << var << ", skipping" << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;

}
//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode RelInfoConeIsolation::calculateRelatedInfo( const LHCb::Particle *top,
							 const LHCb::Particle *seed ) {

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
  m_map.clear();

  if ( seed ) {     
      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "Start looping through different conesizes" << endmsg;

      // --  Loop over the different conesizes
      double coneSize = m_coneSize;

      if ( msgLevel( MSG::VERBOSE ) )
        verbose() << "Filling variables with conesize " << coneSize << endmsg;

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

	  // -- Fill the map with the variables

	  for ( const auto key : m_keys ) {
	    float value = 0;
	    bool filled = true;
	    switch (key) {
	      case RelatedInfoNamed::CC_ANGLE : value = coneSize; break;
	      case RelatedInfoNamed::CC_MULT : value = ccMultiplicity; break;
	      case RelatedInfoNamed::CC_SPT : value = ccScalarPt; break;
	      case RelatedInfoNamed::CC_VPT : value = sqrt( ccP.Perp2() ); break;
	      case RelatedInfoNamed::CC_PX : value = ccP.X(); break;
	      case RelatedInfoNamed::CC_PY : value = ccP.Y(); break;
	      case RelatedInfoNamed::CC_PZ : value = ccP.Z(); break;
	      case RelatedInfoNamed::CC_PASYM : value = ( seed->p()  - sqrt( ccP.Mag2() ) )  / (seed->p()  + sqrt( ccP.Mag2() ) ); break;
	      case RelatedInfoNamed::CC_PTASYM : value = ( seed->pt() - sqrt( ccP.Perp2() ) ) / (seed->pt() + sqrt( ccP.Perp2() ) ); break;
	      case RelatedInfoNamed::CC_PXASYM : value = ( seed->momentum().Px() - ccP.X() )  / (seed->momentum().Px() + ccP.X()); break;
	      case RelatedInfoNamed::CC_PYASYM : value = ( seed->momentum().Py() - ccP.Y() )  / (seed->momentum().Py() + ccP.Y()); break;
	      case RelatedInfoNamed::CC_PZASYM : value = ( seed->momentum().Pz() - ccP.Z() )  / (seed->momentum().Pz() + ccP.Z()); break;
	      case RelatedInfoNamed::CC_DELTAETA : value = ccDeltaEta; break;
	      case RelatedInfoNamed::CC_DELTAPHI : value = ccDeltaPhi; break;
	      case RelatedInfoNamed::CC_IT : value = seed->pt() / sqrt( pow(seed->momentum().Px() + ccP.X(), 2) + pow(seed->momentum().Py() + ccP.Y(), 2) ); break;
	      case RelatedInfoNamed::CC_MAXPT_Q : value = ccMaxPt[4]; break;
	      case RelatedInfoNamed::CC_MAXPT_PT : value = sqrt( pow(ccMaxPt[0], 2) + pow(ccMaxPt[1], 2) ); break;
	      case RelatedInfoNamed::CC_MAXPT_PX : value = ccMaxPt[0]; break;
	      case RelatedInfoNamed::CC_MAXPT_PY : value = ccMaxPt[1]; break;
	      case RelatedInfoNamed::CC_MAXPT_PZ : value = ccMaxPt[2]; break;
	      case RelatedInfoNamed::CC_MAXPT_PE : value = ccMaxPt[3]; break;
	      default: filled = false; break; 
	    }
	    if (filled) {
              debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;
              m_map.insert( std::make_pair(key,value) );
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

	  for ( const auto key : m_keys ) {
	    float value = 0;
	    bool filled = true; 
	    switch (key) {
	      case RelatedInfoNamed::NC_ANGLE : value = coneSize; break;
	      case RelatedInfoNamed::NC_MULT : value = ncMultiplicity; break;
	      case RelatedInfoNamed::NC_SPT : value = ncScalarPt; break; 
	      case RelatedInfoNamed::NC_VPT : value = sqrt( ncP.Perp2() ); break;
	      case RelatedInfoNamed::NC_PX : value = ncP.X(); break;
	      case RelatedInfoNamed::NC_PY : value = ncP.Y(); break;
	      case RelatedInfoNamed::NC_PZ : value = ncP.Z(); break;
	      case RelatedInfoNamed::NC_PASYM : value = ( seed->p()  - sqrt( ncP.Mag2() ) )  / (seed->p()  + sqrt( ncP.Mag2() ) ); break;
	      case RelatedInfoNamed::NC_PTASYM : value = ( seed->pt() - sqrt( ncP.Perp2() ) ) / (seed->pt() + sqrt( ncP.Perp2() ) ); break;
	      case RelatedInfoNamed::NC_PXASYM : value = ( seed->momentum().Px() - ncP.X() )  / (seed->momentum().Px() + ncP.X()); break;
	      case RelatedInfoNamed::NC_PYASYM : value = ( seed->momentum().Py() - ncP.Y() )  / (seed->momentum().Py() + ncP.Y()); break;
	      case RelatedInfoNamed::NC_PZASYM : value = ( seed->momentum().Pz() - ncP.Z() )  / (seed->momentum().Pz() + ncP.Z()); break;
	      case RelatedInfoNamed::NC_DELTAETA : value = ncDeltaEta; break;
	      case RelatedInfoNamed::NC_DELTAPHI : value = ncDeltaPhi; break;
	      case RelatedInfoNamed::NC_IT : value = seed->pt() / sqrt( pow(seed->momentum().Px() + ncP.X(), 2) + pow(seed->momentum().Py() + ncP.Y(), 2) ); break;
	      case RelatedInfoNamed::NC_MAXPT_PT : value = sqrt( pow(ncMaxPt[0], 2) + pow(ncMaxPt[1], 2) ); break;
	      case RelatedInfoNamed::NC_MAXPT_PX : value = ncMaxPt[0]; break;
	      case RelatedInfoNamed::NC_MAXPT_PY : value = ncMaxPt[1]; break;
	      case RelatedInfoNamed::NC_MAXPT_PZ : value = ncMaxPt[2]; break;
	      default: filled = false; break; 
	    }
	    if (filled) {
              debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;
              m_map.insert( std::make_pair(key,value) );
            }
          }
	}
      }

      if ( m_fillCharged && m_fillNeutral ) {
	if ( ( parts->size() != 0 ) && ( photons->size() != 0 ) )
	  for ( const auto key : m_keys ) {
	    float value = 0;
	    if (key == RelatedInfoNamed::CCNC_IT) {
	      value = seed->momentum().Pt() / sqrt( pow(seed->momentum().Px() + ccP.X() + ncP.X(), 2) + pow(seed->momentum().Py() + ccP.Y() + ncP.Y(), 2) );
              debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;
              m_map.insert( std::make_pair(key,value) );
	    }
          }
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
void RelInfoConeIsolation::saveDecayParticles( const LHCb::Particle *top ) {

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
StatusCode RelInfoConeIsolation::ChargedCone( const LHCb::Particle  *seed,
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
StatusCode RelInfoConeIsolation::NeutralCone( const LHCb::Particle  *seed,
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
bool RelInfoConeIsolation::isTrackInDecay( const LHCb::Track *track ) {

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

LHCb::RelatedInfoMap* RelInfoConeIsolation::getInfo(void)
{
  return &m_map;
}
