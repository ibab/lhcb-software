// Include files
// STD & STL
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IHistogramSvc.h"
// AIDA
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// Event
/// Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/EventHeader.h"
#include "Event/CaloHypo.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
// CaloUtils
#include "CaloUtils/dirHbookName.h"
#include "CaloUtils/ClusterFunctors.h"
// local
#include "CaloPhotonMonitor.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class CaloPhotonMonitor
 *  Photon Selection Monitoring
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

// ============================================================================

// ============================================================================
/** @var CaloPhotonMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloPhotonMonitor>         s_factory ;
const        IAlgFactory&CaloPhotonMonitorFactory = s_factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name
 *  @param pSvc service locator
 */
// ============================================================================
CaloPhotonMonitor::CaloPhotonMonitor
( const std::string& name ,
  ISvcLocator*       pSvc )
  : CaloMoniAlg ( name , pSvc )

  , m_nEvents   ( 0 )
  , m_nPhotons  ( 0 )
  , m_nMCPhotons( 0 )
  , m_nWrongs   ( 0 )

  , m_nameOfECAL("/dd/Structure/LHCb/Ecal")
  , m_nameOfSPD ("/dd/Structure/LHCb/Spd")
  , m_nameOfPRS ("/dd/Structure/LHCb/Prs")

  , m_zEcal     ( -100.*km )
  , m_zPrs      ( -100.*km )
  , m_zSpd      ( -100.*km )
  , m_shiftSpd  (  30.5*mm )
  , m_zConv     (12380.*mm )

  , m_vertex    (          )

  , m_incSvc    ( 0        )
  , m_evtSvc    ( 0        )
  , m_histoSvc  ( 0        )

  , m_matchType   ("AssociatorWeighted<CaloCluster,Track,float>")
  , m_matchName   ("PhotonMatch")
  , m_match       ( 0           )

  , m_associatorType ("AssociatorWeighted<CaloCluster,MCParticle,float>")
  , m_associatorName ("CCs2MCPs")
  , m_associator     ( 0 )

  , m_hypotoolType   ("CaloPhotonEstimatorTool")
  , m_hypotoolName   ("PhotonPID")
  , m_hypotool       ( 0 )

  , m_ppSvc          ( 0          )
  , m_gammaName      ( "gamma"    )
  , m_gammaID        ( 0          )
  , m_pi0Name        ( "pi0"      )
  , m_pi0ID          ( 0          )

  , m_extrapolation ( true )

  , m_pdf           (true)
  , m_ebin          (    )
  , m_prsbin        (    )
  , m_chi2bin       (    )
  , m_shapebin      (    )

  , m_zAcc          ( 12490.0 *mm)
  , m_xmin          (   360.1 *mm)
  , m_xmax          (  3721.3 *mm)
  , m_ymin          (   360.1 *mm)
  , m_ymax          (  3001.1 *mm)

  , m_etmin         (200.*MeV)
  , m_dr            (10. *mm )
  , m_dz            (150.*mm )
  , m_de            (0.25    )
  , m_mergedDist    (1.5     )

  , m_lhSig         ( 0 )
  , m_lhSigSpd      ( 0 )
  , m_lhBkg         ( 0 )
  , m_lhBkgSpd      ( 0 )

  , m_nbinlh        (20 )
  , m_effpur        ( 0 )
  , m_effpur_spd    ( 0 )
  , m_effpur_nospd  ( 0 )

  , m_nbinpt        (20     )
  , m_ptmin         (0. *MeV)
  , m_ptmax         (10.*GeV)
  , m_efficiency    ( 0     )
  , m_purity        ( 0     )
  , m_lhcut         ( 0.3   )

