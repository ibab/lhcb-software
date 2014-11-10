// Include files 
#include <iostream>
#include <fstream>
#include <list>
#include <utility>

// from ROOT
#include "TMath.h"
#include "TString.h"

// from LOKI
#include "LoKi/LoKi.h"
#include "LoKi/ParticleContextCuts.h"

// from local
#include "TaggerCharmTool.h"
#include "CharmD0KpiWrapper.h"
#include "CharmD0KpipipiWrapper.h"
#include "CharmD0Kpipi0Wrapper.h"
#include "CharmDpKpipiWrapper.h"
#include "CharmD0KpiXWrapper.h"
#include "CharmD0KeXWrapper.h"
#include "CharmD0KmuXWrapper.h"
#include "CharmLambdaCPKpiWrapper.h"
//#include "CharmD0KspipiWrapper.h"
//#include "CharmDpKspiWrapper.h"
//#include "CharmDstD0KspipiWrapper.h"
//#include "CharmDpKpiXWrapper.h"
//#include "CharmDpKeXWrapper.h"
//#include "CharmDpKmuXWrapper.h"

using namespace std;
using namespace LoKi::Cuts;
using namespace LoKi::Types;
using namespace LoKi::Particles;
using namespace LHCb ;
using namespace Gaudi::Units;

using namespace CharmTaggerSpace;

//--------------------------------------------------------------------
// Implementation file for class : TaggerCharmTool
//
// Author: Riccardo Cenci, Jack Wimberley
// 2013-01-23
//--------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerCharmTool )

//====================================================================
TaggerCharmTool::TaggerCharmTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent ) :
GaudiTool ( type, name, parent ),
//  m_myDATAreader( NULL ),
  m_util(0),
  m_pLifetimeFitter(0),
  m_descend(0),
  m_dva(0)
{
  declareInterface<ITagger>(this);

  std::vector<std::string> def_CharmTagLocations;
  def_CharmTagLocations.push_back("Phys/Tag_StdD02KPi/Particles");
  def_CharmTagLocations.push_back("Phys/Tag_StdD02KPiPiPi/Particles");
  def_CharmTagLocations.push_back("Phys/Tag_StdD02KPiPi0/Particles");
  def_CharmTagLocations.push_back("Phys/Tag_StdDp2KPiPi/Particles");
  declareProperty( "CharmTagLocations", m_CharmTagLocations = def_CharmTagLocations);

  std::vector<std::string> def_CharmInclTagLocations;
  def_CharmInclTagLocations.push_back("Phys/Tag_StdD2KPipart/Particles");
  def_CharmInclTagLocations.push_back("Phys/Tag_StdD2Kepart/Particles");
  def_CharmInclTagLocations.push_back("Phys/Tag_StdD2Kmupart/Particles");
  declareProperty( "CharmInclTagLocations", m_CharmInclTagLocations = def_CharmInclTagLocations);

  std::vector<std::string> def_CharmLambdaTagLocations;
  def_CharmLambdaTagLocations.push_back("Phys/Tag_StdLambdaC2PKPi/Particles");
  declareProperty( "CharmLambdaTagLocations", m_CharmLambdaTagLocations = def_CharmLambdaTagLocations);

  declareProperty( "Charm_P0_Cal",           m_P0_Cal_charm   = 0.0);
  declareProperty( "Charm_P1_Cal",           m_P1_Cal_charm   = 1.0);
  declareProperty( "Charm_Eta_Cal",          m_Eta_Cal_charm  = 0.0);

  // initialize decay map
  //                                                                   Idx   P  K   Pi  E   Mu  D* Nat   Eta     P0      P1
  decayMap[CharmMode::Dz2kpi]          = CharmDecay("D0_Kpi",	         0,    0, 1,	1,  0,	0,	0, 0.23, 0.3797, 0.3821, 0.9789);
  decayMap[CharmMode::Dz2kpipipi]      = CharmDecay("D0_Kpipipi",	     1,    0, 1,	1,  0,	0,	0, 0.23, 0.3568, 0.3751, 0.8550);
  decayMap[CharmMode::Dz2kpipiz]       = CharmDecay("D0_Kpipi0",       3,    0, 1,	1,  0,	0,	0, 0.23, 0.0,    0.0,    1.0);
  decayMap[CharmMode::Dp2kpipi]        = CharmDecay("Dp_Kpipi",	       4,    0, 1,	1,  0,	0,	0, 0.19, 0.3866, 0.3957, 0.8864);
  decayMap[CharmMode::Dz2kpiX]         = CharmDecay("D0_KpiX",         6,    0, 1,	1,  0,	0,	0, 0.23, 0.3946, 0.3998, 0.9511);
  decayMap[CharmMode::Dz2keX]          = CharmDecay("D0_KeX",	         7,    0, 1,	0,  1,	0,	0, 0.23, 0.3511, 0.3838, 0.9048);
  decayMap[CharmMode::Dz2kmuX]         = CharmDecay("D0_KmuX",         8,    0, 1,	0,  0,	1,	0, 0.23, 0.0,    0.0,    1.0);
  decayMap[CharmMode::LambdaC2pkpi]    = CharmDecay("LambdaC_PKPi",   13,    1, 1,	1,  0,	0,	0, 0.00, 0.0,    0.0,    1.0);
  
}


