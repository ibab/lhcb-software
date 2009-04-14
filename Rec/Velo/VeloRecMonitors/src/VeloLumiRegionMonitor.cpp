// $Id: VeloLumiRegionMonitor.cpp,v 1.3 2009-04-14 11:50:04 mjohn Exp $
// Include files
// -------------
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// from Event
#include "Event/HltVertexReports.h"
//from ROOT
#include "TF1.h"
// local
#include "VeloLumiRegionMonitor.h"
// from Boost
#include <boost/lexical_cast.hpp>
//-----------------------------------------------------------------------------
// Implementation file for class : VeloLumiRegionMonitor
//
// VELO LumiRegion monitoring algorithm.
// Produces a set of histograms from the clusters bank in the TES.
//
// 2008-08-26 : Malcolm John
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
DECLARE_ALGORITHM_FACTORY( VeloLumiRegionMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloLumiRegionMonitor::VeloLumiRegionMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , d_vertexDistributionMeanX(0)
  , d_vertexDistributionMeanY(0)
  , d_vertexDistributionMeanZ(0)
  , d_vertexDistributionRMSX(0)
  , d_vertexDistributionRMSY(0)
  , d_vertexDistributionRMSZ(0)
  , d_vertexDistributionGMeanX(0)
  , d_vertexDistributionGMeanY(0)
  , d_vertexDistributionGMeanZ(0)
  , d_vertexDistributionSigmaX(0)
  , d_vertexDistributionSigmaY(0)
  , d_vertexDistributionSigmaZ(0)
  , d_vertexDistributionSlopeXZ(0)
  , d_vertexDistributionSlopeYZ(0)
  , n_acceptableVertices(0)
  , b_genData(0)
  , i_counterEx(0)
  , i_counterAl(0)
  ,vX_hist()
  ,vY_hist()
  ,vZ_hist()
{
  declareProperty("VertexReportLocation",s_vertexReportLocation=LHCb::HltVertexReportsLocation::Default);
  declareProperty("HltLineName",s_correspondingHltLineName);
  declareProperty("MinimumNumberofTracks",i_minTracks=4);
  declareProperty("RefreshRate",d_refreshRate=1000);
  declareProperty("GenerateLumiRegion",b_genData=false);
  declareProperty("GeneratedXPosition",d_generatedX=0.0);
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloLumiRegionMonitor::~VeloLumiRegionMonitor(){}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::initialize() {
  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  setHistoTopDir( "" );

  IRndmGenSvc* rndm = svc<IRndmGenSvc>("RndmGenSvc",true);
  if (0==rndm) {
    return Error ("Unable to initialize IRndmGenSvc", StatusCode::FAILURE);
  }
  m_rndmGaus.initialize  ( rndm , Rndm::Gauss ( 0 , 1 ) ) ;
  m_rndmExpo.initialize  ( rndm , Rndm::Exponential(6)  ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to initialize Gaussian distribution" , sc ) ; }

  std::string side="ASide ";
  if(s_correspondingHltLineName.find("CSide")!=std::string::npos) side="CSide ";

  hX = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionX", side+"Vertices (X) [mm] (last "+boost::lexical_cast<std::string>(d_refreshRate)+")", -35, 35, 3500 ) );
  hY = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionY", side+"Vertices (Y) [mm] (last "+boost::lexical_cast<std::string>(d_refreshRate)+")", - 5,  5,  500 ) );
  hZ = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionZ", side+"Vertices (Z) [mm] (last "+boost::lexical_cast<std::string>(d_refreshRate)+")",-100,100,  200 ) );
  hNtrks  = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "Ntrks", side+"n(Trks) = (n(DoF)+3)/2.", -0.5, 49.5, 50 ) );
  hNtrksU = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "Ntrks_used", side+"n(Trks) = (n(DoF)+3)/2. (used)", -0.5, 49.5, 50 ) );
  hX_cumu = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionX_cumu", side+"Vertices (X) [mm] (accumulated)", -35, 35, 3500 ) );
  hY_cumu = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionY_cumu", side+"Vertices (Y) [mm] (accumulated)", - 5,  5,  500 ) );
  hZ_cumu = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionZ_cumu", side+"Vertices (Z) [mm] (accumulated)",-100,100,  200 ) );
  hX_prof = Gaudi::Utils::Aida2ROOT::aida2root( bookProfile1D( "LumiRegionX_vs_BunchId", side+"Vertices (X) [mm] vs LHC bunchID",-0.5,3563.5,3564 ) );
  hY_prof = Gaudi::Utils::Aida2ROOT::aida2root( bookProfile1D( "LumiRegionY_vs_BunchId", side+"Vertices (Y) [mm] vs LHC bunchID",-0.5,3563.5,3564 ) );
  hZ_prof = Gaudi::Utils::Aida2ROOT::aida2root( bookProfile1D( "LumiRegionZ_vs_BunchId", side+"Vertices (Z) [mm] vs LHC bunchID",-0.5,3563.5,3564 ) );
  hX_hist = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionX_trend", side+"Measure of the lumi.region (X) [mm]",-1000.5, 99.5, 1100 ) );
  hY_hist = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionY_trend", side+"Measure of the lumi.region (Y) [mm]",-1000.5, 99.5, 1100 ) );
  hZ_hist = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionZ_trend", side+"Measure of the lumi.region (Z) [mm]",-1000.5, 99.5, 1100 ) );
  hXZ_hist = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionXZ_trend", side+"Measure of the lumi.region (slope XZ) [mm]",-1000.5, 99.5, 1100 ) );
  hYZ_hist = Gaudi::Utils::Aida2ROOT::aida2root( book1D( "LumiRegionYZ_trend", side+"Measure of the lumi.region (slope YZ) [mm]",-1000.5, 99.5, 1100 ) );

