// $Id: TrackV0Finder.cpp,v 1.1 2007-12-03 15:08:19 wouter Exp $
// Include files 


// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class TrackV0Finder TrackV0Finder.h
 *  
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-10-08
 */
class ITrajPoca ;
class IMagneticFieldSvc ;
class ParticleProperty ;
class ITrackVertexer ;
class ITrackExtrapolator ;
class ITrackInterpolator ;

class TrackV0Finder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackV0Finder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackV0Finder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  //virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_trackInputListName; // Input Tracks container location
  std::string m_pvContainerName;
  std::string m_v0ContainerName;
  IMagneticFieldSvc* m_magfieldsvc ;
  ITrajPoca* m_pocatool ;
  ITrackExtrapolator* m_extrapolator ;
  ITrackInterpolator* m_interpolator ;
  ITrackVertexer* m_vertexer ;
  const ParticleProperty* m_ksProperty ;
  const ParticleProperty* m_lambdaProperty ;
  const ParticleProperty* m_pionProperty ;
  const ParticleProperty* m_protonProperty ;
  double m_zmin ;
  double m_zmax ;
  double m_distanceCutUpstream ;
  double m_distanceCutLong ;
  double m_deltaZCut ;
  double m_deltaZSignificanceCut ;
  double m_ksmasscut ;
  double m_lambdamasscut ;
  double m_chisqProbCut ;
  bool m_useExtrapolator ;
  bool m_excludePVTracks ;
};

#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Event/TrackTraj.h"
#include "Event/TwoProngVertex.h"
#include "Kernel/ITrajPoca.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"
#include <iterator>