TaggerCharmTool::~TaggerCharmTool() {}
//=====================================================================
StatusCode TaggerCharmTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  //
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_pLifetimeFitter = tool<ILifetimeFitter>("PropertimeFitter", this);
  if(!m_pLifetimeFitter){
    fatal() << "Unable to retrieve LifetimeFitter tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (!m_dva) return Error("Couldn't get parent DVAlgorithm",
                           StatusCode::FAILURE);

  //  m_nnet = tool<INNetTool> ( "NNetTool_MLP", this);

  //   //load tmva readers
  //   int nFoundReaders = 0;

  //   if ( msgLevel(MSG::DEBUG) )  debug() << " Number of TMVA readers found: " << nFoundReaders << endreq;

  // std::vector<std::string> variable_names;

  // variable_names.push_back("mult"                    );
  // variable_names.push_back("nnkrec"                  );
  // variable_names.push_back("ptB"                     );
  // variable_names.push_back("charm_mass"              );
  // variable_names.push_back("charm_mode"              );
  // variable_names.push_back("charm_bpvdira"           );
  // variable_names.push_back("charm_probchi2"          );
  // variable_names.push_back("charm_tau"               );
  // variable_names.push_back("charm_flightdistchi2"    );
  // variable_names.push_back("charm_kprobnnk"          );
  // variable_names.push_back("charm_kippvchi2"         );
  // variable_names.push_back("charm_maxprobnnghostdaus");

  //  m_myDATAreader = new CharmOSWrapper(variable_names);

  return sc;
}

//=====================================================================

StatusCode TaggerCharmTool::finalize()
{
  // delete m_myDATAreader; 
  // m_myDATAreader = NULL;
  for ( auto i : m_classifiers ) { delete i.second; }
  m_classifiers.clear();
  return GaudiTool::finalize();
}

