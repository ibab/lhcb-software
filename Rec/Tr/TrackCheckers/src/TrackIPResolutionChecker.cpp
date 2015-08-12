// Include files 
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

#include "GaudiUtils/Aida2ROOT.h"

// from Event
#include "Event/MCHeader.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/TrackFitResult.h"
#include "Kernel/HitPattern.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "LHCbMath/ValueWithError.h"

#include "TrackKernel/CubicStateInterpolationTraj.h"

#include "TH2.h"
#include "TH1.h"
#include "TF1.h"

// Boost
#include <boost/foreach.hpp>

/** @class TrackIPResolutionChecker TrackIPResolutionChecker.cpp "TrackCheckers/TrackIPResolutionChecker"
 * 
 * Class for track monitoring
 *  @author W. Hulsbergen and P. Tsopelas
 *  @date   21-01-2013
 */                 
                                                           
class TrackIPResolutionChecker : public GaudiHistoAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackIPResolutionChecker( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackIPResolutionChecker();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

 /** Algorithm finalize */
  virtual StatusCode finalize();

private:
  void createResolutionProfile( const HistoID& inputname, const HistoID& outputname ) ;
private:
  std::string m_tracklocation ;    ///< Input Tracks container location
  std::string m_linkerlocation ;
  std::string m_pvlocation ;
  int m_minNumTracksReconstructablePV ;
};

