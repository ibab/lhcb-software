// Include files 
#include "TaggerCharmTool.h"
//#include "TaggingHelpers.h"

#include <iostream>
#include <fstream>
#include <list>
#include <utility>

#include "TMath.h"

using namespace std;

#include "LoKi/LoKi.h" 
#include "LoKi/ParticleContextCuts.h" 

using namespace LoKi::Cuts;
using namespace LoKi::Types;
using namespace LoKi::Particles;

//--------------------------------------------------------------------
// Implementation file for class : TaggerCharmTool
//
// Author: rcenci
// 2013-01-23
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerCharmTool )

//====================================================================
  TaggerCharmTool::TaggerCharmTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent ) :
    GaudiTool ( type, name, parent ),
    m_util(0),
    m_pLifetimeFitter(0),
    m_descend(0),
    m_nnet(0)
{
  declareInterface<ITagger>(this);

  declareProperty( "CharmTagLocations", m_CharmTagLocations); // = "Phys/StdLooseD02KPi/Particles");
  declareProperty( "CharmInclTagLocations", m_CharmInclTagLocations); // = "Phys/StdLooseD02KPi/Particles");
  declareProperty( "CharmStarTagLocations", m_CharmStarTagLocations); // = "Phys/StdLooseD02KPi/Particles");

  declareProperty( "Charm_P0_Cal",           m_P0_Cal_charm   = 0.3371); 
  declareProperty( "Charm_P1_Cal",           m_P1_Cal_charm   = 0.9111); 
  declareProperty( "Charm_Eta_Cal",          m_Eta_Cal_charm  = 0.3121);

  // initialize decay map
  CharmDecayModeMap["D0_Kpi"]          = CharmDecayMode("D0_Kpi",0,          1,0,0,0, 1.82, 1.915, -0.0223261, 0.630175 );
  CharmDecayModeMap["D0_Kpipipi"]      = CharmDecayMode("D0_Kpipipi",1,      1,0,0,0, 1.82, 1.915, -0.0118087, 0.420859 );
  CharmDecayModeMap["D0_Kspipi"]       = CharmDecayMode("D0_Kspipi",2,       0,0,0,0, 1.82, 1.915, 0.,         0.       );
  CharmDecayModeMap["D0_Kpipi0"]       = CharmDecayMode("D0_Kpipi0",3,       1,0,0,0, 1.82, 1.915, -0.0199824, 0.0592187);
  CharmDecayModeMap["Dp_Kpipi"]        = CharmDecayMode("Dp_Kpipi",4,        1,0,0,0, 1.82, 1.915, -0.0120574, 0.525746 );
  CharmDecayModeMap["Dp_Kspi"]         = CharmDecayMode("Dp_Kspi",5,         0,0,0,0, 1.82, 1.915, 0.731183,   0.731183 );
  CharmDecayModeMap["D0_KpiX"]         = CharmDecayMode("D0_KpiX",6,         1,0,0,0, 0.8,  1.915, -0.0587858, 0.235058 );
  CharmDecayModeMap["D0_KeX"]          = CharmDecayMode("D0_KeX",7,          1,1,0,0, 0.8,  1.915, -0.0483723, 0.291827 );
  CharmDecayModeMap["D0_KmuX"]         = CharmDecayMode("D0_KmuX",8,         1,0,1,0, 0.8,  1.915, -0.0207928, 0.39514  );
  CharmDecayModeMap["Dstar_D0_Kspipi"] = CharmDecayMode("Dstar_D0_Kspipi",9, 0,0,0,1, 1.82, 1.915, -0.358734,  0.073132 );
  CharmDecayModeMap["Dp_KpiX"]         = CharmDecayMode("Dp_KpiX",10,        1,0,0,0, 0.8,  1.915, -0.115402,  0.10732  );
  CharmDecayModeMap["Dp_KeX"]          = CharmDecayMode("Dp_KeX",11,         1,1,0,0, 0.8,  1.915, -0.0594275, 0.148148 );
  CharmDecayModeMap["Dp_KmuX"]         = CharmDecayMode("Dp_KmuX",12,        1,0,1,0, 0.8,  1.915, -0.054475,  0.189587 );
  
  //const double c_optCutVal[nCharmDecayModes] = { -0.0223261, -0.0118087, 6.95325e-310, -0.0199824, 
  //-0.0120574, 1, -0.0587858, -0.0483723, -0.0207928, -0.358734, -0.115402, -0.0594275, -0.054475};
  //const double c_PurVal[nCharmDecayModes] = { 0.630175, 0.420859, 2.02369e-320, 0.0592187, 0.525746, 
  //0.731183, 0.235058, 0.291827, 0.39514, 0.073132, 0.10732, 0.148148, 0.189587};
  //const int c_nSig[nCharmDecayModes] = { 3711, 1512, 0, 673, 1595, 68, 3834, 310, 457, 23, 6032, 504, 857};
  //const int c_nBkg[nCharmDecayModes] = { 40442, 192514, 0, 2504383, 81255, 25, 102308, 99943, 102233, 
  //25972, 304834, 294041, 299594};
  
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

  m_nnet = tool<INNetTool> ( "NNetTool_MLP", this);

//   //load tmva readers
//   int nFoundReaders = 0;
  
  std::map<std::string, std::vector< std::string> > inputVarMap;

  for( std::map<std::string, CharmDecayMode>::iterator iter = CharmDecayModeMap.begin(); iter !=CharmDecayModeMap.end(); ++iter)
  {

    CharmDecayMode decay = iter->second;

    std::vector< std::string > inputVars;
    
    inputVars.push_back("RecVerts");
    inputVars.push_back("Ntrks");
    inputVars.push_back("SigBpt");
    inputVars.push_back("Mass");
    inputVars.push_back("P");
    inputVars.push_back("Pt");
    inputVars.push_back("DeltaEta");

    inputVars.push_back("log(Probchi2)");
    inputVars.push_back("log(Tau)");
    inputVars.push_back("log(FlightDist)");
    inputVars.push_back("log(FlightDistChi2)");
    inputVars.push_back("log(1-BpvDira)");
    inputVars.push_back("log(MaxProbNNGhostDaus)");

    if (decay.hasK) {
      inputVars.push_back("log(1-KaonProbNNk)");
      inputVars.push_back("log(KaonIppvChi2)");
      inputVars.push_back("log(KaonIpMinChi2)");
    }
 
    if (decay.hasE)
      inputVars.push_back("log(1-ElecProbNNe)");

    if (decay.hasMu)
      inputVars.push_back("log(1-MuonProbNNmu)");

    if (decay.hasDstar) {
      inputVars.push_back("DstarDeltaM");
    }
  
    inputVarMap[decay.mode] = inputVars;
  
  }

//   if ( msgLevel(MSG::DEBUG) )  debug() << " Number of TMVA readers found: " << nFoundReaders << endreq;

  //initialize classifier map
  m_classifiers["D0_Kpi"] = new D0_Kpi::ReadBDT(inputVarMap["D0_Kpi"]);
  m_classifiers["D0_Kpipipi"] = new D0_Kpipipi::ReadBDT(inputVarMap["D0_Kpipipi"]);
  m_classifiers["D0_Kspipi"] = NULL; //new D0_Kspipi::ReadBDT(inputVarMap["D0_Kspipi"]);
  m_classifiers["D0_Kpipi0"] = new D0_Kpipi0::ReadBDT(inputVarMap["D0_Kpipi0"]);
  m_classifiers["Dp_Kpipi"] = new Dp_Kpipi::ReadBDT(inputVarMap["Dp_Kpipi"]);
  m_classifiers["Dp_Kspi"] = new Dp_Kspi::ReadBDT(inputVarMap["Dp_Kspi"]);
  m_classifiers["D0_KpiX"] = new D0_KpiX::ReadBDT(inputVarMap["D0_KpiX"]);
  m_classifiers["D0_KeX"] = new D0_KeX::ReadBDT(inputVarMap["D0_KeX"]);
  m_classifiers["D0_KmuX"] = new D0_KmuX::ReadBDT(inputVarMap["D0_KmuX"]);
  m_classifiers["Dstar_D0_Kspipi"] = new Dstar_D0_Kspipi::ReadBDT(inputVarMap["Dstar_D0_Kspipi"]);
  m_classifiers["Dp_KpiX"] = new Dp_KpiX::ReadBDT(inputVarMap["Dp_KpiX"]);
  m_classifiers["Dp_KeX"] = new Dp_KeX::ReadBDT(inputVarMap["Dp_KeX"]);
  m_classifiers["Dp_KmuX"] = new Dp_KmuX::ReadBDT(inputVarMap["Dp_KmuX"]);
  
  return sc;
  
}