//=====================================================================
Tagger TaggerCharmTool::tag( const Particle* signalB,
                             const RecVertex* RecVert,
                             const int nPV,
                             Particle::ConstVector& tagParticles )
{

  Tagger tcharm;
  if (not RecVert)
    return tcharm;

  std::vector< CharmParticle > charmcands;

  addCands(charmcands, m_CharmTagLocations    ,   *signalB, RecVert, 0);
  addCands(charmcands, m_CharmInclTagLocations,   *signalB, RecVert, 1);
  addCands(charmcands, m_CharmLambdaTagLocations, *signalB, RecVert, 3);
  
  if ( msgLevel(MSG::DEBUG) )  debug() << "Number of charm cands retrieved: "<< charmcands.size() << endreq;

  CharmParticle* thecharm = NULL;
  std::vector< CharmParticle >::iterator ipart;

  // ONLY THE BEST
  /* Eventually, Omega(BDT) will be fit for each mode separately, to MC
     The FOM for each mode to pass will be Tag(BDT)*(1-2*Omega(BDT))^2
     Then, if there are multiple candidates, pick the one with lowest Omega
     Export this Omega as the predicted Omega; this will be calibrated to data
     Until then...
     Omega *should* be correlated with Purity figure of merit
     This ignores intrinsic differences between modes to to oscillation, etc.
     Do the above with this approximation
  */
  

  double min_mistag = 1000.0; // which is omegaDBT

  int ncands = 0, icand = 0;
  for (ipart = charmcands.begin(); ipart != charmcands.end(); ++ipart) {

    icand++;

    CharmParticle *axp = &(*ipart);
    CharmMode mode = axp->mode;
    CharmDecay decay = decayMap[mode];
    
    // Compute BDT variables
    double t_bdtPurity = -10;
    double t_bdtMistag = -10;
    double t_bdtOmega = -10;

    if (true) {
      t_bdtPurity = getMvaVal(axp, nPV, tagParticles.size(), *signalB);
      Float_t purMistag = 0.5*(1.0-t_bdtPurity);
      Float_t natMistag = decay.natMistag;
      t_bdtMistag = purMistag + natMistag - 2.0*purMistag*natMistag;
      t_bdtOmega = getOmega(t_bdtMistag,mode);

      // Mode by mode calibration
      t_bdtOmega = decay.cal_p0 + decay.cal_p1 * (t_bdtOmega - decay.cal_eta);
      
      // Overall calibration
      t_bdtOmega = m_P0_Cal_charm + m_P1_Cal_charm * (t_bdtOmega - m_Eta_Cal_charm);
    }

    // SELECTION

    if (axp->tau <= -100 or
        axp->tau >= 100)
      continue;

    if (axp->bpvdira <= 0.5)
      continue;

    if (axp->maxProbGhostDaus >= 0.8)
      continue;

    if (axp->pchi2 <= 0.001)
      continue;

    ncands++;
    
    // PICK THE BEST CANDIDATE
    if (t_bdtOmega > charmOmegaBDTCut) {
      debug() << " mistag = " << t_bdtOmega << " fails cut; is < " << charmOmegaBDTCut << endl;
      continue;
    }

    if (t_bdtOmega < min_mistag) {
      debug() << "new mistag " << t_bdtOmega << " is less than old mistag " << min_mistag << "; now using mode " << mode << endl;
      thecharm = axp;
      min_mistag = t_bdtOmega; // SET TO NEW
    } else {
      debug() << "new mistag " << t_bdtOmega << " is greater than old mistag " << min_mistag << endl;
    }
  }
  
  if(!thecharm) return tcharm;

  // Calculate omega
  double omega = min_mistag;
  double sign = 1;
#ifndef TRAINING   
  if (omega < 0) return tcharm;
  if(omega>0.5){
    omega = 1-omega;
    sign = -1;
  }
#endif

  int decision = thecharm->flavour > 0 ? +1: -1; // double flip, one for oppsite side, one for D from B
  decision *= sign;
  tcharm.setOmega(omega);
  tcharm.setDecision(decision);
  tcharm.setType( Tagger::OS_Charm );

  // add charm candidate and daughters to tagger particles
  tcharm.addToTaggerParts(thecharm->part);
  const SmartRefVector<Particle>& charmDaus = thecharm->part->daughters();
  for( SmartRefVector<Particle>::const_iterator idau = charmDaus.begin(); idau != charmDaus.end(); ++idau)
    tcharm.addToTaggerParts(*idau);

  return tcharm;

}

