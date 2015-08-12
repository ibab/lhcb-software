// Include files
// STD & STL
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// From PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
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

DECLARE_ALGORITHM_FACTORY( CaloPhotonChecker )

// ============================================================================
/** @file
 *
 *  Implementation file for class CaloPhotonChecker
 *  Photon Selection Monitoring
 *  (LHCb 2004-03)
 *
 *  @author Frederic Machefert frederic.machefert@in2p3.fr
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
  , m_nCandidates ( 0 )
  , m_nPhotons  ( 0 )
  , m_nMCPhotons( 0 )
  , m_nWrongs   ( 0 )

  , m_ecal(NULL)
  , m_spd(NULL)
  , m_prs(NULL)

  , m_IDTableName   ( LHCb::CaloIdLocation::PhotonID )
  , m_TrTableName   ( LHCb::CaloIdLocation::ClusterMatch )
  , m_MCTableName   ("Relations/" + LHCb::CaloClusterLocation::Default )

  , m_gammaName      ( "gamma"    )
  , m_gammaID        ( 0          )
  , m_pi0Name        ( "pi0"      )
  , m_pi0ID          ( 0          )

  , m_pdf           (false)
  , m_prsbin        (    )
  , m_chi2bin       (    )
  , m_seedbin       (    )

  , m_etmin         (200.*Gaudi::Units::MeV)
//  , m_dr            (10. *Gaudi::Units::mm )
//  , m_dz            (150.*Gaudi::Units::mm )
  , m_dr            (-1.) // cut was removed
  , m_dz            (-1.) // cut was removed
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
  declareProperty ( "CC2TrTableName", m_TrTableName );
  declareProperty ( "CC2MCPTableName", m_MCTableName);

  declareProperty ( "Pdf"           , m_pdf);
  declareProperty ( "EPrsBin"       , m_prsbin);
  declareProperty ( "Chi2Bin"       , m_chi2bin);
  declareProperty ( "SeedBin"       , m_seedbin);

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
}
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
{ StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm

//----- locate particle property service
  LHCb::IParticlePropertySvc* ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
  if( 0 == ppSvc ) return Error("Could not locate LHCb::ParticlePropertySvc!");

  const LHCb::ParticleProperty* ppg = ppSvc->find( m_gammaName );
  if( 0 == ppg ) {
    error() << "Could not locate particle ' " <<m_gammaName << " '" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gammaID = ppg->pid() ;
  
  const LHCb::ParticleProperty* ppp = ppSvc->find( m_pi0Name );
  if( 0 == ppp ) {
    error() << "Could not locate particle ' " << m_pi0Name << " '" << endmsg;
    return StatusCode::FAILURE;
  }
  m_pi0ID   = ppp->pid() ;
  
  info()<<"Photon/Pi0 particle properties locatlized."<<endmsg;

//----- Detector recovery

  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_spd  = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Spd );
  m_prs  = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Prs );
  if( !m_spd || !m_prs)return Error("DeCalorimeter does not exist for Prs/Spd");
  
  m_ecalPlane = m_ecal->plane( CaloPlane::ShowerMax );
  m_spdPlane  = m_spd->plane ( CaloPlane::Middle );
  m_prsPlane  = m_prs->plane ( CaloPlane::Middle );
  
  Gaudi::Plane3D spdFront = m_spd->plane( CaloPlane::Front );
  Gaudi::XYZVector normal = spdFront.Normal();
  m_zConv = - spdFront.HesseDistance() /normal.Z(); 
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() <<  "z conversion definition is set to SPD front : " << m_zConv << endmsg;
  
//----- Tool recovery


//----- Check data consistency
  if (m_prsbin.size()!=3)  { return Error("Wrong Binning Parameter (EPrs)"); }
  if (m_chi2bin.size()!=3) { return Error("Wrong Binning Parameter (Chi2Tk)"); }
  if (m_seedbin.size()!=3) { return Error("Wrong Binning Parameter (ESeed)"); }

  // Reset Monitoring parameter
  for (int i=0 ; i<m_nbinpt ; ++i){
    m_mc_g.push_back(0);
    m_rec_bkg.push_back(0);
    m_rec_sig.push_back(0);
  }

  m_lh_mcg=0;
  m_lh_mcg_conv=0;
  m_lh_mcg_noconv=0;
  for (int i=0 ; i<m_nbinlh ; ++i){
    m_lh_recsig.push_back(0);
    m_lh_recbkg.push_back(0);
    m_lh_recsig_conv.push_back(0);
    m_lh_recsig_spd.push_back(0);
    m_lh_recbkg_spd.push_back(0);
    m_lh_recsig_noconv.push_back(0);
    m_lh_recsig_nospd.push_back(0);
    m_lh_recbkg_nospd.push_back(0);
  }

// Likelihood Output for Signal / Background
  m_lhSig    = book1D( "Lh_Sig_noSpd","likelihood Signal - no SPD"     ,0.,1.,50);
  m_lhSigSpd = book1D( "Lh_Sig_Spd"  ,"likelihood Signal - SPD hit"    ,0.,1.,50);
  m_lhBkg    = book1D( "Lh_Bkg_noSpd","likelihood Background - no SPD" ,0.,1.,50);
  m_lhBkgSpd = book1D( "Lh_Bkg_Spd"  ,"likelihood Background - SPD hit",0.,1.,50);

  // Efficiency / Purity versus Pt
  m_efficiency = book1D( "Efficiency" ,"Photon Selection Efficiency vs Pt",m_ptmin,m_ptmax,m_nbinpt);
  m_purity     = book1D( "Purity" ,"Photon Selection Purity vs Pt",m_ptmin,m_ptmax,m_nbinpt);
  
  m_effpur       = book2D( "Eff_Pur", "Efficiency vs Purity - Lh cut",0.,1.,100,0.,1.,100);
  m_effpur_spd   = book2D( "Eff_Pur_Spd", "Efficiency vs Purity - Lh cut - no Conv. sample",0.,1.,100,0.,1.,100);
  m_effpur_nospd = book2D( "Eff_Pur_noSpd", "Efficiency vs Purity - Lh cut - Conv. sample",0.,1.,100,0.,1.,100);


  // Probability Density Functions Definitions
  if (m_pdf){
    m_signalEPrs2D=defHisto(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                           10, std::string("Signal_Prs_noSpdHit"));
    m_signalChi22D=defHisto(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                           20, std::string("Signal_Chi2Tk_noSpdHit"));
    m_signalSeed2D=defHisto(int(m_seedbin[ 0 ]),m_seedbin[ 1 ],m_seedbin[ 2 ],
                            30, std::string("Signal_ESeed_noSpdHit"));

    m_signalEPrsSpd2D=defHisto(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                              15, std::string("Signal_Prs_SpdHit"));
    m_signalChi2Spd2D=defHisto(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                              25, std::string("Signal_Chi2Tk_SpdHit"));
    m_signalSeedSpd2D=defHisto(int(m_seedbin[ 0 ]),m_seedbin[ 1 ],m_seedbin[ 2 ],
                              35, std::string("Signal_ESeed_SpdHit"));

    m_backgrEPrs2D=defHisto(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                           110, std::string("Background_Prs_noSpdHit"));
    m_backgrChi22D=defHisto(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                           120, std::string("Background_Chi2Tk_noSpdHit"));
    m_backgrSeed2D=defHisto(int(m_seedbin[ 0 ]),m_seedbin[ 1 ],m_seedbin[ 2 ],
                            130, std::string("Background ESeed_noSpdHit"));

    m_backgrEPrsSpd2D=defHisto(int(m_prsbin[ 0 ]),m_prsbin[ 1 ],m_prsbin[ 2 ],
                              115, std::string("Background_Prs_SpdHit"));
    m_backgrChi2Spd2D=defHisto(int(m_chi2bin[ 0 ]),m_chi2bin[ 1 ],m_chi2bin[ 2 ],
                              125, std::string("Background_Chi2Tk_SpdHit"));
    m_backgrSeedSpd2D=defHisto(int(m_seedbin[ 0 ]),m_seedbin[ 1 ],m_seedbin[ 2 ],
                              135, std::string("Background_ESeed_SpdHit"));
   }   
   
  if( m_split ){
    Warning( "No area spliting allowed for CaloPhotonChecker").ignore();
    m_split = false;
  }    
  return StatusCode::SUCCESS;
}
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
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;


  info() << "************* Photon Monitoring *****************"<<endmsg;
  info() << "Number of Events Analyzed : " << m_nEvents << endmsg;
  info()
    //<< "MCPhotons (Signal def.) in sample: "<<m_nMCPhotons<<" - "
    <<m_nPhotons<<" Photons (Signal def.) out of "<<m_nCandidates<<" hypos processed."
    <<" - "<<m_nWrongs<<" hypos rejected."<<endmsg;
  char line[70];
  info() << endmsg;
  info() << "     Et(GeV)        | Efficiency |   Purity   "<<endmsg;
  info() << "----------------------------------------------"<<endmsg;
  for (int i=0; i<m_nbinpt ; ++i){
    double pt= m_ptmin+double(i)*(m_ptmax-m_ptmin)/double(m_nbinpt);
    double eff=(m_mc_g[i]>0) ? double(m_rec_sig[i])/double(m_mc_g[i]) : 0.;
    double pur=(m_rec_sig[i]+m_rec_bkg[i]>0) ? m_rec_sig[i]/(m_rec_sig[i]+m_rec_bkg[i]) : 0. ;
    sprintf(line," [ %5.2f - %5.2f ]  |    %4.2f    |    %4.2f    ",
      pt/1000.,(pt+(m_ptmax-m_ptmin)/double(m_nbinpt))/1000.,eff,pur);

    info() << line <<endmsg;

    fill(m_efficiency,pt,eff);
    fill(m_purity,pt,pur);
  }
  info() << endmsg;
  info() << "  L>= |   Total   |   No Conv |    Conv   "<<endmsg;
  info() << "      | Eff   Pur | Eff   Pur | Eff   Pur "<<endmsg;
  info() << " -----------------------------------------"<<endmsg;
  for (int i=0; i<m_nbinlh ; ++i){
    double eff=(m_lh_mcg>0) ? m_lh_recsig[i]/m_lh_mcg : 0.;
    double effnoconv=(m_lh_mcg_noconv>0) ? m_lh_recsig_noconv[i]/m_lh_mcg_noconv : 0.;
    double effconv=(m_lh_mcg_conv>0) ? m_lh_recsig_conv[i]/m_lh_mcg_conv : 0.;
    double pur=(m_lh_recsig[i]+m_lh_recbkg[i]>0) ? m_lh_recsig[i]/(m_lh_recsig[i]+m_lh_recbkg[i]) : 0. ;
    double purnoconv = 
      (m_lh_recsig_nospd[i]+m_lh_recbkg_nospd[i]>0) ? m_lh_recsig_nospd[i]/(m_lh_recsig_nospd[i]+m_lh_recbkg_nospd[i]) : 0. ;
    double purconv=(m_lh_recsig_spd[i]+m_lh_recbkg_spd[i]>0) ?m_lh_recsig_spd[i]/(m_lh_recsig_spd[i]+m_lh_recbkg_spd[i]) : 0. ;

    fill(m_effpur,pur,eff,1.);
    fill(m_effpur_nospd,purnoconv,effnoconv,1.);
    fill(m_effpur_spd,purconv,effconv,1.);

    sprintf(line," %3.2f | %3.2f %3.2f | %3.2f %3.2f | %3.2f %3.2f", 
            double(i)/double(m_nbinlh),eff,pur,effnoconv,purnoconv,effconv,purconv);

    info() << line <<endmsg;

  }
  info() << "*************************************************"<<endmsg;


  return CaloMoniAlg::finalize() ;
}
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
{ 
  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float>  MCTable;
  typedef MCTable::Range                                               MCRange;
  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::Track,float>       Table;
  typedef Table::Range                                                 Range;

  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;

  
  typedef const LHCb::CaloHypo::Container   Hypos;
  typedef const LHCb::MCParticle::Container MCParticles;
  
  // increment number of events
  m_nEvents++;
 
  // get MC photons
  if( !exist<LHCb::MCParticles>(LHCb::MCParticleLocation::Default))
    return Warning("NO MC information",StatusCode::SUCCESS);  
  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<< "MC Particles extracted from event : " << mcParts->size() << endmsg;
  
  for(MCParticles::const_iterator iter = mcParts->begin(); mcParts->end()!=iter; ++iter){
    SmartRef<LHCb::MCParticle> part=*iter;
    if ( !(m_gammaID == part->particleID()) ) continue; // select MC-gamma
    if ( part->momentum().pz() < 0.)          continue; // Pz Acceptance 
    if ( part->momentum().pt() < m_etmin )    continue; // Et acceptance
    // Origin vertex
    SmartRef<LHCb::MCVertex> vertex=part->originVertex();
    if ( 0 == vertex )                         continue; // ask for a vertex
    if ((m_dz>0) && (fabs(vertex->position().Z())>m_dz)) continue; // origin vertex acceptance ... in z
    if ((m_dr>0) && (vertex->position().Rho()>m_dr) )    continue; // ... and in (x,y)
    // Ecal acceptance
    Line line( vertex->position()  , part->momentum().Vect() );
    Gaudi::XYZPoint cross(0.,0.,0.);
    double  mu = 0;
    Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line, m_ecalPlane ,cross,mu);
    const Gaudi::XYZPoint hit = cross;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "MC part momentum " << part->momentum() 
              << " crosses Ecal Plane at point "<< cross << " -> cellID : " << m_ecal->Cell( hit ) << endmsg;
    if( !m_ecal->valid( m_ecal->Cell( hit ) ) )continue; // Ecal acceptance.

    // Conversion 
    Gaudi::XYZPoint decay(0.,0.,1.*Gaudi::Units::km);
    SmartRefVector<LHCb::MCVertex> decays=part->endVertices();
    for(SmartRefVector<LHCb::MCVertex>::const_iterator iver=decays.begin(); decays.end()!=iver;++iver){
      if( (*iver)->position().z() < decay.Z() ) decay = (*iver)->position();
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "MC gamma endVertex.z() " << decay.Z() << endmsg;

    // belong to a merged pi0 ?
    bool isMerged=false;
    SmartRef<LHCb::MCParticle> mother=part->mother();
    if (0!=mother){
      if ( m_pi0ID == mother->particleID() ){
        SmartRef<LHCb::MCVertex>         decayPi0=part->originVertex();
        SmartRefVector<LHCb::MCParticle> products=decayPi0->products();
        if (2!=products.size())          continue; // more than two decay particles
        for (SmartRefVector<LHCb::MCParticle>::const_iterator pIter=products.begin();
             products.end()!=pIter;++pIter){
          SmartRef<LHCb::MCParticle> pi0daughter=*pIter;
          if ( part == pi0daughter ) continue; // we want to have a look at the othe MC photon
          Line line2( decayPi0->position()  , pi0daughter->momentum().Vect() );
          Gaudi::XYZPoint cross2;
          double mu;
          Gaudi::Math::intersection<Line,Gaudi::Plane3D,Gaudi::XYZPoint>(line2, m_ecalPlane,cross2,mu);
          double distance = (cross - cross2).R();
//           const Gaudi::XYZPoint hit1(cross);
          const Gaudi::XYZPoint hit2(cross2);          
//           double param=m_mergedDist*(m_ecal->cellSize(m_ecal->Cell( hit1 ))+
//                                      m_ecal->cellSize(m_ecal->Cell( hit2 )))/2.;
          double param=m_mergedDist*(m_ecal->cellSize(m_ecal->Cell( hit ))+
                                     m_ecal->cellSize(m_ecal->Cell( hit2 )))/2.;
          if (distance<param) {
            isMerged=true;
            if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
              debug() <<"Pi0->Merged Photon :  distance="<<distance
                      <<"  < Criteria="<<param<<" mm"<<endmsg;
          }
        }
      }
    }

    if (isMerged) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() <<"Merged Pi0 photons removed from Signal sample"<<endmsg;
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
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << " Not converted " << m_zConv << endmsg;
      }
      else {
        m_lh_mcg_conv++;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << " converted " << m_zConv << endmsg;
      }
    }
  }
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " MC part all/no-conv/conv" << m_lh_mcg << "/"<<m_lh_mcg_noconv<<"/"<<m_lh_mcg_conv<< endmsg;
  


  // get input hypos
  //=========================================================================

  const Hypos *hypos = get<Hypos>( inputData() );
  if( 0 == hypos ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() <<"No input Data: "<<inputData()<<endmsg;
    return StatusCode::FAILURE ;
  }

  LHCb::Calo2Track::IHypoEvalTable* idTable = NULL;
  if ( exist<LHCb::Calo2Track::IHypoEvalTable>( m_IDTableName ) )   
    idTable = get<LHCb::Calo2Track::IHypoEvalTable> ( m_IDTableName ) ;
  
  // get the relation tables
  MCTable *gtable = get<MCTable> ( m_MCTableName );
  if ( 0 == gtable ) return Error( "No MC Relation Table");
  
  const Table* table = get<Table> ( m_TrTableName );
  if( 0 == table)return Error("Table* points to NULL!");

  // loop over hypos
  for(Hypos::const_iterator iter=hypos->begin(); hypos->end()!=iter;++iter){
    // skip nulls
     if( 0 == *iter ) {
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug() <<"empty CaloHypo : skipping"<<endmsg;
      continue ;
     }
     
     LHCb::CaloHypo    *hypo = *iter;
     if ( 0 == hypo ){Warning( "*CaloHypo* points to NULL").ignore();continue;}
     LHCb::CaloMomentum momentum( hypo );
     m_nCandidates++;

// Transverse Momentum
     if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
       debug() << "==> Processing new CaloHypo : Et=" << momentum.momentum().pt() << endmsg;
     if ( momentum.momentum().pt() < m_etmin ) continue;

     if (hypo->clusters().size()!=1){
       warning() <<"Photon Hypothesis : number of clusters!=1 ..."<<endmsg;
       continue;
     }
     
     const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();
     if ( 0 == cluster )
     { Warning( " *CaloCluster* points to NULL ").ignore();continue; }

     std::vector<LHCb::CaloClusterEntry> entries = cluster->entries();
     if( cluster->entries().empty() ){Warning( " *CaloCluster* empty ").ignore();continue;}
     
     LHCb::CaloCluster::Entries::const_iterator iseed =
       LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                           cluster->entries().end  () ,
                                           LHCb::CaloDigitStatus::SeedCell  ) ;
     if( iseed == cluster->entries().end() ){Warning(" *SeedCell* not found ").ignore();continue;}
     
     const LHCb::CaloDigit* seed = iseed->digit();
     if( 0 == seed ){Warning( " SeedCell *Digit* points to NULL! ").ignore();continue;}

     // seed cell area
     m_area = seed->cellID().area() ;
     
    // Energy
     double energy = momentum.momentum().e();
     if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
       debug() << "- Energy [MeV]=" << energy << endmsg;

     // Chi2
     double chi2;

     const Range range = table->relations( cluster ) ;
     if( range.empty() )  { chi2=1.e+6; }            // bad match -> large value !
     else {
       chi2 = range.front().weight();
     }
     if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
       debug() << " - Chi2        ="<<chi2<<endmsg;
     
     // Cell seed
     double eSeed = energy>0.?(seed->e())/energy:-1.;
     if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
       debug()<<" - Seed Energy ="<<eSeed<<endmsg;
     
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
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug() <<" - SPD "<<cellSpd<<" Energy  ="<<eSpd<<endmsg;
     }

     if( !(LHCb::CaloCellID() == cellPrs) ){
       for (SmartRefVector<LHCb::CaloDigit>::const_iterator
              digit=hypo->digits().begin() ;
            digit != hypo->digits().end() ; digit++ ) {
        if ( (*digit)->cellID() == cellPrs ) {
          ePrs=(*digit)->e();
        }
       }
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug()<<" - PRS "<<cellPrs<<" Energy  ="<<ePrs<<endmsg;
     }

     // ***
     double likelihood = -1.;
     if( NULL != idTable ){
       const LHCb::Calo2Track::IHypoEvalTable::Range idRange = idTable->relations( hypo ) ;
       if ( !idRange.empty() )likelihood = idRange.front().to();
     }
     
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
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug()<<"mctruth : --> pid="<<mc->to()->particleID().pid() <<" weight="<<mc->weight()<<endmsg;
       if( !(m_gammaID == mc->to()->particleID())  )continue;
	     if( mc->weight()<wmax ) continue;
       wmax=mc->weight();
       SmartRef<LHCb::MCParticle> mcpart=mc->to();

       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
         debug()<< "A MC-gamma matches the hypo "  << endmsg;       
         debug()<< " Energy :  "  <<mcpart->momentum().e() << endmsg;       
       }
       
       isPhoton=true;
       SmartRef<LHCb::MCVertex> vertex=mcpart->originVertex();
       if ( 0==vertex ) {
         warning() <<"MC-gamma has no origin vertex !"<<endmsg;
         continue;
       }
           
       // selection
       dr=vertex->position().Rho();
       dz=vertex->position().z();
       de=fabs(energy - mcpart->momentum().e())/energy;
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug() <<"Gamma parameters : dr="<<dr<<" - dz="<<dz<<" - de="<<de<<endmsg;

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
               if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
                 debug() <<"Pi0->Merged Photon :  distance="<<distance
                         <<"  < Criteria="<<param<<" mm"<<endmsg;
             }
           }
         }
       }       
     }
     

     if ( de < m_de && ((m_dr<0)||(dr<m_dr)) && ((m_dz<0)||(dz<m_dz)) && isPhoton && !isMerged){
       m_nPhotons++;
       isSignal=true;
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug() <<"Candidate is Signal according to MC"<<endmsg;
     }else{
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
         debug() <<"Candidate is Background according to MC (photon, merged)"<< isPhoton << " " << isMerged << endmsg;
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
         fill(m_lhSigSpd,likelihood,1.);
       }
       else{
         fill(m_lhSig,likelihood,1.);
       }
     }
     else{
       if (eSpd>1.){
         fill(m_lhBkgSpd,likelihood,1.);
       }
       else{
         fill(m_lhBkg,likelihood,1.);
       }
     }

     if (m_pdf) {
      double eTransf = transform ( energy );
      if (isSignal) {
	if (eSpd>1.){
	  fill(m_signalEPrsSpd2D [ m_area ], ePrs , eTransf, 1.);
	  fill(m_signalChi2Spd2D [ m_area ], chi2 , eTransf,1.);
	  fill(m_signalSeedSpd2D [ m_area ], eSeed, eTransf,1.);
	}
	else {
//	  info()<<"histo fill at "<<ePrs<<" "<<eTransf<<endmsg;
	  fill(m_signalEPrs2D [ m_area ], ePrs , eTransf,1.);
	  fill(m_signalChi22D [ m_area ], chi2 , eTransf,1.);
	  fill(m_signalSeed2D [ m_area ], eSeed, eTransf,1.);
	}
      }
      else {
	if (eSpd>1.){
	  fill(m_backgrEPrsSpd2D [ m_area ], ePrs , eTransf,1.);
	  fill(m_backgrChi2Spd2D [ m_area ], chi2 , eTransf,1.);
	  fill(m_backgrSeedSpd2D [ m_area ], eSeed, eTransf,1.);
	}
	else {
	  fill(m_backgrEPrs2D [ m_area ], ePrs , eTransf,1.);
	  fill(m_backgrChi22D [ m_area ], chi2 , eTransf,1.);
	  fill(m_backgrSeed2D [ m_area ], eSeed, eTransf,1.);
	}
      }
    }
  }
  
  /*
  for (int l=0;l<m_nbinlh;l++){ 
    debug() << "Rec Signal no-conv/conv" << l << " : " 
            << m_lh_recsig_noconv[l] << "  / "<< m_lh_recsig_conv[l] <<" / "
            << m_lh_recsig_nospd[l] << " / "<< m_lh_recsig_spd[l] << " / "
            << m_lh_recbkg_nospd[l] << " / "<< m_lh_recbkg_spd[l] << endmsg;
  }
  */
  
  return StatusCode::SUCCESS;
}

// ============================================================================

// ============================================================================

std::vector<AIDA::IHistogram2D*> CaloPhotonChecker::defHisto(
	const unsigned int bin, const double xmin, const double xmax,
	const unsigned int nhisto, std::string hname){

  std::vector<AIDA::IHistogram2D*> histoList;
  char histoname[60];

  for (unsigned int area=0; area<3; ++area){
    AIDA::IHistogram2D* histo;
    sprintf(histoname,"%s_%i",hname.c_str(),nhisto+area);
    histo = book2D(histoname,histoname,
		   (int)(xmin),(int)(xmax),bin,
		   0., 6.,6);
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() <<"booking Histo ..."<<histoname<<endmsg; 
    histoList.push_back(histo);
  }
  return histoList;
}


