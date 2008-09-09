// Include files
// STD & STL
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "CaloUtils/Calo2Track.h"
#include "Event/CaloHypo.h"
// CaloUtils
#include  "CaloUtils/CaloMomentum.h"
#include "CaloUtils/ClusterFunctors.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// LHCb
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// local
#include "CaloPhotonChecker.h"

DECLARE_ALGORITHM_FACTORY( CaloPhotonChecker );

// ============================================================================
/** @file
 *
 *  Implementation file for class CaloPhotonChecker
 *  Photon Selection Monitoring
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */
// ============================================================================
/** Standard constructor
 *  @param name algorithm name
 *  @param pSvc service locator
 */
// ============================================================================
CaloPhotonChecker::CaloPhotonChecker
( const std::string& name ,
  ISvcLocator*       pSvc )
  : CaloMoniAlg ( name , pSvc )

  , m_nEvents   ( 0 )
  , m_nPhotons  ( 0 )
  , m_nMCPhotons( 0 )
  , m_nWrongs   ( 0 )

  , m_TrTableName   ( LHCb::CaloIdLocation::ClusterMatch )
  , m_MCTableName ("Relations/" + LHCb::CaloClusterLocation::Default )

  , m_hypotoolType   ("CaloPhotonEstimatorTool")
  , m_hypotoolName   ("PhotonPID")
  , m_hypotool       ( 0 )

  , m_ppSvc          ( 0          )
  , m_gammaName      ( "gamma"    )
  , m_gammaID        ( 0          )
  , m_pi0Name        ( "pi0"      )
  , m_pi0ID          ( 0          )


  , m_pdf           (true)
  , m_ebin          (    )
  , m_prsbin        (    )
  , m_chi2bin       (    )
  , m_shapebin      (    )


  , m_etmin         (200.*Gaudi::Units::MeV)
  , m_dr            (10. *Gaudi::Units::mm )
  , m_dz            (150.*Gaudi::Units::mm )
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
  , m_ptmin         (0. *Gaudi::Units::MeV)
  , m_ptmax         (10.*Gaudi::Units::GeV)
  , m_efficiency    ( 0     )
  , m_purity        ( 0     )
  , m_lhcut         ( 0.3   )

{
  declareProperty ( "HypoToolType"  , m_hypotoolType );
  declareProperty ( "HypoToolName"  , m_hypotoolName );
  declareProperty ( "CC2TrTableName", m_TrTableName );
  declareProperty ( "CC2MCPTableName", m_MCTableName);

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


  declareProperty ( "LhCut"        , m_lhcut );
  declareProperty ( "LhNBin"       , m_nbinlh);
  declareProperty ( "PtNBin"       , m_nbinpt);
  declareProperty ( "PtMinHisto"   , m_ptmin);
  declareProperty ( "PtMaxHisto"   , m_ptmax);

  // set the appropriate default values for input data
  setInputData    ( LHCb::CaloHypoLocation::Photons ) ;
};
// ============================================================================