/*
  // variable binning doesn't work because this information isn't transfered across DIM
  unsigned int ibins=0;
  double* xbins = new double[hX->GetXaxis()->GetNbins()];
  for(double d=hX->GetXaxis()->GetXmin();d<hX->GetXaxis()->GetXmax();){
    xbins[ibins]=d;
    ibins++;
    d=d+0.020+0.080*fabs(d)/hX->GetXaxis()->GetXmax();
  }
  hX->SetBins(ibins-1,xbins);
  hX_cumu->SetBins(ibins-1,xbins);
*/

  declareInfo("nEvtsReceived", i_counterEx,"Counter in execute()");
  declareInfo("nEvtswithVertex", i_counterAl,"Counter in monitorLumiRegion()");
  declareInfo("vtxMeanX", d_vertexDistributionMeanX, "Arithmetic mean (X) of verticies");
  declareInfo("vtxMeanY", d_vertexDistributionMeanY, "Arithmetic mean (Y) of verticies");
  declareInfo("vtxMeanZ", d_vertexDistributionMeanZ, "Arithmetic mean (Z) of verticies");
  declareInfo("vtxRMSX",  d_vertexDistributionRMSX,              "RMS (X) of verticies");
  declareInfo("vtxRMSY",  d_vertexDistributionRMSY,              "RMS (Y) of verticies");
  declareInfo("vtxRMSZ",  d_vertexDistributionRMSZ,              "RMS (Z) of verticies");
  declareInfo("vtxGMeanX",d_vertexDistributionGMeanX,    "Fitted mean (X) of verticies");
  declareInfo("vtxGMeanY",d_vertexDistributionGMeanY,    "Fitted mean (Y) of verticies");
  declareInfo("vtxGMeanZ",d_vertexDistributionGMeanZ,    "Fitted mean (Z) of verticies");
  declareInfo("vtxSigmaX",d_vertexDistributionSigmaX,   "Fitted sigma (X) of verticies");
  declareInfo("vtxSigmaY",d_vertexDistributionSigmaY,   "Fitted sigma (Y) of verticies");
  declareInfo("vtxSigmaZ",d_vertexDistributionSigmaZ,   "Fitted sigma (Z) of verticies");
  declareInfo("vtxSlopeXZ",d_vertexDistributionSlopeXZ, "LLSq. slope (XZ) of verticies");
  declareInfo("vtxSlopeYZ",d_vertexDistributionSlopeYZ, "LLSq. slope (YZ) of verticies");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::execute()
{
  counter( "# events" ) += 1;
  if (!exist<LHCb::ODIN> (LHCb::ODINLocation::Default))
    return Error("The ODIN bank is not found",StatusCode::FAILURE,50);
  m_odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  if( b_genData )
    genExampleLumiRegion();
  monitorLumiRegion();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::finalize()
{
  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Monitor the VeloLumiRegions
//=============================================================================
void Velo::VeloLumiRegionMonitor::genExampleLumiRegion()
{
  double v  = m_rndmGaus.shoot();
  int nTrks = 0;
  while(nTrks<3) nTrks=int(m_rndmExpo.shoot());
  // create output container for vertex selections and put it on TES
  LHCb::HltVertexReports* outputSummary = new LHCb::HltVertexReports();
  put( outputSummary, s_vertexReportLocation );  
  // prevent duplicate selections
  if( outputSummary->hasSelectionName( s_correspondingHltLineName ) ) return;
 
  SmartRefVector<LHCb::VertexBase> pVtxs;
  LHCb::VertexBase* pVtx = new LHCb::VertexBase();
  int isCSide = int(s_correspondingHltLineName.find("CSide")!=std::string::npos);
  Gaudi::XYZPoint position( v*(0.070+0.0002*d_generatedX)+d_generatedX+cos(i_counterEx/100000.)*pow(-1.0,isCSide),
			    v *0.070+sin(i_counterEx/100000.),
			    v *7.000);
  pVtx->setPosition( position );
  pVtx->setChi2( 1.6180339887 );
  pVtx->setNDoF( 2*nTrks-3 );
  pVtxs.push_back( SmartRef<LHCb::VertexBase>( pVtx ) );

  // insert selection into the container
  if( outputSummary->insert(s_correspondingHltLineName,pVtxs) == StatusCode::FAILURE ){
    Warning(" Failed to add Hlt vertex selection name "+s_correspondingHltLineName,StatusCode::SUCCESS, 20 );
  }
}

//=============================================================================
// Monitor the VeloLumiRegions
//=============================================================================
void Velo::VeloLumiRegionMonitor::monitorLumiRegion()
{
  i_counterEx++;

  LHCb::HltVertexReports* reports = get<LHCb::HltVertexReports>(s_vertexReportLocation);
  debug() << "Available HLT decisions are:" << reports->selectionNames() << endmsg;
  const LHCb::HltVertexReports::HltVertexReport* report = reports->vertexReport(s_correspondingHltLineName);
  if( !report ){
    debug()<<"No vertex report for \'"<<s_correspondingHltLineName<<"\' at \'"<<s_vertexReportLocation<<"\'"<<endmsg;
    return;
  }else{
    i_counterAl++;
    for( LHCb::HltVertexReports::HltVertexReport::const_iterator iv=report->begin();iv!=report->end();iv++ ){
      const LHCb::VertexBase& v=**iv;
      int nTrks=int((v.nDoF()+3)/2.);   // #dof=#constraints-#fitted parameters, and each track give 2 [slope] constraints 
      Gaudi::XYZPoint p = v.position();

      //The following three lines should be removed if PatVelo provides PatPV3D with tracks in the halfbox frame.
      bool isCSide = s_correspondingHltLineName.find("CSide")!=std::string::npos;
      const DeVeloSensor *anArbitarySensor = m_veloDet->sensor(int(isCSide));
      p=anArbitarySensor->globalToVeloHalfBox(p);

      vertexList.push_back(std::pair<Gaudi::XYZPoint,int>(p,nTrks));
      if( nTrks >= i_minTracks ) n_acceptableVertices++;

      hX_cumu->Fill( p.x() );
      hY_cumu->Fill( p.y() );
      hZ_cumu->Fill( p.z() );
      hX_prof->Fill( m_odin->bunchId(), p.x() );
      hY_prof->Fill( m_odin->bunchId(), p.y() );
      hZ_prof->Fill( m_odin->bunchId(), p.z() );
    }
  }

  if(n_acceptableVertices>0){
    if( n_acceptableVertices/d_refreshRate-int(n_acceptableVertices/d_refreshRate)==0 ){
      updateDataPoints();
    }
  }
}

void Velo::VeloLumiRegionMonitor::updateDataPoints(){
   hX->Reset();
   hY->Reset();
   hZ->Reset();
   hNtrks->Reset();
   hNtrksU->Reset();
   std::vector< std::pair<Gaudi::XYZPoint,int> >::iterator iv=vertexList.begin();
   for(;iv<vertexList.end();iv++ ){
     Gaudi::XYZPoint vtx = (*iv).first;
     int nTrks = (*iv).second;
     hNtrks->Fill(nTrks);
     if( nTrks >= i_minTracks ){
       hNtrksU->Fill(nTrks);
       hX->Fill(vtx.X());
       hY->Fill(vtx.Y());
       hZ->Fill(vtx.Z());
     }
   }

   TF1 *gaussian = new TF1("Gaussian","gaus",-35,35);

   d_vertexDistributionMeanX = hX->GetMean();
   d_vertexDistributionRMSX= hX->GetRMS();
   gaussian->SetParameters(hX->GetMaximum(),hX->GetMean(),hX->GetRMS());
   hX->Fit("Gaussian","Q0");
   d_vertexDistributionGMeanX = gaussian->GetParameter(1);
   d_vertexDistributionSigmaX= gaussian->GetParameter(2);

   d_vertexDistributionMeanY = hY->GetMean();
   d_vertexDistributionRMSY= hY->GetRMS();
   gaussian->SetRange(-5,5);
   gaussian->SetParameters(hY->GetMaximum(),hY->GetMean(),hY->GetRMS());
   hY->Fit("Gaussian","Q0");
   d_vertexDistributionGMeanY = gaussian->GetParameter(1);
   d_vertexDistributionSigmaY= gaussian->GetParameter(2);

   d_vertexDistributionMeanZ = hZ->GetMean();
   d_vertexDistributionRMSZ= hZ->GetRMS();
   gaussian->SetRange(-100,100);
   gaussian->SetParameters(hZ->GetMaximum(),hZ->GetMean(),hZ->GetRMS());
   hZ->Fit("Gaussian","Q0");
   d_vertexDistributionGMeanZ = gaussian->GetParameter(1);
   d_vertexDistributionSigmaZ= gaussian->GetParameter(2);

   delete gaussian;

   calculateSlopesInVertexList();

   hX_hist->Reset();
   hY_hist->Reset();
   hZ_hist->Reset();
   hXZ_hist->Reset();
   hYZ_hist->Reset();
   vX_hist.push_front(std::pair<double,double>(d_vertexDistributionGMeanX,d_vertexDistributionSigmaX));
   vY_hist.push_front(std::pair<double,double>(d_vertexDistributionGMeanY,d_vertexDistributionSigmaY));
   vZ_hist.push_front(std::pair<double,double>(d_vertexDistributionGMeanZ,d_vertexDistributionSigmaZ));
   vXZ_hist.push_front(std::pair<double,double>(d_vertexDistributionSlopeXZ,d_vertexDistributionSlopeErrorXZ));
   vYZ_hist.push_front(std::pair<double,double>(d_vertexDistributionSlopeYZ,d_vertexDistributionSlopeErrorYZ));
   if(vX_hist.size()>1000){
     vX_hist.pop_back();
     vY_hist.pop_back();
     vZ_hist.pop_back();
     vXZ_hist.pop_back();
     vYZ_hist.pop_back();
   }
   for(unsigned int i=0;i<vX_hist.size();i++){
     int iXbin=hX_hist->Fill(-int(i),vX_hist[i].first);
     int iYbin=hY_hist->Fill(-int(i),vY_hist[i].first);
     int iZbin=hZ_hist->Fill(-int(i),vZ_hist[i].first);
     int iXZbin=hXZ_hist->Fill(-int(i),vXZ_hist[i].first);
     int iYZbin=hYZ_hist->Fill(-int(i),vYZ_hist[i].first);
     hX_hist->SetBinError(iXbin,vX_hist[i].second);
     hY_hist->SetBinError(iYbin,vY_hist[i].second);
     hZ_hist->SetBinError(iZbin,vZ_hist[i].second);
     hXZ_hist->SetBinError(iXZbin,vXZ_hist[i].second);
     hYZ_hist->SetBinError(iYZbin,vYZ_hist[i].second);
   }

   n_acceptableVertices=0;
   vertexList.clear();
}

void Velo::VeloLumiRegionMonitor::calculateSlopesInVertexList()
{
  double N=0;
  double sumX=0;
  double sumY=0;
  double sumZ=0;
  double sumXX=0;
  double sumYY=0;
  double sumZZ=0;
  double sumXZ=0;
  double sumYZ=0;
  std::vector< std::pair<Gaudi::XYZPoint,int> >::iterator iv=vertexList.begin();
  for(;iv<vertexList.end();iv++ ){
    Gaudi::XYZPoint vtx = (*iv).first;
    int nTrks = (*iv).second;
    if( nTrks >= i_minTracks ){
      N=N+1;
      sumX+=vtx.X();
      sumY+=vtx.Y();
      sumZ+=vtx.Z();
      sumXX+=vtx.X()*vtx.X();
      sumYY+=vtx.Y()*vtx.Y();
      sumZZ+=vtx.Z()*vtx.Z();
      sumXZ+=vtx.X()*vtx.Z();
      sumYZ+=vtx.Y()*vtx.Z();
    }
  }
  double meanX=sumX/N;
  double meanY=sumY/N;
  double meanZ=sumZ/N;
  //double varianceX=sumXX/N-meanX*meanX;
  //double varianceY=sumYY/N-meanY*meanY;
  double varianceZ=sumZZ/N-meanZ*meanZ;
  double covarianceXZ=sumXZ/N-meanX*meanZ;
  double covarianceYZ=sumYZ/N-meanY*meanZ;

  d_vertexDistributionSlopeXZ=covarianceXZ/varianceZ;
  d_vertexDistributionSlopeYZ=covarianceYZ/varianceZ;

  double interceptXZ=meanZ-meanX*d_vertexDistributionSlopeXZ;
  double interceptYZ=meanZ-meanY*d_vertexDistributionSlopeYZ;

  double sumDeltaZ=0;
  double sumEpsilonX=0;
  double sumEpsilonY=0;
  for(iv=vertexList.begin();iv<vertexList.end();iv++ ){
    Gaudi::XYZPoint vtx = (*iv).first;
    int nTrks = (*iv).second;
    if( nTrks >= i_minTracks ){
      sumDeltaZ+=pow(vtx.Z()-meanZ,2);
      sumEpsilonX+=pow(vtx.X()-interceptXZ-d_vertexDistributionSlopeXZ*vtx.Z(),2);
      sumEpsilonY+=pow(vtx.Y()-interceptYZ-d_vertexDistributionSlopeYZ*vtx.Z(),2);
    }
  }
  d_vertexDistributionSlopeErrorXZ=sqrt(sumEpsilonX/sumDeltaZ)/(N-2);
  d_vertexDistributionSlopeErrorYZ=sqrt(sumEpsilonY/sumDeltaZ)/(N-2);
}
//=============================================================================