//=====================================================================
Tagger TaggerCharmTool::tag( const Particle* signalB,
                                       const RecVertex* RecVert,
                                       const int nPV,
                                       Particle::ConstVector& tagParticles )
{

  Tagger tcharm;

  std::vector< CharmParticle > cands;
  
  addCands(cands, m_CharmTagLocations    , RecVert, 0);
  addCands(cands, m_CharmInclTagLocations, RecVert, 1);
  addCands(cands, m_CharmStarTagLocations, RecVert, 2);
  
  if ( msgLevel(MSG::DEBUG) )  debug() << "Number of charm cands retrieved: "<<cands.size()<<endreq;
    
  // selection
  CharmParticle *thecharm = NULL;
  std::vector< CharmParticle >::iterator ipart;

  double max_mvaVar = -1., curr_purity = 0.;

  int ncands = 0, icand = 0, ipicked = -1;
  for (ipart=cands.begin(); ipart!=cands.end(); ++ipart) {

    CharmParticle *cpart = &(*ipart);
    std::string mode = cpart->mode;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<"cand #"<<icand<<" flavour "<<cpart->flavour<<" mode "<<mode<<" ptr "<<cpart<<" thecharm "<<thecharm<<endreq;
    icand++;

    if (cpart->flavour==0) continue;

    ////////////
    // selection

    // mass
    if ( msgLevel(MSG::DEBUG) ) debug()<<"mass "<<cpart->mass<<endreq;
    if ( cpart->mass <= CharmDecayModeMap[mode].lowMcut || cpart->mass >= CharmDecayModeMap[mode].highMcut ) continue;

    // decay time
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"tau "<<cpart->tau<<endreq;
    if ( cpart->tau <= -100 || cpart->tau >= 100 ) continue;

    if ( msgLevel(MSG::DEBUG) )    debug()<<"bpvdira "<<cpart->bpvdira<<endreq;
    if ( cpart->bpvdira <= 0.5 ) continue;

    // kaon vars
    if (CharmDecayModeMap[mode].hasK) {
      if ( msgLevel(MSG::DEBUG) )      debug()<<"kaonProbnnk "<<cpart->kaonProbnnk<<endreq;
      if ( cpart->kaonProbnnk <= 0.05 ) continue;
      if ( msgLevel(MSG::DEBUG) )      debug()<<" kaonIppvchi2 "<<cpart->kaonIppvchi2<<endreq;
      if ( cpart->kaonIppvchi2 <= 0 ) continue;
    }

    if ( msgLevel(MSG::DEBUG) )    debug()<<"maxProbGhostDaus "<<cpart->maxProbGhostDaus<<endreq;
    if ( cpart->maxProbGhostDaus >= 0.8 ) continue;

    if ( msgLevel(MSG::DEBUG) )    debug()<<"pchi2 "<<cpart->pchi2<<endreq;
    if( cpart->pchi2 <= 0.001 ) continue;

    // mva cut
    double mvaVar = getMvaVal(cpart, nPV, tagParticles.size(), signalB);

    if ( msgLevel(MSG::DEBUG) )    debug()<<"mva "<<mvaVar<<endreq;
    if ( mvaVar < CharmDecayModeMap[mode].mvaCut ) continue;

    ncands++;

    if ( msgLevel(MSG::DEBUG) )    debug()<<"purity "<<CharmDecayModeMap[mode].purity<<endreq;

    if( mvaVar > max_mvaVar && CharmDecayModeMap[mode].purity >= curr_purity ) { //discrVar ordering
      thecharm = cpart;
      max_mvaVar = mvaVar;
      curr_purity = CharmDecayModeMap[mode].purity;
      ipicked = icand-1;
    }
      
  }
  
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Number of charm cands selected: "<<ncands<<" picked idx "<<ipicked<<" thecharm"<<thecharm<<endreq;

  if(!thecharm) return tcharm;

  //calculate omega
  double omega = getOmega(thecharm, nPV, tagParticles.size(), signalB);
  if ( msgLevel(MSG::DEBUG) )    debug()<<"omega "<<omega << endreq; 
  if( omega < 0 ||  omega > 1 )  {
    if ( msgLevel(MSG::DEBUG) )    debug()<<"Something wrong with Charm Training "<<omega << endreq; 
    return tcharm;
  }
  
  if ( msgLevel(MSG::DEBUG) )  debug()<<"the charm flavour "<<thecharm->flavour<<endreq;

  int decision = thecharm->flavour > 0 ? +1: -1; // double flip, one for opposite side, one for D from B
  
  tcharm.setOmega(omega);
  tcharm.setDecision(decision);
  tcharm.setType( Tagger::OS_Charm ); 
  tcharm.addToTaggerParts(thecharm->part);
  
  return tcharm;
  
}

