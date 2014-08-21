// $Id: OTCalibrationAlg.cpp,v 1.4 2010-01-02 22:15:24 wouter Exp $
// Include files

// local
#include "OTCalibrationAlg.h"
#include "IOTCalibrationIOTool.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"

// from Event/TrackEvent
#include "Event/Track.h"
#include "Event/TrackTypes.h"

// from Tr/TrackFitEvent
#include "Event/StateParameters.h"
#include "Event/OTMeasurement.h"
#include "Event/Node.h"
#include "Event/FitNode.h"

// Detector stuff
#include <OTDet/DeOTDetector.h>
#include <OTDet/DeOTModule.h>

// ROOT stuff
#include "TFile.h"
#include <TH1.h>
#include <TProfile.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TH2.h>
#include <TMath.h>
#include <TNtuple.h>

#include <set>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/foreach.hpp>

DECLARE_ALGORITHM_FACTORY( OTCalibrationAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTCalibrationAlg::OTCalibrationAlg( const std::string& name, ISvcLocator* pSvcLocator ) 
  :
  GaudiAlgorithm( name , pSvcLocator ),
  m_projector("TrajOTProjector"),
  m_otdet(0),
  m_numDistanceBins(25),
  m_maxDistance(2.5),
  m_maxDistanceError(0.5),
  m_minTime(-10),
  m_maxResidual(1.0),
  m_maxTimeResidual(15),
  m_maxTrackChisquarePerDof(10),
  m_storeAllWires(true),
  m_calibrateRt(true),
  m_calibrateT0(true),
  m_calibrateResolution(false),
  m_calibrateTimeResolution(true),
  m_minEntriesPerStraw(25),
  m_t0Granularity(T0Module),
  m_combineChannelsInOTIS(true),
  m_rtPolyOrder(2),
  m_numRtBins(25),
  m_createStrawTuple(false),
  m_file(0),
  m_numProcessedTracks(0),
  m_numProcessedHits(0),
  m_numAcceptedHits(0),
  m_numAcceptedOutliers(0),
  m_numVeloHitsH1(0),
  m_numITHitsH1(0),
  m_numOTHitsH1(0),
  m_chisqprobH1(0)
{
  declareProperty( "TrackContainer",m_trackContainer = LHCb::TrackLocation::Default  );
  declareProperty( "HistoFileName", m_histofilename = "otcalib.root" ) ;
  declareProperty("numDistanceBins",m_numDistanceBins) ;
  declareProperty("maxDistance",m_maxDistance) ;
  declareProperty("maxDistanceError",m_maxDistanceError) ;
  declareProperty("minTime",m_minTime) ;
  declareProperty("maxResidual",m_maxResidual) ;
  declareProperty("maxTimeResidual",m_maxTimeResidual) ;
  declareProperty("maxTrackChisquarePerDof",m_maxTrackChisquarePerDof) ;
  declareProperty("storeAllWires",m_storeAllWires) ;
  declareProperty("calibrateRt",m_calibrateRt) ;
  declareProperty("calibrateT0",m_calibrateT0) ;
  declareProperty("calibrateResolution",m_calibrateResolution) ;
  declareProperty("calibrateTimeResolution",m_calibrateTimeResolution) ;
  declareProperty("minEntriesPerStraw",m_minEntriesPerStraw) ;
  declareProperty("rtPolyOrder",m_rtPolyOrder) ;
  declareProperty("createStrawTuple",m_createStrawTuple) ;
  declareProperty("combineChannelsInOTIS",m_combineChannelsInOTIS) ;
  declareProperty("inputCalibTextFile",m_inputcalibfile) ;
  declareProperty("Projector",m_projector) ;
  declareProperty("AcceptOutliers",m_acceptOutliers=true) ;
}


//=============================================================================
// Destructor
//=============================================================================
OTCalibrationAlg::~OTCalibrationAlg() 
{
  if(m_file) delete m_file ;
}

//=============================================================================
// ::initialize()
//=============================================================================
StatusCode OTCalibrationAlg::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_otdet = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  m_file  = new TFile(m_histofilename.c_str(),"RECREATE") ;
  m_iotool = tool<IOTCalibrationIOTool>("OTCalibrationIOTool",this) ;
  m_projector.retrieve() ;
  if(!m_inputcalibfile.empty()) m_iotool->readFromFile( m_inputcalibfile ) ;

  registerhist("Trk",m_numVeloHitsH1 = new TH1F("numVeloHitsH1","",11,-0.5,10.5)) ;
  registerhist("Trk",m_numITHitsH1 = new TH1F("numITHitsH1","",21,-0.5,20.5)) ;
  registerhist("Trk",m_numOTHitsH1 = new TH1F("numOTHitsH1","",101,-0.5,100.5)) ;
  registerhist("Trk",m_chisqprobH1 = new TH1F("chisqprobH1","",100,0,1)) ;
  registerhist("Trk",m_chisqperdofH1 = new TH1F("chisqperdofH1","",100,0,100)) ;
  registerhist("Trk",m_timeresidH1 = new TH1F("timeresH1","",100,-25,25) ) ;
  registerhist("Trk",m_trkt0H1 = new TH1F("trkt0H1","",100,-25,25) ) ;

  registerhist("",m_t0QuarterPr = new TProfile("t0QuarterPr","t0 estimate per quarter",48,-0.5,47.5)) ;
  registerhist("",m_t0ModulePr = new TProfile("t0ModulePr","t0 estimate per module",432,-0.5,431.5 )) ;
  registerhist("",m_t0OtisPr = new TProfile("t0OtisPr","t0 estimate per otis",1728,-0.5,1727.5 )) ;
  registerhist("",m_distErrPr = new TProfile("distErrPr","distErrPr estimate per module",432,-0.5,431.5 )) ;
  registerhist("",m_resScaleFacH1 = new TH1F("resScaleFacH1","",100,0,2) ) ;
  registerhist("",m_ttodcalibh2  = new TH2F("ttodcalibh2","",numDistanceBins(),
					    0,maxDistance(),48,minTime(),maxTime())) ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// ::finalize()
//=============================================================================
StatusCode OTCalibrationAlg::finalize()
{
  info() << "finalize()" << endreq;
  
  if(m_file) {
    updateCalib() ;
    m_file->Write() ;
    m_file->Close() ;
    delete m_file ;
    m_file=0 ;
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// ::execute()
//=============================================================================
StatusCode OTCalibrationAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS ;
  // Get the Tracks
  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackContainer );
  
  
  if(tracks) {
    for(LHCb::Tracks::const_iterator iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ) {
      LHCb::Track* track = *iTrack;
      if( track->checkFitStatus(LHCb::Track::Fitted) &&
	  track->nDoF()>0 &&
	  //track->measurements().size()>10 &&
	  //track->hasVelo() &&
	  track->hasT() 
	  //&&
	  //std::abs(track->firstState().p()) > 5*Gaudi::Units::GeV &&
	  //std::abs(track->firstState().stateVector()(0)) < 10*Gaudi::Units::m 
	  ) {
	accumulate( *track) ;
      }
    }
  }

  return sc ;
}

void OTCalibrationAlg::accumulate(const LHCb::Track& track)
{
  ++m_numProcessedTracks ;
  // select all the OT hits and convert them into local hit format
  std::vector<OTCal::StrawHit> hits ;
  size_t numvelohits(0), numithits(0), numothits(0) ;
  BOOST_FOREACH( const LHCb::Node* node, track.nodes() ) {
    if(node->hasMeasurement() && 
       node->errMeasure2()>0 ) {
      switch( node->measurement().type() ) {
      case LHCb::Measurement::OT:
	++numothits ;
	hits.push_back( OTCal::StrawHit(*m_projector, *node) ) ;
	break;
      case LHCb::Measurement::IT: 
	++numithits ;
	break ;
      case LHCb::Measurement::VeloR:
      case LHCb::Measurement::VeloPhi:
	++numvelohits ;
	break ;
      default: {}
      }
    }
  }

  // now process the hits
  double sumtimeresidual(0) ;
  size_t numacceptedhits(0), numoutliers(0) ;
  for( std::vector<OTCal::StrawHit>::const_iterator ihit = hits.begin() ;
       ihit != hits.end() ; ++ihit ) {
    const OTCal::StrawHit& hit = *ihit ;
    // Some arbitrary hit criteria
    double normchisq = 
      hit.isOutlier() ? track.chi2()/track.nDoF() : ( track.chi2() - hit.chisq()) / (track.nDoF()-1)  ;
    
    if( std::abs( hit.trkDistance() )  < m_maxDistance&&
	//std::abs( hit.residual() )     < m_maxResidual && // don't use residual cut. use timresidual instead.
	//std::abs( hit.timeResidual() ) < m_maxTimeResidual && 
	//hit.trkVariance()>0 &&
	//std::sqrt( hit.trkVariance() ) < m_maxDistanceError &&
	hit.residualScaleFactor() > 0.5 &&
	normchisq < m_maxTrackChisquarePerDof &&
	(m_acceptOutliers || !hit.isOutlier()) ) {
      
      ++numacceptedhits ;
      if( hit.isOutlier() ) ++numoutliers ;
      sumtimeresidual += hit.timeResidual() ;
      ModuleContainer::iterator imod = m_modules.find(hit.module()) ;

      // create a new module accumulator if necessary
      if( imod == m_modules.end() ) {
	std::pair<ModuleContainer::iterator, bool> rc = 
	  m_modules.insert( ModuleContainer::value_type(hit.module(),OTCal::ModuleAccumulator(*hit.module(),*this)) ) ;
	imod = rc.first ;
      }
      imod->second.add( hit ) ;
      m_averagestraw.add( hit ) ;
      m_t0ModulePr->Fill( hit.uniqueModule(), hit.rawTimeResidual() ) ;
      m_t0QuarterPr->Fill( hit.uniqueQuarter(), hit.rawTimeResidual() ) ;
      m_t0OtisPr->Fill( hit.uniqueOtis(), hit.rawTimeResidual() ) ;
      m_distErrPr->Fill( hit.uniqueModule(), std::sqrt( hit.trkVariance() ) )  ;
      if( hit.residualScaleFactor() > 0.8 && normchisq < 4 &&
	  !hit.isOutlier() ) 
	m_ttodcalibh2->Fill(std::abs(hit.trkDistance()),hit.driftTime()) ;
    }
    
    static TH2* tmph2(0) ;
    if(tmph2==0) registerhist("",tmph2= new TH2F("rth2","",120,-40,80,100,-3.0,3.0) ) ;
    tmph2->Fill( hit.driftTime(),hit.driftRadius() ) ;
    m_resScaleFacH1->Fill( hit.residualScaleFactor() ) ;
  }
  
  // fill some histograms with track info
  m_numProcessedHits += hits.size() ;  
  m_numAcceptedHits  += numacceptedhits ;
  m_numAcceptedOutliers += numoutliers ;
  m_numVeloHitsH1->Fill( numvelohits ) ;
  m_numOTHitsH1->Fill( numothits ) ;
  m_numITHitsH1->Fill( numithits ) ;
  m_chisqprobH1->Fill( TMath::Prob(track.chi2(),track.nDoF()) ) ;
  m_chisqperdofH1->Fill(track.chi2()/track.nDoF()) ;
  if(numacceptedhits>0) m_trkt0H1->Fill(sumtimeresidual/numacceptedhits) ;
}


void OTCalibrationAlg::registerhist(const std::string& path, TNamed* h1)
{
  TDirectory* tmpdir = gDirectory ;
  if(m_file==0) {
    m_file = new TFile(m_histofilename.c_str(),"RECREATE") ;
  }
  m_file->cd() ;
  if(path.size()>0) {
    std::string::size_type pos = 0 ;
    do {
      std::string::size_type slashpos = path.find_first_of('/',pos) ;
      std::string thisdir = path.substr(pos,slashpos-pos) ;
      if(gDirectory->GetKey(thisdir.c_str())) gDirectory->cd(thisdir.c_str()) ;
      else                                    gDirectory->mkdir(thisdir.c_str())->cd() ;
      pos = slashpos ;
      if(slashpos!=std::string::npos) ++pos ;
    } while(pos != std::string::npos && pos<path.size()) ;
  }
  // a bit ugly, but it works
  if(      dynamic_cast<TH1*>(h1) ) (dynamic_cast<TH1*>(h1))->SetDirectory(gDirectory) ;
  else if( dynamic_cast<TTree*>(h1) ) (dynamic_cast<TTree*>(h1))->SetDirectory(gDirectory) ;
  if(tmpdir) tmpdir->cd() ;
}

// static double bifurgaus(double* x, double* par)
// {
//   double xi = x[0] - par[1] ;
//   double sigma = xi>0 ? par[2] : par[3] ;
//   xi/=sigma ;
//   return par[0] * exp(-0.5*xi*xi) ;
// }

namespace {

  inline double sqr(double x) { return x*x ; }
  
  TGraph* createRtGraph(const OTDet::RtRelation& rtr)
  {
    int npoints = 50 ;
    float tmin = rtr.drifttime(0) ;
    float tmax = rtr.drifttime(2.5) ;
    float dt = (tmax-tmin)/npoints;
    TGraph* gr = new TGraph(npoints) ;
    gr->SetLineWidth(2) ;
    for(int i=0; i<npoints; ++i) {
      float t = tmin + i*dt ;
      gr->SetPoint(i,rtr.radius(t),t) ;
    }
    return gr ;
  }
  
  double calc3sigmarms(const TH1& h1pos)
  {
    // we assume the histogram comes in as a 'positive only'
    // first calcualte the 'raw rms'
    double sumw(0),sumx2w(0) ; 
    for(int ibin=1; ibin<=h1pos.GetNbinsX(); ++ibin) {
      double x = h1pos.GetBinCenter(ibin) ;
      double c = h1pos.GetBinContent(ibin) ;
      sumw   += c ;
      sumx2w += c*x*x ;
    }
    double rawrms = std::sqrt( sumx2w /  sumw ) ;
    
    // start from that rms
    sumw = sumx2w = 0 ;
    int minbin = h1pos.GetXaxis()->FindBin(rawrms) ;
    double xtrunc = h1pos.GetXaxis()->GetXmax() ;
    for(int ibin=1; ibin<=h1pos.GetNbinsX(); ++ibin) {
      double x = h1pos.GetBinCenter(ibin) ;
      double c = h1pos.GetBinContent(ibin) ;
      double up = h1pos.GetXaxis()->GetBinUpEdge(ibin) ;
      
      double newsumw   = sumw + c ;
      double newsumx2w = sumx2w + c*x*x ;
      if(sumw>0 && ibin>minbin) {
	double newrms    = std::sqrt(  newsumx2w / newsumw ) ;
	if( 3 * newrms < up ) {
	  double drms = newrms - std::sqrt(  sumx2w / sumw ) ;
	  double frac = (3*drms)/h1pos.GetXaxis()->GetBinWidth(ibin) ;
	  //std::cout << frac << " " << drms << std::endl ;
	  xtrunc = (up - (1-frac)*h1pos.GetXaxis()->GetBinWidth(ibin))/3  ;
	  break ;
	}
      }
      sumw = newsumw ;
      sumx2w = newsumx2w ;
    }
    return xtrunc ;
  }
}

void OTCalibrationAlg::updateRtCalib(double& globalDeltaT0, 
				     std::ostringstream& logmessage)
{
  if( m_modules.empty() ) return ;

  // start by fitting the rt-relation. for now, we'll fit a single
  // relation. in the end we want to fit at most one per layer/wheel.
  ModuleContainer::const_iterator it = m_modules.begin() ;
  TH2F *timevsdistanceh2, *ttodcalibh2, *timeresvsdistanceh2, *resvsdistanceh2, *resvstimeh2 ;
  registerhist("",timevsdistanceh2 = new TH2F(*(it->second.timevsdistanceh2())) ) ;
  //registerhist("",ttodcalibh2 = new TH2F(*(it->second.ttodcalibh2())) ) ;
  registerhist("",timeresvsdistanceh2 = new TH2F(*(it->second.timeresvsdistanceh2())) ) ;
  registerhist("",resvsdistanceh2 = new TH2F(*(it->second.resvsdistanceh2())) ) ;
  registerhist("",resvstimeh2 = new TH2F(*(it->second.resvstimeh2())) ) ;
  for(++it ; it != m_modules.end(); ++it) {
    timevsdistanceh2->Add( it->second.timevsdistanceh2() ) ;
    //ttodcalibh2->Add( it->second.ttodcalibh2() ) ;
    timeresvsdistanceh2->Add( it->second.timeresvsdistanceh2() ) ;
    resvsdistanceh2->Add( it->second.resvsdistanceh2() ) ;
    resvstimeh2->Add( it->second.resvstimeh2() ) ;
  }
  ttodcalibh2 = m_ttodcalibh2 ;

  // now fit in slices of the distance. make a graph and assign as
  // primitive to the histo, so that we can look at it later. a fit is
  // actually a bit exagerated here... we could just make it the mean
  // of the highest bin and its two neighbours.
  const double mindistance = 0.1 ;
  double mintime = 999 ;
  TF1* dtfitfunc = new TF1("dtfitfunc","gaus(0)") ;
  dtfitfunc->SetLineWidth(1) ;
  dtfitfunc->SetLineColor(kBlue) ;
  int npoints = ttodcalibh2->GetNbinsX() ;
  TGraphErrors* dtgr = new TGraphErrors(npoints) ;
  for(int i=0; i<npoints; ++i) {
    char hisname[256] ;
    sprintf(hisname,"timeh1_%d",i) ;
    int firstbin(i+1),lastbin(i+1) ;
    TH1* htmp =ttodcalibh2->ProjectionY(hisname,firstbin,lastbin) ;

    // find the maximum 
    int maxbin=1 ;
    double maxval(0),val ;
    for(int ibin=1; ibin<=htmp->GetNbinsX(); ++ibin)
      if( (val = htmp->GetBinContent(ibin))>maxval) {
	maxval = val ;
	maxbin = ibin ;
      }
    
    // now simply fit in fixed range around this maximum
    double width = 10 ;//use fixed range. should make this a parameter. htmp->GetRMS() ;
    double mean  = htmp->GetBinCenter(maxbin) ;
    dtfitfunc->SetRange(mean-width,mean+width) ;
    dtfitfunc->SetParameter(0,htmp->GetMaximum()) ;
    dtfitfunc->SetParameter(1,htmp->GetMean()) ;
    dtfitfunc->SetParameter(2,5) ;
    //dtfitfunc->SetParameter(3,0.8*htmp->GetRMS()) ;
    htmp->Fit(dtfitfunc,"Q0RI") ;
    htmp->GetListOfFunctions()->Add(dtfitfunc->Clone()) ;
    sprintf(hisname,"%4.2f < d < %4.2f  [mm]",
	    ttodcalibh2->GetYaxis()->GetBinLowEdge(firstbin),ttodcalibh2->GetYaxis()->GetBinUpEdge(lastbin)) ;
    htmp->SetTitle(hisname) ;
    registerhist("RTFits",htmp) ;
    //delete htmp ;
    double d = 0.5*(ttodcalibh2->GetXaxis()->GetBinCenter(firstbin) + ttodcalibh2->GetXaxis()->GetBinCenter(lastbin)) ;
    double t = dtfitfunc->GetParameter(1) ;
    dtgr->SetPoint(i,d,t) ;
    dtgr->SetPointError(i,0,dtfitfunc->GetParError(1)) ;
    if( d>=mindistance  && t<mintime ) mintime = t ;
  }

  dtgr->SetMarkerStyle(20) ;
  dtgr->SetMarkerColor(kRed) ;
  dtgr->SetLineColor(kRed) ;
  dtgr->SetLineWidth(2) ;
  dtgr->SetName("dtgr") ;
  ttodcalibh2->GetListOfFunctions()->Add( dtgr ) ;
  //timevsdistanceh2->GetListOfFunctions()->Add( dtgr ) ;

  // fit the average driftvelocity (just for monitoring purposes)
  {
    TF1 pol1("f1","pol1",0.2,2.3) ;
    dtgr->Fit(&pol1,"QR") ;
    double v    = 1/pol1.GetParameter(1) ;
    double verr = pol1.GetParError(1)*v*v ;
    logmessage << "* average driftvelocity [0.2,2.3]: " << v << " +/- " << verr << std::endl ;
  }

  // fit the t(r) function
  char polname[256] ;
  sprintf(polname,"pol%d",m_rtPolyOrder) ;
  TF1* rtfunc = new TF1("rtfunc",polname,0,m_maxDistance) ;
  dtgr->Fit( rtfunc) ;
  rtfunc->SetLineColor(9) ;
  rtfunc->SetLineWidth(2) ;
  ttodcalibh2->GetListOfFunctions()->Add( rtfunc ) ;
  timevsdistanceh2->GetListOfFunctions()->Add( rtfunc ) ;

  float tmin = rtfunc->Eval(0); 
  //float tmax = rtfunc->Eval(m_maxDistance) ;

  // create a graph for the rt-relation and add it to the histo. note
  // that the rt relation class holds the coefficients of a polynomial
  // in r/rmax, so we need to divide by the appropriate power of
  // m_maxDistance

  const double cellRadius = m_modules.begin()->second.module().cellRadius() ;
  std::vector<double> coefficients(m_rtPolyOrder+1) ;
  for(size_t i=0; i<coefficients.size(); ++i) 
    coefficients[i] = rtfunc->GetParameter(i)*std::pow(cellRadius , int(i) ) ;

  // only now correct the offset (which we want to be fixed to zero)
  coefficients[0] = 0 ;

  // Before we create the new rt-relation, we need to decide what we do with the error.
  OTDet::RtRelation* newrtrelation ;
  if( !m_calibrateResolution) {
    // easy: this was for first MC tests. Just chosoe a fixed error in the distance
    //const double sigma = 0.2 ; // * Gaudi::Unit::mm ;
    //newrtrelation = new OTDet::RtRelation(cellRadius,coefficients,sigma) ;
    const double sigmaT = 3.0 ; // [ns]
    newrtrelation = new OTDet::RtRelation(cellRadius,coefficients,
					  std::vector<double>(1,sigmaT)) ;
  } else {
    if( m_calibrateTimeResolution ) {
      // calibrate a (fixed) resolution in the drift time
      TH1* resh1 = timeresvsdistanceh2->ProjectionY("timeresh1") ;
      registerhist("",resh1) ;
      dtfitfunc->SetParameter(0,resh1->GetMaximum()) ;
      dtfitfunc->SetParameter(1,0) ;
      dtfitfunc->SetParameter(2,resh1->GetRMS()) ;
      resh1->Fit(dtfitfunc) ;
      std::vector<double> sigmacoefficients ; 
      sigmacoefficients.push_back( dtfitfunc->GetParameter(2) ) ;
      newrtrelation = new OTDet::RtRelation(cellRadius,coefficients,sigmacoefficients) ;
      logmessage << "* calibrated resolution [ns]: " << dtfitfunc->GetParameter(2) << std::endl ;
    } else {
      // calibrate a fixed resolution in the distance
      TH1* resh1 = resvsdistanceh2->ProjectionY("resh1") ;
      registerhist("",resh1) ;
      dtfitfunc->SetParameter(0,resh1->GetMaximum()) ;
      dtfitfunc->SetParameter(1,0) ;
      dtfitfunc->SetParameter(2,resh1->GetRMS()) ;
      resh1->Fit(dtfitfunc) ;
      newrtrelation = new OTDet::RtRelation(cellRadius,coefficients,dtfitfunc->GetParameter(2)) ;
      logmessage << "* calibrated resolution [mm]: " << dtfitfunc->GetParameter(2) << std::endl ;
    }
  }

  // we need to correct the t0s for an eventual shift in the
  // rt-relation. we define this shift by comparing the new and the
  // old rt-relation at a number of fixed points and then take the
  // average. we take this (weighted) points from the observed
  // distance distribution. but first, we need to define what the old
  // rt-relation was. for now we assume that they are all equal and
  // take the first one
  const OTDet::RtRelation& oldrtrelation = m_modules.begin()->first->rtRelation() ;

  // now calculate the delta-t0
  double sumdt(0),sumw(0) ;
  TH1* rh1 = ttodcalibh2->ProjectionX("rh1") ;
  for(int ibin=1; ibin<=rh1->GetNbinsX(); ++ibin) {
    double r = rh1->GetXaxis()->GetBinCenter(ibin) ;
    double w = rh1->GetBinContent(ibin) ;
    double tnew = newrtrelation->drifttime(r) ;
    double told = oldrtrelation.drifttime(r) ;
    sumdt += w * (told-tnew) ;
    sumw  += w ;
  }

  delete rh1 ;
  globalDeltaT0 = sumdt / sumw ;
  logmessage //<< "* new rtrelation:                " << newrtrelation << std::endl 
	     << "* t(d=0) =                       " << tmin << std::endl
	     << "* global t0 shift:               " << globalDeltaT0 << std::endl ;
  
  // draw the old rt-relation
  TGraph* oldrtgraph = createRtGraph( oldrtrelation ) ;
  oldrtgraph->SetLineColor(8) ;
  oldrtgraph->SetName("oldrtgraph") ;
  ttodcalibh2->GetListOfFunctions()->Add(oldrtgraph) ;
  timevsdistanceh2->GetListOfFunctions()->Add(oldrtgraph) ;

  // now set the rt-relation of all modules
  for(DeOTDetector::Modules::const_iterator it = m_otdet->modules().begin() ;
      it != m_otdet->modules().end(); ++it) 
    (const_cast<DeOTModule*>(*it))->setRtRelation( *newrtrelation ) ;
  delete newrtrelation ;
}

void OTCalibrationAlg::updateT0Calib(const double& globalDeltaT0, 
				     std::ostringstream& logmessage)
{
  TH1F* numentriesh1  = new TH1F("numentriesh1",
				 "number of entries per channel",1001,-0.5,1000.5) ;
  registerhist("",numentriesh1) ;
  //   TH1F* strawt0h1  = new TH1F("strawt0h1","",100,0,50) ;
  //   registerhist("",strawt0h1) ;
  TH1F* t0h1  = new TH1F("t0h1","",100,-50,50) ;
  registerhist("",t0h1) ;
  TH1F* deltat0h1 = new TH1F("deltat0h1","",100,-10,10) ;
  registerhist("",deltat0h1) ;
  TH1F* errort0h1 = new TH1F("errort0h1","",100,0,5) ;
  registerhist("",errort0h1) ;
  
//   TNtuple* strawtuple(0) ;
// //   if(m_createStrawTuple) 
// //     registerhist("",strawtuple = new TNtuple("strawtuple","straw calibration tuple",
// // 					     "bec:mod:lay:strawlay:straw:num:t0:t0err:chisq:resrms")) ;
  
//   //const unsigned int minentries = m_minEntriesPerStraw ;
  unsigned int numStrawsSeen(0) ;
  unsigned int numStrawsCalibrated(0) ;
  size_t numModulesSeen(0) ;
  size_t numModulesCalibrated(0) ;
  
  // for now, we only implement the calibration os a single number per
  // otis. we'll do the rest later.

  for(ModuleContainer::const_iterator imod = m_modules.begin() ; imod != m_modules.end(); ++imod) {
    const OTCal::ModuleAccumulator& mod = imod->second ;
    std::vector<double> newt0 ;
    ++numModulesSeen ;
    if( m_t0Granularity == T0Module ) {
      numentriesh1->Fill( mod.nentries() ) ;
      
      double modulet0 = mod.avrawtimeresidual() + globalDeltaT0 ;
      double oldmodulet0 = mod.module().strawT0(1) ;
      if( mod.nentries()<m_minEntriesPerStraw) 
	modulet0 = oldmodulet0 ;
      deltat0h1->Fill( modulet0 - oldmodulet0 ) ;
      errort0h1->Fill( mod.avrawtimeresidualerror() ) ;
      t0h1->Fill( modulet0 ) ;
      newt0.push_back( modulet0 ) ;
      ++numModulesCalibrated ;
    } else {

      size_t numotis = mod.module().nChannels()/32 ;
      assert( mod.module().nChannels()%32 == 0 ) ;
      newt0.resize( numotis, 0 ) ;
      for(size_t iotis = 0; iotis<numotis; ++iotis) {
	// create the otis average
	OTCal::StrawAccumulator otisaccumulator ;
	for(size_t ichan=0; ichan<32; ++ichan) 
	  otisaccumulator.add( mod.straws()[ iotis * 32 + ichan ] ) ;
	numentriesh1->Fill( otisaccumulator.nentries() ) ;
	++numStrawsSeen ;
	
	if( otisaccumulator.nentries()>m_minEntriesPerStraw) {
	  ++numStrawsCalibrated ;
	  
	  double otist0    = otisaccumulator.avtimeresidual() + globalDeltaT0 ;
	  double otist0err = otisaccumulator.avrawtimeresidualerror() ;
	  
	  // calculate the 'old' otis t0
	  double oldotist0(0) ;
	  for(size_t ichan=0; ichan<32; ++ichan) 
	    oldotist0 += mod.module().strawT0( iotis * 32 + ichan + 1 ) ;
	  oldotist0 /= 32 ;
	  
	  // fill some monitoring histograms
	  t0h1->Fill( otist0 ) ;
	  deltat0h1->Fill( otist0 - oldotist0 ) ;
	  errort0h1->Fill( otist0err ) ;
	  newt0[iotis] = otist0 ;
	}
      }
    }
    (const_cast<DeOTModule&>(mod.module())).setStrawT0s( newt0 ) ;
  }
  
  logmessage 
    << "* average time residual:       " << m_averagestraw.avtimeresidual() << std::endl ;
  if( numStrawsSeen > 0 ) {
    logmessage << "* number of otis seen:       " << numStrawsSeen << std::endl
	       << "* number of otis calibrated: " << numStrawsCalibrated << std::endl ;
  } else {
    logmessage << "* number of modules seen:       " << numModulesSeen << std::endl
	       << "* number of modules calibrated: " << numModulesCalibrated << std::endl ;
  }
}

void OTCalibrationAlg::updateCalib()
{  
  std::ostringstream logmessage ;
  logmessage << "*******************************************************" << std::endl
	     << "******** In OTCalibrationAlg::updateCalib() **********" << std::endl 
	     << "* number of processed tracks: " << m_numProcessedTracks  << std::endl 
	     << "* number of processed hits:   " << m_numProcessedHits    << std::endl
	     << "* number of accepted hits:    " << m_numAcceptedHits     << std::endl
	     << "* number of accepted outliers:" << m_numAcceptedOutliers << std::endl ;
  
  // write the existing calibration in all formats you could ever want to
  m_iotool->writeToFile( "oldcalib.txt", m_combineChannelsInOTIS) ;

  // for now, copy the original constants and put the new ones 'on
  // top. the alternative is to start from scratch. for future, need
  // user parameter for this.
  double globalDeltaT0(0) ;
  if( m_calibrateRt ) updateRtCalib(globalDeltaT0,logmessage) ;
  if( m_calibrateT0 ) updateT0Calib(globalDeltaT0,logmessage) ;
  m_iotool->writeToFile( "newcalib.txt", m_combineChannelsInOTIS) ;

  logmessage << "*******************************************************" << std::endl ;
  info() << logmessage.str() << endreq ;

  std::string m_logFilename("otcaliblog.txt") ;
  if(!m_logFilename.empty()) {
    std::ofstream logfile(m_logFilename.c_str()) ;
    logfile << logmessage.str() << std::flush ;
    logfile.close() ;
  }
}