// ============================================================================
/** standard algorithm initialization
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonChecker::initialize()
{ StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm


//----- locate particle property service

  sc = service( "ParticlePropertySvc" , m_ppSvc , true );
  if( sc.isFailure() )error()<<"Could not locate ParticlePropertySvc! "<<sc<<endreq;
    if( 0 == m_ppSvc )error()<<"Could not locate ParticlePropertySvc!"<<endreq;

  m_ppSvc->addRef();
  const ParticleProperty* ppg = m_ppSvc->find( m_gammaName );
  const ParticleProperty* ppp = m_ppSvc->find( m_pi0Name );
  if( 0 == ppg )error()<<"Could not locate particle ' "<<m_gammaName<<" '"<<endreq;
  if( 0 == ppp )error()<<"Could not locate particle ' "<<m_pi0Name<<" '"<<endreq;
  m_gammaID = LHCb::ParticleID( ppg->jetsetID() ) ;
  m_pi0ID = LHCb::ParticleID( ppp->jetsetID() ) ;

//----- Detector recovery

  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_spd  = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  m_prs  = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  
  m_ecalPlane = m_ecal->plane( CaloPlane::ShowerMax );
  m_spdPlane  = m_spd->plane( CaloPlane::Middle );
  m_prsPlane  = m_prs->plane( CaloPlane::Middle );
  Gaudi::Plane3D spdFront = m_spd->plane( CaloPlane::Front );
  Gaudi::XYZVector normal = spdFront.Normal();
  m_zConv = - spdFront.HesseDistance() /normal.Z(); 
  debug() <<  "z conversion is set to SPD front : " << m_zConv << endreq;
  
//----- Tool recovery

// Hypo Tool
  m_hypotool = tool<ICaloHypoLikelihood>( m_hypotoolType, m_hypotoolName);



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


// Likelihood Output for Signal / Background
  hBook1( "1","likelihood Signal - no SPD" , 0.,1.,100);
  hBook1( "2","likelihood Signal - SPD hit", 0.,1.,100);
  hBook1( "3","likelihood Background - no SPD",0.,1.,100);
  hBook1( "4","likelihood Background - SPD hit",0.,1.,100);
// Efficiency / Purity versus Pt
  hBook1( "10" ,"Photon Selection Efficiency",m_ptmin,m_ptmax,m_nbinpt);
  hBook1( "11" ,"Photon Selection Purity",m_ptmin,m_ptmax,m_nbinpt);
  hBook2( "15", "Efficiency vs Purity - Lh cut", 0.,1.,100,0.,1.,100);
  hBook2( "16", "Efficiency vs Purity - Lh cut - no Conv. sample", 0.,1.,100,0.,1.,100);
  hBook2( "17", "Efficiency vs Purity - Lh cut - Conv. sample", 0.,1.,100,0.,1.,100);

  // Probability Density Functions Definitions
  if (m_pdf){
    std::string directory = "PDF/";
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
StatusCode CaloPhotonChecker::finalize()
{
  debug() << "==> Finalize" << endreq;


  info() << "************* Photon Monitoring *****************"<<endreq;
  info() << "Number of Events Analyzed : " << m_nEvents << endreq;
  info()
    //<< "MCPhotons (Signal def.) in sample: "<<m_nMCPhotons<<" - "
    <<m_nPhotons<<" Photons (Signal def.) out of "<<m_nCandidats<<" hypos processed."
    <<" - "<<m_nWrongs<<" hypos rejected."<<endreq;
  char line[70];
  info() << endreq;
  info() << "     Et(GeV)        | Efficiency |   Purity   "<<endreq;
  info() << "----------------------------------------------"<<endreq;
  for (int i=0; i<m_nbinpt ; ++i){
    float pt=m_ptmin+float(i)*(m_ptmax-m_ptmin)/float(m_nbinpt);
    float eff=(m_mc_g[i]>0) ? float(m_rec_sig[i])/float(m_mc_g[i]) : (0.F);
    float pur=(m_rec_sig[i]+m_rec_bkg[i]>0) ?
          float(m_rec_sig[i])/float(m_rec_sig[i]+m_rec_bkg[i]) : (0.F) ;
    sprintf(line," [ %5.2f - %5.2f ]  |    %4.2f    |    %4.2f    ",
      pt/1000.,(pt+(m_ptmax-m_ptmin)/float(m_nbinpt))/1000.,eff,pur);

    info() << line <<endreq;

    hFill1("10",pt,eff);
    hFill1("11",pt,pur);
  }
  info() << endreq;
  info() << "  L>= |   Total   |   No Conv |    Conv   "<<endreq;
  info() << "      | Eff   Pur | Eff   Pur | Eff   Pur "<<endreq;
  info() << " -----------------------------------------"<<endreq;
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

    hFill2("15",pur,eff);
    hFill2("16",purnoconv,effnoconv);
    hFill2("17",purconv,effconv);

    sprintf(line," %3.2f | %3.2f %3.2f | %3.2f %3.2f | %3.2f %3.2f",
            float(i)/float(m_nbinlh),eff,pur,effnoconv,purnoconv,effconv,purconv);

    info() << line <<endreq;

  }
  info() << "*************************************************"<<endreq;


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
StatusCode CaloPhotonChecker::execute()
{ typedef IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float>  MCTable;
  typedef MCTable::Range                                               MCRange;
  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::Track,float>       Table;
  typedef Table::Range                                                 Range;

  typedef const LHCb::CaloHypo::Container   Hypos;
  typedef const LHCb::MCParticle::Container MCParticles;

  
  debug() <<"  == > Execute "<<endreq;

  m_nEvents++;
  
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;


  // get MC photons
  SmartDataPtr< MCParticles > mcParts (eventSvc(),LHCb::MCParticleLocation::Default);
  for(MCParticles::const_iterator iter = mcParts->begin() ;
                                  mcParts->end()!=iter ; ++iter ){
    SmartRef<LHCb::MCParticle> part=*iter;
    if ( !(m_gammaID == part->particleID()) )continue; // select MC-gamma
    if(part->momentum().pz() < 0.)continue;         // Pz Acceptance 
    if( part->momentum().pt() < m_etmin )continue;  // Et acceptance
    // Origin vertex
    SmartRef<LHCb::MCVertex> vertex=part->originVertex();
    if ( 0 == vertex )continue;
    if ( fabs(vertex->position().Z() ) > m_dz)continue;
    if ( vertex->position().Rho() > m_dr ) continue; // origin vertex acceptance
    // Ecal acceptance
    Line line( vertex->position()  , part->momentum().Vect() );
    Gaudi::XYZPoint cross(0.,0.,0.);
    double  mu = 0;
    Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line, m_ecalPlane ,cross,mu);
    const Gaudi::XYZPoint hit = cross;
    debug() << "MC part momentum " << part->momentum() 
            << " crosses Ecal Plane at point "<< cross << " -> cellID : " << m_ecal->Cell( hit ) << endreq;
    if( !m_ecal->valid( m_ecal->Cell( hit ) ) )continue; // Ecal acceptance.


    // Conversion 
    Gaudi::XYZPoint decay(0.,0.,1.*Gaudi::Units::km);
    SmartRefVector<LHCb::MCVertex> decays=part->endVertices();
    for(SmartRefVector<LHCb::MCVertex>::const_iterator iver=decays.begin(); decays.end()!=iver;++iver){
      if( (*iver)->position().z() < decay.Z() )decay = (*iver)->position();
    }
    debug() << "MC gamma endVertex.z() " << decay.Z() << endreq;
    

    

    // belong to a merged pi0 ?
    bool isMerged=false;
    SmartRef<LHCb::MCParticle> mother=part->mother();
    if (0!=mother){
      if ( m_pi0ID == mother->particleID() ){
        SmartRef<LHCb::MCVertex> decayPi0=part->originVertex();
        SmartRefVector<LHCb::MCParticle> products=decayPi0->products();
        if (2!=products.size()) {continue;}
        for (SmartRefVector<LHCb::MCParticle>::const_iterator pIter=products.begin();
             products.end()!=pIter;++pIter){
          SmartRef<LHCb::MCParticle> pi0daughter=*pIter;
          if ( part == pi0daughter ) {continue;}

          Line line2( decayPi0->position()  , pi0daughter->momentum().Vect() );
          Gaudi::XYZPoint cross2;
          double mu;
          Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line2, m_ecalPlane,cross2,mu);
          double distance = (cross - cross2).R();
          const Gaudi::XYZPoint hit1(cross);
          const Gaudi::XYZPoint hit2(cross2);          
          double param=m_mergedDist*(m_ecal->cellSize(m_ecal->Cell( hit1 ))+
                                     m_ecal->cellSize(m_ecal->Cell(hit2)))/2.;
          if (distance<param) {
            isMerged=true;
            debug() <<"Pi0->Merged Photon :  distance="<<distance
                 <<"  < Criteria="<<param<<" mm"<<endreq;
          }
        }
      }
    }

    if (isMerged) {
      debug() <<"Merged Pi0 photons removed from Signal sample"<<endreq;
      continue;
    }
    m_nMCPhotons++;

    int ibin=int(m_nbinpt*( part->momentum().pt() - m_ptmin)/(m_ptmax-m_ptmin));
    if (ibin>=0 && ibin<m_nbinpt){
      // efficiency / purity versus pt
      m_mc_g[ibin]++;
      // efficiency / purity versus likelihood
      m_lh_mcg++;
      if ( decay.Z() > m_zConv) {
        m_lh_mcg_noconv++;
        debug() << " Not converted " << m_zConv << endreq;
      }
      else {
        m_lh_mcg_conv++;
        debug() << " converted " << m_zConv << endreq;
      }
    }
  }
  debug() << " MC part all/no-conv/conv" << m_lh_mcg << "/"<<m_lh_mcg_noconv<<"/"<<m_lh_mcg_conv<< endreq;
  


  // get input hypos
  //=========================================================================

  const Hypos *hypos = get<Hypos>( inputData() );
  if( 0 == hypos ) {
    debug() <<"No input Data: "<<inputData()<<endreq;
    return StatusCode::FAILURE ;
  }

// get the relation table
  MCTable *gtable = get<MCTable> ( m_MCTableName );
  if ( 0 == gtable ) info() << "No Relation Table" << endmsg;

  // loop over hypos
  for(Hypos::const_iterator iter=hypos->begin(); hypos->end()!=iter;++iter){
    // skip nulls
     if( 0 == *iter ) {
       debug() <<"empty CaloHypo : skipping"<<endreq;
      continue ;
     }
     
     LHCb::CaloHypo    *hypo = *iter;
     if ( 0 == hypo ) return Exception( "*CaloHypo* points to NULL" );
     LHCb::CaloMomentum momentum( hypo );

     m_nCandidats++;

// Transverse Momentum
     debug() << "==> Processing new CaloHypo : Et=" << momentum.momentum().pt() << endmsg;
     if ( momentum.momentum().pt() < m_etmin ) continue;

     if (hypo->clusters().size()!=1){
       warning() <<"Photon Hypothesis : number of clusters!=1 ..."<<endreq;
       continue;
     }
     
     const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();
     if ( 0 == cluster )
       { continue; return Exception( " *CaloCluster* points to NULL " ); }

     std::vector<LHCb::CaloClusterEntry> entries = cluster->entries();
     if( cluster->entries().empty() )
     { return Exception( " *CaloCluster* empty " );}
     
     LHCb::CaloCluster::Entries::const_iterator iseed =
       LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                           cluster->entries().end  () ,
                                           LHCb::CaloDigitStatus::SeedCell  ) ;
     if( iseed == cluster->entries().end() )
     { return Exception( " *SeedCell* not found ");}
     
     const LHCb::CaloDigit* seed = iseed->digit();
     if( 0 == seed )
     { return Exception( " SeedCell *Digit* points to NULL! ");}

     
// Energy
     double energy = momentum.momentum().e();
     debug() << "- Energy [MeV]=" << energy << endmsg;

     /// Chi2
     double chi2;
     const Table* table = get<Table> ( m_TrTableName );
     if( 0 == table     ) { return Error("Table* points to NULL!");} // RETURN

     const Range range = table->relations( cluster ) ;
     if( range.empty() )  { chi2=1.e+6; }            // bad match -> large value !
     else {
       chi2 = range.front().weight();
     }
     debug() << " - Chi2        ="<<chi2<<endreq;
     // Shower Shape
     double shape=hypo->position()->spread()(1,1)+hypo->position()->spread()(2,2);
     debug()<<" - Shower Shape="<<shape<<endreq;
     //Spd hit and Prs deposit


     Line line( Gaudi::XYZPoint(0.,0.,0.) , momentum.momentum().Vect() );
     Gaudi::XYZPoint spdHit,prsHit;
     double mu;
     Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line, m_spdPlane , spdHit,mu);
     Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line, m_prsPlane , prsHit,mu);
     const Gaudi::XYZPoint spdPoint=spdHit;
     const Gaudi::XYZPoint prsPoint=prsHit;
     const LHCb::CaloCellID cellSpd = m_spd->Cell( spdPoint );
     const LHCb::CaloCellID cellPrs = m_prs->Cell( prsPoint );
     
     double eSpd=0.;
     double ePrs=0.;

     // Get CaloCell Deposits in the SPD and PRS
     if( !(LHCb::CaloCellID() == cellSpd) ){
       for (SmartRefVector<LHCb::CaloDigit>::const_iterator
              digit=hypo->digits().begin() ;
            digit != hypo->digits().end() ; digit++ ) {
        if ( (*digit)->cellID() == cellSpd ) {
	   			eSpd=(*digit)->e();
        }
       }
       debug() <<" - SPD "<<cellSpd<<" Energy  ="<<eSpd<<endreq;
     }

     if( !(LHCb::CaloCellID() == cellPrs) ){
       for (SmartRefVector<LHCb::CaloDigit>::const_iterator
              digit=hypo->digits().begin() ;
            digit != hypo->digits().end() ; digit++ ) {
        if ( (*digit)->cellID() == cellPrs ) {
          ePrs=(*digit)->e();
        }
       }
       debug()<<" - PRS "<<cellPrs<<" Energy  ="<<ePrs<<endreq;
     }

     // ***
     double likelihood=(*m_hypotool)(hypo);
     debug()<< "Likelihood = "<<likelihood<<endreq;
     
     if(likelihood<0.){m_nWrongs++;}
     
     // MCTruth Information
     MCRange grange = gtable->relations(cluster);     
     Gaudi::XYZPoint decay(0.,0.,1.*Gaudi::Units::km);

     double wmax=-1.e6;
     double dr=1.e+6;
     double dz=1.e+6;
     double de=1.e+6;
     bool isSignal=false;
     bool isPhoton=false;
     bool isMerged=false;

     for( MCTable::iterator mc = grange.begin() ; grange.end() != mc ; ++mc ) {
     	 if( 0 == mc->to() )  continue ; 
	     debug()<<"mctruth : --> pid="<<mc->to()->particleID().pid() <<" weight="<<mc->weight()<<endreq;
       if( !(m_gammaID == mc->to()->particleID())  )continue;
	     if( mc->weight()<wmax ) continue;
       wmax=mc->weight();
       SmartRef<LHCb::MCParticle> mcpart=mc->to();

       debug()<< "A MC-gamma matches the hypo "  << endreq;       
       debug()<< " Energy :  "  <<mcpart->momentum().e() << endreq;       

       isPhoton=true;
       SmartRef<LHCb::MCVertex> vertex=mcpart->originVertex();
       if ( 0==vertex ) {
         warning() <<"MC-gamma has no origin vertex !"<<endreq;
         continue;
       }
           
       // selection
       dr=vertex->position().Rho();
       dz=vertex->position().z();
       de=fabs(energy - mcpart->momentum().e())/energy;
       debug() <<"Gamma parameters : dr="<<dr<<" - dz="<<dz<<" - de="<<de<<endreq;

       SmartRefVector<LHCb::MCVertex> decays=mcpart->endVertices();
       for(SmartRefVector<LHCb::MCVertex>::const_iterator viter=decays.begin();decays.end()!=viter;++viter){
         if((*viter)->position().z() < decay.Z())decay=(*viter)->position();             
       }
       
       isMerged=false;       
       // belong to a merged pi0 ?
       SmartRef<LHCb::MCParticle> mother=mcpart->mother();
       if (0!=mother && isPhoton){
         if ( m_pi0ID == mother->particleID() ){
           SmartRef<LHCb::MCVertex> decayPi0=mcpart->originVertex();
           SmartRefVector<LHCb::MCParticle> p=decayPi0->products();
           if (2!=p.size()) {continue;}
           for (SmartRefVector<LHCb::MCParticle>::const_iterator pIter=p.begin();
                p.end()!=pIter;++pIter){
             SmartRef<LHCb::MCParticle> pi0daughter=*pIter;
             if ( mcpart == pi0daughter ) {continue;}
             Line line1( decayPi0->position()  , mcpart->momentum().Vect() );
             Line line2( decayPi0->position()  , pi0daughter->momentum().Vect() );
             Gaudi::XYZPoint cross1;
             Gaudi::XYZPoint cross2;
             double mu;
             Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line1, m_ecalPlane,cross1,mu);
             Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line2, m_ecalPlane,cross2,mu);
             double distance = (cross1 - cross2).R();
             const Gaudi::XYZPoint hit1=cross1;
             const Gaudi::XYZPoint hit2=cross2;
             double param=m_mergedDist*(m_ecal->cellSize(m_ecal->Cell( hit1 ))+
                                        m_ecal->cellSize(m_ecal->Cell( hit2)))/2.;
             if (distance<param) {
               isMerged=true;
               debug() <<"Pi0->Merged Photon :  distance="<<distance
                       <<"  < Criteria="<<param<<" mm"<<endreq;
             }
           }
         }
       }       
     }
     

     if ( de < m_de && dr < m_dr && dz < m_dz && isPhoton && !isMerged){
       m_nPhotons++;
       isSignal=true;
       debug() <<"Candidate is Signal according to MC"<<endreq;
     }else{
       debug() <<"Candidate is Background according to MC (photon, merged)"<< isPhoton << " " << isMerged << endreq;
     }
     

     // Efficiency and purity versus pt
     if (likelihood>m_lhcut){
       int ibin=int(m_nbinpt*(momentum.momentum().pt()-m_ptmin)/(m_ptmax-m_ptmin));
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
         if (decay.Z()>m_zConv) {m_lh_recsig_noconv[l]++;}
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
         hFill1("2",likelihood);
       }
       else{
         hFill1("1",likelihood);
       }
     }
     else{
       if (eSpd>1.){
         hFill1("4",likelihood);
       }
       else{
         hFill1("3",likelihood);
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
  
  for (int l=0;l<m_nbinlh;l++){
    
    debug() << "Rec Signal no-conv/conv" << l << " : " 
            << m_lh_recsig_noconv[l] << "  / "<< m_lh_recsig_conv[l] <<" / "
            << m_lh_recsig_nospd[l] << " / "<< m_lh_recsig_spd[l] << " / "
            << m_lh_recbkg_nospd[l] << " / "<< m_lh_recbkg_spd[l] << endreq;
  }
  

  return StatusCode::SUCCESS;
}

// ============================================================================

// ============================================================================

std::vector<AIDA::IHistogram1D*> CaloPhotonChecker::defHisto1d(
	const unsigned int bin, const double xmin, const double xmax,
  const unsigned int nhisto, std::string hname, const std::string dir,
  const unsigned int n){


  std::vector<AIDA::IHistogram1D*> histoList;
	char histoname[60];

  for (unsigned int i=0; i<n; ++i){
		AIDA::IHistogram1D* histo;
    sprintf(histoname,"%s #%i",hname.c_str(),nhisto+i);
    std::stringstream hid("");
    hid << nhisto+i ;
    histo  = hBook1( dir + hid.str() , histoname ,  int(xmin), int(xmax),bin );

		debug() <<"booking Histo ..."<<histoname<<endreq; 

    if( 0 == histo  ) {
			info()<<"Histogram "<<histoname<<" not booked "<<endreq;
      info()<<"Directory : "<<dir<<endreq;
      info()<<"id        : "<<nhisto+i<<endreq;
      info()<<"Title     : "<<histoname<<endreq;
      info()<<"bin-low/up: "<<bin<<"-"
            <<xmin<<"/"<<xmax<<endreq;
    }
		histoList.push_back(histo);
   }
	return histoList;
}