//=====================================================================
int TaggerCharmTool::addCands(std::vector< CharmParticle >& cands, const std::vector<std::string>& locations,
                              const LHCb::Particle& signalB, const RecVertex* RecVert, const int type)
{

  if (not RecVert)
    return 0;

  for( std::vector<std::string>::const_iterator ilist = locations.begin(); ilist != locations.end(); ++ilist) {
    if (exist<Particle::Range>(*ilist)){
      LHCb::Particle::Range partsin  = get<Particle::Range>(*ilist) ;

      if ( msgLevel(MSG::DEBUG) )
        debug() << "Found "<<partsin.size()<<" charm cands for location "<<*ilist<<endreq;

      // removing candidates with daughters in common with signal B
      LHCb::Particle::ConstVector purgedParts = m_util->purgeCands(partsin, signalB);
      
      // Functors
      Fun fDIRA = DIRA(RecVert);
      Fun fVD = VD(RecVert);
      Fun fVDCHI2 = VDCHI2(RecVert);
      VFun dist = VVDCHI2(RecVert);

      // Loop
      for (Gaudi::Range_<LHCb::Particle::ConstVector>::const_iterator icand = purgedParts.begin();
           icand != purgedParts.end(); ++icand) {

        // Require same PV
        const RecVertex* CharmBestPV = (const RecVertex*) m_dva->bestVertex(*icand);
        if (not CharmBestPV) {
          err() << "No Best PV for charm candidate" << endreq;
          return StatusCode::SUCCESS;
        }
        const double vdchi2 = dist(CharmBestPV);
        if (vdchi2 >= 1) {
          continue;
        }

        ///////////////
        // CALC INFO //
        ///////////////

        // Set Charm, D*, soft pion
        const Particle *cand, *dstar_cand, *pisoft_cand;
        if (type==2) {
          dstar_cand = (*icand);
          cand = (*icand)->daughters().at(0);
          pisoft_cand = (*icand)->daughters().at(1);
        } else {
          dstar_cand = pisoft_cand = NULL;
          cand = (*icand);
        }

        // ID
        CharmMode mode = m_util->getCharmDecayMode(cand,type);
        CharmDecay decay = decayMap[mode];
        int charmId = cand->particleID().pid();
        //        int charmStarId = (dstar_cand) ? dstar_cand->particleID().pid() : 0;

        // Kinematics
        double mass = cand->measuredMass()/GeV;
        float dstarDm = (dstar_cand) ? dstar_cand->measuredMass()/GeV - mass : 0.0 ;

        // Vertex
        double vchi2 = cand->endVertex()->chi2();
        double vndof = cand->endVertex()->nDoF();
        double pchi2 = TMath::Prob(vchi2,vndof);

        // Lifetime
        double ct = 0.0, ctErr = 0.0, ctChi2 = 0.0;
        StatusCode sc = m_pLifetimeFitter->fit(*RecVert, *cand, ct, ctErr, ctChi2);
        double tau = (sc) ? ct/picosecond : -1.0;

        // Evaluate Functors
        const double bpvdira = fDIRA(cand);
        const double fd = fVD(cand);
        const double fdchi2 = fVDCHI2(cand);

        // CUTS
        if (fdchi2 <= 25)
          continue;
        
        if (bpvdira <= 0.998)
          continue;

        // Get info from daughters
        Fun fCTAU = CTAU(cand->endVertex());

        LHCb::VertexBase::ConstVector verts; verts.push_back(RecVert);
        Fun ipPvChi2 = MIPCHI2 ( verts , m_util->getDistanceCalculator() ) ;
        const MIPCHI2DV ipChi2Min  = MIPCHI2DV ( "" ) ;

        double maxProbGhostDaus = 0.0;
        double sumPT = 0.0;
        bool setProton = false, setKaon = false, setPion = false;
        double proId = 0.0, proPT = 0.0, proNNp = 1.0, proNNk = 1.0, proIppvchi2 = -1.0, proIpMinchi2 = -1.0;
        double kaonId = 0.0, kaonPT = 0.0, kaonNNp = 1.0, kaonNNk = 1.0, kaonIppvchi2 = -1.0, kaonIpMinchi2 = -1.0;
        double pionId = 0.0, pionPT = 0.0, pionNNp = 1.0, pionNNk = 1.0, pionIppvchi2 = -1.0, pionIpMinchi2 = -1.0;
        double elecNNe = 1.0, elecPT = 0.0, muonNNmu = 1.0, muonPT = 0.0;
        double elecIpMinchi2 = -1.0, muonIpMinchi2 = -1.0;
        
        bool cut = false;
    
        // double ksCtau = 0.0, lambdaCtau = 0.0;
        // double ksMass = 0.0, lambdaMass = 0.0;

        const SmartRefVector<Particle>& daus = cand->daughters();
        debug() << "Charm cand, dau vector size = " << daus.size() << endreq;
        for (SmartRefVector<Particle>::const_iterator idau = daus.begin(); idau != daus.end(); ++idau) {

          const Particle *interCand = *idau;
      
          // LAMBDA0
          std::vector<const Particle*> sublist;
          if (interCand->particleID() == LoKi::Particles::_ppFromName("Lambda0")->particleID() or
              interCand->particleID() == LoKi::Particles::_ppFromName("Lambda~0")->particleID()) {
            //        std::cout << "Examining " << interCand->particleID() << " ... " << std::endl;
            // lambdaCtau = fCTAU(interCand);
            // lambdaMass = interCand->measuredMass()/GeV;
            const SmartRefVector<Particle>& subdaus = interCand->daughters();
            for (SmartRefVector<Particle>::const_iterator iidau = subdaus.begin(); iidau != subdaus.end(); ++iidau) {
              const Particle* subdau = *iidau;
              sublist.push_back(subdau);
            }
          } else {
            sublist.push_back(interCand);
          }
            
          // LOOP OVER THIS DAUGHTER OR SUBDAUGHTERS
        
          for (std::vector<const Particle*>::const_iterator iidau = sublist.begin(); iidau != sublist.end(); ++iidau) {
        
            const Particle* daucand = *iidau;
            //        std::cout << "Examining " << daucand->particleID() << " ... " << std::endl;
        
            // KS0
            if (daucand->particleID() == LoKi::Particles::_ppFromName("KS0")->particleID()) {
              // ksCtau = fCTAU(daucand);
              // ksMass = daucand->measuredMass()/GeV;
            }
        
            // Ghost
            if (daucand->proto()) {
              double pghostnn = daucand->proto()->info(ProtoParticle::ProbNNghost, -1.0 );
              if (maxProbGhostDaus < pghostnn)
                maxProbGhostDaus = pghostnn;
            }
        
            // PT
            if (daucand->proto()) {
              sumPT += daucand->proto()->info(ProtoParticle::TrackPt, 0.0);
            }
        
            // PROTON
            if (daucand->particleID() == LoKi::Particles::_ppFromName("p+")->particleID() or
                daucand->particleID() == LoKi::Particles::_ppFromName("p~-")->particleID()) {
              if (not setProton) {
                setProton = true;
                proId = daucand->particleID().pid();
                if (daucand->proto()) {
                  proPT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                  // proDLLp = daucand->proto()->info(ProtoParticle::CombDLLp, -1.0);
                  // proDLLk = daucand->proto()->info(ProtoParticle::CombDLLk, -1.0);
                  proNNp = daucand->proto()->info(ProtoParticle::ProbNNp, -1.0);
                  proNNk = daucand->proto()->info(ProtoParticle::ProbNNk, -1.0);
                }
                proIppvchi2 = ipPvChi2(daucand);
                proIpMinchi2 = ipChi2Min(daucand);
              } else {
                warning() << "Second proton found!" << endreq;
              }
            }      
        
            // KAON
            if (daucand->particleID() == LoKi::Particles::_ppFromName("K+")->particleID() or
                daucand->particleID() == LoKi::Particles::_ppFromName("K-")->particleID()) {
              if (not setKaon) {
                setKaon = true;
                kaonId = daucand->particleID().pid();
                if (daucand->proto()) {
                  kaonPT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                  // kaonDLLp = daucand->proto()->info(ProtoParticle::CombDLLp, -1.0);
                  // kaonDLLk = daucand->proto()->info(ProtoParticle::CombDLLk, -1.0);
                  kaonNNp = daucand->proto()->info(ProtoParticle::ProbNNp, -1.0);
                  kaonNNk = daucand->proto()->info(ProtoParticle::ProbNNk, -1.0);
                }
                kaonIppvchi2 = ipPvChi2(daucand);
                kaonIpMinchi2 = ipChi2Min(daucand);
              } else {
                warning() << "Second kaon found!" << endreq;
              }
            }      
        
            // PION EXTREMUM
            if (daucand->particleID() == LoKi::Particles::_ppFromName("pi+")->particleID() or
                daucand->particleID() == LoKi::Particles::_ppFromName("pi-")->particleID()) {
              if (not setPion) {
                setPion = true;
                pionId = daucand->particleID().pid();
                if (daucand->proto()) {
                  pionPT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                  // pionDLLp = daucand->proto()->info(ProtoParticle::CombDLLp, -1.0);
                  // pionDLLk = daucand->proto()->info(ProtoParticle::CombDLLk, -1.0);
                  pionNNp = daucand->proto()->info(ProtoParticle::ProbNNp, -1.0);
                  pionNNk = daucand->proto()->info(ProtoParticle::ProbNNk, -1.0);
                }
                pionIppvchi2 = ipPvChi2(daucand);
                pionIpMinchi2 = ipChi2Min(daucand);
              } else {
                double charge = daucand->particleID().pid(); charge /= abs(charge);
                pionId *= charge;
                if (daucand->proto()) {
                  double PT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                  // double DLLp = daucand->proto()->info(ProtoParticle::CombDLLp, -1.0);
                  // double DLLk = daucand->proto()->info(ProtoParticle::CombDLLk, -1.0);
                  double NNp = daucand->proto()->info(ProtoParticle::ProbNNp, -1.0);
                  double NNk = daucand->proto()->info(ProtoParticle::ProbNNk, -1.0);
                  pionPT = std::min(pionPT, PT);
                  // pionDLLp = std::max(pionDLLp, DLLp);
                  // pionDLLk = std::max(pionDLLk, DLLk);
                  pionNNp = std::max(pionNNp, NNp);
                  pionNNk = std::max(pionNNk, NNk);
              
                }
                double Ippvchi2 = ipPvChi2(daucand);
                double IpMinchi2 = ipChi2Min(daucand);
                pionIppvchi2 = std::min(pionIppvchi2, Ippvchi2);
                pionIpMinchi2 = std::min(pionIpMinchi2, IpMinchi2);
              }
            }
        
            // ELECTRON
            if (daucand->particleID() == LoKi::Particles::_ppFromName("e+")->particleID() or
                daucand->particleID() == LoKi::Particles::_ppFromName("e-")->particleID()) {
              if (daucand->proto()){
                elecNNe = daucand->proto()->info(ProtoParticle::ProbNNe, -1.0);
                elecPT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                elecIpMinchi2 = ipChi2Min(daucand);
              }
            }
        
            // MUON
            if (daucand->particleID() == LoKi::Particles::_ppFromName("mu+")->particleID() or 
                daucand->particleID() == LoKi::Particles::_ppFromName("mu-")->particleID()) {
              if (daucand->proto()) {
                muonNNmu = daucand->proto()->info(ProtoParticle::ProbNNmu, -1.0);
                muonPT = daucand->proto()->info(ProtoParticle::TrackPt, -1.0);
                muonIpMinchi2 = ipChi2Min(daucand);
              }
            }
          }
        }
    
        if (mode == CharmMode::Dz2kpipipi)
          if (kaonIppvchi2 < 6 or pionIppvchi2 < 6)
            cut = true;
        if (mode == CharmMode::Dz2kpiX)
          if (kaonIpMinchi2 < 10 or pionIpMinchi2 < 10)
            cut = true;
        if (mode == CharmMode::Dz2keX)
          if (kaonIpMinchi2 < 10 or elecIpMinchi2 < 10)
            cut = true;
        if (mode == CharmMode::Dz2kmuX)
          if (kaonIpMinchi2 < 10 or muonIpMinchi2 < 10)
            cut = true;

        if (cut)
          continue;

        ///////////////
        // Fill INFO //
        ///////////////

        int flavour = 0;
        
        switch (mode) {
          
        case CharmMode::LambdaC2pkpi:
          flavour = int(proId)/abs(int(proId));
          break;
          
        case CharmMode::Dz2kpi:
        case CharmMode::Dz2kpipipi:
        case CharmMode::Dz2kpipiz:
        case CharmMode::Dz2kpiX:
        case CharmMode::Dz2keX:
        case CharmMode::Dz2kmuX:
          flavour = -1 * int(kaonId)/abs(int(kaonId));
          break;
          
        case CharmMode::Dp2kpipi:
          flavour = int(charmId)/abs(int(charmId));
          break;
          
        default:
          warning() << "No charge assigned for mode" << decay.name << endreq;
          break;
          
        }

        // Add charm candidate

        cands.push_back(CharmParticle(mode, cand, mass, flavour,
                                      pchi2, fd, fdchi2, tau, bpvdira,
                                      maxProbGhostDaus,
                                      proId, proNNp, proNNk, proIppvchi2, proIpMinchi2, proPT,
                                      kaonId, kaonNNp, kaonNNk, kaonIppvchi2, kaonIpMinchi2, kaonPT,
                                      pionId, pionNNp, pionNNk, pionIppvchi2, pionIpMinchi2, pionPT,
                                      elecNNe, elecPT,
                                      muonNNmu, muonPT,
                                      dstarDm));
      }
    
    } else {
      error() << "Location not found: "<<*ilist<<endreq;
    }
  }
    
  return 0;
}

