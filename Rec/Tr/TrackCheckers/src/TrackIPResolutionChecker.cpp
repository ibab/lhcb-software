// Include files 
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

#include "GaudiUtils/Aida2ROOT.h"

// from Event
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Kernel/HitPattern.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/AlgFactory.h"
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

private:
  std::string m_tracklocation ;    ///< Input Tracks container location
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

  return StatusCode::SUCCESS;
}

namespace {
  Gaudi::Math::ValueWithError calc3sigmarms(const TH1& h1)
  {
    //finally, the 3sigma RMS. that one is most hard. we want the limits
    //that are 3*RMS such that the RMS within these limits is
    //consistent.
    
    // lets first map the histogram into something more symmetric. the
    // lazy way. FIXME: not needed for IP3D!
    TH1F h1pos("h1pos","",h1.GetNbinsX()/2,0,h1.GetXaxis()->GetXmax()) ;
    for(int ibin=1; ibin<=h1.GetNbinsX(); ++ibin) 
      h1pos.Fill( std::abs(h1.GetBinCenter(ibin)), h1.GetBinContent(ibin)) ;
    // now just start counting
    double sumw(0),sumx2w(0) ;
    double xtrunc(0);
    for(int ibin=1; ibin<=h1pos.GetNbinsX(); ++ibin) {
      double x = h1pos.GetBinCenter(ibin) ;
      double c = h1pos.GetBinContent(ibin) ;
      double up = h1pos.GetXaxis()->GetBinUpEdge(ibin) ;
      
      double newsumw   = sumw + c ;
      double newsumx2w = sumx2w + c*x*x ;
      if(sumw>0) {
	double newrms    = sqrt(  newsumx2w / newsumw ) ;
	if( 3 * newrms < up ) {
	  double drms = newrms - sqrt(  sumx2w / sumw ) ;
	  double frac = (3*drms)/h1pos.GetXaxis()->GetBinWidth(ibin) ;
	  //std::cout << frac << std::endl ;
	  xtrunc = (up - (1-frac)*h1pos.GetXaxis()->GetBinWidth(ibin))/3  ;
	  break ;
	}
      }
      sumw = newsumw ;
      sumx2w = newsumx2w ;
    }
    return sumw > 0 ? Gaudi::Math::ValueWithError(xtrunc, xtrunc*xtrunc/sumw) : Gaudi::Math::ValueWithError(0,0) ;
  }

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

}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackIPResolutionChecker::finalize()
{
  // let's do some analysis, as an example
  const TH2* IP3DVsInvTruePtH2 = Gaudi::Utils::Aida2ROOT::aida2root ( histo2D( HistoID("IP/IP3DVsInvTruePtH2") ) ) ;
  // create it
  TH1* IP3DResolutionVsInvTruePt = Gaudi::Utils::Aida2ROOT::aida2root ( book1D( "IP/IP3DResolutionVsInvTruePt",
										IP3DVsInvTruePtH2->GetTitle(),
										IP3DVsInvTruePtH2->GetXaxis()->GetXmin(),
										IP3DVsInvTruePtH2->GetXaxis()->GetXmax(),
										IP3DVsInvTruePtH2->GetNbinsX() ) ) ;
  
  //IP3DResolutionVsInvTruePt->Sumw2() ;
  // now fill it
  for( int ibin=1; ibin<= IP3DResolutionVsInvTruePt->GetNbinsX(); ++ibin) {
    TH1* h1tmp = IP3DVsInvTruePtH2->ProjectionY("tmp",ibin,ibin) ;
    if( h1tmp->Integral() > 20 ) {
      Gaudi::Math::ValueWithError sigma = calc3sigmarms(*h1tmp) ;
      IP3DResolutionVsInvTruePt->SetBinContent(ibin, sigma.value()) ;
      IP3DResolutionVsInvTruePt->SetBinError(ibin, sigma.error()) ;
    }
    delete h1tmp ;
  }
  TF1 pol1("pol1","[0] + x * [1]",0.2,3) ;
  pol1.SetLineColor(8) ;
  IP3DResolutionVsInvTruePt->Fit(&pol1,"R") ;
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
  LinkedTo<LHCb::MCParticle> linker( evtSvc(), msgSvc(), m_tracklocation );  

  // create the list of true PVs. count how many reconstructed tracks in each PV.
  std::map< const LHCb::MCVertex*, int > truepvs ;
  
  // loop over the tracks
  BOOST_FOREACH(const LHCb::Track* track, tracks) {
    const LHCb::MCParticle* mcparticle = linker.first(track) ;
    if( mcparticle && mcparticle->originVertex() ) {
      // make some selection for the IP study
      if(mcparticle->momentum().Pz()>0 &&
	 mcparticle->momentum().Pt()> 200 ) {
	
	double trueptGeV = mcparticle->momentum().Pt() / Gaudi::Units::GeV ;
	double truephi = mcparticle->momentum().Phi() ;
	double invtrueptGeV = 1/trueptGeV ;
	plot1D( trueptGeV, "IP/TruePtH1", "true Pt in GeV", 0, 5 ) ;
	
	plot(track->lhcbIDs().size(),"NumLHCBIDs","Number of LHCbIDs",-0.5,40.5,41) ;
	Gaudi::XYZPoint trueorigin = mcparticle->originVertex()->position() ;
	plot(trueorigin.z(),"IP/TrueOriginZ","True origin z",-20,20) ;
	plot(trueorigin.x(),"IP/TrueOriginX","True origin x",-2,2) ;
	plot(trueorigin.y(),"IP/TrueOriginY","True origin y",-2,2) ;
	
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
	plot2D( invtrueptGeV, IPx, "IP/IPXVsInvTruePtH2", "IPx versus 1/pt_true", 0, maxinvpt, -maxip, maxip, nbinsinvpt, 200 ) ;
	plot2D( invtrueptGeV, IPy, "IP/IPYVsInvTruePtH2", "IPy versus 1/pt_true", 0, maxinvpt, -maxip, maxip, nbinsinvpt, 200 ) ;
	plot2D( invtrueptGeV, IP3D, "IP/IP3DVsInvTruePtH2", "IP versus 1/pt_true", 0, maxinvpt, 0, maxip, nbinsinvpt, 200 ) ;

	if( trueptGeV > 0.5 ) {
	  const double pi = M_PI ;
	  plot2D( truephi, IPx, "IP/IPXVsTruePhiH2", "IPx versus phi_{true}", -pi, pi, -maxip, maxip, 24, 100 ) ;
	  plot2D( truephi, IPy, "IP/IPYVsTruePhiH2", "IPy versus phi_{true}", -pi, pi, -maxip, maxip, 24, 100 ) ;
	}
	
	plot1D( IPx, "IP/IPxH1","IP x", -maxip,maxip) ;
	plot1D( IPy, "IP/IPyH1","IP y", -maxip,maxip) ;
	plot1D( IP3D, "IP/IP3DH1","IP 3D",0,maxip) ;
	plot1D( IPx/std::sqrt( state.covariance()(0,0) ), "IP/IPxPullH1","IP x pull", -5,5) ;
	plot1D( IPy/std::sqrt( state.covariance()(1,1) ), "IP/IPyPullH1","IP y pull", -5,5) ;
      }
      
      // for computing IPs, we use the true origin vertex, even if this is not a PV.
      // however, we also need the list of PVs, to look at PV resolution.
      const LHCb::MCVertex* pv = findpv( *mcparticle ) ;
      if(pv) truepvs[pv] +=1 ;
    }
  }

  // remove PVs that are not reconstructable
  std::vector< const LHCb::MCVertex* > reconstructabletruepvs ;
  for(std::map< const LHCb::MCVertex*, int >::const_iterator it = truepvs.begin() ;
      it != truepvs.end(); ++it) {
    plot1D( it->second, "PV/TruePVNumTracksH1","Number of reconstructed tracks in true PVs", 0.5, 50.5, 50 ) ;
    if( it->second >= m_minNumTracksReconstructablePV ) reconstructabletruepvs.push_back( it->first) ;
  }
  
  plot2D( reconstructabletruepvs.size(), pvs.size(), "PV/NumPVsVsNumTruePVs","Number of PVs vs number of true reconstructable PVs",
	  -0.5,10.5,-0.5,10.5,11,11) ;
  
  BOOST_FOREACH(const LHCb::RecVertex* pv, pvs) {
    // plot the number of tracks per PV
    plot1D( pv->tracks().size(), "PV/RecoPVNumTracksH1","Number of reconstructed tracks in reconstructed PVs", 0.5, 50.5, 50 ) ;
    
    // find the closest PV, in absolute distance
    const LHCb::MCVertex* truepv(0) ;
    double bestdistance2 = 0 ;
    BOOST_FOREACH(const LHCb::MCVertex* thistruepv, reconstructabletruepvs) {
      double distance2 = ( pv->position() - thistruepv->position() ).Mag2() ;
      if( truepv==0 || distance2<bestdistance2 ) {
	bestdistance2 = distance2 ;
	truepv = thistruepv ;
      }
    }
    if( truepv ) {
      Gaudi::XYZVector delta = pv->position() -truepv->position() ;
      plot1D(delta.x(),"PV/dxH1","x_{PV} - x_{true}", -0.2,0.2) ;
      plot1D(delta.y(),"PV/dyH1","y_{PV} - y_{true}", -0.2,0.2) ;
      plot1D(delta.z(),"PV/dzH1","z_{PV} - z_{true}", -2,2) ;
      plot1D(delta.x()/std::sqrt(pv->covMatrix()(0,0)),"PV/dxpullH1","x_{PV} - x_{true} pull", -5,5) ;
      plot1D(delta.y()/std::sqrt(pv->covMatrix()(1,1)),"PV/dypullH1","y_{PV} - y_{true} pull", -5,5) ;
      plot1D(delta.z()/std::sqrt(pv->covMatrix()(2,2)),"PV/dzpullH1","z_{PV} - z_{true} pull", -5,5) ;
    }
  }

  // Finally, we also want to monitor the reconstructed IP, so the IP with respect to the reconstructed PVs.
  if( !pvs.empty() ) {
    BOOST_FOREACH(const LHCb::Track* track, tracks) {
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
      plot1D( std::sqrt(bestip2), "IPRecPV/IP3DH1","IP 3D wrt to reconstructed vertex",0,maxip) ;

      LHCb::State stateAtVtx = tracktraj.state( pv->position().z() ) ;
      double dx  = stateAtVtx.x() - pv->position().x() ;
      double dy  = stateAtVtx.y() - pv->position().y() ;
      plot1D( dx, "IPRecPV/IPxH1","IP X wrt reconstructed vertex", -maxip, maxip ) ;
      plot1D( dy, "IPRecPV/IPyH1","IP Y wrt reconstructed vertex", -maxip, maxip ) ;
      
      // now compute the errors. this isn't quite right because
      // - PV is biased
      // - correction for Z error is approximate
      double tx = stateAtVtx.tx() ;
      double sigmaX2 = state.covariance()(0,0) + pv->covMatrix()(0,0) +
	2*tx*pv->covMatrix()(0,2)+ tx*tx * pv->covMatrix()(2,2)  ;
      plot1D( dx / std::sqrt( sigmaX2 ), "IPRecPV/IPxPullH1","IP X pull wrt reconstructed vertex", -5,5 ) ;
      double ty = stateAtVtx.ty() ;
      double sigmaY2 = state.covariance()(1,1) + pv->covMatrix()(1,1) +
	2*ty*pv->covMatrix()(1,2)+ ty*ty * pv->covMatrix()(2,2)  ;
      plot1D( dx / std::sqrt( sigmaY2 ), "IPRecPV/IPyPullH1","IP Y pull wrt reconstructed vertex", -5,5 ) ;
    }
  }
  
  return StatusCode::SUCCESS ;
}