//=====================================================================
int TaggerCharmTool::addCands(std::vector< CharmParticle >& cands, const std::vector<std::string>& locations, 
                              const RecVertex* RecVert, const int type)
{

  for( std::vector<std::string>::const_iterator ilist = locations.begin(); ilist != locations.end(); ++ilist) {
    if (exist<Particle::Range>(*ilist)){
      LHCb::Particle::Range partsin  = get<Particle::Range>(*ilist) ;

      if ( msgLevel(MSG::DEBUG) )
        debug() << "Found "<<partsin.size()<<" charm cands for location "<<*ilist<<endreq;
    
      for (Gaudi::Range_<LHCb::Particle::ConstVector>::const_iterator icand = partsin.begin(); 
           icand != partsin.end(); ++icand) {

        const Particle *cand, *dstar_cand, *pisoft_cand;
        if (type==2) {
          dstar_cand = (*icand);
          cand = (*icand)->daughters().at(0);
          pisoft_cand = (*icand)->daughters().at(1);
        } else {
          dstar_cand = pisoft_cand = NULL;  
          cand = (*icand);
        }
    
        // compute vars
        float mass = cand->measuredMass()/GeV;

        int charmId = cand->particleID().pid();
        int charmStarId = 0; if (dstar_cand) charmStarId = dstar_cand->particleID().pid();

        float dstarDm = dstar_cand ? dstar_cand->measuredMass()/GeV - mass : 0. ;

        double ct=0., ctErr=0., ctChi2=0.;
        double tau = -999.;
        StatusCode sc = m_pLifetimeFitter->fit(*RecVert, *cand, ct, ctErr, ctChi2);
        if(sc) tau = ct/picosecond;

        
        double pchi2 = TMath::Prob(cand->endVertex()->chi2(),cand->endVertex()->nDoF());

        // construct the functors
        Fun fDIRA = DIRA( RecVert );
        const float bpvdira = fDIRA( cand ) ;
        
        Fun fVD = VD ( RecVert );
        const float fd = fVD( cand ) ;
        
        Fun fVDCHI2 = VDCHI2 ( RecVert );
        const float fdchi2 = fVDCHI2( cand ) ;
        
        LHCb::VertexBase::ConstVector verts; verts.push_back(RecVert);
        Fun ipPvChi2 = MIPCHI2 ( verts , m_util->getDistanceCalculator() ) ;
        const MIPCHI2DV ipChi2Min  = MIPCHI2DV ( "" ) ;

        std::string mode = m_util->getCharmDecayMode(cand,type);

        if ( msgLevel(MSG::DEBUG) )        debug()<<"mode "<<mode<<endreq;

        float maxProbGhostDaus = 0.;
        float kaonId = 0, kaonProbnnk = 1., kaonIppvchi2 = -1, kaonIpMinchi2 = -1;
        float elecProbnne = 1., muonProbnnmu = 1.;
        
        const SmartRefVector<Particle>& daus = cand->daughters();
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Charm cand, dau vector size = " << daus.size() << endreq;
        for( SmartRefVector<Particle>::const_iterator idau = daus.begin(); idau != daus.end(); ++idau) {
          const Particle *daucand = *idau;
          if (daucand->proto()) {
            float pghostnn = daucand->proto()->info(ProtoParticle::ProbNNghost, -1. );
            if(maxProbGhostDaus < pghostnn ) maxProbGhostDaus = pghostnn;
          }
          if ( daucand->particleID() == LoKi::Particles::_ppFromName("K+")->particleID() || 
               daucand->particleID() == LoKi::Particles::_ppFromName("K-")->particleID() ) {
            kaonId = daucand->particleID().pid();
            if (daucand->proto()) {
              kaonProbnnk = daucand->proto()->info(ProtoParticle::ProbNNk, -1. );
            }
            kaonIppvchi2 = ipPvChi2(daucand) ;
            kaonIpMinchi2 = ipChi2Min(daucand) ;        // use the functor:
            
          }      
          
          if ( daucand->particleID() == LoKi::Particles::_ppFromName("e+")->particleID() || 
               daucand->particleID() == LoKi::Particles::_ppFromName("e-")->particleID() ) 
            if (daucand->proto()) 
              elecProbnne = daucand->proto()->info(ProtoParticle::ProbNNe, -1. );
          
          if ( daucand->particleID() == LoKi::Particles::_ppFromName("mu+")->particleID() || 
               daucand->particleID() == LoKi::Particles::_ppFromName("mu-")->particleID() ) 
            if (daucand->proto()) 
              muonProbnnmu = daucand->proto()->info(ProtoParticle::ProbNNmu, -1. );
          
        }
        
        // compute flavour charge
        int flavour = 0;
        if (mode=="D0_Kpi" || mode=="D0_Kpipipi" || mode=="D0_Kpipi0" || mode=="D0_KpiX" || mode=="D0_KeX" || mode=="D0_KmuX")
          flavour = -1 * int(kaonId)/abs(int(kaonId));
        else if (mode=="Dp_Kpipi" || mode=="Dp_Kspi" || mode=="Dp_KpiX" || mode=="Dp_KeX" || mode=="Dp_KmuX")
          flavour = int(charmId)/abs(int(charmId));
        else if (mode=="Dstar_D0_Kspipi")
          flavour = int(charmStarId)/abs(int(charmStarId));

        cands.push_back(CharmParticle(mode, cand, mass, flavour, 
                                      pchi2, fd, fdchi2, tau, bpvdira,
                                      maxProbGhostDaus, 
                                      kaonId, kaonProbnnk, kaonIppvchi2, kaonIpMinchi2,
                                      elecProbnne, muonProbnnmu,
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
                                  const LHCb::Particle* signalB)
{

  const Particle *part = cpart->part;
  std::string mode = cpart->mode;

  CharmDecayMode decay = CharmDecayModeMap[mode];

  std::vector<double> inputVals;
  
  inputVals.push_back(nPV);
  inputVals.push_back(multiplicity);

  inputVals.push_back(signalB->pt()/GeV);

  inputVals.push_back(part->measuredMass()/GeV);
  inputVals.push_back(part->p()/GeV);
  inputVals.push_back(part->pt()/GeV);
  inputVals.push_back(part->momentum().Eta() - signalB->momentum().Eta());

  inputVals.push_back(log(cpart->pchi2));      

  inputVals.push_back(log(cpart->tau));      
  inputVals.push_back(log(cpart->fd));      

  inputVals.push_back(log(cpart->fdchi2));      
  inputVals.push_back(log(1-cpart->bpvdira));      
  inputVals.push_back(log(cpart->maxProbGhostDaus));      

  if (decay.hasK) {
    inputVals.push_back(log(1-cpart->kaonProbnnk));      
    inputVals.push_back(log(cpart->kaonIppvchi2));      
    inputVals.push_back(log(cpart->kaonIpMinchi2));      
  }
 
  if (decay.hasE)
    inputVals.push_back(log(1-cpart->elecProbnne));      

  if (decay.hasMu)
    inputVals.push_back(log(1-cpart->muonProbnnmu));

  if (decay.hasDstar) {
    inputVals.push_back(cpart->dstarDm);
  }

  if ( msgLevel(MSG::DEBUG) )
    debug()<<"Set MvaCharm Var: recv "<<nPV<<" mult "<<multiplicity<<" sigpt "<<signalB->pt()/GeV
           <<" m "<<part->measuredMass()/GeV<<" p "<<part->p()/GeV<<" pt "<<part->pt()/GeV
           <<" deta "<<part->momentum().Eta() - signalB->momentum().Eta()
           <<" logpchi2 "<<log(cpart->pchi2)<<" logtau "<<log(cpart->tau)<<" logfd "<<log(cpart->fd)
           <<" logfdchi2 "<<log(cpart->fdchi2)<<" logdira "<<log(1-cpart->bpvdira)<<" logmghost "<<log(cpart->maxProbGhostDaus)
           <<" logkprobk "<<log(1-cpart->kaonProbnnk)<<" logkipchi2 "<<log(cpart->kaonIppvchi2)
           <<" logprobe "<<log(1-cpart->elecProbnne)<<" logprobmu "<<log(1-cpart->muonProbnnmu)
           <<" dstarm "<<cpart->dstarDm
           <<endreq;

  return m_classifiers[mode] ? m_classifiers[mode]->GetMvaValue(inputVals) : -10.;
  
}

/////////////////////////////////////////////
double TaggerCharmTool::getOmega(const CharmParticle* cpart, const int nPV, const int multiplicity, const LHCb::Particle* signalB)
{

  const Particle *part = cpart->part;

  std::vector<std::string> inputVars;
  std::vector<double> inputVals;

  inputVars.push_back("mult"                    ); inputVals.push_back( (double)multiplicity                        );
  inputVars.push_back("nnkrec"                  ); inputVals.push_back( (double)nPV                                 );
  inputVars.push_back("ptB"                     ); inputVals.push_back( (double)log(signalB->pt()/GeV)              );
  inputVars.push_back("charm_mass"              ); inputVals.push_back( (double)part->measuredMass()/GeV            );
  inputVars.push_back("charm_mode"              ); inputVals.push_back( (double)CharmDecayModeMap[cpart->mode].index);
  inputVars.push_back("charm_bpvdira"           ); inputVals.push_back( (double)cpart->bpvdira                      );
  inputVars.push_back("charm_probchi2"          ); inputVals.push_back( (double)cpart->pchi2                        );
  inputVars.push_back("charm_tau"               ); inputVals.push_back( (double)cpart->tau                          );
  inputVars.push_back("charm_flightdistchi2"    ); inputVals.push_back( (double)cpart->fdchi2                       );
  inputVars.push_back("charm_kprobnnk"          ); inputVals.push_back( (double)cpart->kaonProbnnk                  );
  inputVars.push_back("charm_kippvchi2"         ); inputVals.push_back( (double)cpart->kaonIppvchi2                 );
  inputVars.push_back("charm_maxprobnnghostdaus"); inputVals.push_back( (double)cpart->maxProbGhostDaus             );

  if (msgLevel(MSG::DEBUG)) {
    debug()<<"Set NNetCharm Omega Var:";
    for (unsigned int ivec = 0; ivec<inputVars.size();ivec++){
      debug()<<" "<<inputVars[ivec]<<" "<<inputVals[ivec];
    } 
    debug()<<endreq;
  }
  
  double omega = 1 - m_nnet->MLPcharmTMVA(inputVars,inputVals);

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  omega = m_P0_Cal_charm + m_P1_Cal_charm * ( omega-m_Eta_Cal_charm);
  if ( msgLevel(MSG::DEBUG) )  debug()<<" Vtx pn="<<1-omega<<" w="<<omega<<endmsg;

  return omega;
}