//=====================================================================
double TaggerCharmTool::getMvaVal(const CharmParticle *cpart, const int nPV, const int multiplicity,
                                  const LHCb::Particle& signalB)
{

  const Particle *part = cpart->part;
  CharmMode mode = cpart->mode;
  CharmDecay decay = decayMap[mode];

  auto logtrans = [&] (double var) {
    return safe_log(var)-safe_log(1.0-var);
  };

  std::vector<double> inputVals;

  inputVals.push_back(part->measuredMass()/GeV);
  inputVals.push_back(part->p()/GeV);
  inputVals.push_back(part->pt()/GeV);
  //  inputVals.push_back(part->momentum().Eta() - signalB.momentum().Eta());

  inputVals.push_back(safe_log(cpart->pchi2));
  inputVals.push_back(safe_log(cpart->tau));
  inputVals.push_back(safe_log(cpart->fd));
  inputVals.push_back(safe_log(cpart->fdchi2));
  inputVals.push_back(safe_log(1-cpart->bpvdira));
  inputVals.push_back(logtrans(cpart->maxProbGhostDaus));

  if (decay.hasP) {
    inputVals.push_back(logtrans(cpart->proProbnnp));
    inputVals.push_back(logtrans(cpart->proProbnnk));
    inputVals.push_back(safe_log(cpart->proIppvchi2));
    inputVals.push_back(safe_log(cpart->proIpMinchi2));
    inputVals.push_back(safe_log(cpart->proPT));
  }

  if (decay.hasK) {
    inputVals.push_back(logtrans(cpart->kaonProbnnp));
    inputVals.push_back(logtrans(cpart->kaonProbnnk));
    inputVals.push_back(safe_log(cpart->kaonIppvchi2));
    inputVals.push_back(safe_log(cpart->kaonIpMinchi2));
    inputVals.push_back(safe_log(cpart->kaonPT));
  }

  if (decay.hasPi) {
    inputVals.push_back(logtrans(cpart->pionProbnnp));
    inputVals.push_back(logtrans(cpart->pionProbnnk));
    inputVals.push_back(safe_log(cpart->pionIppvchi2));
    inputVals.push_back(safe_log(cpart->pionIpMinchi2));
    inputVals.push_back(safe_log(cpart->pionPT));
  }

  if (decay.hasE)
    inputVals.push_back(logtrans(cpart->elecProbnne));

  if (decay.hasMu)
    inputVals.push_back(logtrans(cpart->muonProbnnmu));

  if (decay.hasDstar) {
    inputVals.push_back(cpart->dstarDm);
  }

  if ( msgLevel(MSG::DEBUG) )
    debug()<<"Set MvaCharm Var: recv "<<nPV<<" mult "<<multiplicity<<" sigpt "<<signalB.pt()/GeV
           <<" m "<<part->measuredMass()/GeV<<" p "<<part->p()/GeV<<" pt "<<part->pt()/GeV
           <<" deta "<<part->momentum().Eta() - signalB.momentum().Eta()
           <<" logpchi2 "<<safe_log(cpart->pchi2)<<" logtau "<<safe_log(cpart->tau)<<" logfd "<<safe_log(cpart->fd)
           <<" logfdchi2 "<<safe_log(cpart->fdchi2)<<" logdira "<<safe_log(1-cpart->bpvdira)<<" logmghost "<<safe_log(cpart->maxProbGhostDaus)
           <<" logkprobk "<<safe_log(1-cpart->kaonProbnnk)<<" logkipchi2 "<<safe_log(cpart->kaonIppvchi2)
           <<" logprobe "<<safe_log(1-cpart->elecProbnne)<<" logprobmu "<<safe_log(1-cpart->muonProbnnmu)
           <<" dstarm "<<cpart->dstarDm
           <<endreq;

  TMVAWrapper * mva = getMVA(mode);
  return ( mva ? mva->GetMvaValue(inputVals) : -10.0 );
}

