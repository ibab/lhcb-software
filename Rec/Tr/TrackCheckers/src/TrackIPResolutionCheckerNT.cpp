// Include files 
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Kernel/HitPattern.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// Boost
#include <boost/foreach.hpp>

/** @class TrackMonitor TrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackIPResolutionCheckerNT : public GaudiTupleAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackIPResolutionCheckerNT( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackIPResolutionCheckerNT();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

private:
  std::string m_tracklocation;    ///< Input Tracks container location
};

DECLARE_ALGORITHM_FACTORY( TrackIPResolutionCheckerNT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackIPResolutionCheckerNT::TrackIPResolutionCheckerNT(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator )
{
  declareProperty( "TrackContainer", m_tracklocation = LHCb::TrackLocation::Velo );
}

//=============================================================================
// Destructor
//=============================================================================
TrackIPResolutionCheckerNT::~TrackIPResolutionCheckerNT() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackIPResolutionCheckerNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

namespace
{  
    struct ForwardMCHitSorter
    {
        // copy constructor & destructor made by default
        ForwardMCHitSorter(){}
        bool operator()( const LHCb::MCHit* lhs, const LHCb::MCHit* rhs)
        {
            return lhs->entry().z() < rhs->entry().z() ;
        }
    } ;
    struct BackwardMCHitSorter
    {
        BackwardMCHitSorter(){}
        bool operator()( const LHCb::MCHit* lhs, const LHCb::MCHit* rhs)
        {
            return rhs->entry().z() < lhs->entry().z() ;
        }
    } ;

}


//=============================================================================
// Execute
//=============================================================================
StatusCode TrackIPResolutionCheckerNT::execute()
{
  // get the list of tracks
  const LHCb::Tracks* tracks = get<LHCb::Tracks>(m_tracklocation);

  // get the linker table track -> mcparticle
  LinkedTo<LHCb::MCParticle> linker( evtSvc(), msgSvc(), m_tracklocation );  

  // create the ntuple
  Tuple theTuple = nTuple( "tracks" , "" , CLID_ColumnWiseTuple );

  // create a map from all MCParticles to MCHits
  typedef std::vector<const LHCb::MCHit*> MCHitVector ;
  typedef std::map<const LHCb::MCParticle *, MCHitVector > MCHitMap ;  
  MCHitMap mchitmap ;
  const std::string mchitlocation = "/Event/MC/VP/Hits" ;
  const LHCb::MCHits* hits = get<LHCb::MCHits>( mchitlocation ) ;
  BOOST_FOREACH( const LHCb::MCHit* mchit, *hits ) {
    if( mchit->mcParticle() )
        mchitmap[ mchit->mcParticle() ].push_back( mchit ) ;
  }
  
  
  // loop over the tracks
  BOOST_FOREACH(const LHCb::Track* aTrack, *tracks) {
    double tx = aTrack->firstState().tx() ;
    double ty = aTrack->firstState().ty() ;
      
    theTuple->column("p", aTrack->p()/Gaudi::Units::GeV );
    theTuple->column("probChi2", aTrack->probChi2() );
    theTuple->column("chi2",aTrack->chi2() );
    theTuple->column("ndof",aTrack->nDoF() );
    theTuple->column("type",aTrack->type() ) ;
    theTuple->column("z", aTrack->firstState().z() ) ;
    theTuple->column("x", aTrack->firstState().x() ) ;
    theTuple->column("y", aTrack->firstState().y() ) ;
    theTuple->column("tx", tx ) ;
    theTuple->column("ty", ty ) ;
    theTuple->column("qop", aTrack->firstState().qOverP() ) ;
    theTuple->column("veloChi2", aTrack->info(LHCb::Track::FitVeloChi2,0)) ;
    theTuple->column("veloNdof", aTrack->info(LHCb::Track::FitVeloNDoF,0)) ;
    theTuple->column("TChi2", aTrack->info(LHCb::Track::FitTChi2,0)) ;
    theTuple->column("TNdof", aTrack->info(LHCb::Track::FitTNDoF,0)) ;
    
    LHCb::HitPattern hitpattern(aTrack->lhcbIDs()) ;
    theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
    theTuple->column("numVeloStationsOverlap",hitpattern.numVeloStationsOverlap()) ;
    theTuple->column("numITStationsOverlap",hitpattern.numITStationsOverlap()) ;
    theTuple->column("numITOTStationsOverlap",hitpattern.numITOTStationsOverlap()) ;
    theTuple->column("numVeloHoles",hitpattern.numVeloHoles()) ;
    theTuple->column("numTHoles",hitpattern.numTHoles()) ;
    theTuple->column("numTLayers",hitpattern.numTLayers()) ;
    theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
    theTuple->column("numVeloClusters",hitpattern.numVeloR()+hitpattern.numVeloPhi()) ;
    theTuple->column("numhits",aTrack->lhcbIDs().size()) ;     // latest one implemented
      
    int waslinked = false ;
    const LHCb::MCParticle* mcparticle = linker.first(aTrack) ;
    if(mcparticle && mcparticle->originVertex()) {
      waslinked= true ;
      
      Gaudi::XYZPoint trueorigin = mcparticle->originVertex()->position() ;
      const LHCb::State& state   = aTrack->firstState() ;
      theTuple->column("truex",trueorigin.x()) ;
      theTuple->column("truey",trueorigin.y()) ;
      theTuple->column("truez",trueorigin.z()) ;
      theTuple->column("truemom",mcparticle->p()) ;
      theTuple->column("truepid",mcparticle->particleID().pid()) ;
      const LHCb::MCParticle* mother = mcparticle->originVertex()->mother() ;
      theTuple->column("truemotherpid", mother? mother->particleID().pid() : 0 ) ;
      theTuple->column("truepx",mcparticle->momentum().Px()) ;
      theTuple->column("truepy",mcparticle->momentum().Py()) ;
      theTuple->column("truepz",mcparticle->momentum().Pz()) ;
      theTuple->column("truept",mcparticle->momentum().Pt()) ;
      theTuple->column("trueeta",mcparticle->momentum().eta()) ;   
      theTuple->column("truephi",mcparticle->momentum().phi()) ;

      double dz = trueorigin.z() - state.z() ;
      double IPx = (state.x() + dz* state.tx()) - trueorigin.x() ;
      double IPy = (state.y() + dz* state.ty()) - trueorigin.y() ;
      theTuple->column("IPx",IPx) ;
      theTuple->column("IPy",IPy) ;
      theTuple->column("IP3D",std::sqrt( (IPx*IPx+IPy*IPy)/(1 + tx*tx + ty*ty))) ;
        
      // store the z-position of the first MC hit
      MCHitVector& mchits = mchitmap[mcparticle] ;
      if( mcparticle->momentum().Pz() > 0)
          std::sort( mchits.begin(), mchits.end(), ForwardMCHitSorter() ) ;
      else
          std::sort( mchits.begin(), mchits.end(), BackwardMCHitSorter() ) ;
          
        
      theTuple->column("nummchits",mchits.size()) ;
      if( !mchits.empty() ) {
        const LHCb::MCHit* mchit = mchits.front() ;
        const LHCb::MCHit* mchitL = mchits.back() ;      // added by me
        Gaudi::XYZPoint poshit = mchit->entry() ;
        theTuple->column("firstmchitz",poshit.z()) ;
        theTuple->column("firstmchitdz",mchit->displacement().z()) ;
        theTuple->column("lastmchitz",mchitL->entry().z()) ;
        double dz = poshit.z() - state.z() ;
        theTuple->column("IPxfirsthit",(state.x() + dz* state.tx()) - poshit.x()) ;
        theTuple->column("IPyfirsthit",(state.y() + dz* state.ty()) - poshit.y()) ;
        theTuple->column("truetxfirsthit",mchit->dxdz()) ;
        theTuple->column("truetyfirsthit",mchit->dydz()) ;
        theTuple->column("dtxfirsthit",state.tx() -  mchit->dxdz()) ;
        theTuple->column("dtyfirsthit",state.ty() -  mchit->dydz()) ;

        theTuple->column("truetxlasthit",mchitL->dxdz()) ;
        theTuple->column("truetylasthit",mchitL->dydz()) ;  
      
          
        // let's now extrapolate the mchit of the first hit to the z position of the vertex, as if there were no scattering
        dz = trueorigin.z() - poshit.z() ;
        double extrapolatedmchitx = poshit.x() + dz * mchit->dxdz() ;
        double extrapolatedmchity = poshit.y() + dz * mchit->dydz() ;

        dz = trueorigin.z() - state.z() ;
        theTuple->column("IPxfirsthitatvertex",(state.x() + dz* state.tx()) - extrapolatedmchitx) ;        
        theTuple->column("IPyfirsthitatvertex",(state.y() + dz* state.ty()) - extrapolatedmchity) ;
        

          
      }
    }
    theTuple->column("waslinked",waslinked) ;    
    theTuple->write();
  }

  return StatusCode::SUCCESS ;
  
}