{
  declareProperty ( "HypoToolType"  , m_hypotoolType );
  declareProperty ( "HypoToolName"  , m_hypotoolName );
  declareProperty ( "MatchType"     , m_matchType );
  declareProperty ( "MatchName"     , m_matchName );
  declareProperty ( "AssociatorType", m_associatorType);
  declareProperty ( "AssociatorName", m_associatorName);

  declareProperty ( "DetEcal"       , m_nameOfECAL);
  declareProperty ( "DetSpd"        , m_nameOfSPD);
  declareProperty ( "DetPrs"        , m_nameOfPRS);

  declareProperty ( "ShiftSPDCell"  ,m_shiftSpd);
  declareProperty ( "ZConversion"   ,m_zConv);

  declareProperty ( "Extrapolation" , m_extrapolation);

  declareProperty ( "Pdf"           , m_pdf);
  declareProperty ( "EBin"          , m_ebin);
  declareProperty ( "EPrsBin"       , m_prsbin );
  declareProperty ( "Chi2Bin"       , m_chi2bin );
  declareProperty ( "ShapBin"       , m_shapebin );

  declareProperty ( "Etmin"         , m_etmin);
  declareProperty ( "Dr"            , m_dr);
  declareProperty ( "Dz"            , m_dz);
  declareProperty ( "DE"            , m_de);
  declareProperty ( "MergedDist"    , m_mergedDist);

  declareProperty ( "AccZ"         , m_zAcc);
  declareProperty ( "AccXmin"      , m_xmin);
  declareProperty ( "AccXmax"      , m_xmax);
  declareProperty ( "AccYmin"      , m_ymin);
  declareProperty ( "AccYmax"      , m_ymax);

  declareProperty ( "LhCut"        , m_lhcut );
  declareProperty ( "LhNBin"       , m_nbinlh);
  declareProperty ( "PtNBin"       , m_nbinpt);
  declareProperty ( "PtMinHisto"   , m_ptmin);
  declareProperty ( "PtMaxHisto"   , m_ptmax);

  // set the appropriate default values for input data
  setInputData    ( CaloHypoLocation::Photons ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloPhotonMonitor::~CaloPhotonMonitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonMonitor::initialize()
{
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = CaloMoniAlg::initialize() ;
  if( sc.isFailure() )
    { return Error("Base class could not be initialized",sc);}

//----- locate particle property service

  sc = service( "ParticlePropertySvc" , m_ppSvc , true );
  if( sc.isFailure() ) {
    logbk<<MSG::ERROR<<"Could not locate ParticlePropertySvc! "<<sc<<endreq;
  }
  if( 0 == m_ppSvc ) {
    logbk<<MSG::ERROR<<"Could not locate ParticlePropertySvc!"<<endreq;
  }

  m_ppSvc->addRef();
  const ParticleProperty* ppg = m_ppSvc->find( m_gammaName );
  const ParticleProperty* ppp = m_ppSvc->find( m_pi0Name );
  if( 0 == ppg ) {
    logbk<<MSG::ERROR<<"Could not locate particle ' "<<m_gammaName<<" '"<<endreq;
  }
  if( 0 == ppp ) {
    logbk<<MSG::ERROR<<"Could not locate particle ' "<<m_pi0Name<<" '"<<endreq;
  }
  m_gammaID = ParticleID( ppg->jetsetID() ) ;
  m_pi0ID = ParticleID( ppp->jetsetID() ) ;

//----- Detector recovery

  // ECAL
  SmartDataPtr<DeCalorimeter> detecal (detSvc(),m_nameOfECAL);
  if(!detecal)
    logbk<<MSG::ERROR<<"Unable to retrieve ECAL detector "
       <<m_nameOfECAL<<endreq;
  if (!detecal){ return StatusCode::FAILURE ;}

  // SPD
  SmartDataPtr<DeCalorimeter> detspd (detSvc(),m_nameOfSPD);
  if(!detspd)
    logbk<<MSG::ERROR<<"Unable to retrieve SPD detector "
       <<m_nameOfSPD<<endreq;
  if (!detspd){    return StatusCode::FAILURE ; }

  // PRS
  SmartDataPtr<DeCalorimeter> detprs (detSvc(),m_nameOfPRS);
  if(!detprs)
    logbk<<MSG::ERROR<<"Unable to retrieve PRS detector "
       <<m_nameOfPRS<<endreq;
  if (!detprs){ return StatusCode::FAILURE ; }

  // Convert Detectors to DeCalorimeter
  m_detEcal = (DeCalorimeter*) detecal;
  m_detSpd = (DeCalorimeter*) detspd;
  m_detPrs = (DeCalorimeter*) detprs;

//----- Geometry information
  const IGeometryInfo* geoinf = m_detEcal->geometry() ;
  if( 0 == geoinf ) {logbk<<MSG::ERROR<<"IGeometryInfo not available!"<<endreq;}
  // center of the detector in mother reference frame
  HepPoint3D center = geoinf->toGlobal( HepPoint3D() );
  m_zEcal = center.z() + m_detEcal->zShowerMax ();

  const IGeometryInfo* geoinfSpd = m_detSpd->geometry() ;
  if( 0 == geoinfSpd ) {logbk<<MSG::ERROR<<"IGeometryInfo not available!"<<endreq;}
  // center of the detector in mother reference frame
  HepPoint3D centerSpd = geoinfSpd->toGlobal( HepPoint3D() );
  m_zSpd = centerSpd.z();

  const IGeometryInfo* geoinfPrs = m_detPrs->geometry() ;
  if( 0 == geoinfPrs ) {logbk<<MSG::ERROR<<"IGeotryInfo not available!"<<endreq;}
  // center of the detector in mother reference frame
  HepPoint3D centerPrs = geoinfPrs->toGlobal( HepPoint3D() );
  m_zPrs = centerPrs.z();

//----- Tool recovery

// locate the associator

  sc = toolSvc()->retrieveTool( m_associatorType , m_associatorName , m_associator ) ;
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << " Unable to retrieve gamma Associator tool"
        << endreq;
    return sc;
  }

// locate match
  m_match  = tool<IMatch>( m_matchType  , m_matchName  , m_match  ) ;
  if( 0 == m_match  ) { return StatusCode::FAILURE ; }

// Hypo Tool
  m_hypotool = tool<ICaloHypoLikelihood>( m_hypotoolType, m_hypotoolName);

// IP
  m_vertex=HepPoint3D(0.,0.,0.);


//----- Check data consistency
  unsigned int nEbin=m_ebin.size()+1;
  if (m_prsbin.size()!=3){ return Error("Wrong Binning Parameter (EPrs)"); }
  if (m_chi2bin.size()!=3){ return Error("Wrong Binning Parameter (Chi2Tk)"); }
  if (m_shapebin.size()!=3){ return Error("Wrong Binning Parameter (ShShape)"); }

  // Reset Monitoring parameter
  for (int i=0 ; i<m_nbinpt ; ++i){
    m_mc_g.push_back(0);
    m_rec_bkg.push_back(0);
    m_rec_sig.push_back(0);
  }

  for (int i=0 ; i<m_nbinlh ; ++i){
    m_lh_mcg=0;
    m_lh_recsig.push_back(0);
    m_lh_recbkg.push_back(0);
    m_lh_mcg_conv=0;
    m_lh_recsig_conv.push_back(0);
    m_lh_recsig_spd.push_back(0);
    m_lh_recbkg_spd.push_back(0);
    m_lh_mcg=0;
    m_lh_recsig_noconv.push_back(0);
    m_lh_recsig_nospd.push_back(0);
    m_lh_recbkg_nospd.push_back(0);
  }
  // Histograms

  if( 0 != m_histoSvc ) { m_histoSvc->release () ; m_histoSvc = 0 ; }
  sc = service ("HistogramDataSvc" , m_histoSvc );
  if( sc.isFailure() )
    { return Error ( "HistogramDataSvc is not available!" , sc ) ; }
  logbk<<MSG::DEBUG<<"HistogramDataSvc is loaded properly"<<endreq;

// Likelihood Output for Signal / Background
  m_lhSig   = m_histoSvc->book( dirHbookName( "Calo/"+name() ) ,
                              1                                ,
                              "likelihood Signal - no SPD"     ,
                              100,0.,1.);
  m_lhSigSpd= m_histoSvc->book( dirHbookName( "Calo/"+name() ) ,
                              2                                ,
                              "likelihood Signal - SPD hit"    ,
                              100,0.,1.);
  m_lhBkg   = m_histoSvc->book( dirHbookName( "Calo/"+name() ) ,
                              3                                ,
                              "likelihood Background - no SPD" ,
                              100,0.,1.);
  m_lhBkgSpd= m_histoSvc->book( dirHbookName( "Calo/"+name() ) ,
                              4                                ,
                              "likelihood Background - SPD hit",
                              100,0.,1.);
// Efficiency / Purity versus Pt
  m_efficiency = m_histoSvc->book(
    dirHbookName ( "Calo/"+name() ),10,
    "Photon Selection Efficiency",
    m_nbinpt,m_ptmin,m_ptmax);

  m_purity = m_histoSvc->book(
    dirHbookName ( "Calo/"+name() ),11,
    "Photon Selection Purity",
    m_nbinpt,m_ptmin,m_ptmax);

  m_effpur = m_histoSvc->book(
      dirHbookName ( "Calo/"+name() ),15,
      "Efficiency vs Purity - Lh cut",
      100,0.,1.,100,0.,1.);

  m_effpur_nospd = m_histoSvc->book(
      dirHbookName ( "Calo/"+name() ),16,
      "Efficiency vs Purity - Lh cut - no Conv. sample",
      100,0.,1.,100,0.,1.);

  m_effpur_spd = m_histoSvc->book(
      dirHbookName ( "Calo/"+name() ),17,
      "Efficiency vs Purity - Lh cut - Conv. sample",
      100,0.,1.,100,0.,1.);

  // Probability Density Functions Definitions
  if (m_pdf){
    const std::string directory ( dirHbookName( "Calo/"+name() + "/PDF" ) );

    m_signalEPrs=defHisto1d(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                           100, std::string("Signal Prs noSpdHit"),
                           directory,nEbin);
    m_signalChi2=defHisto1d(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                           200, std::string("Signal Chi2Tk noSpdHit"),
                           directory,nEbin);
    m_signalShape=defHisto1d(int(m_shapebin[ 0 ]),m_shapebin[ 1 ],m_shapebin[ 2 ],
                            300, std::string("Signal Shower Shape noSpdHit"),
                            directory,nEbin);

    m_signalEPrsSpd=defHisto1d(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                              110, std::string("Signal Prs SpdHit"),
                              directory,nEbin);
    m_signalChi2Spd=defHisto1d(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                              210, std::string("Signal Chi2Tk SpdHit"),
                              directory,nEbin);
    m_signalShapeSpd=defHisto1d(int(m_shapebin[ 0 ]),m_shapebin[ 1 ],m_shapebin[ 2 ],
                              310, std::string("Signal Shower Shape SpdHit"),
                              directory,nEbin);

    m_backgrEPrs=defHisto1d(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                           1100, std::string("Background Prs noSpdHit"),
                           directory,nEbin);
    m_backgrChi2=defHisto1d(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                           1200, std::string("Background Chi2Tk noSpdHit"),
                           directory,nEbin);
    m_backgrShape=defHisto1d(int(m_shapebin[ 0 ]),m_shapebin[ 1 ],m_shapebin[ 2 ],
                            1300, std::string("Background Shower Shape noSpdHit"),
                            directory,nEbin);

    m_backgrEPrsSpd=defHisto1d(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                              1110, std::string("Background Prs SpdHit"),
                              directory,nEbin);
    m_backgrChi2Spd=defHisto1d(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                              1210, std::string("Background Chi2Tk SpdHit"),
                              directory,nEbin);
    m_backgrShapeSpd=defHisto1d(int(m_shapebin[ 0 ]),m_shapebin[ 1 ],m_shapebin[ 2 ],
                              1310, std::string("Background Shower Shape SpdHit"),
                              directory,nEbin);
   }


  // General WARNING
  logbk<<MSG::WARNING<<"SPD scintillator offset HARD-CODED : value="
    <<m_shiftSpd<<endreq;
  logbk<<MSG::WARNING<<"Vertex location HARD-CODED : IP(0.,0.,0.)"<<endreq;
  logbk<<MSG::WARNING<<"MCTruth Conversion definition is z="<<m_zConv<<endreq;

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonMonitor::finalize()
{
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG  << "==> Finalize" << endreq;


  logbk << MSG::INFO << "************* Photon Monitoring *****************"<<endreq;
  logbk << MSG::INFO << "Number of Events Analyzed : " << m_nEvents << endreq;
  logbk << MSG::INFO
    //<< "MCPhotons (Signal def.) in sample: "<<m_nMCPhotons<<" - "
    <<m_nPhotons<<" Photons (Signal def.) out of "<<m_nCandidats<<" hypos processed."
    <<" - "<<m_nWrongs<<" hypos rejected."<<endreq;
  char line[70];
  logbk << MSG::INFO << endreq;
  logbk << MSG::INFO << "     Et(GeV)        | Efficiency |   Purity   "<<endreq;
  logbk << MSG::INFO << "----------------------------------------------"<<endreq;
  for (int i=0; i<m_nbinpt ; ++i){
    float pt=m_ptmin+float(i)*(m_ptmax-m_ptmin)/float(m_nbinpt);
    float eff=(m_mc_g[i]>0) ? float(m_rec_sig[i])/float(m_mc_g[i]) : (0.F);
    float pur=(m_rec_sig[i]+m_rec_bkg[i]>0) ?
          float(m_rec_sig[i])/float(m_rec_sig[i]+m_rec_bkg[i]) : (0.F) ;
    sprintf(line," [ %5.2f - %5.2f ]  |    %4.2f    |    %4.2f    ",
      pt/1000.,(pt+(m_ptmax-m_ptmin)/float(m_nbinpt))/1000.,eff,pur);

    logbk << MSG::INFO << line <<endreq;

    m_efficiency->fill(pt,eff);
    m_purity->fill(pt,pur);
  }
  logbk << MSG::INFO << endreq;
  logbk << MSG::INFO << "  L>= |   Total   |   No Conv |    Conv   "<<endreq;
  logbk << MSG::INFO << "      | Eff   Pur | Eff   Pur | Eff   Pur "<<endreq;
  logbk << MSG::INFO << " -----------------------------------------"<<endreq;
  for (int i=0; i<m_nbinlh ; ++i){
    float eff=(m_lh_mcg>0) ? float(m_lh_recsig[i])/float(m_lh_mcg) : (0.F);
    float effnoconv=(m_lh_mcg_noconv>0) ?
      float(m_lh_recsig_noconv[i])/float(m_lh_mcg_noconv) : (0.F);
    float effconv=(m_lh_mcg_conv>0) ?
      float(m_lh_recsig_conv[i])/float(m_lh_mcg_conv) : (0.F);

    float pur=(m_lh_recsig[i]+m_lh_recbkg[i]>0) ?
          float(m_lh_recsig[i])/float(m_lh_recsig[i]+m_lh_recbkg[i]) : (0.F) ;
    float purnoconv=(m_lh_recsig_nospd[i]+m_lh_recbkg_nospd[i]>0) ?
          float(m_lh_recsig_nospd[i])/float(m_lh_recsig_nospd[i]+m_lh_recbkg_nospd[i]) : (0.F) ;
    float purconv=(m_lh_recsig_spd[i]+m_lh_recbkg_spd[i]>0) ?
          float(m_lh_recsig_spd[i])/float(m_lh_recsig_spd[i]+m_lh_recbkg_spd[i]) : (0.F) ;

    m_effpur->fill(pur,eff,1.);
    m_effpur_nospd->fill(purnoconv,effnoconv,1.);
    m_effpur_spd->fill(purconv,effconv,1.);

    sprintf(line," %3.2f | %3.2f %3.2f | %3.2f %3.2f | %3.2f %3.2f",
            float(i)/float(m_nbinlh),eff,pur,effnoconv,purnoconv,effconv,purconv);

    logbk << MSG::INFO << line <<endreq;

//    m_efficiency->fill(pt,eff);
//    m_purity->fill(pt,pur);
  }
  logbk << MSG::INFO << "*************************************************"<<endreq;


  return CaloMoniAlg::finalize() ;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonMonitor::execute() {

  MsgStream logbk( msgSvc() , name() );

  logbk<<MSG::DEBUG<<"  == > Execute "<<endreq;

  m_nEvents++;

  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );
  //int run=evt->runNum();
  //int nevt=evt->evtNum();

  // avoid long names
  typedef CaloHypos        Hypos    ;

  // get MC photons
  SmartDataPtr< MCParticles > mcParts (eventSvc(),MCParticleLocation::Default);
  for(MCParticles::const_iterator iter = mcParts->begin() ;
                                  mcParts->end()!=iter ; ++iter ){
    SmartRef<MCParticle> part=*iter;
    if ( m_gammaID == part->particleID() ) {

      // keep conversion information
      double conv=1.*km;

  	  SmartRefVector<MCVertex> decay=part->endVertices();
      for(SmartRefVector<MCVertex>::const_iterator iter=decay.begin();
        decay.end()!=iter;++iter){
        double zpos=(*iter)->position().x();
        if(conv>zpos){conv=zpos;}
      }

      double mcpz=part->momentum().pz();
      double mcpt=part->momentum().perp();

      // Etmin Acceptance - Pz >0.
      if( mcpt < m_etmin || mcpz < 0.){
        continue;
      }

  	  SmartRef<MCVertex> vert=part->originVertex();
  	  if ( 0==vert ) {
	      continue;
	    }

      // Origin of the photon
      double mcix=vert->position().x();
      double mciy=vert->position().y();
      double mciz=vert->position().z();

      if ( fabs(mciz)>m_dz || (mcix*mcix+mciy*mciy)>(m_dr*m_dr) ) {
        continue;
      }

      // Detector acceptance
      double mcpx=part->momentum().px();
      double mcpy=part->momentum().py();

      double lambda=(m_zAcc-mciz)/mcpz;
      float mcxAcc=(float)(mcix+lambda*mcpx);
      float mcyAcc=(float)(mciy+lambda*mcpy);

      bool mcInAcceptance=(fabs(mcxAcc)<m_xmax&&fabs(mcyAcc)<m_ymax)&&
                   (fabs(mcxAcc)>m_xmin||fabs(mcyAcc)>m_ymin);

      if(!mcInAcceptance){
        continue;
      }

      // belong to a merged pi0 ?
      bool isMerged=false;
      SmartRef<MCParticle> mother=part->mother();
      if (0!=mother){
        if ( m_pi0ID == mother->particleID() ){
          SmartRef<MCVertex> decayPi0=part->originVertex();
          SmartRefVector<MCParticle> p=decayPi0->products();
          if (2!=p.size()) {continue;}
          for (SmartRefVector<MCParticle>::const_iterator pIter=p.begin();
            p.end()!=pIter;++pIter){
              SmartRef<MCParticle> pi0daughter=*pIter;
              if ( part == pi0daughter ) {continue;}

              double mc2px=pi0daughter->momentum().px();
              double mc2py=pi0daughter->momentum().py();
              double mc2pz=pi0daughter->momentum().pz();

              double zi=m_zEcal-decayPi0->position().z();
              double xi=decayPi0->position().x();
              double yi=decayPi0->position().y();

              float mcx1=(float)(xi+zi*mcpx/mcpz);
              float mcy1=(float)(yi+zi*mcpy/mcpz);
              float mcx2=(float)(xi+zi*mc2px/mc2pz);
              float mcy2=(float)(yi+zi*mc2py/mc2pz);

              double distance=sqrt(pow(mcx1-mcx2,2)+pow(mcy1-mcy2,2));

              HepPoint3D pt1(mcx1,mcy1,m_zEcal);
              HepPoint3D pt2(mcx2,mcy2,m_zEcal);

              double param=m_mergedDist*(m_detEcal->cellSize(m_detEcal->Cell(pt1))+
                m_detEcal->cellSize(m_detEcal->Cell(pt2)))/2.;

              if (distance<param) {
                isMerged=true;
                logbk<<MSG::DEBUG<<"Pi0->Merged Photon :  distance="<<distance
                  <<"  < Criteria="<<param<<" mm"<<endreq;
              }
            }
        }
      }

      if (isMerged) {
        logbk<<MSG::DEBUG<<"Merged Pi0 photons removed from Signal sample"<<endreq;
        continue;
      }
      /*
      logbk<<MSG::INFO<<"4-momentum "<<mcpx<<" "<<mcpy<<" "<<mcpz<<endreq;
      logbk<<MSG::INFO<<"Acceptance "<<mcxAcc<<" "<<mcyAcc<<endreq;
      logbk<<MSG::INFO<<"Origin     "<<mcix<<" "<<mciy<<" "<<mciz<<endreq;
      */

      m_nMCPhotons++;

      int ibin=int(m_nbinpt*(mcpt-m_ptmin)/(m_ptmax-m_ptmin));
      if (ibin>=0 && ibin<m_nbinpt){
        // efficiency / purity versus pt
        m_mc_g[ibin]++;
        // efficiency / purity versus lieklihood
        m_lh_mcg++;
        if (conv>m_zConv) {m_lh_mcg_noconv++;}
        else {m_lh_mcg_conv++;}
      }
    }
  }

  // get input hypos
  const Hypos *hypos = get<Hypos>(inputData());
  if( 0 == hypos ) {
    logbk<<MSG::DEBUG<<"No input Data: "<<inputData()<<endreq;
    return StatusCode::FAILURE ;
  }

  // Table for gammas
  const GammaMCTable* gtable = m_associator->direct();
  if( 0 == gtable ) {
    logbk << MSG::INFO <<"No Relation Table" << endreq;
  }

  // loop over hypos
  for(Hypos::const_iterator iter=hypos->begin(); hypos->end()!=iter;++iter){
     // skip nulls
     if( 0 == *iter ) {
      logbk<<MSG::DEBUG<<"empty CaloHypo : skipping"<<endreq;
      continue ;
     }

     CaloHypo* hypo=(*iter);

     // Get the relevant information - basic checks
     if( 0 == hypo ){
       return Exception( " *CaloHypo* points to NULL " );
     }

     m_nCandidats++;

     // Energy
     double et=hypo->momentum()->momentum().perp();
     logbk<<MSG::DEBUG<<" ==> Processing new CaloHypo : Et="<<et<<endreq;

     if (et<m_etmin){
       continue;
     }

     if (hypo->clusters().size()!=1){
       logbk<<MSG::WARNING<<"Photon Hypothesis : number of clusters!=1..."<<endreq;
       continue;
     }

     const SmartRef<CaloCluster> cluster=hypo->clusters().front();
     if ( 0 == cluster )
       { continue; return Exception( " *CaloCluster* points to NULL " ); }

     std::vector<CaloClusterEntry> entries = cluster->entries();
     if( cluster->entries().empty() )
       { return Exception( " *CaloCluster* empty " );}

     CaloCluster::Entries::const_iterator iseed =
     ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                   cluster->entries().end  () ,
                                   CaloDigitStatus::SeedCell  ) ;
      if( iseed == cluster->entries().end() )
        { return Exception( " *SeedCell* not found ");}

     const CaloDigit* seed = iseed->digit();
     if( 0 == seed )
        { return Exception( " SeedCell *Digit* points to NULL! ");}

// ***
// Evaluate Estimator Parameters : Energy, EPrs, Chi2 and Shower Shape ...

     const CaloPosition *pos = hypo->position() ;
     const  HepPoint3D position (pos->x(),pos->y(),pos->z());

     // in Acceptance
     //transport to zAcc
     double lambda=(m_zAcc-m_vertex.z())/(pos->z()-m_vertex.z());
     double xAcc=m_vertex.x()+lambda*(pos->x()-m_vertex.x());
     double yAcc=m_vertex.y()+lambda*(pos->y()-m_vertex.y());
     bool inAcceptance=(fabs(xAcc)<m_xmax&&fabs(yAcc)<m_ymax)&&
                       (fabs(xAcc)>m_xmin||fabs(yAcc)>m_ymin);

     if (!inAcceptance){
       continue;
     }

     // Energy
     double energy=hypo->momentum()->momentum().e();
     logbk<<MSG::DEBUG<<"  - Energy [MeV]="<<energy<<endreq;

     /// Chi2
     double chi2;
     const Table* table = m_match->direct();
     if( 0 == table     ) { return Error("Table* points to NULL!");} // RETURN

     const Range range = table->relations( cluster ) ;
     if( range.empty() )  { chi2=1.e+6; }            // bad match -> large value !
     else {
       chi2 = range.front().weight();
     }
     logbk<<MSG::DEBUG<<"  - Chi2        ="<<chi2<<endreq;
     // Shower Shape
     double shape=pos->spread()(1,1)+pos->spread()(2,2);
     logbk<<MSG::DEBUG<<"  - Shower Shape="<<shape<<endreq;
     //Spd hit and Prs deposit
     double eSpd=0.;
     double ePrs=0.;

     // point in the detector
     HepPoint3D spdPoint,prsPoint;
     if (!m_extrapolation){
       spdPoint.setX(pos->x());
       spdPoint.setY(pos->y());
       spdPoint.setZ( m_zSpd + m_shiftSpd );
       prsPoint.setX(pos->x());
       prsPoint.setY(pos->y());
       prsPoint.setZ(m_zPrs);
     }
     else {
       HepPoint3D position(pos->x(),pos->y(),pos->z());
       spdPoint  =  ( position - m_vertex );
       spdPoint *=  ( m_zSpd + m_shiftSpd - m_vertex.z() ) /
                                    ( position.z() - m_vertex.z() );
       spdPoint +=  m_vertex ;
       prsPoint  =  ( position - m_vertex );
       prsPoint *=  ( m_zPrs - m_vertex.z() ) /
                                     ( position.z() - m_vertex.z() );
       prsPoint +=  m_vertex ;
     }

     const CaloCellID cellSpd = m_detSpd->Cell( spdPoint );
     const CaloCellID cellPrs = m_detPrs->Cell( prsPoint );


	   // Get CaloCell Deposits in the SPD and PRS
     if( !(CaloCellID() == cellSpd) ){
       for (SmartRefVector<CaloDigit>::const_iterator
	   	  digit=hypo->digits().begin() ;
	   		digit != hypo->digits().end() ; digit++ ) {
	       if ( (*digit)->cellID() == cellSpd ) {
	   			eSpd=(*digit)->e();
	       }
	     }
       logbk<<MSG::DEBUG<<"  - SPD "<<cellSpd<<" Energy  ="<<eSpd<<endreq;
	   }

     if( !(CaloCellID() == cellPrs) ){
    	 for (SmartRefVector<CaloDigit>::const_iterator
	       digit=hypo->digits().begin() ;
	      digit != hypo->digits().end() ; digit++ ) {
    	    if ( (*digit)->cellID() == cellPrs ) {
    	 	  	ePrs=(*digit)->e();
          }
    	  }
        logbk<<MSG::DEBUG<<"  - PRS "<<cellPrs<<" Energy  ="<<ePrs<<endreq;
     }

     // ***
     double likelihood=(*m_hypotool)(hypo);
     logbk<<MSG::DEBUG<<"Likelihood = "<<likelihood<<endreq;

     if(likelihood<0.){m_nWrongs++;}

     // MCTruth Information
     GammaMCRange grange = gtable->relations(cluster);

     int mcpartid=0;
     double conv=1.*km;
     double wmax=-1.e6;
     double mce=0.;
     unsigned int nmc=0;
     double dr=1.e+6;
     double dz=1.e+6;
     double de=1.e+6;
     bool isSignal=false;
     bool isPhoton=false;
     bool isMerged=false;
     for( GammaMCTable::iterator mc = grange.begin() ; grange.end() != mc ; ++mc ) {
    	 if( 0 == mc->to() ) { continue ; }
	     nmc++;
	     logbk <<MSG::DEBUG<<"mctruth : --> pid="<<mc->to()->particleID().pid()
		                     <<" weight="<<mc->weight()<<endreq;
	     if( mc->weight()>wmax ){
         if( m_gammaID == mc->to()->particleID() ) {
           isPhoton=true;
           SmartRef<MCParticle> mcpart=mc->to();
           wmax=mc->weight();
           mcpartid=mcpart->particleID().pid();
        	 logbk<<MSG::DEBUG<<"MC Gamma: E="<<mcpart->momentum().e()
                            <<" - Weight="<<wmax<<endreq;
        	 SmartRef<MCVertex> vertex=mcpart->originVertex();
           if ( 0==vertex ) {
	           logbk<<MSG::WARNING<<"Gamma has no origin vertex!"<<endreq;
	           continue;
	         }

           mce=mcpart->momentum().e();
           dr=sqrt(vertex->position().x()*vertex->position().x()+
                   vertex->position().y()*vertex->position().y());
           dz=vertex->position().z();
           de=fabs(energy-mce)/energy;
           logbk<<MSG::DEBUG<<"dr="<<dr<<" - dz="<<dz<<" - de="<<de<<endreq;
           SmartRefVector<MCVertex> decay=mcpart->endVertices();
           for(SmartRefVector<MCVertex>::const_iterator iter=decay.begin();
             decay.end()!=iter;++iter){
               double zpos=(*iter)->position().x();
               if(conv>zpos){conv=zpos;}
           }

           // belong to a merged pi0 ?
           SmartRef<MCParticle> mother=mcpart->mother();
           if (0!=mother){
             if ( m_pi0ID == mother->particleID() ){
               SmartRef<MCVertex> decayPi0=mcpart->originVertex();
               SmartRefVector<MCParticle> p=decayPi0->products();
               if (2!=p.size()) {continue;}
               for (SmartRefVector<MCParticle>::const_iterator pIter=p.begin();
                    p.end()!=pIter;++pIter){
                 SmartRef<MCParticle> pi0daughter=*pIter;
                 if ( mcpart == pi0daughter ) {continue;}

                 double mcpx=mcpart->momentum().px();
                 double mcpy=mcpart->momentum().py();
                 double mcpz=mcpart->momentum().pz();

                 double mc2px=pi0daughter->momentum().px();
                 double mc2py=pi0daughter->momentum().py();
                 double mc2pz=pi0daughter->momentum().pz();

                 double zi=m_zEcal-decayPi0->position().z();
                 double xi=decayPi0->position().x();
                 double yi=decayPi0->position().y();

                 float mcx1=(float)(xi+zi*mcpx/mcpz);
                 float mcy1=(float)(yi+zi*mcpy/mcpz);
                 float mcx2=(float)(xi+zi*mc2px/mc2pz);
                 float mcy2=(float)(yi+zi*mc2py/mc2pz);

                 double distance=sqrt(pow(mcx1-mcx2,2)+pow(mcy1-mcy2,2));

                 HepPoint3D pt1(mcx1,mcy1,m_zEcal);
                 HepPoint3D pt2(mcx2,mcy2,m_zEcal);

                 double param=m_mergedDist*(m_detEcal->cellSize(m_detEcal->Cell(pt1))+
                 m_detEcal->cellSize(m_detEcal->Cell(pt2)))/2.;

                 if (distance<param) {
                   isMerged=true;
                   logbk<<MSG::DEBUG<<"Pi0->Merged Photon :  distance="<<distance
                     <<"  < Criteria="<<param<<" mm"<<endreq;
                 }
               }
             }
           }
         }
         else {
           isPhoton=false;
         }
	     }
     }

     if ( de<m_de && dr<m_dr && dz<m_dz && !isMerged){
       m_nPhotons++;
       isSignal=true;
       logbk<<MSG::DEBUG<<"Candidate is Signal according to MC"<<endreq;
     }

     // Efficiency and purity versus pt
     if (likelihood>m_lhcut){
       double pt=hypo->momentum()->momentum().perp();
       int ibin=int(m_nbinpt*(pt-m_ptmin)/(m_ptmax-m_ptmin));
       if (ibin>=0 && ibin<m_nbinpt){
         if (isSignal) {m_rec_sig[ibin]++;}
         else {m_rec_bkg[ibin]++;}
       }
     }

     // Efficiency and purity versus likelihood
     int lhbin=int(likelihood*double(m_nbinlh));
     for (int l=0;l<lhbin;l++){
       if (isSignal) {
         m_lh_recsig[l]++;
         if (eSpd>1.){m_lh_recsig_spd[l]++;}
         else {m_lh_recsig_nospd[l]++;}
         if (conv>m_zConv) {m_lh_recsig_noconv[l]++;}
         else {m_lh_recsig_conv[l]++;}
       }
       else {
         m_lh_recbkg[l]++;
         if (eSpd>1.){m_lh_recbkg_spd[l]++;}
         else {m_lh_recbkg_nospd[l]++;}
       }
     }



     // Fill General Monitoring histograms
     if (isSignal){
       if (eSpd>1.){
         m_lhSigSpd->fill(likelihood);
       }
       else{
         m_lhSig->fill(likelihood);
       }
     }
     else{
       if (eSpd>1.){
         m_lhBkgSpd->fill(likelihood);
       }
       else{
         m_lhBkg->fill(likelihood);
       }
     }

     if (m_pdf) {
     // Fill PDF histograms
     // Corresponding Energy bin
       unsigned int ebin = bin(energy,m_ebin);

       if (isSignal) {
         if (eSpd>1.){
           m_signalEPrsSpd  [ ebin ] -> fill(ePrs ,1.);
           m_signalChi2Spd  [ ebin ] -> fill(chi2 ,1.);
           m_signalShapeSpd [ ebin ] -> fill(shape,1.);
         }
         else {
           m_signalEPrs  [ ebin ] -> fill(ePrs ,1.);
           m_signalChi2  [ ebin ] -> fill(chi2 ,1.);
           m_signalShape [ ebin ] -> fill(shape,1.);
         }
       }
       else {
         if (eSpd>1.){
           m_backgrEPrsSpd  [ ebin ] -> fill(ePrs ,1.);
           m_backgrChi2Spd  [ ebin ] -> fill(chi2 ,1.);
           m_backgrShapeSpd [ ebin ] -> fill(shape,1.);
         }
         else {
           m_backgrEPrs  [ ebin ] -> fill(ePrs ,1.);
           m_backgrChi2  [ ebin ] -> fill(chi2 ,1.);
           m_backgrShape [ ebin ] -> fill(shape,1.);
         }
       }
     }
   }
  return StatusCode::SUCCESS;
}

// ============================================================================

// ============================================================================

std::vector<IHistogram1D*> CaloPhotonMonitor::defHisto1d(
	const unsigned int bin, const double xmin, const double xmax,
  const unsigned int nhisto, std::string hname, const std::string dir,
  const unsigned int n){

  MsgStream logbk( msgSvc() , name() );

  std::vector<IHistogram1D*> histoList;
	char histoname[60];

  for (unsigned int i=0; i<n; ++i){
		IHistogram1D* histo;
    sprintf(histoname,"%s #%i",hname.c_str(),nhisto+i);
    histo  = m_histoSvc->book( dir                      ,
                                   nhisto+i                 ,
                                   histoname                ,
                                   bin, int(xmin), int(xmax) );

		logbk<<MSG::DEBUG<<"booking Histo ..."<<histoname<<endreq;

    if( 0 == histo  ) {
			logbk<<MSG::INFO<<"Histogram "<<histoname<<" not booked "<<endreq;
      logbk<<MSG::INFO<<"Directory : "<<dir<<endreq;
      logbk<<MSG::INFO<<"id        : "<<nhisto+i<<endreq;
      logbk<<MSG::INFO<<"Title     : "<<histoname<<endreq;
      logbk<<MSG::INFO<<"bin-low/up: "<<bin<<"-"
                      <<xmin<<"/"<<xmax<<endreq;
    }
		histoList.push_back(histo);
   }
	return histoList;
}

// ============================================================================
// The END
// ============================================================================