TMVAWrapper * TaggerCharmTool::getMVA( CharmMode mode )
{
  Classifiers::const_iterator iC = m_classifiers.find(mode);

  // If found, just return
  if ( iC != m_classifiers.end() ) { return iC->second; }

  // Else, make the MVA

  // initialize classifier
  // CRJ : Only create as needed, as oppossed to all all the time, to reduce memory footprint....

  std::map<CharmMode, std::vector< std::string> > inputVarMap;
  for (int index = 0; index != CharmMode::Max; index++) 
  {    
    CharmMode iMode = static_cast<CharmMode>(index);
    CharmDecay decay = decayMap[iMode];

    std::vector< std::string > inputVars;

    inputVars.push_back("Mass");
    inputVars.push_back("P");
    inputVars.push_back("Pt");
    inputVars.push_back("log(Probchi2)");
    inputVars.push_back("log(Tau)");
    inputVars.push_back("log(FlightDist)");
    inputVars.push_back("log(FlightDistChi2)");
    inputVars.push_back("log(1-BpvDira)");
    inputVars.push_back("log(MaxProbGhost)-log(1-MaxProbGhost)");

    if (decay.hasP) {
      inputVars.push_back("log(ProNNp)-log(1-ProNNp)");
      inputVars.push_back("log(ProNNk)-log(1-ProNNk)");
      inputVars.push_back("log(ProIppvChi2)");
      inputVars.push_back("log(ProIpMinChi2)");
      inputVars.push_back("log(ProPT)");
    }
    
    if (decay.hasK) {
      inputVars.push_back("log(KaonNNp)-log(1-KaonNNp)");
      inputVars.push_back("log(KaonNNk)-log(1-KaonNNk)");
      inputVars.push_back("log(KaonIppvChi2)");
      inputVars.push_back("log(KaonIpMinChi2)");
      inputVars.push_back("log(KaonPT)");
    }
    
    if (decay.hasPi) {
      inputVars.push_back("log(PionNNp)-log(1-PionNNp)");
      inputVars.push_back("log(PionNNk)-log(1-PionNNk)");
      inputVars.push_back("log(PionIppvChi2)");
      inputVars.push_back("log(PionIpMinChi2)");
      inputVars.push_back("log(PionPT)");
   }
    
    if (decay.hasE)
      inputVars.push_back("log(ElecNNe)-log(1-ElecNNe)");
    
    if (decay.hasMu)
      inputVars.push_back("log(MuonNNmu)-log(1-MuonNNmu)");
    
    if (decay.hasDstar)
      inputVars.push_back("DstarDeltaM");

    inputVarMap[iMode] = inputVars;
  }

  switch(mode) 
  {

  case CharmMode::Dz2kpi:
    m_classifiers[mode] = new CharmD0KpiWrapper(inputVarMap[mode]);
    break;
  
  case CharmMode::Dz2kpipipi:
    m_classifiers[mode] = new CharmD0KpipipiWrapper(inputVarMap[mode]);
    break;
    
  case CharmMode::Dz2kpipiz:
    m_classifiers[mode] = new CharmD0Kpipi0Wrapper(inputVarMap[mode]);
    break;

  case CharmMode::Dp2kpipi:
    m_classifiers[mode] = new CharmDpKpipiWrapper(inputVarMap[mode]);
    break;

  case CharmMode::Dz2kpiX:
    m_classifiers[mode] = new CharmD0KpiXWrapper(inputVarMap[mode]);
    break;
    
  case CharmMode::Dz2keX:
    m_classifiers[mode] = new CharmD0KeXWrapper(inputVarMap[mode]);
    break;
    
  case CharmMode::Dz2kmuX:
    m_classifiers[mode] = new CharmD0KmuXWrapper(inputVarMap[mode]);
    break;

  case CharmMode::LambdaC2pkpi:
    m_classifiers[mode] = new CharmLambdaCPKpiWrapper(inputVarMap[mode]);
    break;

  default:
    warning() << "Unknown charm mode" << endreq;
    break;

  }

  return m_classifiers[mode];
}