DECLARE_ALGORITHM_FACTORY( TrackIPResolutionChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackIPResolutionChecker::TrackIPResolutionChecker(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator )
{
  declareProperty( "TrackContainer", m_tracklocation = LHCb::TrackLocation::Velo );
  declareProperty( "LinkerLocation", m_linkerlocation ) ;
  declareProperty( "PVContainer", m_pvlocation = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "MinNumTracksReconstructablePV", m_minNumTracksReconstructablePV = 5 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackIPResolutionChecker::~TrackIPResolutionChecker() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackIPResolutionChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }
  if( m_linkerlocation.empty() ) m_linkerlocation = m_tracklocation ;

  return StatusCode::SUCCESS;
}

namespace {
  Gaudi::Math::ValueWithError calc3sigmarms(const TH1& h1)
  {
    //finally, the 3sigma RMS. that one is most hard. we want the limits
    //that are 3*RMS such that the RMS within these limits is
    //consistent.
    
    // lets first map the histogram into something more symmetric. the
    // lazy way.
    const TH1* h1pos = &h1 ;
    TH1F* ownedh1pos(0) ;
    if( h1.GetXaxis()->GetXmin() < 0 ) {
      // create a new histogram that has exactly half the bins and just fill it with abs(x)
      ownedh1pos = new TH1F("h1pos","",h1.GetNbinsX()/2,0,h1.GetXaxis()->GetXmax()) ;
      for(int ibin=1; ibin<=h1.GetNbinsX(); ++ibin) 
	ownedh1pos->Fill( std::abs(h1.GetBinCenter(ibin)), h1.GetBinContent(ibin)) ;
      h1pos = ownedh1pos ;
    }
    
    // now just start counting
    double sumw(0),sumx2w(0) ;
    double xtrunc(0);
    int ibin=1 ;
    for(; ibin<=h1pos->GetNbinsX(); ++ibin) {
      double x = h1pos->GetBinCenter(ibin) ;
      double c = h1pos->GetBinContent(ibin) ;
      double up = h1pos->GetXaxis()->GetBinUpEdge(ibin) ;
      
      double newsumw   = sumw + c ;
      double newsumx2w = sumx2w + c*x*x ;
      if(sumw>0 && x > h1pos->GetMean() ) {
	double newrms    = sqrt(  newsumx2w / newsumw ) ;
	if( 3 * newrms < up ) {
	  double drms = newrms - sqrt(  sumx2w / sumw ) ;
	  double frac = (3*drms)/h1pos->GetXaxis()->GetBinWidth(ibin) ;
	  //std::cout << frac << std::endl ;
	  xtrunc = (up - (1-frac)*h1pos->GetXaxis()->GetBinWidth(ibin))/3  ;
	  break ;
	}
      }
      sumw = newsumw ;
      sumx2w = newsumx2w ;
    }
    if(ibin>h1pos->GetNbinsX() && sumw>0 )  // just return the rms
      xtrunc = sqrt( sumx2w / sumw ) ;
    if(ownedh1pos) delete ownedh1pos ;
    return sumw > 0 ? Gaudi::Math::ValueWithError(xtrunc, xtrunc*xtrunc/sumw) : Gaudi::Math::ValueWithError(0,0) ;
  }

  // return the ultiimate origin vertex of this particle
  const LHCb::MCVertex* findpv(const LHCb::MCParticle& p)
  {
    const LHCb::MCVertex* vertex = p.originVertex() ;
    if( vertex && !vertex->isPrimary() ) {
      const LHCb::MCParticle* mother = vertex->mother() ;
      // if the mother origin vertex is less than 1 micron away from this vertex, then we also consider the mother's vertex as origin.
      const double tolerance = 1e-6 ; // 1 micron^2
      if( mother && mother->originVertex() &&
	  (mother->originVertex() == vertex ||
	   (mother->originVertex()->position() - vertex->position()).Mag2() < tolerance ) ) // less than 1 micron
	{
	  vertex = findpv( *mother ) ;
	} else {
	vertex = 0 ;
      }
    }
    return vertex ;
  }
  
  // count the number of charged stable daughters in the LHCb acceptance for this particle
  size_t numChargedDaughtersInAcceptance( const LHCb::MCVertex& vertex ) 
  {
    size_t rc(0) ;
    BOOST_FOREACH( const LHCb::MCParticle* particle, vertex.products() ) {
      // if 'resonance' then just add it
      const LHCb::MCVertex* endvertex = particle->endVertices().size()>0 ? &(*(particle->endVertices()[0])) : 0 ;
      if( endvertex && std::abs(endvertex->position().z() - vertex.position().z()) < 0.001*Gaudi::Units::mm )
	rc += numChargedDaughtersInAcceptance( *endvertex) ;
      // add charged daughters that live long enough
      else if(// require that it is charged
	      particle->particleID().threeCharge()!=0 // charged 
	      // require a minimum momentum to traverse the velo
	      && particle->p()>300*Gaudi::Units::MeV 
	      // require that its eta is within the acceptance
	      && 2 < particle->pseudoRapidity() && particle->pseudoRapidity() < 5
	      // require that there is no endvertex, or that it is sort-of outside the velo
	      && (endvertex==0 || (endvertex->position() - vertex.position()).Rho() > 10*Gaudi::Units::cm ) ) {
	++rc ;
      }
    }
    return rc ;
  }

}

// routine that creates a resolution profile from a 2D histogram
void TrackIPResolutionChecker::createResolutionProfile( const HistoID& inputname,
							const HistoID& outputname )
{
  // get the histogram
  const TH2* h2 = Gaudi::Utils::Aida2ROOT::aida2root ( histo2D( inputname ) ) ;
  // create the 1D histogram
  if( !h2 ) {
    debug() << "Cannot find histogram with name: " << inputname << endreq ;
  } else {
    std::string title = h2->GetTitle() ;
    size_t ipos = title.find(" versus") ;
    if( ipos==std::string::npos ) ipos = title.find(" vs") ;
    if( ipos!=std::string::npos ) title.insert(ipos," resolution") ;
    
    TH1* h1 = Gaudi::Utils::Aida2ROOT::aida2root ( book1D(outputname,
							  title.c_str(),
							  h2->GetXaxis()->GetXmin(),
							  h2->GetXaxis()->GetXmax(),
							  h2->GetNbinsX() ) ) ;
    
    // now fill it
    for( int ibin=1; ibin<= h2->GetNbinsX(); ++ibin) {
      TH1* h1tmp = h2->ProjectionY("tmp",ibin,ibin) ;
      if( h1tmp->Integral() > 5 ) {
	Gaudi::Math::ValueWithError sigma = calc3sigmarms(*h1tmp) ;
	h1->SetBinContent(ibin, sigma.value()) ;
	h1->SetBinError(ibin, sigma.error()) ;
      }
      delete h1tmp ;
    }
  }
}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackIPResolutionChecker::finalize()
{
  // fill some resolution profiles
  const char names[3][256] = { "IP/Velo/","IP/Long/","IP/Backward/" } ;
  for(int i=0; i<3; ++i) {
    std::string prefix(names[i]) ;
    createResolutionProfile( prefix + "IP3DVsInvTruePtH2", prefix + "IP3DResolutionVsInvTruePt") ;
    createResolutionProfile( prefix + "IPxVsInvTruePtH2",  prefix + "IPxResolutionVsInvTruePt") ;
    createResolutionProfile( prefix + "IPyVsInvTruePtH2",  prefix + "IPyResolutionVsInvTruePt") ;
  }
  createResolutionProfile( "PV/dxVersusNTrk","PV/PVXResolutionVsNTrk") ;
  createResolutionProfile( "PV/dyVersusNTrk","PV/PVYResolutionVsNTrk") ;
  createResolutionProfile( "PV/dzVersusNTrk","PV/PVZResolutionVsNTrk") ;
  
  return GaudiHistoAlg::finalize();
}




//=============================================================================
// Execute
//=============================================================================
StatusCode TrackIPResolutionChecker::execute()
{
  // some constant that should probably become properties
  const double maxinvpt = 3.0 ;
  const double maxip = 0.5 ;
  const int nbinsinvpt = 15 ;
  
  // get the list of tracks
  const LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_tracklocation);

  // get the list of PVs as well
  const LHCb::RecVertex::Range pvs = get<LHCb::RecVertex::Range>(m_pvlocation) ;

  // get the linker table track -> mcparticle
  LinkedTo<LHCb::MCParticle> linker( evtSvc(), msgSvc(), m_linkerlocation );

  // loop over the tracks
  BOOST_FOREACH(const LHCb::Track* track, tracks) {
    const LHCb::MCParticle* mcparticle = linker.first(track) ;
    if( mcparticle && mcparticle->originVertex() ) {
      // make some selection for the IP study
      if(mcparticle->momentum().Pz()>0 && 2 < mcparticle->pseudoRapidity() && mcparticle->pseudoRapidity() < 5 ) {
	
	std::string prefix = std::string("IP/") + Gaudi::Utils::toString(track->type()) + "/" ;

	double trueptGeV = mcparticle->momentum().Pt() / Gaudi::Units::GeV ;
	plot1D( trueptGeV, prefix + "TruePtH1", "true Pt in GeV", 0, 5 ) ;
	plot1D( mcparticle->p() / Gaudi::Units::GeV, prefix + "TruePH1", "true momentum in GeV", 0, 100 ) ;
	int trackWasFitted = track->fitResult() && !track->fitResult()->nodes().empty() ;
	plot1D( trackWasFitted, prefix + "TrackWasFittedH1","track was fitted by K-filter",-0.5,1.5,2) ;
          
	if( trueptGeV > 0.2 ) {
	  double truephi = mcparticle->momentum().Phi() ;
	  double invtrueptGeV = 1/trueptGeV ;
	  
	  plot(track->lhcbIDs().size(),"NumLHCBIDs","Number of LHCbIDs",-0.5,40.5,41) ;
	  Gaudi::XYZPoint trueorigin = mcparticle->originVertex()->position() ;
	  plot(trueorigin.z(),prefix + "TrueOriginZ","True origin z",-100,100) ;
	  plot(trueorigin.x(),prefix + "TrueOriginX","True origin x",-0.5,0.5) ;
	  plot(trueorigin.y(),prefix + "TrueOriginY","True origin y",-0.5,0.5) ;
	  
	  // for now, the track state is simply firststate
	  const LHCb::State& firststate = track->firstState() ;
	  // we can use the CubicStateInterpolationTraj (or later tracktraj) to do the error propagation
	  LHCb::CubicStateInterpolationTraj tracktraj( firststate, Gaudi::XYZVector() ) ;
	  LHCb::State state = tracktraj.state( trueorigin.z() ) ;
	  double tx = state.tx() ;
	  double ty = state.ty() ;
	  double IPx = state.x() - trueorigin.x() ;
	  double IPy = state.y() - trueorigin.y() ;
	  double IP3D = std::sqrt( (IPx*IPx+IPy*IPy)/(1 + tx*tx + ty*ty) ) ;
	  plot2D( invtrueptGeV, IPx, prefix + "IPxVsInvTruePtH2", "IPx versus 1/pt_true", 0, maxinvpt, -maxip, maxip, nbinsinvpt, 200 ) ;
	  plot2D( invtrueptGeV, IPy, prefix + "IPyVsInvTruePtH2", "IPy versus 1/pt_true", 0, maxinvpt, -maxip, maxip, nbinsinvpt, 200 ) ;
	  plot2D( invtrueptGeV, IP3D, prefix + "IP3DVsInvTruePtH2", "IP versus 1/pt_true", 0, maxinvpt, 0, maxip, nbinsinvpt, 200 ) ;
	  
	  if( trueptGeV > 0.5 ) {
	    const double pi = M_PI ;
	    plot2D( truephi, IPx, prefix + "IPxVsTruePhiH2", "IPx versus phi_{true}", -pi, pi, -maxip, maxip, 24, 100 ) ;
	    plot2D( truephi, IPy, prefix + "IPyVsTruePhiH2", "IPy versus phi_{true}", -pi, pi, -maxip, maxip, 24, 100 ) ;
	  }
	  
	  plot1D( IPx, prefix + "IPxH1","IP x", -maxip,maxip) ;
	  plot1D( IPy, prefix + "IPyH1","IP y", -maxip,maxip) ;
	  plot1D( IP3D, prefix + "IP3DH1","IP 3D",0,maxip) ;
	  plot1D( IPx/std::sqrt( state.covariance()(0,0) ), prefix + "IPxPullH1","IP x pull", -5,5) ;
	  plot1D( IPy/std::sqrt( state.covariance()(1,1) ), prefix + "IPyPullH1","IP y pull", -5,5) ;
	}
      }
    }
  }

  // keep track of which true PVs were found
  const LHCb::MCHeader* mcheader = get<const LHCb::MCHeader*>(LHCb::MCHeaderLocation::Default) ;
  std::set<const LHCb::MCVertex*> foundpvs ;
  BOOST_FOREACH(const LHCb::RecVertex* pv, pvs) {
    // plot the number of tracks per PV
    plot1D( pv->tracks().size(), "PV/RecoPVNumTracksH1","Number of reconstructed tracks in reconstructed PVs", 0.5, 50.5, 50 ) ;
    
    // find the closest PV, in absolute distance
    const LHCb::MCVertex* truepv(0) ;
    double bestdistance2 = 0 ;
    BOOST_FOREACH(const LHCb::MCVertex* thistruepv,  mcheader->primaryVertices() ) {
      double distance2 = ( pv->position() - thistruepv->position() ).Mag2() ;
      if( truepv==0 || distance2<bestdistance2 ) {
	bestdistance2 = distance2 ;
	truepv = thistruepv ;
      }
    }
    if( truepv ) {
      foundpvs.insert( truepv ) ;
      Gaudi::XYZVector delta = pv->position() -truepv->position() ;
      plot1D(delta.x(),"PV/dxH1","x_{PV} - x_{PV,true}", -0.08,0.08) ;
      plot1D(delta.y(),"PV/dyH1","y_{PV} - y_{PV,true}", -0.08,0.08) ;
      plot1D(delta.z(),"PV/dzH1","z_{PV} - z_{PV,true}", -0.4,0.4) ;
      plot2D(pv->tracks().size(),delta.x(),"PV/dxVersusNTrk","x_{PV} - x_{PV,true} vs number of tracks",5,105,-0.08,0.08,20,80) ;
      plot2D(pv->tracks().size(),delta.y(),"PV/dyVersusNTrk","y_{PV} - y_{PV,true} vs number of tracks",5,105,-0.08,0.08,20,80) ;
      plot2D(pv->tracks().size(),delta.z(),"PV/dzVersusNTrk","z_{PV} - z_{PV,true} vs number of tracks",5,105,-0.4,0.4,20,80) ;
      plot1D(delta.x()/std::sqrt(pv->covMatrix()(0,0)),"PV/dxpullH1","x_{PV} pull", -5,5) ;
      plot1D(delta.y()/std::sqrt(pv->covMatrix()(1,1)),"PV/dypullH1","y_{PV} pull", -5,5) ;
      plot1D(delta.z()/std::sqrt(pv->covMatrix()(2,2)),"PV/dzpullH1","z_{PV} pull", -5,5) ;
    }
  }

  // plot the total number of true PVs
  size_t numtruepvs = mcheader->numOfPrimaryVertices() ;
  plot1D( numtruepvs, "PV/NumTruePVs", "Number of true PVs",-0.5,20.5,21) ;

  // make a plot of PV efficiency versus Z and versus number of tracks
  size_t numreconstructablepvs(0) ;
  BOOST_FOREACH( const LHCb::MCVertex* pv, mcheader->primaryVertices() ) {
    size_t numreconstructable =  numChargedDaughtersInAcceptance( *pv ) ;
    bool found  = foundpvs.find( pv ) != foundpvs.end() ;
    profile1D( double(numreconstructable), found, 
	       "PV/PVEfficiencyVsNumReconstructableTracks", "PV efficiency versus num tracks in acceptance",
	       0.5, 50.5, 50 ) ;
    if( numreconstructable >=5 ) {
      profile1D( pv->position().z(), found, "PV/PVEfficiencyVsZ", "PV efficiency versus Z",-100, 100, 40) ;
      profile1D( numtruepvs, found, "PV/PVEfficiencyVsNumTruePVs", "PV efficiency versus num true PVs",0.5,20.5,20) ;
      ++numreconstructablepvs ;
    }
  }
  plot2D(numreconstructablepvs , pvs.size(),
	 "PV/NumPVsVsNumTrueReconstructablePVs","Number of PVs vs number of true reconstructable PVs",
	 -0.5,20.5,-0.5,20.5,21,21) ;
  
  // Finally, we also want to monitor the reconstructed IP, so the IP with respect to the reconstructed PVs.
  if( !pvs.empty() ) {
    BOOST_FOREACH(const LHCb::Track* track, tracks)
      if( !track->checkFlag( LHCb::Track::Backward) ) {
      const LHCb::MCParticle* mcparticle = linker.first(track) ; 
      // distinghuish secondaries, from primaries, from ghosts
      bool hasMCMatch = mcparticle && mcparticle->originVertex() ;
      bool isFromPV   = hasMCMatch && mcparticle->originVertex()->isPrimary() ;
      std::string prefix = isFromPV ? "IPRecPV/TruePrimary/" :
        ( hasMCMatch ? "IPRecPV/TrueSecondary/" : "IPRecPV/Ghost/" ) ;
          
          
      // find the closest PV, again using the minimal distance
      const LHCb::RecVertex* pv(0) ;
      double bestip2(0) ;
      const LHCb::State& state = track->firstState() ;
      LHCb::CubicStateInterpolationTraj tracktraj( state, Gaudi::XYZVector() ) ;
      Gaudi::XYZPoint trkpos( state.position() ) ;
      Gaudi::XYZVector trkdir( state.slopes().Unit() ) ;
      
      BOOST_FOREACH(const LHCb::RecVertex* thispv, pvs) {
          Gaudi::XYZVector dx = trkpos - thispv->position()  ;
          Gaudi::XYZVector delta = dx - trkdir * dx.Dot(trkdir) ;
          double ip2 = delta.Mag2() ;
          if( pv==0 || ip2 < bestip2 ) {
              bestip2 = ip2 ;
              pv = thispv ;
          }
      }
      
      plot1D( std::sqrt(bestip2), prefix + "IP3DH1","IP 3D wrt to reconstructed vertex",0,maxip) ;

      LHCb::State stateAtVtx = tracktraj.state( pv->position().z() ) ;
      double dx  = stateAtVtx.x() - pv->position().x() ;
      double dy  = stateAtVtx.y() - pv->position().y() ;
      plot1D( dx, prefix + "IPxH1","IP X wrt reconstructed vertex", -maxip, maxip ) ;
      plot1D( dy, prefix + "IPyH1","IP Y wrt reconstructed vertex", -maxip, maxip ) ;
      
      // now compute the errors. this isn't quite right because
      // - PV is biased
      // - correction for Z error is approximate
      double tx = stateAtVtx.tx() ;
      double sigmaX2 = state.covariance()(0,0) + pv->covMatrix()(0,0) +
	2*tx*pv->covMatrix()(0,2)+ tx*tx * pv->covMatrix()(2,2)  ;
      plot1D( dx / std::sqrt( sigmaX2 ), prefix + "IPxPullH1","IP X pull wrt reconstructed vertex", -5,5 ) ;
      double ty = stateAtVtx.ty() ;
      double sigmaY2 = state.covariance()(1,1) + pv->covMatrix()(1,1) +
	2*ty*pv->covMatrix()(1,2)+ ty*ty * pv->covMatrix()(2,2)  ;
      plot1D( dx / std::sqrt( sigmaY2 ), prefix + "IPyPullH1","IP Y pull wrt reconstructed vertex", -5,5 ) ;
    }
  } // end of !pvs.empty()
  
  return StatusCode::SUCCESS ;
}