//-----------------------------------------------------------------------------
// Implementation file for class : TrackV0Finder
//
// 2007-10-08 : Wouter HULSBERGEN
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackV0Finder );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackV0Finder::TrackV0Finder( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_trackInputListName(LHCb::TrackLocation::Default),
    m_zmin(-100*Gaudi::Units::cm),
    m_zmax( 300*Gaudi::Units::cm),
    m_distanceCutUpstream(5*Gaudi::Units::mm),
    m_distanceCutLong(1*Gaudi::Units::mm),
    m_deltaZCut(5*Gaudi::Units::cm),
    m_deltaZSignificanceCut(5),
    m_ksmasscut(40*Gaudi::Units::MeV),
    m_lambdamasscut(20*Gaudi::Units::MeV),
    m_chisqProbCut(0),
    m_useExtrapolator(true),
    m_excludePVTracks(true)
{
  declareProperty( "TrackContainer", m_trackInputListName = LHCb::TrackLocation::Default  );
  declareProperty( "PVContainer", m_pvContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "V0Container", m_v0ContainerName = LHCb::TwoProngVertexLocation::Default ) ;
  declareProperty( "KsMassCut", m_ksmasscut) ;
  declareProperty( "LambdaMassCut", m_lambdamasscut) ;
  declareProperty( "UseExtrapolator", m_useExtrapolator) ;
  declareProperty( "ExcludePVTracks", m_excludePVTracks) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackV0Finder::~TrackV0Finder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackV0Finder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_magfieldsvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_pocatool = tool<ITrajPoca>( "TrajPoca" );
  m_vertexer = tool<ITrackVertexer>( "TrackVertexer" );
  if(m_useExtrapolator) {
    m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator/TrackFitExtrapolator" ) ;
    m_interpolator = tool<ITrackInterpolator>( "TrackInterpolator" ) ;
  }

  IParticlePropertySvc* propertysvc = svc<IParticlePropertySvc>("ParticlePropertySvc",true) ;
  m_ksProperty     = propertysvc->find( "KS0" ) ;
  m_lambdaProperty = propertysvc->find( "Lambda0" ) ;
  m_pionProperty   = propertysvc->find( "pi+" ) ;
  m_protonProperty = propertysvc->find( "p+" ) ;
  if( m_ksProperty==0 ||  m_lambdaProperty==0 || m_pionProperty==0 || m_protonProperty==0) {
    error() << "Did not find all properties." << endreq ;
    sc = StatusCode::FAILURE ;
  }
  
  return sc;
}

inline bool inAnyVertex( const LHCb::Track& track,
                         const LHCb::RecVertices& vertexcontainer)
{
  bool found(false) ;
  for( LHCb::RecVertices::const_iterator ivertex = vertexcontainer.begin() ;
       ivertex != vertexcontainer.end() && !found; ++ivertex) 
    for( SmartRefVector<LHCb::Track>::const_iterator itrack = (*ivertex)->tracks().begin() ;
         itrack != (*ivertex)->tracks().end() && !found; ++itrack) 
      found = &track == &(**itrack) ;
  return found ;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackV0Finder::execute() 
{
  //info() << "TrackV0Finder::execute" << std::endl ;
  // Get the primary vertices. Locate the one that's most downstream.
  const LHCb::RecVertices* pvcontainer(0) ;
  try {
    if(exist<LHCb::RecVertices>( m_pvContainerName ))
      pvcontainer = get<LHCb::RecVertices>( m_pvContainerName ) ;
  } catch (const GaudiException & exp) {
    warning() << "caught exception when asking for primary vertices! " << exp << endreq ;
  }

  double zprimary(0) ;
  if( pvcontainer && !pvcontainer->empty() ) {
    zprimary = -9999 ;
    for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ; 
         ipv != pvcontainer->end(); ++ipv ) 
      zprimary = std::max(zprimary, (*ipv)->position().z()) ;
  }
  
  // Get the Tracks
  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackInputListName );

  // Sort them by charge, make some cuts
  typedef std::vector<const LHCb::Track*> TrackContainer ;
  TrackContainer postracks,negtracks ;
  for( LHCb::Tracks::const_iterator it = tracks->begin() ;
       it != tracks->end(); ++it) {
    const LHCb::Track* track = *it ;
    // require tracks with T and (TT or Velo)
    if( track->hasT() && (track->hasVelo()||track->hasTT()) &&
        // remove tracks from PVs, if required
        (pvcontainer==0 || !m_excludePVTracks || !inAnyVertex(*track,*pvcontainer) ) ) {
      if( (*it)->firstState().qOverP()>0 ) postracks.push_back(*it) ;
      else                                 negtracks.push_back(*it) ;
    }
  }
  
  // turn them into trajectories
  typedef std::vector<LHCb::TrackTraj> TrackTrajContainer ;
  TrackTrajContainer postrajs,negtrajs ;
  for( TrackContainer::const_iterator ipos = postracks.begin() ;
       ipos != postracks.end(); ++ipos) 
    postrajs.push_back(LHCb::TrackTraj(**ipos,m_magfieldsvc)) ;
  for( TrackContainer::const_iterator ineg = negtracks.begin() ;
       ineg != negtracks.end(); ++ineg) 
    negtrajs.push_back(LHCb::TrackTraj(**ineg,m_magfieldsvc)) ;
  
  counter("numcombinations") += postracks.size()*negtracks.size() ;
  counter("numpostracks") += postracks.size() ;
  counter("numnegtracks") += negtracks.size() ;
  const double pimass = m_pionProperty->mass() ;
  const double pmass  = m_protonProperty->mass() ;
  const double ksmass  = m_ksProperty->mass() ;
  const double lambdamass = m_lambdaProperty->mass() ;
  
  // Create the output container
  LHCb::RecVertices* v0container = new LHCb::RecVertices() ;
  put(v0container, m_v0ContainerName) ;
  //LHCb::TrackworongContainer* v0container = new LHCb::TrackTwoProngContainer ;

  // Now do some poca studies to see if this works at all
  for( TrackContainer::iterator ipos = postracks.begin() ;
       ipos != postracks.end(); ++ipos) {
    for( TrackContainer::iterator ineg = negtracks.begin() ;
         ineg != negtracks.end(); ++ineg) {
      LHCb::TrackTraj& postraj = postrajs[std::distance(postracks.begin(),ipos)] ;
      LHCb::TrackTraj& negtraj = negtrajs[std::distance(negtracks.begin(),ineg)] ;
      
      // seed the z position with the intersection in the yz-plane
      double mupos = postraj.beginRange() + 10*Gaudi::Units::mm ;
      double muneg = negtraj.beginRange() + 10*Gaudi::Units::mm ;
      double ypos  = postraj.position(mupos).y() ;
      double yneg  = negtraj.position(muneg).y() ;
      double typos = postraj.direction(mupos).y() ;
      double tyneg = negtraj.direction(muneg).y() ;
      double z_yz = - (ypos - yneg + muneg*tyneg - mupos*typos) / ( typos - tyneg ) ;
      mupos = muneg = z_yz ;

      // Calls pocatool
      Gaudi::XYZVector deltaX; 
      StatusCode sc = m_pocatool->minimize( postraj,mupos,negtraj,muneg,deltaX,0.001*Gaudi::Units::mm );
      if( sc.isSuccess() ) {
        double distance = deltaX.R() ;
        double z = 0.5*(mupos+muneg) ;
        // make the cut on the distance and the z-position
        bool isVeloCombi = (*ipos)->hasVelo() && (*ineg)->hasVelo() ;
        if( (distance < m_distanceCutLong ||
             (distance < m_distanceCutUpstream && !isVeloCombi)) &&
            zprimary + m_deltaZCut < z && z< m_zmax ) {
          // now make an invariant mass cut
          Gaudi::XYZVector mompos = postraj.momentum(mupos) ;
          Gaudi::XYZVector momneg = negtraj.momentum(muneg) ;
          Gaudi::LorentzVector p4pos(mompos.X(),mompos.Y(),mompos.Z(),
                                     std::sqrt(mompos.Mag2()+pimass*pimass)) ;
          Gaudi::LorentzVector p4neg(momneg.X(),momneg.y(),momneg.Z(),
                                     std::sqrt(momneg.Mag2()+pimass*pimass)) ;
          double pipimass = (p4pos+p4neg).M() ;
          p4pos.SetE( std::sqrt(mompos.Mag2()+pmass*pmass)) ;
          double ppimass = (p4pos+p4neg).M() ;
          p4pos.SetE( std::sqrt(mompos.Mag2()+pimass*pimass)) ;
          p4neg.SetE( std::sqrt(momneg.Mag2()+pmass*pmass)) ;
          double pipmass = (p4pos+p4neg).M() ;
          if( fabs(pipimass - ksmass)    < m_ksmasscut ||
              fabs(pipmass - lambdamass) < m_lambdamasscut ||
              fabs(ppimass - lambdamass) < m_lambdamasscut ) {
            // Determine the states passed to the vertexer. At this
            // stage combinatorics is small enough that we can
            // eventually use a real extrapolator.
            LHCb::State posstate,negstate ;
            if( m_useExtrapolator ) {
              // If we can, we use interpolators, because those can do
              // this correctly even if we are between nodes. (The
              // extrapolators are guaranteed to be wrong half the
              // time!)
              StatusCode sc ;
              if( (*ipos)->nodes().empty() ) sc = m_extrapolator->propagate( **ipos, z, posstate ) ;
              else                           sc = m_interpolator->interpolate( **ipos, z, posstate ) ;
              if(!sc.isSuccess() ) {
                warning() << "Extrapolation failed. Rely on trajectory interpolation." << endreq ;
                posstate = postraj.state(z) ;
              }
              if( (*ineg)->nodes().empty() ) sc = m_extrapolator->propagate( **ineg, z, negstate ) ;
              else                           sc = m_interpolator->interpolate( **ineg, z, negstate ) ;
              if(!sc.isSuccess() ) {
                warning() << "Extrapolation failed. Rely on trajectory interpolation." << endreq ;
                negstate = negtraj.state(z) ;
              }
            } else {
              posstate = postraj.state(z) ;
              negstate = negtraj.state(z) ;
            }
            
            // finally, create the vertex and cut on the significance
            LHCb::TwoProngVertex* vertex = m_vertexer->fit(posstate,negstate ) ;
            double dz     = vertex->position().z() - zprimary ;
            double sigmaz = std::sqrt(vertex->covMatrix()(2,2)) ;
            if( dz/sigmaz > m_deltaZSignificanceCut ) {
              vertex->addInfo(0,distance) ;
              vertex->addInfo(1,z-zprimary) ;
              vertex->addInfo(2,pipimass) ;
              vertex->addInfo(3,ppimass) ;
              vertex->addInfo(4,pipmass) ;
              vertex->addToTracks(*ipos) ;
              vertex->addToTracks(*ineg) ;
              // create a rec vertex and put it in the event
              v0container->add( vertex ) ;
            } else {
              delete vertex ;
            }
          }
        }
      }
    }
  } 
  counter("numselected") += v0container->size() ;
  return StatusCode::SUCCESS;
}

//=============================================================================