/////////////////////////////////////////////
double TaggerCharmTool::getOmega(double mistag, CharmMode mode)
{

  double p0 = 0.0;
  double p1 = 0.0;
  double p2 = 1.0;
  double p3 = 0.0;
  double p4 = 0.0;
  double p5 = 1.0;

  switch (mode) {
  case CharmMode::Dz2kpi:
    p0 = 0.3689;
    p1 = 0.3118;
    p2 = 0.02822;
    p3 = 0.3344;
    p4 = 0.3904;
    p5 = 0.06097;
    break;
  case CharmMode::Dz2kpipipi:
    p0 = -0.8049;
    p1 = 0.0;
    p2 = 0.1814;
    break;
  case CharmMode::Dz2kpipiz:
    p0 = -0.9212;
    p1 = 0.4608;
    p2 = 0.01278;
    break;
  case CharmMode::Dp2kpipi:
    p0 = 0.2409;
    p1 = 0.3868;
    p2 = 0.06829;
    break;
  case CharmMode::Dz2kpiX:
    p0 = -0.3519;
    p1 = 1.0;
    p2 = 1.147;
    p3 = 0.4085;
    p4 = 0.4785;
    p5 = 0.02616;
    break;
  case CharmMode::Dz2keX:
    p0 = -0.4702;
    p1 = 0.3457;
    p2 = 0.044;
    p3 = -0.1537;
    p4 = 0.4954;
    p5 = 0.006842;
    break;
  case CharmMode::Dz2kmuX:
    break;
  case CharmMode::LambdaC2pkpi:
    p0 = -1.181;
    p1 = 0.3721;
    p2 = 0.03328;
    break;
  default:
    break;
  }

  auto gauss = [] (double x, double mu, double sigma) {
    double z = (x-mu)/sigma;
    return exp(-0.5*z*z);
  };

  auto func = [&] (double x) {
    double g1 = p0*gauss(x,p1,p2);
    double g2 = p3*gauss(x,p4,p5);
    return x + x*(1-x)*(g1 + g2);
  };

  double omegaBDT = func(mistag);
  return omegaBDT;

}

