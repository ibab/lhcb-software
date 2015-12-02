// Include files
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/Particle.h"  
// kernel 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticle2MCAssociator.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
//#include "Kernel/IOnOffline.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>

// MC stuff
#include "Event/GenHeader.h" 
#include "Event/MCHeader.h" 

#include "TupleToolMuonVariables.h"
#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
#include <vector>
#include <iostream>
#include "Kernel/IRelatedPVFinder.h"
#include "Math/Boost.h"

#include "Event/RecSummary.h"
#include <math.h>

using namespace std;
using namespace LHCb;
 
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMuonVariables
//
// 2011-08-08 : Fatima Soomro 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TupleToolMuonVariables )

typedef std::vector<double> Array;
typedef std::vector<Array> Matrix;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMuonVariables::TupleToolMuonVariables( const std::string& type,
						const std::string& name,
						const IInterface* parent) 
  : TupleToolBase ( type, name , parent )
  , m_nvariables(6)
  , m_dva(0)
  , m_dist(0)
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  
  declareProperty("ParticlePath",   m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");
  declareProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty("angle"     , m_angle  = 0.27     ); // 
  declareProperty("fc"        , m_fc  = 0.60     ); // 
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); // 
  declareProperty("ips"       , m_ips  = 3.0     ); // 
  declareProperty("svdis"     , m_svdis  = -0.15     ); // 
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); // 
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); // 
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); // 
  declareProperty("isMC", m_isMC = false); 
  declareProperty("isKsana", m_isKsana = false); 
  declareProperty("dumpFullInfo", m_dumpFullInfo = false); 
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType =  "DaVinciSmartAssociator");
  declareProperty("BDTSRootFile", m_bdts_root_file = "HflatBDTS_7Dec.root");
  declareProperty("BDTSXMLFile", m_bdts_xml_file = "TMVA_7Dec.weights.xml"); 

  //addition to compute the isoBDT
  declareProperty("isoBDT1_xmlFile",m_isoBDT1_xmlFile="600_2500_4_30_0.75_1_1_BDT.weights.xml");
  declareProperty("isoBDT2_xmlFile",m_isoBDT2_xmlFile="600_2500_4_30_0.75_1_8_BDT.weights.xml");
  declareProperty("isoBDT3_xmlFile",m_isoBDT3_xmlFile="600_2500_4_30_0.75_1_9_BDT.weights.xml");
  declareProperty("isoBDT4_xmlFile",m_isoBDT4_xmlFile="600_2500_4_30_0.75_1_4_BDT.weights.xml");
  declareProperty("isoBDT_xmlFilePath",m_isoBDT_xmlFilePath="/home/mrama/workdir/weights/isobdt_3mar2014/");

  declareProperty("dumpVeloTracks", m_dumpVeloTracks = true); 
  declareProperty("dumpUpstreamTracks", m_dumpUpstreamTracks = true); 
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolMuonVariables::~TupleToolMuonVariables() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TupleToolMuonVariables::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  
  
  if(m_isMC){
    m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);
    if (0==m_p2mcAssoc) return Error("Couldn't get MC associator!! ",  StatusCode::FAILURE);  
  }
  
  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);  
  
  m_dist = m_dva->distanceCalculator ();
  if( !m_dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }
  
  //Fill m_vertices with the list of PVs
  /*
  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
    m_vertices = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  }
  */

  m_pvReFitter = tool<IPVReFitter>("LoKi::PVReFitter:PUBLIC", this );
  if(! m_pvReFitter) {
    fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
    return StatusCode::FAILURE;
  }
  
  // BDTS stuff, from Francesco
  m_values = new float[m_nvariables];   //< variable values
  m_reader_BDTS = new TMVA::Reader( "!Color:!Silent" );
  for(int index = 0; index < m_nvariables ; index++){
    std::ostringstream name;
    name << "vars_" << index << "";
    const TString var = name.str().c_str();
    m_reader_BDTS->AddVariable( var, &m_values[index] );
  }
  m_methodName = "method";
  m_reader_BDTS->BookMVA(m_methodName, m_bdts_xml_file );
  
  //   if(!m_reader_BDTS){
  //     warning() << "Unable to instantiateretrive the TMVA Reader")
  //   }
  TFile *f2_BDTS = new TFile(m_bdts_root_file.c_str());
  if(!f2_BDTS ) {
    error() << "Unable to load BDTS file for histograms " << endmsg;
    return StatusCode::FAILURE;
  }
  TH1D *h_flat_BDTS = (TH1D*)f2_BDTS->Get("h_flat");
  if(!h_flat_BDTS){
    error() << "Unable to load the histograms for BDTS " << endmsg;
    return StatusCode::FAILURE;
  }
  double integral = h_flat_BDTS->Integral();
  m_h_cumul_BDTS = (TH1D*)h_flat_BDTS->Clone("h_cumulative");
  for (int i=0;i<h_flat_BDTS->GetNbinsX();++i){
    m_h_cumul_BDTS->SetBinContent(i,h_flat_BDTS->Integral(0,i)/integral);
  }
  info() << "Initialized BDTS  "  << " WeightsFile = " << m_bdts_xml_file 
         << ", FlatHistoFile = " << m_bdts_root_file << endmsg;
  //end BDTS stuff 
  

  //isoBDT stuff
  //isobdt_values = new float[15];

  for (int i=0;i<4;i++) isobdt_reader[i] = new TMVA::Reader("!Color:!Silent");
  //
  // Legenda set of vars: isobdt1: set1; isobdt2: set1+3; isobdt3: set1+3+4; isobdt4: set1+2+3+4
  //
  TString isobdt_vars_set1[6]={"log(trk_ips)","trk_pvdis1","trk_svdis1","log(trk_doca1)","trk_angle1","trk_fc1"};
  TString isobdt_vars_set2[2]={"trk_gho","trk_chi"};
  TString isobdt_vars_set3[3]={"trk_dphi1","abs(trk_deta1)","trk_pt"};
  TString isobdt_vars_set4[4]={"trk_ch","trk_nnmu","trk_nnpi","trk_nnk"};
  
  for (int i=0;i<6;i++){//set of vars #1
    isobdt_reader[0]->AddVariable(isobdt_vars_set1[i],&isobdt_values[i]);//isobdt1
    isobdt_reader[1]->AddVariable(isobdt_vars_set1[i],&isobdt_values[i]);//isobdt2
    isobdt_reader[2]->AddVariable(isobdt_vars_set1[i],&isobdt_values[i]);//isobdt3
    isobdt_reader[3]->AddVariable(isobdt_vars_set1[i],&isobdt_values[i]);//isobdt4
  }
  for (int i=0;i<2;i++){//set of vars #2
    isobdt_reader[3]->AddVariable(isobdt_vars_set2[i],&isobdt_values[6+i]);//isobdt4
  }
  for (int i=0;i<3;i++){//set of vars #3
    isobdt_reader[1]->AddVariable(isobdt_vars_set3[i],&isobdt_values[8+i]);//isobdt2
    isobdt_reader[2]->AddVariable(isobdt_vars_set3[i],&isobdt_values[8+i]);//isobdt3
    isobdt_reader[3]->AddVariable(isobdt_vars_set3[i],&isobdt_values[8+i]);//isobdt4
  }
  for (int i=0;i<4;i++){//set of vars #4
    isobdt_reader[2]->AddVariable(isobdt_vars_set4[i],&isobdt_values[11+i]);//isobdt3
    isobdt_reader[3]->AddVariable(isobdt_vars_set4[i],&isobdt_values[11+i]);//isobdt4
  }
  
  isobdt_methodName = "method";
  isobdt_reader[0]->BookMVA(isobdt_methodName,m_isoBDT_xmlFilePath+m_isoBDT1_xmlFile);
  isobdt_reader[1]->BookMVA(isobdt_methodName,m_isoBDT_xmlFilePath+m_isoBDT2_xmlFile);
  isobdt_reader[2]->BookMVA(isobdt_methodName,m_isoBDT_xmlFilePath+m_isoBDT3_xmlFile);
  isobdt_reader[3]->BookMVA(isobdt_methodName,m_isoBDT_xmlFilePath+m_isoBDT4_xmlFile);
  
  //end isoBDT v2 stuff
  
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;

}//end initialize()


//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode TupleToolMuonVariables::fill( const LHCb::Particle *, const LHCb::Particle *part,
                                          const std::string &  	head, Tuples::Tuple &  	tuple	) {
  
  const LHCb::Particle::ConstVector part_dau_vec = part->daughtersVector();
  //cout<<"part num daughters: "<<part_dau_vec.size()<<endl;
  for (LHCb::Particle::ConstVector::const_iterator idau=part_dau_vec.begin();idau!=part_dau_vec.end();++idau){
    //cout<<"part dau ID: "<<(*idau)->particleID().pid()<<endl;
  }

  const std::string prefix=fullName(head);
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;
  
  if ( abs(part->particleID().pid()) ==13 
       || abs( part->particleID().pid() )  == 11 )  // only for muons/electrons
    {  
      StatusCode scTrackInfo = fillTrackHitInfo(part, prefix, tuple);
      if(!scTrackInfo) return scTrackInfo;
    }
  
  StatusCode scEta = fillEta(part, prefix, tuple);
  if(!scEta) return scEta;
  
  const LHCb::Particle *myJpsi= NULL;
  const SmartRefVector< LHCb::Particle > Bdaughters = part->daughters();
  int countDaughters =0; bool checkDau=true;
  JpsiInDau = false ; 
  int dauIds[2] = {13,11};
  if(m_isKsana) {
    dauIds[0] = 13;
    dauIds[1] = 321;
  }
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = Bdaughters.begin() ; 
       idau != Bdaughters.end() ; ++idau){
    if (NULL == (*idau)->proto() )
      {
	checkDau =false;
	debug()<<"daughter no "<<countDaughters<<"  is NULL!! "<<endreq;
	if( abs ((*idau)->particleID().pid() ) == 443) { 
	  JpsiInDau = true; 
	  myJpsi = (*idau);
	}
      }
    else if (  abs((*idau)->particleID().pid()) == dauIds[0] 
               || abs((*idau)->particleID().pid() )  == dauIds[1] ) countDaughters++;  
    // has muon or electron as a daughter
    
  } //smartRef
  
  
  if(JpsiInDau) {
    StatusCode fillJpsi = fillFakeVtx(part, myJpsi, prefix, tuple);
    StatusCode fillJC = ConstrMass(part, prefix, tuple);
    if(!fillJC || !fillJpsi) return StatusCode::FAILURE;
  }
  

  // Doca b/w the muon tracks is needed for the 2 body and for the jpsi
  if  (checkDau && countDaughters ==2 ) {
    StatusCode scfillDoca=fillDoca(part, prefix, tuple);
    if (!scfillDoca) return scfillDoca;

    StatusCode fillIsolation_custom = fillIsolation(part, prefix, tuple);
    if (!fillIsolation_custom) return fillIsolation_custom;

    StatusCode scfillIso= CDFIsolation_2( part, part->daughtersVector().at(0), 
                                          part->daughtersVector().at(1), prefix, tuple); 
    if (!scfillIso) return scfillIso;

    StatusCode scfillCosNK = fillCosNK(part, prefix, tuple);
    if(!scfillCosNK) return scfillCosNK;
  }
  
  
  //   ====   BDTS for all B decays ==== ///
  if ( abs(part->particleID().pid()) >500 ) {
    StatusCode bdts = fillBDTS(part, myJpsi, JpsiInDau, prefix, tuple);
    if (!bdts) return bdts;

    bdts = fillLoKiTau(part, prefix, tuple );
    if(!bdts) return bdts;
  }

  // ---------------------------------------------//
  // === Fill these only for B-> 2 body decays ===//
  // ---------------------------------------------//
  
  if  (checkDau && countDaughters ==2 && abs(part->particleID().pid()) >500 ){
    
    StatusCode fillIsolation_custom = fillIsolation(part, prefix, tuple);
    if (!fillIsolation_custom) return fillIsolation_custom;
    
    //Dump the track isolation input variables.
    dumpIsoVariables(part, tuple);
    
    //Dump the isoBDT (it needs 4 BDT weight files)
    fillTrackIsoBDT(part,prefix,tuple);

    /*
    //Dump primary vertices info
    fillPVList(tuple);
    
    //Dump tracks vs PV doca and err(doca)
    fillTrkDocaPV(tuple);

    //Dump B pos and mom covariance matrix
    fillBposmomCov(part,prefix,tuple);

    //Dump Velo tracks info
    if (m_dumpVeloTracks) fillVeloTracksInfo(tuple);

    //Dump Velo tracks info
    if (m_dumpUpstreamTracks) fillUpstreamTracksInfo(tuple);
    */

    StatusCode scfillIso= CDFIsolation( part, part->daughtersVector().at(0),part->daughtersVector().at(1), prefix, tuple); 
    if (!scfillIso) return scfillIso;
    
    scfillIso= CDFIsolation_2( part, part->daughtersVector().at(0),part->daughtersVector().at(1), prefix, tuple); 
    if (!scfillIso) return scfillIso;
    
    StatusCode fillOtherB = OtherB(part, part->daughtersVector().at(0),part->daughtersVector().at(1), prefix, tuple);
    if(! fillOtherB) return fillOtherB;
    
    StatusCode scfillCosNK = fillCosNK(part, prefix, tuple);
    if(!scfillCosNK) return scfillCosNK;
  }//end checkDau && countDaughters==2 ...
  
  //Call fillMCDecayTree(...)
  if ( abs(part->particleID().pid()) >500 && m_dumpFullInfo ) {
    StatusCode fillTrack = fillTrackInfo( tuple ); 
    if (!fillTrack) return fillTrack;

    //Dump primary vertices info
    fillPVList(tuple);
    fillPVrefitList(part,tuple);
    
    //Dump tracks vs PV doca and err(doca)
    fillTrkDocaPV(tuple);

    //Dump B pos and mom covariance matrix
    fillBposmomCov(part,prefix,tuple);

    //Dump Velo tracks info
    if (m_dumpVeloTracks) fillVeloTracksInfo(tuple);

    //Dump Velo tracks info
    if (m_dumpUpstreamTracks) fillUpstreamTracksInfo(tuple);
  }
  if (m_isMC){
    StatusCode fillMCDecay = fillMCDecayTree(tuple);
    if (!fillMCDecay) return fillMCDecay;
    StatusCode fillMC = fillMCTruth(part, prefix, tuple);
    if (!fillMC) return fillMC;
  }

  return StatusCode::SUCCESS;
}//end fill(...)


//=============================================================================
//  Fill Eta
//=============================================================================
StatusCode TupleToolMuonVariables::fillEta(const LHCb::Particle *part,
					   const std::string prefix,
					   Tuples::Tuple& tuple ){
  
  bool test = tuple->column( prefix+"_eta", (part)->momentum().Eta()); 
  test &= tuple->column( prefix+"_phi", (part)->momentum().Phi()); 
  
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
}



//=============================================================================
// Track info, for muons only
//=============================================================================
StatusCode TupleToolMuonVariables::fillTrackHitInfo(const LHCb::Particle *part,
						 const std::string prefix,
						 Tuples::Tuple& tuple ){
  if( !part->isBasicParticle() )    return StatusCode::SUCCESS;
  
  if( abs(part->particleID().pid()) == 13 
      || abs( part->particleID().pid() )  == 11 ) {
    debug()<<"particle is "<<part->particleID().pid()<<endmsg; }
  else return StatusCode::SUCCESS;
  
  const LHCb::ProtoParticle* protop = part->proto();
  if(!protop) return StatusCode::SUCCESS;
  const LHCb::Track* track = protop->track();
  if(!track) return StatusCode::SUCCESS;
  int nVeloIDs(0), nITIDs(0), nOTIDs(0), nTTIDs(0);


  const std::vector<LHCb::LHCbID>& vids = track->lhcbIDs(); 
  
  for (std::vector<LHCb::LHCbID>::const_iterator iter = vids.begin() ; 
       iter != vids.end() ; ++iter)
    {
      if( iter->isVelo()) nVeloIDs++;
      if( iter->isTT()) nTTIDs++;
      if( iter->isIT()) nITIDs++;
      if( iter->isOT()) nOTIDs++;
    }

  bool test = tuple->column( prefix+"_nVeloIDs", nVeloIDs);
  test &= tuple->column( prefix+"_nITIDs", nITIDs);
  test &= tuple->column( prefix+"_nTTIDs", nTTIDs);
  test &= tuple->column( prefix+"_nOTIDs", nOTIDs);
  
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}

//=============================================================================
// Store the proper time from LoKi functor -> matches Blife_ps_r of CERN
//=============================================================================

StatusCode TupleToolMuonVariables::fillLoKiTau(const LHCb::Particle *part,
					       const std::string prefix,
					       Tuples::Tuple& tuple ){
  
  
  double light_cte = 1/0.299792458;
  const LoKi::Types::Fun CTAU = LoKi::Cuts::CTAU(  m_dva->bestVertex(part) );
  double temp_tau  = CTAU (part);
  temp_tau = temp_tau*light_cte;  // should be in ps now
  bool test = tuple->column( prefix+"_CTAU_ps", temp_tau);
  
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
}


//=============================================================================
// Store the BDTS from Cosme
//=============================================================================
StatusCode TupleToolMuonVariables::fillBDTS(const LHCb::Particle *part,
					    const LHCb::Particle *myJpsi,
					    bool JpsiInDau,
					    const std::string prefix,
					    Tuples::Tuple& tuple ){
  
  if( part->isBasicParticle() ) return StatusCode::SUCCESS;
  bool test =true;
  
  const LHCb::VertexBase* bpv = m_dva->bestVertex(part);

  if(JpsiInDau) debug()<<" From fillBDTS: I have found the Jpsi  "
                       << myJpsi->endVertex()->position() <<endreq;
  
  // 0 -> SQRT of IPCHI2 of the B
  const LoKi::Cuts::BPVIPCHI2 m_BPVIPCHI2; ///< BPVIPCHI2()
  m_values[0] = TMath::Sqrt(m_BPVIPCHI2(part));
  
  // 1-> DOCA of the muons
  const LoKi::ATypes::AFun AMINDOCA = LoKi::Cuts::AMINDOCA(m_dist);
  if(JpsiInDau) m_values[1] = AMINDOCA(myJpsi->daughtersVector())/Gaudi::Units::mm;
  else          m_values[1] = AMINDOCA(part->daughtersVector())/Gaudi::Units::mm;
  
  // 2-> Vertex chi2 of the B/ Jpsi
  if(JpsiInDau) m_values[2] = myJpsi->endVertex()->chi2();
  else          m_values[2] = part->endVertex()->chi2();  

  // 3-> DIRA of the B
  const LoKi::Types::Fun dira = LoKi::Cuts::DIRA(bpv);
  m_values[3] = dira(part);
  
  // 4-> IP of the B
  const LoKi::Types::Fun ip = LoKi::Cuts::IP(bpv,m_dist);
  m_values[4] = ip(part);
  
  // 5-> min of the IP of the two muons 
  const LoKi::Cuts::MIPDV mip  = LoKi::Cuts::MIPDV ( "" ) ;
  const LoKi::Types::AFun ipMin = LoKi::Cuts::AMINCHILD(mip);
  if (JpsiInDau)  m_values[5] = ipMin(myJpsi->daughtersVector())/Gaudi::Units::mm;
  else            m_values[5] = ipMin(part->daughtersVector())/Gaudi::Units::mm;
  
  debug() << "Fatima the values input into BDTD were "<< endreq;
  for (int i=0;i < m_nvariables; i++) debug() <<"  i "<<i<<" var: "<<m_values[i];
  debug() <<"\n"<<endreq;
  
  test &= tuple->column (prefix+"_BDTS_BIPS", m_values[0] );
  test &= tuple->column (prefix+"_BDTS_DOCA", m_values[1] );
  test &= tuple->column (prefix+"_BDTS_VTXCHI2", m_values[2] );
  test &= tuple->column (prefix+"_BDTS_DIRA", m_values[3] );
  test &= tuple->column (prefix+"_BDTS_BIP", m_values[4] );
  test &= tuple->column (prefix+"_BDTS_MuonMINIP", m_values[5] );
  
  
  
  // ------- evaluate and store the BDTS, from Francesco -----------
  double BDTout_BDTS = m_reader_BDTS->EvaluateMVA(m_methodName);
  
  if (!m_h_cumul_BDTS){
    error() << "BDTS: Cumulative histogram not present! " << endreq;
    return StatusCode::FAILURE;
  }
  
  int bin = m_h_cumul_BDTS->FindBin(BDTout_BDTS);   
  debug () << " BDTS: Evaluating the BDTS flat " << endreq;
  double BDTout_BDTS_flat = m_h_cumul_BDTS->GetBinContent(bin);
  
  
  debug()<<"The BDTS is "<<BDTout_BDTS_flat<<endreq;
  debug () << " BDTS: filling ntuple " << endreq;

  test &= tuple ->column (prefix+"_BDTS", BDTout_BDTS_flat); 
  debug() <<" BDTS was "<<  BDTout_BDTS_flat << endreq;
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}


//=============================================================================
// Other B quantities 
// From /afs/cern.ch/user/s/serranoj/public/Bs2MuMu/Bs2MuMuAnalysis.cpp
//=============================================================================

StatusCode TupleToolMuonVariables::OtherB(const LHCb::Particle *part, 
                                          const LHCb::Particle *vdau1, const LHCb::Particle *vdau2,
                                          const std::string prefix, Tuples::Tuple& tuple ){
  
  
  const LHCb::VertexBase* goodPV = m_dva->bestVertex(part);
  
  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();
  
  double m_otherB_mag=0.;
  double m_otherB_angle=-1.;
  double m_otherB_boost_mag=0.;
  double m_otherB_boost_angle=-1.;
  
  LHCb::Particles OTHERB_parts;
  int otherBtracks= 0;
  
  // in cppm code m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles";
  LHCb::Particles* m_allparts = get<LHCb::Particles>(m_ParticlePath);
  for ( LHCb::Particles::const_iterator ipp = m_allparts->begin(); ipp != m_allparts->end() ; ++ipp) {
    Gaudi::XYZVector p_track = ((*ipp)->proto()->track()->momentum());
    double pttrack = p_track.rho();
    double ptrackmag = p_track.R();
    double imp = 0.;
    double ips = -1.;
    m_dist->distance ( (*ipp), goodPV, imp, ips );
    ips = TMath::Sqrt(ips);
    
    if (ips>4.0 && ips<40.0 && pttrack>200.0 && pttrack<2000.0 
        && ptrackmag<30000.0 && ratio(pttrack, ptmu1) > 0.0001 && ratio(pttrack,ptmu2) > 0.0001) 
    {
      LHCb::Particle* track = *(ipp);
      OTHERB_parts.add(track) ;
      otherBtracks++;
    }
    
  }// ipp 
  
  
  if ( otherBtracks>0 )
    {
      //double otherB_boost_angle=0.;
      Gaudi::XYZVector ptproj(0.,0.,0.);
      LHCb::Particles::const_iterator ipp;
      for ( ipp = (&OTHERB_parts)->begin(); ipp != (&OTHERB_parts)->end() ; ++ipp) {
	Gaudi::XYZVector ptrack = ((*ipp)->proto()->track()->momentum());
	ptproj += ptrack;
      }
      
      m_otherB_mag = ptproj.R();
      m_otherB_angle=arcosine( ptproj, part->slopes());
      
      const LHCb::Particle *myDau = NULL;
      if (vdau1->charge()>0) myDau = vdau1;
      else if (vdau2->charge()>0) myDau = vdau2;
      
      // to calculate 
      //const void Bs2MuMuAnalysis::otherB_boost(const LHCb::Particle* B, 
      //const LHCb::Particle* muplus, LHCb::Particles* otherb, double& mag, double& angle)
      Gaudi::XYZVector ptproj2(0.,0.,0.);
      const Gaudi::LorentzVector p2(part->momentum());
      ROOT::Math::Boost boostToB( p2.BoostToCM() );
      const Gaudi::LorentzVector p3(myDau->momentum());
      const Gaudi::LorentzVector boostedMu = boostToB( p3 );
      const Gaudi::XYZVector pmuplus = Gaudi::XYZVector( boostedMu ) ;
      //HCb::Particles::const_iterator ipp;
      for ( ipp = (&OTHERB_parts)->begin(); ipp != (&OTHERB_parts)->end() ; ++ipp) {
	const Gaudi::LorentzVector p1((*ipp)->momentum());
	const Gaudi::LorentzVector boostedTrk = boostToB( p1 );
	const Gaudi::XYZVector ptrack = Gaudi::XYZVector( boostedTrk ) ;
	ptproj2 += ptrack;
      }
      
      m_otherB_boost_mag= ptproj2.R(); 
      m_otherB_boost_angle =  arcosine(pmuplus,ptproj2);
      
    } // if ntracks>0
  else {
    m_otherB_mag=0.;
    m_otherB_angle= -1; //1.;
    m_otherB_boost_mag=0.;
    m_otherB_boost_angle= -1; //(TMath::Pi())/2;
  }
  
  bool test = tuple->column( prefix+"_otherB_mag",  m_otherB_mag); 
  test &= tuple->column( prefix+"_otherB_ang",  m_otherB_angle); 
  test &= tuple->column( prefix+"_otherB_ntrk", otherBtracks);
  test &= tuple->column( prefix+"_otherB_boo_mag", m_otherB_boost_mag);
  test &= tuple->column( prefix+"_otherB_boo_ang", m_otherB_boost_angle);
  
  if (!test) return StatusCode::FAILURE;
  else return StatusCode::SUCCESS;
  
} 
  


//=============================================================================
// Jpsi vertex distanct chi2 wrt the PV which minimizes the B IPchi2
//=============================================================================
StatusCode TupleToolMuonVariables::fillFakeVtx(const LHCb::Particle *part,
					       const LHCb::Particle *myJpsi,
					       const std::string ,
					       Tuples::Tuple& tuple ){

  if( part->isBasicParticle() ) return StatusCode::SUCCESS;
  
  bool test =true;

  const LHCb::VertexBase* Bvertex = part->endVertex();
  const LHCb::VertexBase* JpsiVtx = myJpsi->endVertex();  
 
  double ip, chi2;
  double chi2final=-100; double  minipchi =-1;
  double temp_dist_noRefit=0.0;
  const LHCb::RecVertex::Range PVrange = m_dva->primaryVertices(); 
  if ( !PVrange.empty() )
    {
      if(msgLevel(MSG::VERBOSE)) 
        verbose() << "Filling Jpsi_dist_chi2 but the no of PVs is" << PVrange.size() << endmsg ;
      
      for ( LHCb::RecVertex::Range::const_iterator pv = PVrange.begin() ; pv!=PVrange.end() ; ++pv)
	{
	  StatusCode test2 = m_dist->distance ( *pv, Bvertex, ip, chi2 );
	  if( test2 )
	    {
	      if ( (chi2 < minipchi) || (minipchi<0.) )
		{
		  m_dist->distance ( *pv, JpsiVtx, ip, chi2 );
		  minipchi  = chi2;
		  chi2final = chi2;

		  const LoKi::Types::Fun BPVVDCHI2_b = LoKi::Cuts::VDCHI2(*pv);
		  temp_dist_noRefit = BPVVDCHI2_b(myJpsi); 
		}
	    }
	}// recVtx
    } // PVrange
  
  if(chi2final>=0) chi2final = TMath::Sqrt(chi2final);
  else chi2final = -1;

  test &= tuple ->column ("Jpsi_dist_chi2", chi2final);
  test &= tuple ->column ("Another_Jpsi_dist_chi2", TMath::Sqrt(temp_dist_noRefit));
  
  const LHCb::VertexBase* goodPV = m_dva->bestVertex(part);  
  double dist = -100; double distChi =-100;   
  StatusCode c1 = m_dist->distance( goodPV, JpsiVtx, dist, distChi);   
  if (! c1) distChi = -100;
  if(distChi>=0) distChi = TMath::Sqrt(distChi);
  else distChi = -1;
  test &= tuple ->column ("Jpsi_dist_chi2_r", distChi );
  
  
  const LoKi::Types::Fun BPVVDCHI2 = LoKi::Cuts::VDCHI2(goodPV);
  double temp_dist = BPVVDCHI2(myJpsi); 
  if(temp_dist>=0) temp_dist =  TMath::Sqrt(temp_dist);
  else temp_dist = -1;

  test &= tuple ->column ("Another_Jpsi_dist_chi2_r", temp_dist );

  if (test) return StatusCode::SUCCESS;
  else  return StatusCode::FAILURE;
  
}


//-------------------------------------------------------------------------
// isolation variables
//-------------------------------------------------------------------------

//
// compute Giampi's isolations and store them in ntuple
//
StatusCode TupleToolMuonVariables::fillIsolation(const LHCb::Particle *part,
						 const std::string prefix,
						 Tuples::Tuple& tuple ){
  //cout<<"Entro in fillIsolation(...). part = "<<part<<" prefix = "<<prefix<<endl;
  m_count_mum = 0;
  m_count_mup = 0;
  
  m_count_mum_f = 0;
  m_count_mup_f = 0;

  const LHCb::VertexBase* goodPV =  m_dva->bestVertex(part); //getRelatedPV(part);
  Gaudi::XYZVector PV_pos;
  
  if (NULL==goodPV) {
    warning() << "No best PV for " << part->particleID().pid() << endmsg ;
    return StatusCode::SUCCESS;
  }
  
  PV_pos = goodPV->position();
  
  const LHCb::Particle::ConstVector& pv = part->daughtersVector();
  
  int idx = 0;
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) 
    {
      if ( NULL==(*ipart)->proto() ) continue;
      idx++;
    }
  if(idx != 2 )  {
    return StatusCode::SUCCESS;
  }
  
  double pt_track;
  double pt_part;
  
  double pt_mu1(-1100.);
  double pt_mu2(-1100.);
  
  Gaudi::XYZVector pos_track;
  Gaudi::XYZVector pos_part;
  
  Gaudi::XYZVector mom_track;
  Gaudi::XYZVector mom_part;
  
  bool test =true; 
  std::string prefix1, prefix2; 

  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
  
    if(((*ipart)->charge())>0){
      pt_mu1=(*ipart)->proto()->track()->pt();
      prefix1 = prefix+"_D1";
    }
    
    if(((*ipart)->charge())<0) {
      pt_mu2=(*ipart)->proto()->track()->pt();
      prefix2 = prefix+"_D2";
    }
    
  }

  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
    
    const LHCb::Track*  mytrack= (*ipart)->proto()->track();

    pt_part= mytrack->pt();

    pos_part = mytrack->position();
    mom_part = mytrack->momentum();
    int fCountIso=0;
    LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
    typedef std::vector<const LHCb::Track*> TrackContainer ;
    for( LHCb::Tracks::const_iterator it = tracks->begin() ; it != tracks->end(); ++it) {
     
      if(!((*it)->checkType(LHCb::Track::Long))){  continue; }
      fCountIso++;
      pt_track = (*it)->pt();

      pos_track = (*it)->position();
      mom_track = (*it)->momentum();
      
      if(pt_track==pt_mu1)continue;
      if(pt_track==pt_mu2)continue; 

      double rapporto = TMath::Abs(pt_track-pt_part)/TMath::Abs(pt_track+pt_part); 
      if(rapporto<0.0001)continue;    
      
      Gaudi::XYZPoint vtx;
      Gaudi::XYZPoint close;
      Gaudi::XYZPoint close_mu;
      Gaudi::XYZPoint o( (*it) ->position());
      Gaudi::XYZPoint o_mu(mytrack->position());
      bool fail_f = false;
      double fc;
      closest_point(o,  mom_track, o_mu, mom_part, close, close_mu, vtx, fail_f); 
      
      Gaudi::XYZPoint rv_2 = vtx - PV_pos;
      Gaudi::XYZVector rv_vec  (rv_2.x(), rv_2.y(), rv_2.z());
      m_doca_tr  = (close-close_mu).R();
      double ZZ = rv_vec.z();
      double pt = mom_track.Rho()+ mom_part.Rho();
      Gaudi::XYZVector ptot(mom_track+mom_part);
      double alpha_2 = arcosine( rv_vec, ptot);
      double num_ff= ptot.R()*(alpha_2);
      double den_ff= num_ff +pt ;
      fc = num_ff/den_ff;
      
      bool flag_m = 0;
      bool flag_p = 0;
      
      if(((*ipart)->charge())>0) flag_p = 1;//mu-
      if(((*ipart)->charge())<0) flag_m = 1;//mu+
      
      if(flag_m && m_doca_tr<0.2 && ZZ>0 && ZZ<30 && fc <0.4 
         && fc >-1000 && pt_track>2.) m_count_mum++;// _f++;
      if(flag_p && m_doca_tr<0.2 && ZZ>0 && ZZ<30 && fc <0.4 
         && fc >-1000 && pt_track>2.) m_count_mup++; //_f++;
    
    }//loop tracks
    
  }//loop part
  

  //-------------------  Giampi's isolation: -------------------//
  int isotype = 5;
  debug()<<" GIAMPI just before calling getIso "<<endreq;
  
  //calling Giampi's tool. setting isotype=5 will output the new isolation variable 
  std::vector<int> iso5 = getIso( part, isotype, false);  
  std::vector<int> iso5_cut = getIso( part, isotype, true);  
  double m_count_mum_Giampi_cut, m_count_mup_Giampi_cut;

  std::vector<int> iso1_new = getIso( part, 66131, true);   // //  6 june 2013
  double m_count_mum_iso1, m_count_mup_iso1;

  std::vector<int> iso2_new = getIso( part, 66132, true);  
  double m_count_mum_iso2, m_count_mup_iso2;

  std::vector<int> isoD0_new = getIso( part, 66133, true);  
  double m_count_mum_isoD0, m_count_mup_isoD0;
  debug()<<" GIAMPI just after calling getIso "<<"GIAMPI  iso "<<iso5[0]<<"  "<<iso5[1]<<endreq;
  
  if (iso5[0]==-999) return StatusCode::FAILURE;
  
  if (pv.at(0)->charge() < 0) {
    m_count_mum_Giampi = static_cast<float>( iso5[0] );
    m_count_mup_Giampi = static_cast<float>( iso5[1] );
    
    m_count_mum_Giampi_cut = static_cast<float>( iso5_cut[0] );
    m_count_mup_Giampi_cut = static_cast<float>( iso5_cut[1] );
    
    m_count_mum_iso1 = static_cast<float>( iso1_new[0] );
    m_count_mup_iso1 = static_cast<float>( iso1_new[1] );
    m_count_mum_iso2 = static_cast<float>( iso2_new[0] );
    m_count_mup_iso2 = static_cast<float>( iso2_new[1] );
    m_count_mum_isoD0 = static_cast<float>( isoD0_new[0] );
    m_count_mup_isoD0 = static_cast<float>( isoD0_new[1] );
    
  }
  else{
    m_count_mum_Giampi = static_cast<float>( iso5[1] );
    m_count_mup_Giampi = static_cast<float>( iso5[0] );

    m_count_mum_Giampi_cut = static_cast<float>( iso5_cut[1] );
    m_count_mup_Giampi_cut = static_cast<float>( iso5_cut[0] );

    m_count_mum_iso1 = static_cast<float>( iso1_new[1] );
    m_count_mup_iso1 = static_cast<float>( iso1_new[0] );
    m_count_mum_iso2 = static_cast<float>( iso2_new[1] );
    m_count_mup_iso2 = static_cast<float>( iso2_new[0] );
    m_count_mum_isoD0 = static_cast<float>( isoD0_new[1] );
    m_count_mup_isoD0 = static_cast<float>( isoD0_new[0] );

  }

  debug()<<"filling tuple with Giampi's isolation vars "
         <<  m_count_mup_Giampi<<"  "<< m_count_mum_Giampi <<endmsg;
  
  test &= tuple->column( prefix1+"_isolation_Giampi", m_count_mup_Giampi);
  test &= tuple->column( prefix2+"_isolation_Giampi", m_count_mum_Giampi);

  test &= tuple->column( prefix1+"_isolation_Giampi_tc", m_count_mup_Giampi_cut);
  test &= tuple->column( prefix2+"_isolation_Giampi_tc", m_count_mum_Giampi_cut);

  test &= tuple->column( prefix1+"_isolation_Giampi_new1", m_count_mup_iso1);
  test &= tuple->column( prefix2+"_isolation_Giampi_new1", m_count_mum_iso1);
  test &= tuple->column( prefix1+"_isolation_Giampi_new2", m_count_mup_iso2);
  test &= tuple->column( prefix2+"_isolation_Giampi_new2", m_count_mum_iso2);
  test &= tuple->column( prefix1+"_isolation_Giampi_newD0", m_count_mup_isoD0);
  test &= tuple->column( prefix2+"_isolation_Giampi_newD0", m_count_mum_isoD0);
  
  //  std::cout<<"attention, filled ntuple (mup) "<<m_count_mup_iso1<<" "<<m_count_mup_iso2<<" "<<m_count_mup_isoD0<<std::endl;
  //  std::cout<<"attention, filled ntuple (mum) "<<m_count_mum_iso1<<" "<<m_count_mum_iso2<<" "<<m_count_mum_isoD0<<std::endl;
  //-------------------  Giampi's isolation ends -------------------//
  
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}//fillIsolation(part)



//=============================================================================
// method to calculate Giampi's isolation
//=============================================================================
std::vector<int>  TupleToolMuonVariables::getIso(const LHCb::Particle* B, int isotype, bool cutOnTracks){
  const LHCb::VertexBase *PV = m_dva->bestVertex(B);  
  /// Fatima, this will be the BPV of the jpsi in some cases!
  const LHCb::VertexBase *SV = B->endVertex();
  const LHCb::Particle::ConstVector parts = B->daughtersVector();  
  
  std::vector<int> iso(2, -9999);
  
  
  if(parts.size() !=2) {
    return iso;
  }
  
  StatusCode sc = StatusCode::SUCCESS;
  
  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  if (!allparts) {
    error() << " Failed to get particles container "
            <<  m_ParticlePath << endmsg;
    
    return iso;
  }
  
  
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();
  
  int i = 0;
  
  ROOT::Math::SVector<int, 2> iso0;
  iso0[0]=0;
  iso0[1]=0; 
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;

  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;

  //Loop over input particles, get their simple kinematics
  LHCb::Particle::ConstVector::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {    
    
    const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    const LHCb::Track* track = proto->track();
    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i] = sqrt(pow(p_mu(0,i),2)+pow(p_mu(1,i),2));
    i++;
  }
  
  if (i>2) { error()<<"more than 2 daughters of the B" <<endmsg;
    return iso;
  }
  
  
  for ( int i2 = 0; i2 < 2; i2++) {
    bool hltgood = false;
    double fc = 0.;
    Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
    Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));
    
    //Loop over all particles  
    LHCb::Particles::const_iterator ip;
    int j=0;  // for counting ntracks
    for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {
      
      const LHCb::ProtoParticle * proto =  (*ip)->proto();
      const LHCb::Track* track = proto->track();
      const LHCb::Particle*  cand = (*ip);
      Gaudi::XYZPoint o(track->position());
      Gaudi::XYZVector p(track->momentum());
      double pt = p.Rho();

      if(cutOnTracks && isotype ==5 && (track->ghostProbability()>0.3 || track->chi2PerDoF()> 3)) continue; 
      
      if (track->type()!=m_tracktype)   continue; 
      j++; 
      
      // skip if other particle is in input list 
      if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {  continue; }

      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc); 
      
      if (track->type()== m_tracktype) {
        iso0[i2] += hltgood;
        debug()<<"isisold "<< i2 << iso0[i2]<< endreq;
      }
      
      // find doca and angle between input and other tracks
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);          
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);
      
      // find impact parameters, distances from secondary and primary vertex
      double imp = 0.;
      double impchi2 = 0.;
      double ips,pvdis,svdis;
      ips=100000.;
      LHCb::RecVertex::Container::const_iterator iv;
      LHCb::RecVertex::Container* verts = NULL;
      
      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation))
      {
        verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
      }
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
        m_dist->distance(&(*cand),(*iv),imp,impchi2);
        if (impchi2<ips) ips = impchi2;
      }      
      ips=sqrt(ips);
      
      //      double deltaZvtx = (vtx.z()-PosPV.z());
      
      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
      
      // non-isolating criteria #5
      if (isotype==5 && angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips && 
          svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h ){
	iso5[i2] += 1;
      }// if 

     double iso_angle  =0.;
     double iso_fc = 0.;
     double iso_doca = 0.;
     double iso_ips = 0.;
     double iso_svdis=0.; double iso_svdis_h = 0.;
     double iso_pvdis=0.; double iso_pvdis_h = 0.;
     float  iso_track_clonedist = track->info(LHCb::Track::CloneDist, 10000);
     float  iso_track_ghost = 0.; 
     float  iso_track_trchi2 =0.;

     if(isotype == 66131){
       iso_angle=0.32;
       iso_fc=0.62;
       iso_doca=0.125;
       iso_ips=2.75;
       iso_svdis= -0.35;
       iso_svdis_h=1000000.;
       iso_pvdis= 0.7;
       iso_pvdis_h=1000000.;
       iso_track_clonedist = 99999.;
       iso_track_ghost = 0.35;
       iso_track_trchi2 = 4.;
     } 
     
     if(isotype == 66132){
       iso_angle=0.27;
       iso_fc=0.60;
       iso_doca=0.13;
       iso_ips=3.0;
       iso_svdis=-0.15;
       iso_svdis_h=330.;
       iso_pvdis=0.5;
       iso_pvdis_h=340.;
       iso_track_clonedist = 99999.;
       iso_track_ghost = 10000000;
       iso_track_trchi2 = 5.;
     }

     if(isotype == 66133){
       iso_angle=0.13;
       iso_fc=0.67;
       iso_doca=0.13;
       iso_ips=2.6;
       iso_svdis= -2.2;
       iso_svdis_h=1000000.;
       iso_pvdis= -0.8;
       iso_pvdis_h=1000000.;
       iso_track_clonedist = 99999.;
       iso_track_ghost = 0.6;
       iso_track_trchi2 = 5.;
     }  
     
     float mytrack_clone = track->info(LHCb::Track::CloneDist, 100);
     float mytrack_gp =  track->ghostProbability();
     float mytrack_trchi2 = track->chi2PerDoF();

     if (isotype >100 
	 && angle < iso_angle 
	 && fc < iso_fc 
	 && doca < iso_doca 
	 && ips > iso_ips 
	 && svdis > iso_svdis 
	 && svdis < iso_svdis_h 
	 && pvdis > iso_pvdis 
	 && pvdis < iso_pvdis_h
	 && mytrack_clone <  iso_track_clonedist
	 && mytrack_gp < iso_track_ghost
	 && mytrack_trchi2 < iso_track_trchi2
	 ) {
       iso5[i2] += 1;
     }//  if

     if(false){
     //if(true){
       std::cout<< " isotype was "<<isotype<<"   "<< angle <<" - "<< iso_angle<<" "<<
	 fc <<" - "<< iso_fc<< " \n "<<
	 doca <<" -  "<< iso_doca<<" \n "<<
	 ips <<" - "<< iso_ips<<" \n"<<
	 svdis <<" - "<< iso_svdis<<" \n"<<
	 svdis <<" - "<< iso_svdis_h<<"\n "<<
	 pvdis <<" - "<< iso_pvdis<<" \n"<<
	 pvdis <<" - "<< iso_pvdis_h<<" \n"<<
	 mytrack_clone<<" - "<<iso_track_clonedist<<" \n"<<
	 mytrack_gp <<" - "<< iso_track_ghost <<" \n "<<
	 mytrack_trchi2 <<" - "<<iso_track_trchi2<< std::endl;
     }

    }// allparts
    
  } //i2

  if (isotype==0) {
    iso[0] = iso0[0] ;
    iso[1] = iso0[1] ; 
  }
  else if ( isotype==5 ){
    iso[0] = iso5[0] ;
    iso[1] = iso5[1] ;
  }
  
  else if( isotype>100 ) {
    iso[0] = iso5[0] ;
    iso[1] = iso5[1] ;
  }

  return iso;

}//end getIso


//=============================================================================
// IsHLTGood method,used by isolation calculation
//=============================================================================
void  TupleToolMuonVariables::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
					 Gaudi::XYZPoint o_mu,Gaudi::XYZVector 
					 p_mu, Gaudi::XYZPoint PV, bool& hltgood,
					 double& fc) {
  //cout<<"Sono dentro IsHltGood"<<endl;
  Gaudi::XYZVector rv;
  Gaudi::XYZPoint vtx;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);
  closest_point(o,p,o_mu,p_mu,close,close_mu,vtx,fail);

  if (fail) {
    fc = -1.;
    hltgood = -1;
  }
  else {
    double pete = p.Rho();
    rv = vtx - PV;
    double DOCA_b = (close-close_mu).R();
    double ZZ = rv.z();
    fc = pointer(rv,p,p_mu);
    hltgood=( (DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.) );
  }
}


double TupleToolMuonVariables::pointer (Gaudi::XYZVector vertex,
                                 Gaudi::XYZVector p, Gaudi::XYZVector
                                 p_mu)  {
  //cout<<"Da IsHltGood entro in pointer per il calcolo di fc"<<endl;
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double temp = arcosine(vertex,ptot);
  double  num=ptot.R()*sin(temp);
  double  den=(num+pt);
  double fc = num/den;
  //cout<<"fc = "<<fc<<" alpha = "<<temp<<" sin(alpha) = "<<sin(temp)<<" num = "<<num<<" den = "<<den<<endl;
  return fc;
}

// MR: Consider 4 types of isoBDT depending on the set of input variables (4 weight files)
// For each isoBDT, compute the 3 largest isoBDT values of the longtracks wrt each of 
// the 2 muon tracks from the B cand
// Store the 2(muons)x4(weight files)x3=24 values in the ntuple
StatusCode TupleToolMuonVariables::fillTrackIsoBDT(const LHCb::Particle *B,
						   const std::string prefix,
						   Tuples::Tuple& tuple ){
  /*
  const LHCb::RecSummary * summary = get<LHCb::RecSummary>(LHCb::RecSummaryLocation::Default);
  int nlong=summary->info(LHCb::RecSummary::nLongTracks,-999);
  int nupstream=summary->info(LHCb::RecSummary::nUpstreamTracks,-999);
  //int nvelo=summary->info(LHCb::RecSummary::nVeloTracks,-999);//NB: this gives a number
  //larger than the actual num of Velo(+VeloR) tracks in the list LHCB::TrackLocation::Default
  //cout<<"nlong = "<<nlong<<" nupstream = "<<nupstream<<" nvelo = "<<nvelo<<endl;

  int mynlong(0),mynupstream(0),mynvelo(0),mynvelo2d(0);
  std::string m_inputLocation = LHCb::TrackLocation::Default;
  LHCb::Track::Range tracks  = get<LHCb::Track::Range>(m_inputLocation) ;
  int type_arr[11]; for (int j=0;j<11;j++) type_arr[j]=0;
  for (LHCb::Track::Range::const_iterator itrack = tracks.begin();itrack != tracks.end(); ++itrack) {
    if ((*itrack)->type()==LHCb::Track::Long) mynlong++;
    if ((*itrack)->type()==LHCb::Track::Upstream) mynupstream++;
    if ((*itrack)->type()==LHCb::Track::Velo) mynvelo++;
    if ((*itrack)->type()==LHCb::Track::VeloR) mynvelo2d++;
    const int index=(*itrack)->type();
    type_arr[index]++;
    //cout<<"type = "<<(*itrack)->type()<<" pz = "<<(*itrack)->momentum().z()<<endl;
  }
  for (int j=0;j<11;j++) cout<<"type_arr["<<j<<"] = "<<type_arr[j]<<endl;
  //12/mar/2015: provo a prendere la lista delle protoparticles cariche
  //track->type() map: 0=unknown,1=Velo,2=VeloR,3=Long,4=Upstream,5=Downstream,6=Ttrack,7=Muon,8=Calo,9=TT,10=UT
  int mynlong2(0),mynupstream2(0),mynvelo2(0),mynvelo2d2(0);
  int type_arr2[11]; for (int j=0;j<11;j++) type_arr2[j]=0;
  std::string m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  LHCb::ProtoParticles * protos = getIfExists<ProtoParticles>(m_protoPath);
  for ( ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto )
    {
      const Track* track=(*iProto)->track();
      //int type=track->type();
      //updateRICH(*iProto);
      if (track->type()==LHCb::Track::Long) mynlong2++;
      if (track->type()==LHCb::Track::Upstream) mynupstream2++;
      if (track->type()==LHCb::Track::Velo) mynvelo2++;
      if (track->type()==LHCb::Track::VeloR) mynvelo2d2++;
      const int index=track->type();
      type_arr2[index]++;
    }
  for (int j=0;j<11;j++) cout<<"type_arr2["<<j<<"] = "<<type_arr2[j]<<endl;

  */
  //Get the best PV wrt the B, and the B vertex
  const LHCb::VertexBase *PV = m_dva->bestVertex(B);  
  const LHCb::VertexBase *SV = B->endVertex();

  //Get the vector with the B cand daughters. If B has not 2 daughters, exit
  const LHCb::Particle::ConstVector parts = B->daughtersVector();  
  if(parts.size() !=2) {
    error()<<"Number of B daughters != 2: "<<parts.size()<<endmsg;
    return StatusCode::FAILURE;
  }
  
  //define the arrays containing the 3 highest isobdt wrt muplus
  double mup_isobdt_1st_vec[4];
  double mup_isobdt_2nd_vec[4];
  double mup_isobdt_3rd_vec[4];

  //define the arrays containing the 3 highest isobdt wrt muminus
  double mum_isobdt_1st_vec[4];
  double mum_isobdt_2nd_vec[4];
  double mum_isobdt_3rd_vec[4];


  //
  //Loop over the B cand daughters
  //
  const LHCb::Track* mup_trk=parts[0]->proto()->track();
  const LHCb::Track* mum_trk=parts[1]->proto()->track();
  
  LHCb::Particle::ConstVector::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {  
    const LHCb::Particle* part = *ip_part;
    //Get the particle list (longtracks). It if has a null pointer, exit 
    LHCb::Particles* particles = get<LHCb::Particles>(m_ParticlePath);
    if (!particles) {
      error()<<"Failed to get particles container "<<m_ParticlePath<<endmsg;
      return StatusCode::FAILURE;
    }
    //set A)
    double log_trk_ips = 0;//log(IP significance of the track)
    double trk_pvdis=0;
    double trk_svdis=0;
    double trk_doca=0;
    double trk_angle=0;
    double trk_fc=0;
    //set B)
    double trk_gho=0;//track ghost probability
    double trk_chi=0;//track chi2
    //set C)
    double trk_dphi=0;//abs(trk_phi - muon_phi)
    double trk_deta=0;//abs(trk pseudorapidity - muon pseudorapidity)
    double trk_pt=0;//track pT
    //set D)
    double trk_ch=0;//charge(track)*charge(muon)
    double trk_nnmu=0;//track ProbNN(mu)
    double trk_nnpi=0;//track ProbNN(pi)
    double trk_nnk=0;//track ProbNN(K)
    
    //get the track from the input part
    const LHCb::Track* part_track=part->proto()->track();

    //define the arrays containing the 3 highest isobdt (wrt the current B dau track) for each 
    //of the 4 isoBDT implementations
    double isobdt_1st_vec[4];
    double isobdt_2nd_vec[4];
    double isobdt_3rd_vec[4];
    for (int i=0;i<4;i++){ isobdt_1st_vec[i]=-10; isobdt_2nd_vec[i]=-10; isobdt_3rd_vec[i]=-10; }
    
    //
    // Start loop over the longtracks
    //
    int counter(0);
    LHCb::Particles::const_iterator part_it, part_it_end( particles->end() ) ;
    for ( part_it = particles->begin() ; part_it != part_it_end ; ++part_it ) {
      const LHCb::Track * track = (*part_it)->proto()->track() ;
      //cout<<"track = "<<track<<" part_track = "<<part_track<<" mum_trk = "<<mum_trk<<" mup_trk = "<<mup_trk<<endl;
      //if the track is equal to one of the two muons, skip the track
      if (track==mup_trk || track==mum_trk) continue;
      //if the track is not longtrack skip the track
      if (track->type()!=3) continue;

      //get p,x of track and muon
      Gaudi::XYZVector trackMomentum = track->momentum();
      Gaudi::XYZPoint trackPosition = track->position();
      Gaudi::XYZVector partMomentum = part_track->momentum();
      Gaudi::XYZPoint partPosition = part_track->position();
      
      //
      //compute the isoBDT input variables
      //
      //set A)
      log_trk_ips = log(calcIPToAnyPV(*part_it));
      // Get the (track,muon) "vertex" (does not exploit the error track)
      Gaudi::XYZPoint perpFoot_track, perpFoot_daughter;
      Gaudi::XYZPoint vertex_mu_track;
      bool failed = false;
      getPerpFeet( trackPosition, trackMomentum, partPosition, partMomentum,perpFoot_track, perpFoot_daughter, vertex_mu_track, failed);
      if(failed && msgLevel(MSG::DEBUG) ) debug() << "Vertex calculation ( perpendicular feet ) failed: denom == 0!" << endmsg;
      double pvDist,pvDistChi2 ;
      if(PV) {
	StatusCode sc_pv    = m_dist->distance(PV, vertex_mu_track, pvDist, pvDistChi2);
	trk_pvdis     = calcVertexDist(vertex_mu_track, PV);
      }
      double svDist, svDistChi2 ;
      if(SV) {
	StatusCode sc_sv    = m_dist->distance(SV, vertex_mu_track, svDist, svDistChi2);
	trk_svdis     = calcVertexDist(vertex_mu_track, SV);
      }
      StatusCode sc_doca  = m_dist->distance(track,part_track,trk_doca);
      //24/2/15 MR: for consistency take same simplified doca as used by getIso(...) to compute the 'classic' muon iso
      double doca_simplified;
      double angle_temp;
      Gaudi::XYZPoint vtx_temp(0.,0.,0.);
      InCone(track->position(),track->momentum(),part_track->position(),part_track->momentum(),vtx_temp,doca_simplified,angle_temp);
      //cout<<"trk_doca = "<<trk_doca<<" doca_simplified = "<<doca_simplified<<" diff = "<<trk_doca-doca_simplified<<endl;
      trk_doca=doca_simplified;
      if(!sc_doca)  return StatusCode(sc_doca);
      trk_angle         = enclosedAngle(trackMomentum, partMomentum);
      if(PV)
	trk_fc = calcFC(  trackMomentum, partMomentum, vertex_mu_track, PV);
      if(trk_fc==-1 && msgLevel(MSG::DEBUG) ) debug() << "FC calculation failed: fc_denom == 0!" << endmsg;
      //set B)
      trk_gho = track->ghostProbability();
      trk_chi = track->chi2PerDoF();
      //set C)
      trk_dphi = fabs(track->momentum().Phi() - part_track->momentum().Phi());
      trk_deta = fabs(track->momentum().Eta() - part_track->momentum().Eta());
      trk_pt = track->pt();
      //set D)
      trk_ch = (track->charge())*(part_track->charge());
      trk_nnmu = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNmu,-9999);
      if (trk_nnmu<=0) trk_nnmu=-0.1;
      trk_nnpi = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNpi,-9999);
      trk_nnk = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNk,-9999);
      
      isobdt_values[0]=log_trk_ips;
      isobdt_values[1]=trk_pvdis;
      isobdt_values[2]=trk_svdis;
      isobdt_values[3]=log(trk_doca);
      isobdt_values[4]=trk_angle;
      isobdt_values[5]=trk_fc;
      isobdt_values[6]=trk_gho;
      isobdt_values[7]=trk_chi;
      isobdt_values[8]=trk_dphi;
      isobdt_values[9]=trk_deta;
      isobdt_values[10]=trk_pt;
      isobdt_values[11]=trk_ch;
      isobdt_values[12]=trk_nnmu;
      isobdt_values[13]=trk_nnpi;
      isobdt_values[14]=trk_nnk;
      
      if (0){
	cout<<"Traccia num :"<<counter++<<endl;
	cout<<"log(trk_ips) = "<<log_trk_ips<<endl;
	cout<<"trk_pvdis = "<<trk_pvdis<<endl;
	cout<<"trk_svdis = "<<trk_svdis<<endl;
	cout<<"log(trk_doca) = "<<log(trk_doca)<<endl;
	cout<<"trk_angle = "<<trk_angle<<endl;
	cout<<"trk_fc = "<<trk_fc<<endl;
	cout<<"trk_dphi = "<<trk_dphi<<endl;
	cout<<"trk_deta = "<<trk_deta<<endl;
	cout<<"trk_pt = "<<trk_pt<<endl;
	cout<<"trk_ch = "<<trk_ch<<endl;
	cout<<"trk_nnmu = "<<trk_nnmu<<endl;
	cout<<"trk_nnpi = "<<trk_nnpi<<endl;
	cout<<"trk_nnk = "<<trk_nnk<<endl;
      }
      //
      //For each of the 4 isoBDT variants, store the 3 highest isoBDT values among all tracks  
      //
      double isobdt_val[4];
      for (int i=0;i<4;i++){//loop over the 4 isoBDT variants
	isobdt_val[i]=isobdt_reader[i]->EvaluateMVA(isobdt_methodName);
	if (isobdt_val[i]>isobdt_1st_vec[i]){
	  isobdt_3rd_vec[i]=isobdt_2nd_vec[i];
	  isobdt_2nd_vec[i]=isobdt_1st_vec[i];
	  isobdt_1st_vec[i]=isobdt_val[i] ;
	}
	else if (isobdt_val[i]>isobdt_2nd_vec[i]) {
	  isobdt_3rd_vec[i]=isobdt_2nd_vec[i];
	  isobdt_2nd_vec[i]=isobdt_val[i];
	}
	else if (isobdt_val[i]>isobdt_3rd_vec[i]) {
	  isobdt_3rd_vec[i]=isobdt_val[i];
	}
      }//end i<4 loop
    }//end longtracks loop
    
    for (int i=0;i<4;i++){
      if (part->particleID().pid()==13){
	mup_isobdt_1st_vec[i]=isobdt_1st_vec[i];
	mup_isobdt_2nd_vec[i]=isobdt_2nd_vec[i];
	mup_isobdt_3rd_vec[i]=isobdt_3rd_vec[i];
      }
      else if (part->particleID().pid()==-13){
	mum_isobdt_1st_vec[i]=isobdt_1st_vec[i];
	mum_isobdt_2nd_vec[i]=isobdt_2nd_vec[i];
	mum_isobdt_3rd_vec[i]=isobdt_3rd_vec[i];
      }
      else {
	cout<<"ERROR: B dau is not a muon"<<endl;
      }
    }//end i<4 loop
  }//end B cand daughters loop
  
  //
  // Fill the ntuple
  //
  for (int i=0;i<4;i++){
    TString flag=prefix+"_D1_isobdt"; flag+=i+1;
    tuple->column((flag+"_1st").Data(),mup_isobdt_1st_vec[i]);
    tuple->column((flag+"_2nd").Data(),mup_isobdt_2nd_vec[i]);
    tuple->column((flag+"_3rd").Data(),mup_isobdt_3rd_vec[i]);
    flag=prefix+"_D2_isobdt"; flag+=i+1;
    tuple->column((flag+"_1st").Data(),mum_isobdt_1st_vec[i]);
    tuple->column((flag+"_2nd").Data(),mum_isobdt_2nd_vec[i]);
    tuple->column((flag+"_3rd").Data(),mum_isobdt_3rd_vec[i]);
  }//end i<4 loop
  
 return StatusCode::SUCCESS;
 
}//end fillTrackIsoBDT

StatusCode TupleToolMuonVariables::fillPVList(Tuples::Tuple& tuple){
  //Get the list of primary vertices and store their vtx info in ntuple
  LHCb::RecVertex::Container* verts = NULL;
  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  LHCb::RecVertex::Container::const_iterator iv;
  vector<double> pv_x,pv_y,pv_z;
  vector<double> pv_covxx,pv_covyy,pv_covzz,pv_covxy,pv_covxz,pv_covyz;
  vector<double> pv_chi2ndof;
  vector<int> pv_ndof,pv_ntrk;
  for ( iv = verts->begin(); iv != verts->end(); iv++) {
    Gaudi::XYZPoint PosPV = (*iv)->position();
    const Gaudi::SymMatrix3x3 & vtx = (*iv)->covMatrix();
    pv_x.push_back(PosPV.x());
    pv_y.push_back(PosPV.y());
    pv_z.push_back(PosPV.z());
    pv_covxx.push_back(vtx(0,0));
    pv_covyy.push_back(vtx(1,1));
    pv_covzz.push_back(vtx(2,2));
    pv_covxy.push_back(vtx(0,1));
    pv_covxz.push_back(vtx(0,2));
    pv_covyz.push_back(vtx(1,2));
    pv_chi2ndof.push_back((*iv)->chi2PerDoF());
    pv_ndof.push_back((*iv)->nDoF());
    //std::vector<const LHCb::Particle*> outgoing_part_vec=(*iv)->outgoingParticlesVector();
    const SmartRefVector<LHCb::Track> outgoing_part_vec=(*iv)->tracks();
    pv_ntrk.push_back((int)outgoing_part_vec.size());
  }
  bool test=true;
  int PV_maxnum=30;
  test &= tuple->farray("PV_x",pv_x.begin(),pv_x.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_y",pv_y.begin(),pv_y.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_z",pv_z.begin(),pv_z.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covxx",pv_covxx.begin(),pv_covxx.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covyy",pv_covyy.begin(),pv_covyy.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covzz",pv_covzz.begin(),pv_covzz.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covxy",pv_covxy.begin(),pv_covxy.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covxz",pv_covxz.begin(),pv_covxz.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_covyz",pv_covyz.begin(),pv_covyz.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_chi2PerDoF",pv_chi2ndof.begin(),pv_chi2ndof.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_nDoF",pv_ndof.begin(),pv_ndof.end(),"PV_num",PV_maxnum);
  test &= tuple->farray("PV_ntrk",pv_ntrk.begin(),pv_ntrk.end(),"PV_num",PV_maxnum);

  if (test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;

}//end fillPVList

//store in ntuple list of refitted PVs (with no tracks from the B candidate)
StatusCode TupleToolMuonVariables::fillPVrefitList(const LHCb::Particle* part,Tuples::Tuple& tuple){
  //Get the list of primary vertices and store their vtx info in ntuple
  LHCb::RecVertex::Container* verts = NULL;
  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  LHCb::RecVertex::Container::const_iterator iv;
  vector<double> pv_x,pv_y,pv_z;
  vector<double> pv_covxx,pv_covyy,pv_covzz,pv_covxy,pv_covxz,pv_covyz;
  vector<double> pv_chi2ndof;
  vector<int> pv_ndof,pv_ntrk;
  for ( iv = verts->begin(); iv != verts->end(); iv++) {
    RecVertex newPV(**iv);
    StatusCode scfit = m_pvReFitter->remove(part, &newPV);
    if(!scfit) { Warning("ReFitter fails!",StatusCode::SUCCESS,10).ignore(); continue; }
    //LHCb::VertexBase* newPVPtr = (LHCb::VertexBase*)&newPV;
    RecVertex* newPVPtr = &newPV;

    Gaudi::XYZPoint PosPV = newPVPtr->position();
    const Gaudi::SymMatrix3x3 & vtx = newPVPtr->covMatrix();
    pv_x.push_back(PosPV.x());
    pv_y.push_back(PosPV.y());
    pv_z.push_back(PosPV.z());
    pv_covxx.push_back(vtx(0,0));
    pv_covyy.push_back(vtx(1,1));
    pv_covzz.push_back(vtx(2,2));
    pv_covxy.push_back(vtx(0,1));
    pv_covxz.push_back(vtx(0,2));
    pv_covyz.push_back(vtx(1,2));
    pv_chi2ndof.push_back(newPVPtr->chi2PerDoF());
    pv_ndof.push_back(newPVPtr->nDoF());
    const SmartRefVector<LHCb::Track> outgoing_part_vec=newPVPtr->tracks();
    pv_ntrk.push_back((int)outgoing_part_vec.size());
  }
  bool test=true;
  int PV_maxnum=30;
  test &= tuple->farray("PV2_x",pv_x.begin(),pv_x.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_y",pv_y.begin(),pv_y.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_z",pv_z.begin(),pv_z.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covxx",pv_covxx.begin(),pv_covxx.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covyy",pv_covyy.begin(),pv_covyy.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covzz",pv_covzz.begin(),pv_covzz.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covxy",pv_covxy.begin(),pv_covxy.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covxz",pv_covxz.begin(),pv_covxz.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_covyz",pv_covyz.begin(),pv_covyz.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_chi2PerDoF",pv_chi2ndof.begin(),pv_chi2ndof.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_nDoF",pv_ndof.begin(),pv_ndof.end(),"PV2_num",PV_maxnum);
  test &= tuple->farray("PV2_ntrk",pv_ntrk.begin(),pv_ntrk.end(),"PV2_num",PV_maxnum);

  if (test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;

}//end fillPVrefitList

// MR: Ideally the info would go into the EVB block. However, that would require 
// storing in the ntuple a matrix with a variable-size num. of rows and columns.
// Unfortunately it's only possible to store a matrix with variable number of rows but
// fixed num of columns using the 'fmatrix' Tuple method. Hence, the solution below was
// adopted (in order not to waste space using fmatrix)
StatusCode TupleToolMuonVariables::fillTrkDocaPV(Tuples::Tuple& tuple){
  vector<double> PVTRK_ip,PVTRK_ips;

  int PVTRK_maxnum=7500;
  int counter(0);
  int i_pv=0;
  LHCb::RecVertex::Container* verts = NULL;
  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  LHCb::RecVertex::Container::const_iterator iv;
  for ( iv = verts->begin(); iv != verts->end(); iv++) {
    LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
    if (!parts) { error() << " Failed to get particles container " << endmsg; return StatusCode::SUCCESS; }
    double ip,ips;
    LHCb::Particles::const_iterator ipp;
    int i_trk=0;
    for(ipp= parts->begin();ipp!=parts->end();ipp++){
      m_dist->distance(*ipp,*iv,ip,ips);
      PVTRK_ip.push_back(ip);
      PVTRK_ips.push_back(ips);
      i_trk++;
      counter++;
      if (counter>PVTRK_maxnum) break;
    }
    i_pv++;
  }//end loop over the PVs
  bool test=true;
  test &= tuple->farray("PVTRK_ip",PVTRK_ip.begin(),PVTRK_ip.end(),"PVTRK_num", PVTRK_maxnum);
  test &= tuple->farray("PVTRK_ips",PVTRK_ips.begin(),PVTRK_ips.end(),"PVTRK_num", PVTRK_maxnum);
  if (test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}//end fillTrkDocaPV

//=============================================================================
// Fill DOCA:
//=============================================================================
StatusCode TupleToolMuonVariables::fillDoca(const LHCb::Particle *part, 
					     const std::string prefix, 
					     Tuples::Tuple& tuple ){
  bool test = true;
  
  double doca=-999;

  const LHCb::Particle::ConstVector pv = part->daughtersVector();

  const LHCb::Particle *imp, *imm;
  int idx = 0;
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
    
    if ( NULL==(*ipart)->proto() ) continue;
    
    if(idx == 0) {imp = (*ipart);}
    else if(idx == 1) {imm = (*ipart);}
    idx++;
  }
  if(idx != 2)  {
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc_s = m_dist->distance(imp, imm, doca);
  if(sc_s)m_doca=doca;
  
  test &= tuple->column( prefix+"_doca",  m_doca);
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}


//=============================================================================
// Fill cosnk
//=============================================================================

StatusCode TupleToolMuonVariables::fillCosNK(const LHCb::Particle *part, 
					      const std::string prefix, 
					      Tuples::Tuple& tuple ){
  
  //---------------------------------------------------------------------
  // (by Marco)
  // Translation of Yury's python code
  
  bool test = true;
  
  LHCb::Particle::ConstVector vdaugh = part->daughtersVector();
  int idx=0;
  LHCb::Particle::ConstVector::const_iterator it;
  for( it = vdaugh.begin(); vdaugh.end()!=it; ++it ){
    if ( NULL==(*it)->proto() ) continue;
    else idx++;
  }
  
  if(idx != 2)  {
    return StatusCode::SUCCESS;
  }
  
  
  double px_mu_minp;
  double px_part;
  double py_mu_minp;
  double py_part;

  //masses PDG
  
  double mass_mu = 105.658;
  double mmass_B;
  
  px_part = part->momentum().px();
  py_part = part->momentum().py();
  mmass_B = part->measuredMass();
  
  debug() <<"Inside cosnk, obtained B quantities" <<endreq;
  
  //  if ( vdaugh.at(0)->momentum().pt() < vdaugh.at(1)->momentum().pt() ) {
  if ( vdaugh.at(0)->charge()> 0 ) {
    debug() <<"obtaining daughter quantities" <<endreq;
    px_mu_minp = vdaugh.at(0)->momentum().px();
    py_mu_minp = vdaugh.at(0)->momentum().py();
  } else {
    debug() << "obtaining daughter quantities" <<endreq;
    px_mu_minp = vdaugh.at(1)->momentum().px();
    py_mu_minp = vdaugh.at(1)->momentum().py();
  }

  m_cosnk = -2;
  if((mmass_B*mmass_B - 4*mass_mu*mass_mu) && (px_part*px_part + py_part*py_part)) 
    m_cosnk = 2*(py_mu_minp*px_part - px_mu_minp*py_part) / 
      sqrt(px_part*px_part + py_part*py_part) / sqrt(mmass_B*mmass_B - 4*mass_mu*mass_mu);
  
  debug() <<"filling cosnk =   "<<m_cosnk << endreq;
  test &= tuple->column( prefix+"_cosnk",  m_cosnk );

  if(test)   return StatusCode::SUCCESS;  
  else return StatusCode::FAILURE;
  
}



//=============================================================================
// Other functions needed by isolation 
//=============================================================================

double TupleToolMuonVariables::getphi(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  double dphi = vdau1->momentum().Phi() - vdau2->momentum().Phi();
  return dphi;
}


//=============================================================================
double TupleToolMuonVariables::gettheta(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  
  double dtheta = vdau1->momentum().Eta() -  vdau2->momentum().Eta();
  return dtheta;
}


//=============================================================================
double TupleToolMuonVariables::ratio( double p1, double p2){  
  return fabs(p1 -p2)*(1./fabs(p1+p2)); 
}



//=============================================================================
double TupleToolMuonVariables::IsClose(const LHCb::Particle* p1,const LHCb::Particle* p2) {
  
  double deta = gettheta(p1,p2);
  double dphi = getphi(p1,p2);
  return sqrt(deta*deta+dphi*dphi);
}


//=============================================================================
void TupleToolMuonVariables::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
					    Gaudi::XYZPoint o_mu,Gaudi::XYZVector p_mu, 
					    Gaudi::XYZPoint& close1, 
					    Gaudi::XYZPoint& close2, 
					    Gaudi::XYZPoint& vertex, bool& fail) {
  
  
  Gaudi::XYZVector v0(o - o_mu);
  Gaudi::XYZVector v1(p.unit());
  Gaudi::XYZVector v2(p_mu.unit());
  Gaudi::XYZPoint temp1(0.,0.,0.);
  Gaudi::XYZPoint temp2(0.,0.,0.);
  fail = false;
  double  d02 = v0.Dot(v2); 
  double  d21 = v2.Dot(v1); 
  double  d01 = v0.Dot(v1);
  double  d22 = v2.Dot(v2);
  double  d11 = v1.Dot(v1); 
  double  denom = d11 * d22 - d21 * d21; 
  if (fabs(denom) <= 0.) {
    close1 = temp1;
    close2 = temp2;
    fail = true;
  }
  else {
    double numer = d02 * d21 - d01 * d22; 
    double mu1 = numer / denom;            
    double mu2 = (d02 + d21 * mu1) / d22; 
    close1 = o+v1*mu1;
    close2 = o_mu+v2*mu2;
  }
  vertex = (close1+(close2-close1)*0.5);
}


double TupleToolMuonVariables::arcosine(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {
  
  double num    = (p1.Cross(p2)).R();
  double den    = p1.R()*p2.R();
  double seno   = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha  = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;
  }
  return alpha;
}



//=============================================================================
void TupleToolMuonVariables::InCone(Gaudi::XYZPoint o1, 
				     Gaudi::XYZVector p1,Gaudi::XYZPoint o2, 
				     Gaudi::XYZVector p2,
				     Gaudi::XYZPoint& vtx, double& 
				     doca, double& angle){
  
  Gaudi::XYZPoint rv;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);
  closest_point(o1,p1,o2,p2,close,close_mu,vtx, fail);
  if (fail) {
    doca =-1.;
    angle=-1.;
  } 
  else {  
    doca = (close-close_mu).R();
    angle = arcosine(p1,p2);
  }
}

//=============================================================================
// B mass, by constraining the Jpsi mass. For 3/4 body decays. 
//=============================================================================
StatusCode TupleToolMuonVariables::ConstrMass(const LHCb::Particle* B, 
                                              const std::string prefix, Tuples::Tuple& tuple ){
  
  const LoKi::Types::Fun  constrainedMass = 
    LoKi::Cuts::DTF_FUN (LoKi::Cuts::M , true , "J/psi(1S)" ) ;
  //  CERN code   CandidateInfo["Bmass_JC"] = DTF_FUN(M,True,"J/psi(1S)")(b)
  bool   test = tuple->column( prefix+"_JCMass", constrainedMass(B) );
  if(!test) return StatusCode::FAILURE;
  else return StatusCode::SUCCESS;
  
}

//=============================================================================
// To match the CERN definition... should implement this in CDFisolation and remove this
//=============================================================================
StatusCode TupleToolMuonVariables::CDFIsolation_2(const LHCb::Particle* B,
						 const LHCb::Particle* vdau1,
						 const LHCb::Particle* vdau2,
						 const std::string prefix, 
						 Tuples::Tuple& tuple ){
  
  bool test = true;
  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug()<<"Inside CDFisloation_2, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }

  
  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho(); //vdau2->proto()->track()->pt(); 
  double pt_bs = B->momentum().rho(); //pt();

  double iso_giampi = 0.0;
  double iso_giampi_tc = 0.0;
     
  for(LHCb::Particles::const_iterator ipp=parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	
        Gaudi::XYZVector ptrack =  ((*ipp)->proto()->track()->momentum()); 
        //double ptrack = atrack->p(); //
        double pttrack = ptrack.rho(); //   (*ipp)->pt();  //  

        double del1 = (TMath::Abs (ptmu1 - pttrack))/(TMath::Abs (ptmu1 + pttrack));
        double del2 = (TMath::Abs (ptmu2 - pttrack))/(TMath::Abs (ptmu2 + pttrack));
        if ( del1 > 0.0001 && del2 > 0.0001 ){
	  
          double deta      =  B->momentum().Eta() - (*ipp)->momentum().Eta() ; 
          double delta_phi =  B->momentum().Phi() - (*ipp)->momentum().Phi(); 
          delta_phi = TMath::Abs(delta_phi);
	  
          if (delta_phi > TMath::Pi() )  delta_phi = 2*TMath::Pi() - delta_phi;   
	  
          double rad_cone = TMath::Sqrt(  (delta_phi*delta_phi + deta*deta) );
          if (  rad_cone <=1.0)
          {
            iso_giampi += pttrack;

            if(atrack->ghostProbability()<0.3 && atrack->chi2PerDoF()< 3){
              iso_giampi_tc += pttrack;
            }
          }
	    
        }// del1/2
      } // atrack
    } //proto
  } // ipp
  
  iso_giampi = pt_bs/(iso_giampi+pt_bs);
  test &= tuple->column( prefix+"_yetanother_CDF_iso", iso_giampi);
  
  //Version with track cuts
  iso_giampi_tc = pt_bs/(iso_giampi_tc+pt_bs);
  test &= tuple->column( prefix+"_yetanother_CDF_iso_tc", iso_giampi_tc);
  
  return test;
  
}



//=============================================================================
// compute CDF isolation variable
//=============================================================================
StatusCode TupleToolMuonVariables::CDFIsolation(const LHCb::Particle* B,
                                                const LHCb::Particle* vdau1,
                                                const LHCb::Particle* vdau2,
                                                const std::string prefix, 
                                                Tuples::Tuple& tuple){
  
  
  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug()<<"Inside CDFisloation, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  bool test = true ;
  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();  
  double ptbs  =  B->momentum().rho(); 
  double ptclose = 0.; 
  double ptclose_tc = 0.;
  double isoCDF;
  double isoCDF_tc;
  
  debug()<<" pts:: "<<ptmu1<<" "<<ptmu2<<" "<<ptbs<<endreq;

  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Particles::const_iterator ipp;

  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	
        Gaudi::XYZVector ptrack(atrack->momentum());
        double pttrack = ptrack.rho();
        
        if (ratio(pttrack, ptmu1) > 0.0001 && ratio(pttrack,ptmu2) > 0.0001) 
        { 
          if ( IsClose(B,(*ipp)) <= 1.0)
          {
            ptclose += pttrack;
            
            // Cuts go here
            if(atrack->ghostProbability()<0.3 && atrack->chi2PerDoF()< 3){
              ptclose_tc += pttrack;
            }
          }
        }
      }// if atrack
    } // if proto
  } // allparts
  
  isoCDF=ptbs/(ptclose+ptbs);

  m_CDFIso = isoCDF;
  test &= tuple->column( prefix+"_CDFiso", m_CDFIso);
  
  // version with track cuts
  isoCDF_tc=ptbs/(ptclose_tc+ptbs);
  test &= tuple->column( prefix+"_CDFiso_tc", isoCDF_tc);
  
  
  if (test)   return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}



//=============================================================================
// CDFisolation, directly from diego (Flavio)
//=============================================================================

StatusCode TupleToolMuonVariables::CDFIsolation_Yuri(const LHCb::Particle* B,
						      const LHCb::Particle* vdau1,
						      const LHCb::Particle* vdau2,
						      const std::string prefix, 
						      Tuples::Tuple& tuple ){
  
  bool test =true; 
  debug()<<" Computation of Yuri's CDF iso variable "<<endreq;

  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug() <<"Inside CDFisloation_Yuri, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS; 
  }
  
  double PI = TMath::Pi();// 3.1415926;
  double ptmu1 =  vdau1->proto()->track()->pt();
  double ptmu2 =  vdau2->proto()->track()->pt();

  double ptbs  =  B->pt();  
  double pxbs  =  B->momentum().px();
  double pybs  =  B->momentum().py();  

  double pbs  =  B->p();
  double plbs  =  sqrt(pbs*pbs+ptbs*ptbs);
  
  Gaudi::XYZVector PV_pos;
  const LHCb::VertexBase* goodPV =  m_dva->bestVertex(B); 
  
  if (NULL==goodPV) {
    warning() << "No best PV for " << B->particleID().pid() << endmsg ;
    return StatusCode::SUCCESS;
  }
  PV_pos = goodPV->position();
  

  double isoCDFyuri;
  
  debug()<<" pts:: "<<ptmu1<<" "<<ptmu2<<" "<<ptbs<<endreq;
  
  double ethabs = 0.5*log((pbs+plbs)/(pbs-plbs ));
  if (pybs<0) ethabs = -ethabs;
  
  double phibs = acos(pxbs/ptbs);
  if (pybs>0 && pxbs>0) phibs =  phibs;
  if (pybs<0 && pxbs>0) phibs = -phibs;
  if (pybs<0 && pxbs<0) phibs = -phibs;
  if (pybs>0 && pxbs<0) phibs =  phibs;
  
  double iso_bs  = ptbs;
  
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Particles::const_iterator ipp;
  
  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	Gaudi::XYZVector ptrack(atrack->momentum());
	double phitrack = ptrack.Phi();
	double deltaphi = fabs(phibs - phitrack);
	if (deltaphi>PI)  deltaphi = 2*PI-deltaphi;
	
	double pltrack = sqrt(atrack->p()*atrack->p()-atrack->pt()*atrack->pt());
	double ethatrack = 0.5*log((atrack->p()+pltrack)/(atrack->p()-pltrack));
	if (phitrack < 0) ethatrack = -ethatrack;
	double deltaetha = abs(ethabs - ethatrack);
	double radcone = sqrt(deltaphi*deltaphi+deltaetha*deltaetha);
	double trpt = atrack->pt();
	
	double trIP = 0;
	m_dist->distance(atrack, goodPV, trIP  );
	
	double mindeltapt = fabs(ptmu1-atrack->pt());
	if (fabs(ptmu2-atrack->pt())<mindeltapt) mindeltapt= fabs(ptmu2-atrack->pt());
	
	bool flag =( mindeltapt>0.01 && radcone<1.);
	debug() <<"radcone:: "<< radcone << " and mindeltapt:: "
          << mindeltapt <<"  so the flag is  "<< flag << endreq; 
	
	if (flag && trIP<0.1 && atrack->pt()> 200.) iso_bs += trpt;
	
      } // if(atrack)
    } // if (proto)
  } // parts->begin
  
  
  isoCDFyuri = ptbs/iso_bs;
  debug()<<" iso_bs & ptbs for iso:: "<<iso_bs<<" "<<ptbs<<endreq;
  m_isoCDFyuri = isoCDFyuri;

  test &= tuple->column( prefix+"_CDFiso_yuri",  m_isoCDFyuri);
  if(test)   return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
  
}  // isoCDFYuri




//=============================================================================
// ATLAS isolation 
//=============================================================================

StatusCode TupleToolMuonVariables::ATLASIsolation(const LHCb::Particle* B,
						   const std::string prefix, 
						   Tuples::Tuple& tuple ){
  
  bool test = true;
  
  const LHCb::VertexBase* primary =  m_dva->bestVertex(B);
  const LHCb::VertexBase* secondary =  B->endVertex();
  
  double n_incone = 0.;
  
  Gaudi::XYZVector pbs(B->slopes());
  
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::Particles::const_iterator ipp;
  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	
	Gaudi::XYZVector ptrack(atrack->momentum());
	Gaudi::XYZPoint otrack(atrack->position());
	

	Gaudi::XYZPoint PosSV = secondary->position();
	Gaudi::XYZPoint vtx(0.,0.,0.);
	double doca(-1.);          
	double angle(-1.);
	
	double imp = 0.;
	double ips = -1.;
	InCone(PosSV,pbs,otrack, ptrack,vtx, doca, angle);
	m_dist->distance((*ipp),primary,imp,ips);
	ips=sqrt(ips);
	
	if (ips > 0. and doca < 0.2 and angle < 0.2618) {
	  // 0.2mm + 15deg
	  n_incone+=1;
	}
      
      }// if (atrack)
    } // if(proto)
  } // parts->begin

  test &= tuple->column( prefix+"_ATLASiso", n_incone);
  if (test) return StatusCode::SUCCESS; 
  else return StatusCode::FAILURE;
  
} // ATLASIsolation 



//==============================
// MC Ancestor Finding Routine:
//==============================
const LHCb::MCParticle* TupleToolMuonVariables::originof( const LHCb::MCParticle* product ) {
  const LHCb::MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product;
  else return originof( mother );
}


//=============================================================================
// Fill MC quantities
//=============================================================================
StatusCode TupleToolMuonVariables::fillTrackInfo(Tuples::Tuple& tuple ){

  if (m_isMC) Assert( m_p2mcAssoc,"The DaVinci smart associator hasn't been initialized!");
  
  bool test =true;
  const LHCb::MCParticle* MCp = NULL;
  const LHCb::MCParticle* ancestor =NULL;
  
  const int NMaxEVBpart = 500;
  
  //Here's where I get my hands on the LHCbs particles
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  int howManyParts(0);
  bool matched = kFALSE;
  bool hasMother = kFALSE;
  bool hasGranny = kFALSE;
  bool hasMotGranny = kFALSE;
  bool hasGraGranny = kFALSE;
  
  vector<double> par_pos_x, par_pos_y, par_pos_z, par_chg;
  vector<double> par_pos_covxx,par_pos_covyy,par_pos_covzz,par_pos_covxy,par_pos_covxz,par_pos_covyz;
  //pos vs mom Cov matrix of tracks (6x6)
  vector<double> par_px_cov00,par_px_cov11,par_px_cov22,par_px_cov33,par_px_cov44,par_px_cov55;
  vector<double> par_px_cov01,par_px_cov02,par_px_cov12,par_px_cov34,par_px_cov35,par_px_cov45;
  vector<double> par_px_cov03,par_px_cov04,par_px_cov05,par_px_cov13,par_px_cov14,par_px_cov15,par_px_cov23,par_px_cov24,par_px_cov25;

  vector<double> par_px, par_py, par_pz, par_pt, par_p, par_eta;
  vector<double> par_slope_covxx,par_slope_covyy,par_slope_covxy;//NB: slope c.v. derivable from mom vec

  vector<double> par_NNe,par_NNmu,par_NNpi,par_NNk,par_NNp,par_NNghost;//MR: PID ProbNN
  vector<double> par_DLLe,par_DLLmu,par_DLLk,par_DLLp;//MR: PID DLL
  vector<double> par_isMuon;//MR: isMuon
  
  vector<int> par_typ;
  vector<double> par_gho, par_chi, par_clo, par_ip, par_ips;
  vector<bool> par_hlt;

  vector<int> par_PVind;
  //initialize the vectors for track<-->PV association
  std::vector<double> trkPV_pz, trkPV_gho;
  std::vector<int> trkPV_num;
  trkPVassociator(trkPV_pz,trkPV_gho,trkPV_num,"long");

  
  vector<bool> MC_match, MC_par_hasMother, 
    MC_mot_hasMother, MC_gra_hasMother,
    MC_mogra_hasMother;
  
  vector<double> MC_origin_id, MC_ori_vtx_y, MC_ori_vtx_z, MC_ori_vtx_x;
  vector<double> MC_par_py, MC_par_pz, MC_par_px, MC_par_M, MC_par_E;
  //vector<double> MC_par_trueID;
  vector<int> MC_par_trueID;

  vector<double> MC_mot_ori_vtx_y, MC_mot_ori_vtx_z, MC_mot_ori_vtx_x;
  vector<double> MC_mot_py, MC_mot_pz, MC_mot_px, MC_mot_M, MC_mot_E;
  //vector<double> MC_mot_trueID;
  vector<int> MC_mot_trueID;

  vector<double> MC_gra_ori_vtx_y, MC_gra_ori_vtx_z, MC_gra_ori_vtx_x;
  vector<double> MC_gra_py, MC_gra_pz, MC_gra_px, MC_gra_M, MC_gra_E;
  //vector<double> MC_gra_trueID;
  vector<int> MC_gra_trueID;

  vector<double> MC_mogra_ori_vtx_y, MC_mogra_ori_vtx_z, MC_mogra_ori_vtx_x;
  vector<double> MC_mogra_py, MC_mogra_pz, MC_mogra_px, MC_mogra_M, MC_mogra_E;
  //vector<double> MC_mogra_trueID;
  vector<int> MC_mogra_trueID;

  vector<double> MC_grgra_ori_vtx_y, MC_grgra_ori_vtx_z, MC_grgra_ori_vtx_x;
  vector<double> MC_grgra_py, MC_grgra_pz, MC_grgra_px, MC_grgra_M, MC_grgra_E;
  vector<int> MC_grgra_trueID;

  LHCb::Particles::const_iterator ipp;
  for(ipp= parts->begin();ipp!=parts->end();ipp++){

    howManyParts++;
    if (m_isMC) MCp = m_p2mcAssoc->relatedMCP(*ipp);
    
    Gaudi::XYZPoint track_pos;
    Gaudi::XYZVector track_mom;
    Gaudi::SymMatrix6x6 track_posmom_cov;
    const LHCb::Track* track=(*ipp)->proto()->track(); 
    (*ipp)->proto()->track()->positionAndMomentum(track_pos,track_mom,track_posmom_cov);
    par_pos_x.push_back(track_pos.x());
    par_pos_y.push_back(track_pos.y());
    par_pos_z.push_back(track_pos.z());
    
    par_pos_covxx.push_back(track_posmom_cov(0,0));
    par_pos_covyy.push_back(track_posmom_cov(1,1));
    par_pos_covzz.push_back(track_posmom_cov(2,2));
    par_pos_covxy.push_back(track_posmom_cov(0,1));
    par_pos_covxz.push_back(track_posmom_cov(0,2));
    par_pos_covyz.push_back(track_posmom_cov(1,2));
    
    par_px_cov00.push_back(track_posmom_cov(0,0));
    par_px_cov11.push_back(track_posmom_cov(1,1));
    par_px_cov22.push_back(track_posmom_cov(2,2));
    par_px_cov33.push_back(track_posmom_cov(3,3));
    par_px_cov44.push_back(track_posmom_cov(4,4));
    par_px_cov55.push_back(track_posmom_cov(5,5));
    par_px_cov01.push_back(track_posmom_cov(0,1));
    par_px_cov02.push_back(track_posmom_cov(0,2));
    par_px_cov12.push_back(track_posmom_cov(1,2));
    par_px_cov34.push_back(track_posmom_cov(3,4));
    par_px_cov35.push_back(track_posmom_cov(3,5));
    par_px_cov45.push_back(track_posmom_cov(4,5));
    
    par_px_cov03.push_back(track_posmom_cov(0,3));
    par_px_cov04.push_back(track_posmom_cov(0,4));
    par_px_cov05.push_back(track_posmom_cov(0,5));
    par_px_cov13.push_back(track_posmom_cov(1,3));
    par_px_cov14.push_back(track_posmom_cov(1,4));
    par_px_cov15.push_back(track_posmom_cov(1,5));
    par_px_cov23.push_back(track_posmom_cov(2,3));
    par_px_cov24.push_back(track_posmom_cov(2,4));
    par_px_cov25.push_back(track_posmom_cov(2,5));
    
    par_px.push_back(track_mom.x());
    par_py.push_back(track_mom.y());
    par_pz.push_back(track_mom.z());
    par_pt.push_back((*ipp)->proto()->track()->pt());
    par_p.push_back((*ipp)->proto()->track()->p());
    par_eta.push_back(track_mom.Eta());
    par_chg.push_back((*ipp)->proto()->track()->charge());

    //take slope and its cov. matrix, and fill the cov matrix (x,y)-subspace
    Gaudi::XYZVector track_slope;
    Gaudi::SymMatrix3x3 track_slope_cov;
    (*ipp)->proto()->track()->slopes(track_slope,track_slope_cov);
    par_slope_covxx.push_back(track_slope_cov(0,0));
    par_slope_covyy.push_back(track_slope_cov(1,1));
    par_slope_covxy.push_back(track_slope_cov(0,1));

    par_NNe.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNe,-9999));
    par_NNmu.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNmu,-9999));
    par_NNpi.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNpi,-9999));
    par_NNk.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNk,-9999));
    par_NNp.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNp,-9999));
    par_NNghost.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::ProbNNghost,-9999));
    par_DLLe.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::CombDLLe,-9999));
    par_DLLmu.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::CombDLLmu,-9999));
    //par_DLLpi.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::CombDLLpi,-9999));
    par_DLLk.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::CombDLLk,-9999));
    par_DLLp.push_back((*ipp)->proto()->info(LHCb::ProtoParticle::CombDLLp,-9999));
    int ismuon_flag=0;
    if ((*ipp)->proto()->muonPID()) ismuon_flag=(*ipp)->proto()->muonPID()->IsMuon()?1:0;
    par_isMuon.push_back(ismuon_flag);

    par_typ.push_back((*ipp)->proto()->track()->type());
    par_gho.push_back((*ipp)->proto()->track()->ghostProbability());
    par_chi.push_back((*ipp)->proto()->track()->chi2PerDoF());
    par_clo.push_back((*ipp)->proto()->track()->info(LHCb::Track::CloneDist, 100));
    //Find the impact parameter significance of the track
    double imp=0.;
    double impchi2=0.;
    double ips;
    ips=100000.;
    
    LHCb::RecVertex::Container* verts = NULL;
    if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
    LHCb::RecVertex::Container::const_iterator iv;      
    for ( iv = verts->begin(); iv != verts->end(); iv++) {
      m_dist->distance(&(*(*ipp)),(*iv),imp,impchi2);
      if (impchi2<ips) ips = impchi2;
    }      
    ips=sqrt(ips);
    par_ip.push_back(imp);
    par_ips.push_back(ips);

    //PV association
    par_PVind.push_back(getAssociatedPV(track,trkPV_pz,trkPV_gho,trkPV_num));

    matched = kFALSE;
    hasMother = kFALSE;
    hasGranny = kFALSE;
    hasMotGranny = kFALSE;
    hasGraGranny = kFALSE;
    
    //MC truth info
    if (m_isMC){
      if (!MCp) {
	debug()<<" Didn't find MCP "<<endmsg;
	MC_ori_vtx_x.push_back(-9999);
	MC_ori_vtx_y.push_back(-9999);
	MC_ori_vtx_z.push_back(-9999);
	MC_par_trueID.push_back(-9999);
	MC_par_px.push_back(-9999);
	MC_par_py.push_back(-9999);
	MC_par_pz.push_back(-9999);
	MC_par_M.push_back(-9999);
	MC_par_E.push_back(-9999);
	MC_origin_id.push_back(-9999);
      }
    
      else {
	matched = kTRUE;
	
	//MC particle info.
	//Origin vtx      
	MC_ori_vtx_x.push_back(MCp->originVertex()->position().x());
	MC_ori_vtx_y.push_back(MCp->originVertex()->position().y());
	MC_ori_vtx_z.push_back(MCp->originVertex()->position().z());
	
	MC_par_trueID.push_back(MCp->particleID().pid());
	MC_par_px.push_back(MCp->momentum().Px());
	MC_par_py.push_back(MCp->momentum().Py());
	MC_par_pz.push_back(MCp->momentum().Pz());
	MC_par_M.push_back(MCp->momentum().M());
	MC_par_E.push_back(MCp->momentum().E());
	
	m_typeOrigin =  MCp->originVertex()->type();
	
	//Ancestor info
	ancestor = originof(MCp) ;
	MC_origin_id.push_back(ancestor->particleID().pid());
	debug()<<" Filled ancestor PID "<<endreq;      
	
	//Number of end vertices: not tupled for now
	SmartRefVector< LHCb::MCVertex >::const_iterator iVtx;
	if (0==MCp->endVertices().size())
	  {
	    debug() << " vertex failed " << endreq;
	    m_endVertices  = -100;
	  }
	else 
	  {
	    m_endVertices = MCp->endVertices().size();
	  }
	
	//MC mother info
	const LHCb::MCParticle* mother = MCp->mother();
	if(mother) {
	  hasMother = kTRUE;
	  MC_par_hasMother.push_back(kTRUE);
	  MC_mot_ori_vtx_x.push_back(mother->originVertex()->position().x());
	  MC_mot_ori_vtx_y.push_back(mother->originVertex()->position().y());
	  MC_mot_ori_vtx_z.push_back(mother->originVertex()->position().z());
	  
	  MC_mot_trueID.push_back(mother->particleID().pid());
	  MC_mot_px.push_back(mother->momentum().Px());
	  MC_mot_py.push_back(mother->momentum().Py());
	  MC_mot_pz.push_back(mother->momentum().Pz());
	  MC_mot_M.push_back(mother->momentum().M());
	  MC_mot_E.push_back(mother->momentum().E());
	  
	  //MC grandmother info
	  const LHCb::MCParticle* granny = mother->mother();
	  
	  if(granny) {
	    hasGranny = kTRUE;
	    MC_mot_hasMother.push_back(kTRUE);
	    
	    MC_gra_ori_vtx_x.push_back(granny->originVertex()->position().x());
	    MC_gra_ori_vtx_y.push_back(granny->originVertex()->position().y());
	    MC_gra_ori_vtx_z.push_back(granny->originVertex()->position().z());
	    
	    MC_gra_trueID.push_back(granny->particleID().pid());
	    MC_gra_px.push_back(granny->momentum().Px());
	    MC_gra_py.push_back(granny->momentum().Py());
	    MC_gra_pz.push_back(granny->momentum().Pz());
	    MC_gra_M.push_back(granny->momentum().M());
	    MC_gra_E.push_back(granny->momentum().E());
	    
	    //MC mother of grandmother info
	    const LHCb::MCParticle* motgranny = granny->mother();
	    
	    if(motgranny) {
	      hasMotGranny = kTRUE;
	      MC_gra_hasMother.push_back(kTRUE);
	      MC_mogra_ori_vtx_x.push_back(motgranny->originVertex()->position().x());
	      MC_mogra_ori_vtx_y.push_back(motgranny->originVertex()->position().y());
	      MC_mogra_ori_vtx_z.push_back(motgranny->originVertex()->position().z());
	      
	      MC_mogra_trueID.push_back(motgranny->particleID().pid());
	      MC_mogra_px.push_back(motgranny->momentum().Px());
	      MC_mogra_py.push_back(motgranny->momentum().Py());
	      MC_mogra_pz.push_back(motgranny->momentum().Pz());
	      MC_mogra_M.push_back(motgranny->momentum().M());
	      MC_mogra_E.push_back(motgranny->momentum().E());
	      
	      //MC grandmother of grandmother info
	      const LHCb::MCParticle* gragranny = motgranny->mother();
	      
	      if(gragranny) {
		hasGraGranny = kTRUE;
		MC_mogra_hasMother.push_back(kTRUE);
		MC_grgra_ori_vtx_x.push_back(gragranny->originVertex()->position().x());
		MC_grgra_ori_vtx_y.push_back(gragranny->originVertex()->position().y());
		MC_grgra_ori_vtx_z.push_back(gragranny->originVertex()->position().z());
		
		MC_grgra_trueID.push_back(gragranny->particleID().pid());
		MC_grgra_px.push_back(gragranny->momentum().Px());
		MC_grgra_py.push_back(gragranny->momentum().Py());
		MC_grgra_pz.push_back(gragranny->momentum().Pz());
		MC_grgra_M.push_back(gragranny->momentum().M());
		MC_grgra_E.push_back(gragranny->momentum().E());
	      } else {
		hasGraGranny = kFALSE;
	      } //Granny of granny
	    } else {
	      hasMotGranny = kFALSE;
	    } //Mother of granny
	  } else {
	    hasGranny = kFALSE;
	  } //Granny
	} else {
	  hasMother = kFALSE;
	} //Mother
      }// i.e. we have an MCp
      if(!hasMother) {      
	MC_par_hasMother.push_back(kFALSE);
	MC_mot_ori_vtx_x.push_back(-9999);
	MC_mot_ori_vtx_y.push_back(-9999);
	MC_mot_ori_vtx_z.push_back(-9999);
	
	MC_mot_trueID.push_back(-9999);
	MC_mot_px.push_back(-9999);
	MC_mot_py.push_back(-9999);
	MC_mot_pz.push_back(-9999);
	MC_mot_M.push_back(-9999);
	MC_mot_E.push_back(-9999);
      }
      if(!hasGranny) {
	MC_mot_hasMother.push_back(kFALSE);
	MC_gra_ori_vtx_x.push_back(-9999);
	MC_gra_ori_vtx_y.push_back(-9999);
	MC_gra_ori_vtx_z.push_back(-9999);
	
	MC_gra_trueID.push_back(-9999);
	MC_gra_px.push_back(-9999);
	MC_gra_py.push_back(-9999);
	MC_gra_pz.push_back(-9999);
	MC_gra_M.push_back(-9999);
	MC_gra_E.push_back(-9999);
      }
      if(!hasMotGranny) {
	MC_gra_hasMother.push_back(kFALSE);
	MC_mogra_ori_vtx_x.push_back(-9999);
	MC_mogra_ori_vtx_y.push_back(-9999);
	MC_mogra_ori_vtx_z.push_back(-9999);
	
	MC_mogra_trueID.push_back(-9999);
	MC_mogra_px.push_back(-9999);
	MC_mogra_py.push_back(-9999);
	MC_mogra_pz.push_back(-9999);
	MC_mogra_M.push_back(-9999);
	MC_mogra_E.push_back(-9999);
      }
      
      if(!hasGraGranny) {
	MC_mogra_hasMother.push_back(kFALSE);
	MC_grgra_ori_vtx_x.push_back(-9999);
	MC_grgra_ori_vtx_y.push_back(-9999);
	MC_grgra_ori_vtx_z.push_back(-9999);
	
	MC_grgra_trueID.push_back(-9999);
	MC_grgra_px.push_back(-9999);
	MC_grgra_py.push_back(-9999);
	MC_grgra_pz.push_back(-9999);
	MC_grgra_M.push_back(-9999);
	MC_grgra_E.push_back(-9999);
      }
      MC_match.push_back(matched);
    }//end m_isMC cond
  }//end loop over particles

  test &= tuple->column( "EVB_Nparts", howManyParts);
  test &= tuple->farray( "EVB_par_pos_x", par_pos_x.begin(), par_pos_x.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pos_y", par_pos_y.begin(), par_pos_y.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pos_z", par_pos_z.begin(), par_pos_z.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pos_covxx", par_pos_covxx.begin(), par_pos_covxx.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pos_covyy", par_pos_covyy.begin(), par_pos_covyy.end(), "pbIdx", NMaxEVBpart );
  //test &= tuple->farray( "EVB_par_pos_covzz", par_pos_covzz.begin(), par_pos_covzz.end(), "pbIdx", NMaxEVBpart );//=0
  test &= tuple->farray( "EVB_par_pos_covxy", par_pos_covxy.begin(), par_pos_covxy.end(), "pbIdx", NMaxEVBpart );
  //test &= tuple->farray( "EVB_par_pos_covxz", par_pos_covxz.begin(), par_pos_covxz.end(), "pbIdx", NMaxEVBpart );//=0
  //test &= tuple->farray( "EVB_par_pos_covyz", par_pos_covyz.begin(), par_pos_covyz.end(), "pbIdx", NMaxEVBpart );//=0
  
  //Inserisco blocco Cov xp (6x6)
  test &= tuple->farray( "EVB_par_xp_cov00", par_px_cov00.begin(), par_px_cov00.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov11", par_px_cov11.begin(), par_px_cov11.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov22", par_px_cov22.begin(), par_px_cov22.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov33", par_px_cov33.begin(), par_px_cov33.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov44", par_px_cov44.begin(), par_px_cov44.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov55", par_px_cov55.begin(), par_px_cov55.end(), "pbIdx", NMaxEVBpart );

  test &= tuple->farray( "EVB_par_xp_cov01", par_px_cov01.begin(), par_px_cov01.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov02", par_px_cov02.begin(), par_px_cov02.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov12", par_px_cov12.begin(), par_px_cov12.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov34", par_px_cov34.begin(), par_px_cov34.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov35", par_px_cov35.begin(), par_px_cov35.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov45", par_px_cov45.begin(), par_px_cov45.end(), "pbIdx", NMaxEVBpart );

  test &= tuple->farray( "EVB_par_xp_cov03", par_px_cov03.begin(), par_px_cov03.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov04", par_px_cov04.begin(), par_px_cov04.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov05", par_px_cov05.begin(), par_px_cov05.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov13", par_px_cov13.begin(), par_px_cov13.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov14", par_px_cov14.begin(), par_px_cov14.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov15", par_px_cov15.begin(), par_px_cov15.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov23", par_px_cov23.begin(), par_px_cov23.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov24", par_px_cov24.begin(), par_px_cov24.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_xp_cov25", par_px_cov25.begin(), par_px_cov25.end(), "pbIdx", NMaxEVBpart );

  test &= tuple->farray( "EVB_par_px", par_px.begin(), par_px.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_py", par_py.begin(), par_py.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pz", par_pz.begin(), par_pz.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_pt", par_pt.begin(), par_pt.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_p", par_p.begin(), par_p.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_eta", par_eta.begin(), par_eta.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_chg", par_chg.begin(), par_chg.end(), "pbIdx", NMaxEVBpart );

  test &= tuple->farray( "EVB_par_typ", par_typ.begin(), par_typ.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_gho", par_gho.begin(), par_gho.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_chi", par_chi.begin(), par_chi.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_clo", par_clo.begin(), par_clo.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_ip", par_ip.begin(), par_ip.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_ips", par_ips.begin(), par_ips.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_hlt", par_hlt.begin(), par_hlt.end(), "pbIdx", NMaxEVBpart );
  
  test &= tuple->farray( "EVB_par_slope_covxx", par_slope_covxx.begin(), par_slope_covxx.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_slope_covyy", par_slope_covyy.begin(), par_slope_covyy.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_slope_covxy", par_slope_covxy.begin(), par_slope_covxy.end(), "pbIdx", NMaxEVBpart );

  //MR:PID
  test &= tuple->farray( "EVB_par_NNe", par_NNe.begin(), par_NNe.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_NNmu", par_NNmu.begin(), par_NNmu.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_NNpi", par_NNpi.begin(), par_NNpi.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_NNk", par_NNk.begin(), par_NNk.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_NNp", par_NNp.begin(), par_NNp.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_NNghost", par_NNghost.begin(), par_NNghost.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_DLLe", par_DLLe.begin(), par_DLLe.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_DLLmu", par_DLLmu.begin(), par_DLLmu.end(), "pbIdx", NMaxEVBpart );
  //test &= tuple->farray( "EVB_par_DLLpi", par_DLLpi.begin(), par_DLLpi.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_DLLk", par_DLLk.begin(), par_DLLk.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_DLLp", par_DLLp.begin(), par_DLLp.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_isMuon", par_isMuon.begin(), par_isMuon.end(), "pbIdx", NMaxEVBpart );
  test &= tuple->farray( "EVB_par_PVind", par_PVind.begin(), par_PVind.end(), "pbIdx", NMaxEVBpart );

  
  if (m_isMC){
    //MC truth info
    test &= tuple->farray( "EVB_isMCMatched", MC_match.begin(), MC_match.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_Origin_ID", MC_origin_id.begin(), MC_origin_id.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_part_Ori_Vtx_x", MC_ori_vtx_x.begin(), MC_ori_vtx_x.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_Ori_Vtx_y", MC_ori_vtx_y.begin(), MC_ori_vtx_y.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_Ori_Vtx_z", MC_ori_vtx_z.begin(), MC_ori_vtx_z.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_part_ID", MC_par_trueID.begin(), MC_par_trueID.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_Px", MC_par_px.begin(), MC_par_px.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_Py", MC_par_py.begin(), MC_par_py.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_Pz", MC_par_pz.begin(), MC_par_pz.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_E", MC_par_E.begin(), MC_par_E.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_part_M", MC_par_M.begin(), MC_par_M.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_part_hasMother", MC_par_hasMother.begin(), MC_par_hasMother.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_moth_Ori_Vtx_x", MC_mot_ori_vtx_x.begin(), MC_mot_ori_vtx_x.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_Ori_Vtx_y", MC_mot_ori_vtx_y.begin(), MC_mot_ori_vtx_y.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_Ori_Vtx_z", MC_mot_ori_vtx_z.begin(), MC_mot_ori_vtx_z.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_moth_ID", MC_mot_trueID.begin(), MC_mot_trueID.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_Px", MC_mot_px.begin(), MC_mot_px.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_Py", MC_mot_py.begin(), MC_mot_py.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_Pz", MC_mot_pz.begin(), MC_mot_pz.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_E", MC_mot_E.begin(), MC_mot_E.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_moth_M", MC_mot_M.begin(), MC_mot_M.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_moth_hasMother", MC_mot_hasMother.begin(), MC_mot_hasMother.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_gran_Ori_Vtx_x", MC_gra_ori_vtx_x.begin(), MC_gra_ori_vtx_x.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_Ori_Vtx_y", MC_gra_ori_vtx_y.begin(), MC_gra_ori_vtx_y.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_Ori_Vtx_z", MC_gra_ori_vtx_z.begin(), MC_gra_ori_vtx_z.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_gran_ID", MC_gra_trueID.begin(), MC_gra_trueID.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_Px", MC_gra_px.begin(), MC_gra_px.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_Py", MC_gra_py.begin(), MC_gra_py.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_Pz", MC_gra_pz.begin(), MC_gra_pz.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_E", MC_gra_E.begin(), MC_gra_E.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_gran_M", MC_gra_M.begin(), MC_gra_M.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_gran_hasMother", MC_gra_hasMother.begin(), MC_gra_hasMother.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_mogr_Ori_Vtx_x", MC_mogra_ori_vtx_x.begin(), MC_mogra_ori_vtx_x.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_Ori_Vtx_y", MC_mogra_ori_vtx_y.begin(), MC_mogra_ori_vtx_y.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_Ori_Vtx_z", MC_mogra_ori_vtx_z.begin(), MC_mogra_ori_vtx_z.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_mogr_ID", MC_mogra_trueID.begin(), MC_mogra_trueID.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_Px", MC_mogra_px.begin(), MC_mogra_px.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_Py", MC_mogra_py.begin(), MC_mogra_py.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_Pz", MC_mogra_pz.begin(), MC_mogra_pz.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_E", MC_mogra_E.begin(), MC_mogra_E.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_mogr_M", MC_mogra_M.begin(), MC_mogra_M.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_mogr_hasMother", MC_mogra_hasMother.begin(), MC_mogra_hasMother.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_grgr_Ori_Vtx_x", MC_grgra_ori_vtx_x.begin(), MC_grgra_ori_vtx_x.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_Ori_Vtx_y", MC_grgra_ori_vtx_y.begin(), MC_grgra_ori_vtx_y.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_Ori_Vtx_z", MC_grgra_ori_vtx_z.begin(), MC_grgra_ori_vtx_z.end(), "pbIdx", NMaxEVBpart );
    
    test &= tuple->farray( "EVB_true_grgr_ID", MC_grgra_trueID.begin(), MC_grgra_trueID.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_Px", MC_grgra_px.begin(), MC_grgra_px.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_Py", MC_grgra_py.begin(), MC_grgra_py.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_Pz", MC_grgra_pz.begin(), MC_grgra_pz.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_E", MC_grgra_E.begin(), MC_grgra_E.end(), "pbIdx", NMaxEVBpart );
    test &= tuple->farray( "EVB_true_grgr_M", MC_grgra_M.begin(), MC_grgra_M.end(), "pbIdx", NMaxEVBpart );
  }//end m_isMC cond
    
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}


//=============================================================================
// Fill MC quantities
//=============================================================================
StatusCode TupleToolMuonVariables::fillMCTruth(const LHCb::Particle *part, 
                                               const std::string prefix, 
                                               Tuples::Tuple& tuple ){
  
  if(! m_isMC){
    
    debug()<<" Nothing to fill from fillMCTruth "<<endmsg;
    return StatusCode::SUCCESS;
  }
    
  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");
  
  bool test =true;
  const LHCb::MCParticle* MCp = NULL;
  const LHCb::MCParticle* ancestor =NULL;
  
  MCp = m_p2mcAssoc->relatedMCP(part);
  if (!MCp) {
    debug()<<" Didnt find MCP "<<endmsg;
  }
  
  else {
    ancestor = originof(MCp) ;
    m_mcancestorpid =  ancestor->particleID().pid();
    debug()<<" Filled ancestor PID "<<endreq;      
    
    SmartRefVector< LHCb::MCVertex >::const_iterator iVtx;
    if (0==MCp->endVertices().size())
      {
	debug() << " vertex failed " << endreq;
	m_endVertices  = -100;
      }
    else 
      {
	m_endVertices = MCp->endVertices().size();
      }
  
    
    
   
      m_zOrigin = MCp->originVertex()->position().z();
      m_typeOrigin =  MCp->originVertex()->type();
  
  }// i.e. we have an MCp
   
  m_MCI = -1000; m_MCPV = -1000;
  const LHCb::GenHeader* gh = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  if(gh && gh->collisions().size()>0)  
    {
      m_MCI = gh->collisions().size() ;
      verbose() << "retrieved MCInteractions from genHeader:  " << m_MCI<< endmsg;
    }
  
 
  test &= tuple->column( prefix+"_typeOriginVtx", m_typeOrigin);
  test &= tuple->column( prefix+"_nEndVertices",  m_endVertices);
  test &= tuple->column( prefix+"_ancestorpid", m_mcancestorpid);
  test &= tuple->column( prefix+"_MCI", m_MCI);
  
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}

//=============================================================================
// method to dump all variables info needed to compute isolation
//=============================================================================
//
// Polished&Updated by M. Rama
void  TupleToolMuonVariables::dumpIsoVariables(const LHCb::Particle* B, Tuples::Tuple& tuple ){

  const int NMaxIsopart = 500;

  //If B has not 2 daughters, exit
  const LHCb::Particle::ConstVector parts = B->daughtersVector();  
  if(parts.size() !=2) {
    error()<<"Number of B daughters != 2: "<<parts.size()<<endmsg;
    return;
  }
  
  //Get the particle list (longtracks). It if has a null pointer, exit 
  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  if (!allparts) {
    error()<<"Failed to get particles container "<<m_ParticlePath<<endmsg;
    return;
  }

  //Get the primary and secondary vertices, and their positions
  const LHCb::VertexBase *PV = m_dva->bestVertex(B);  
  const LHCb::VertexBase *SV = B->endVertex();
  Gaudi::XYZPoint PosPV,PosSV;

  if(PV)
    PosPV = PV->position();
  if(SV)
    PosSV = SV->position();

  //Loop over the B candidate daughters and store their position, p and pt
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;		
  std::vector<const LHCb::Particle*> mucand_vec;
  std::vector<const LHCb::Track*> mutrack_vec;

  int i=0;
  LHCb::Particle::ConstVector::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {    
    //const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    //const LHCb::Track* track = proto->track();
    const LHCb::Track* track = (*ip_part)->proto()->track();
    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i] = sqrt(pow(p_mu(0,i),2)+pow(p_mu(1,i),2));
    i++;
    mucand_vec.push_back(*ip_part);
    mutrack_vec.push_back((*ip_part)->proto()->track());
  }

  vector<double> fim_angle, fim_fc, fim_doca, fim_svdis, fim_pvdis;
  vector<double> fim_doca2, fim_docachi2;
  vector<double> fim_vtx_x,fim_vtx_y,fim_vtx_z;
  vector<double> sem_angle, sem_fc, sem_doca, sem_svdis, sem_pvdis;
  vector<double> sem_doca2, sem_docachi2;
  vector<double> sem_vtx_x,sem_vtx_y,sem_vtx_z;
  vector<double> trk_gho, trk_chi, trk_typ, trk_chg, trk_pt;
  vector<double> trk_ips, trk_clo;//MR 5feb14
  vector<bool> trk_hlt;

  //Loop over the two B daughter muon tracks. For each muon track, loop over the 
  //tracks of the event and build the (muon,track) quantities used by the track 
  //isolation. Store these quantities in 3 sets of arrays. fim_xxx, sem_xxx, trk_xxx.
  // fim_xxx contains the variables related to the 'first muon' (ch=??) and the tracks
  // sem_xxx contains the variables related to the 'second muon' (ch=??) and the tracks
  // trk_xxx contains the variables related to the tracks properties
  for (int i=0;i<2;i++){
    bool hltgood = false;
    double fc = 0.;
    Gaudi::XYZPoint omu(o_mu(0,i),o_mu(1,i),o_mu(2,i));
    Gaudi::XYZVector pmu(p_mu(0,i),p_mu(1,i),p_mu(2,i));
    const LHCb::Particle* mupart=mucand_vec[i];
    
    //Loop over all (long) tracks of the event. 
    LHCb::Particles::const_iterator ip;
    for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {
      
      //const LHCb::ProtoParticle * proto =  (*ip)->proto();
      //const LHCb::Track* track = proto->track();
      const LHCb::Track* track = (*ip)->proto()->track();
      const LHCb::Particle*  cand = (*ip);
      Gaudi::XYZPoint o(track->position());
      Gaudi::XYZVector p(track->momentum());
      double pt = p.Rho();
      
      if (track->type()!=m_tracktype)   continue; 

      // If the track is one of the 2 signal muons, fill fim_xxx and sem_xxx with a dummy entry
      /*
      if ((ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) &&
	  track!=mutrack_vec[0] && track!=mutrack_vec[1]) {
	cout<<"ATTENTION: (*ip) = "<<(*ip)<<" mucand_vec[0] = "<<mucand_vec[0]<<" mucand_vec[1] = "<<mucand_vec[1]<<" mucand_vec[0]->proto()->track() = "<<mucand_vec[0]->proto()->track()<<" mucand_vec[1]->proto()->track() = "<<mucand_vec[1]->proto()->track()<<" track = "<<track<<" ratio(pt,ptmu[0]) = "<<ratio(pt,ptmu[0])<<" ratio(pt,ptmu[1]) = "<<ratio(pt,ptmu[1])<<" Questa particella non e' uno dei due mu candidates"<<endl;
      }
      */

      //if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {
      if (track==mutrack_vec[0] || track==mutrack_vec[1]) {
	if (!(ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001)) {
	  cout<<"ATTENTION!!: ratio(pt,ptmu[0]) = "<<ratio(pt,ptmu[0])<<" ratio(pt,ptmu[1]) = "<<ratio(pt,ptmu[1])<<endl;
	}
	if(i==0) {
	  //if track = first muon, fill the fim_xxx block with a dummy value
	  fim_angle.push_back(0);
	  fim_fc.push_back(0);
	  fim_doca.push_back(0);
	  fim_doca2.push_back(0);
	  fim_docachi2.push_back(0);
	  fim_svdis.push_back(0);
	  fim_pvdis.push_back(0);
	  fim_vtx_x.push_back(0);
	  fim_vtx_y.push_back(0);
	  fim_vtx_z.push_back(0);
	  //The track quantities are independent on muons (hence filled once)
	  trk_gho.push_back(0);
	  trk_chi.push_back(0);
	  trk_chg.push_back(0);
	  trk_pt.push_back(0);
	  trk_typ.push_back(0);
	  trk_clo.push_back(0);
	  trk_ips.push_back(0);
	  trk_hlt.push_back(0);
	} 
	else {
	  //if track = second muon, fill the fim_xxx block with a dummy value
	  sem_angle.push_back(0);
	  sem_fc.push_back(0);
	  sem_doca.push_back(0);
	  sem_doca2.push_back(0);
	  sem_docachi2.push_back(0);
	  sem_svdis.push_back(0);
	  sem_pvdis.push_back(0);
	  sem_vtx_x.push_back(0);
	  sem_vtx_y.push_back(0);
	  sem_vtx_z.push_back(0);
	}
	//then jump to the next track 
	continue; 
      }//end "check of track pt vs the 2 muons pt"
      
      //Find hltgood and fc between the muon and the track
      //  omu,pmu=position and p vectors of the current muon
      //  o,p=position and p vectors of the current track
      //  IsHltGood takes as input o,p,omu,pmu,PosPV and fill hltgood and fc.
      //DOM: come e' def. hltgood? VEDERE
      if(PV)
	IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc); 
      
      //Find doca and angle between the muona and the track
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);          
      double angle(-1.);
      InCone(omu,pmu,o,p,vtx,doca,angle);
      double doca_precise(-1);
      double doca_chi2(-1);
      m_dist->distance(mupart,cand,doca_precise,doca_chi2);
      
      //Find the impact parameter significance of the track and the distance between (muon,track) and the secondary and primary vertices
      double imp=0.;
      double impchi2=0.;
      double ips,pvdis,svdis;
      ips=100000.;
      
      LHCb::RecVertex::Container* verts = NULL;
      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
      LHCb::RecVertex::Container::const_iterator iv;      
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
        m_dist->distance(&(*cand),(*iv),imp,impchi2);
        if (impchi2<ips) ips = impchi2;
      }      
      ips=sqrt(ips);
      int sign_pv, sign_sv;
      if(PV) {
	sign_pv = vtx.z()-PosPV.z()>=0?1:-1;
	pvdis = sign_pv*(vtx-PosPV).R();
      }
      if(SV) {
	sign_sv=vtx.z()-PosSV.z()>=0?1:-1;
	svdis = sign_sv*(vtx-PosSV).R();
      }
      //pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      //svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
	
      if(i==0) {
	//First muon
	fim_angle.push_back(angle);
	fim_fc.push_back(fc);
	fim_doca.push_back(doca);
	fim_doca2.push_back(doca_precise);
	fim_docachi2.push_back(doca_chi2);
	fim_svdis.push_back(svdis);
	fim_pvdis.push_back(pvdis);
	fim_vtx_x.push_back(vtx.x());
	fim_vtx_y.push_back(vtx.y());
	fim_vtx_z.push_back(vtx.z());
	//The track quantities are independent on muons (hence filled once)
	trk_gho.push_back(track->ghostProbability());
	trk_chi.push_back(track->chi2PerDoF());
	trk_chg.push_back(track->charge());
	trk_pt.push_back(track->pt());
	trk_typ.push_back(track->type());
	trk_clo.push_back(track->info(LHCb::Track::CloneDist, 100));
	trk_ips.push_back(ips);
	trk_hlt.push_back(hltgood);
      } 
      else {
	//Second muon
	sem_angle.push_back(angle);
	sem_fc.push_back(fc);
	sem_doca.push_back(doca);
	sem_doca2.push_back(doca_precise);
	sem_docachi2.push_back(doca_chi2);
	sem_svdis.push_back(svdis);
	sem_pvdis.push_back(pvdis);
	sem_vtx_x.push_back(vtx.x());
	sem_vtx_y.push_back(vtx.y());
	sem_vtx_z.push_back(vtx.z());
      }

    }//end loop on all tracks
  }//end i<2 for loop (loop on the 2 muons)
  
  tuple->farray( "IsoST_trk_typ", trk_typ.begin(), trk_typ.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_chi", trk_chi.begin(), trk_chi.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_chg", trk_chg.begin(), trk_chg.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_pt", trk_pt.begin(), trk_pt.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_gho", trk_gho.begin(), trk_gho.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_clo", trk_clo.begin(), trk_clo.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_ips", trk_ips.begin(), trk_ips.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_trk_hlt", trk_hlt.begin(), trk_hlt.end(), "IsoSTIdx", NMaxIsopart );
  
  tuple->farray( "IsoST_fim_angle", fim_angle.begin(), fim_angle.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_fc", fim_fc.begin(), fim_fc.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_doca", fim_doca.begin(), fim_doca.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_doca2", fim_doca2.begin(), fim_doca2.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_docachi2", fim_docachi2.begin(), fim_docachi2.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_svdis", fim_svdis.begin(), fim_svdis.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_pvdis", fim_pvdis.begin(), fim_pvdis.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_vtxx", fim_vtx_x.begin(), fim_vtx_x.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_vtxy", fim_vtx_y.begin(), fim_vtx_y.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_fim_vtxz", fim_vtx_z.begin(), fim_vtx_z.end(), "IsoSTIdx", NMaxIsopart );
  
  tuple->farray( "IsoST_sem_angle", sem_angle.begin(), sem_angle.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_fc", sem_fc.begin(), sem_fc.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_doca", sem_doca.begin(), sem_doca.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_doca2", sem_doca2.begin(), sem_doca2.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_docachi2", sem_docachi2.begin(), sem_docachi2.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_svdis", sem_svdis.begin(), sem_svdis.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_pvdis", sem_pvdis.begin(), sem_pvdis.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_vtxx", sem_vtx_x.begin(), sem_vtx_x.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_vtxy", sem_vtx_y.begin(), sem_vtx_y.end(), "IsoSTIdx", NMaxIsopart );
  tuple->farray( "IsoST_sem_vtxz", sem_vtx_z.begin(), sem_vtx_z.end(), "IsoSTIdx", NMaxIsopart );
  
  return;

}//end dumpIsoVariables






//=============================================================================
// Finalize
//=============================================================================
StatusCode TupleToolMuonVariables::finalize(){
  
  return TupleToolBase::finalize();
  

}


double TupleToolMuonVariables::calcIPToAnyPV( const LHCb::Particle * cand )
{
   double imp=0.;
   double impchi2=0.;
   double ips=100000;
   
   double imp_trk=0.;
   double impchi2_trk=0.;
   double ips_trk=100000;
   
   LHCb::RecVertex::Container::const_iterator iv;      
   int counter(0);
   LHCb::RecVertex::Container* verts = NULL;
   if (exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
       verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
   }
   else{ cout<<"calcIPToAnyPV: verts = NULL. return -1"<<endl; return -1; } 
   for ( iv = verts->begin(); iv != verts->end(); iv++) {
     //for ( iv = m_vertices->begin(); iv != m_vertices->end(); iv++) {
     counter++;
     //cout<<"(*iv)->position (x,y,z) = ("<<(*iv)->position().x()<<","<<(*iv)->position().y()<<","<<(*iv)->position().z()<<")"<<endl;
     m_dist->distance(cand,(*iv),imp,impchi2);
     //cout<<"imp = "<<imp<<" impchi2 = "<<impchi2<<endl;
     if (impchi2<ips) ips = impchi2;

     const LHCb::Track*  mytrack=cand->proto()->track();
     m_dist->distance(mytrack,(*iv),imp_trk,impchi2_trk);
     //cout<<"imp_trk = "<<imp_trk<<" impchi2_trk = "<<impchi2_trk<<endl;
     if (impchi2_trk<ips_trk) ips_trk = impchi2_trk;
   }      

   return sqrt(ips);
}//end calcIPToAnyPV

double  TupleToolMuonVariables::enclosedAngle(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {
  double den      = p1.R()*p2.R();
  double cosAngle = p1.Dot(p2)/den;
  double angle    = acos(fabs(cosAngle));
  if (cosAngle < 0 ) {
    angle = ROOT::Math::Pi() - angle;
  }
  return angle;
}//end enclosedAngle(...)

void TupleToolMuonVariables::getPerpFeet(Gaudi::XYZPoint track_pos,
                                           Gaudi::XYZVector track_p,
                                           Gaudi::XYZPoint mu_pos,
                                           Gaudi::XYZVector mu_p,
                                           Gaudi::XYZPoint& perpFootTrack,
                                           Gaudi::XYZPoint& perpFootMu,
                                           Gaudi::XYZPoint& vertex, bool& fail)
{
  // def difference of positions
  Gaudi::XYZVector  diffPos(track_pos - mu_pos);
  // Get unit vectors
  Gaudi::XYZVector  unitTrack_p(track_p.unit());
  Gaudi::XYZVector  unitMu_p(mu_p.unit());
  Gaudi::XYZPoint   temp1(0.,0.,0.);
  Gaudi::XYZPoint   temp2(0.,0.,0.);
  fail = false;
  // def used scalar-products
  double  d_DiffTr  = diffPos.Dot( unitTrack_p);
  double  d_DiffMu  = diffPos.Dot( unitMu_p);
  double  d_MuTrack = unitMu_p.Dot( unitTrack_p);
  double  d_TrTr    = unitTrack_p.Dot( unitTrack_p);
  double  d_MuMu    = unitMu_p.Dot( unitMu_p);
  double  denom     = d_MuTrack * d_MuTrack - d_MuMu * d_TrTr;
  if (fabs(denom)<1E-27) {
    perpFootTrack = temp1;
    perpFootMu    = temp2;
    fail = true;
  }
  else {
    double numer  = d_DiffTr * d_MuTrack - d_TrTr * d_DiffMu;
    double mu2    = numer / denom;
    double mu1    = ( mu2 * d_MuTrack - d_DiffTr ) / d_TrTr ;
    perpFootTrack = track_pos + unitTrack_p *mu1;
    perpFootMu    = mu_pos    + unitMu_p    *mu2;
  }
  if ( fail && msgLevel(MSG::DEBUG) ) debug() << d_DiffTr << '\t' << d_DiffMu << '\t' << d_MuTrack << '\t' << d_TrTr << '\t' << denom << endmsg ;
  //
  vertex = ( perpFootTrack + ( perpFootMu - perpFootTrack ) * 0.5 );

}//end getPerpFeet(...)

double TupleToolMuonVariables::calcFC( Gaudi::XYZVector track_mom,
                                         Gaudi::XYZVector mu_mom,
                                         Gaudi::XYZPoint mu_track_vertex,
                                         const LHCb::VertexBase* PV)
{
  //cout<<"Entro in calcFC"<<endl;
  double fc   = -1;
  Gaudi::XYZPoint  pv              = PV->position();
  Gaudi::XYZVector track_plus_mu_p( track_mom + mu_mom );
  Gaudi::XYZVector pv_to_tmuVertex( mu_track_vertex - pv);
  //double angle      = enclosedAngle(track_plus_mu_p, pv_to_tmuVertex);
  double angle      = sin(enclosedAngle(track_plus_mu_p, pv_to_tmuVertex));
  double fc_num     = track_plus_mu_p.R() * angle;
  double fc_denom   = track_plus_mu_p.R() * angle + track_mom.Rho() + mu_mom.Rho() ;
  if(fc_denom != 0 ) fc = fc_num / fc_denom ;

  return fc;
}//end calcFC(...)

double TupleToolMuonVariables::calcVertexDist(Gaudi::XYZPoint muTrack, const LHCb::VertexBase* v){
  if (v==NULL) return 0 ;
  Gaudi::XYZPoint vertex = v->position();
  return ( (muTrack.z()-vertex.z())/fabs(muTrack.z()-vertex.z())*(muTrack-vertex).R() );
}


//
// a) Loop over MC particles and find particles with bottom content and no parent
//    (origin of the decay tree)
// b) Loop over particles in a) and call fillMCInfo(...) to fill std::vectors with
//    info of particles in the decay tree
//
StatusCode TupleToolMuonVariables::fillMCDecayTree(Tuples::Tuple& tuple){
  if(! m_isMC){
    debug()<<" Nothing to fill from fillMCDecayTree "<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  //
  // Loop over MC particles and find those with bottom content and no parent
  //
  LHCb::MCParticles* mcpart_list = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !mcpart_list ) {
    fatal() << "Unable to find MC particles at '" 
            << LHCb::MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }
  std::vector<LHCb::MCParticle*> mcpart_vec(mcpart_list->begin(), mcpart_list->end());
  std::vector<LHCb::MCParticle*>::iterator mcpart_it; 
  std::vector<LHCb::MCParticle*> mc_b_vec;  	
  for (mcpart_it=mcpart_vec.begin() ; mcpart_it!=mcpart_vec.end() ; ++mcpart_it ){
    LHCb::MCParticle* mccand = *mcpart_it;
    if (mccand->particleID().hasBottom() && mccand->mother()==0) mc_b_vec.push_back(mccand);
  }//end for loop
  for(std::vector<LHCb::MCParticle*>::iterator it = mc_b_vec.begin(); it != mc_b_vec.end(); ++it) {
  }//end loop
  
  //
  // Loop over bottom particles with no parent and save the corresponding decay tree (actually
  // decay+interaction tree)
  //
  vector<int> index_vec,lund_vec;
  vector<double> px_vec,py_vec,pz_vec;
  
  int depth=0;
  int flag=1;
  for(std::vector<LHCb::MCParticle*>::iterator it = mc_b_vec.begin(); it != mc_b_vec.end(); ++it) {
    const LHCb::MCParticle* mccand=*it;
    mc_partindex=-1;
    fillMCInfo(mccand,mc_partindex,depth,flag);
  }//end loop
  for (int i=0;i<(int)mc_motindex_vec.size();i++){
  }//end i<mc_motindex_vec.size()
  
  //
  // Fill the ntuple
  //
  bool test =true;
  int MCGEN_maxnum=500;
  //test &= tuple->column("MCGEN_num",MCGEN_num);
  test &= tuple->farray("MCGEN_partindex", mc_partindex_vec.begin(),mc_partindex_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_motindex", mc_motindex_vec.begin(),mc_motindex_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_lundid", mc_lundid_vec.begin(),mc_lundid_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_px", mc_px_vec.begin(),mc_px_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_py", mc_py_vec.begin(),mc_py_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_pz", mc_pz_vec.begin(),mc_pz_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_vtx_x", mc_vtx_x_vec.begin(),mc_vtx_x_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_vtx_y", mc_vtx_y_vec.begin(),mc_vtx_y_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_vtx_z", mc_vtx_z_vec.begin(),mc_vtx_z_vec.end(), "MCGEN_num", MCGEN_maxnum );
  test &= tuple->farray("MCGEN_origtype", mc_origtype_vec.begin(),mc_origtype_vec.end(), "MCGEN_num", MCGEN_maxnum );
  
  //
  // Reset vectors for next use
  //
  mc_partindex_vec.clear();
  mc_motindex_vec.clear();
  mc_lundid_vec.clear();
  mc_px_vec.clear();
  mc_py_vec.clear();
  mc_pz_vec.clear();
  mc_vtx_x_vec.clear();
  mc_vtx_y_vec.clear();
  mc_vtx_z_vec.clear();
  mc_origtype_vec.clear();

  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}//end fillMCDecayTree


//
// Recursive function which put in std::vectors the info of particles belonging to decay Tree 
// Called by fillMCTree(...)
//
void  TupleToolMuonVariables::fillMCInfo(const LHCb::MCParticle* part,int mot_index,int depth,int flag){
  mc_partindex++;
  
  mc_partindex_vec.push_back(mc_partindex);
  mc_motindex_vec.push_back(mot_index);
  mc_lundid_vec.push_back(part->particleID().pid());
  mc_px_vec.push_back(part->momentum().Px());
  mc_py_vec.push_back(part->momentum().Py());
  mc_pz_vec.push_back(part->momentum().Pz());
  mc_vtx_x_vec.push_back(part->originVertex()->position().x());
  mc_vtx_y_vec.push_back(part->originVertex()->position().y());
  mc_vtx_z_vec.push_back(part->originVertex()->position().z());
  mc_origtype_vec.push_back(part->originVertex()->type());
  
  depth++;
  //type: http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/davinci/releases/latest/doxygen/dd/d97/class_l_h_cb_1_1_m_c_vertex.html -> MCVertexType
  //if depth>6 and particle is not originated from a decay, skip
  if (depth>6 && part->originVertex()->type()!=2) return;
  
  mot_index=mc_partindex;
  //loop over the particle's interaction vertices (can be decay vertex or interaction with matter)
  SmartRefVector<LHCb::MCVertex>::const_iterator iv;
  for ( iv = part->endVertices().begin();iv!=part->endVertices().end();iv++) {
    //in each vertex loop over possible daughters
    SmartRefVector<LHCb::MCParticle>::const_iterator idau;
    for ( idau = (*iv)->products().begin();idau!=(*iv)->products().end();idau++) {
      const  LHCb::MCParticle* daucand=*idau;
      fillMCInfo(daucand,mot_index,depth,flag);
    }//end idau loop
  }//end endVertices() loop
  
  return;

}//end fillMCInfo

StatusCode TupleToolMuonVariables::fillBposmomCov(const LHCb::Particle* B, std::string prefix, Tuples::Tuple& tuple ){
  const Gaudi::SymMatrix7x7 & covMat = B->covMatrix ();
  bool test=true;
  for (int i=0;i<7;i++)
    for (int j=i;j<7;j++){
      char* name=new char[100]; sprintf(name,"_cov%i%i",i,j);
      test &= tuple->column(prefix+name,covMat(i,j));
      delete[] name;
    }
  if (test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;

}//end fillBposmomCov

StatusCode TupleToolMuonVariables::fillVeloTracksInfo(Tuples::Tuple& tuple){
  bool test=true;
  vector<double> par_pos_x, par_pos_y, par_pos_z;
  vector<double> par_pos_covxx,par_pos_covyy,par_pos_covxy;

  vector<double> par_px, par_py, par_pz, par_pt, par_p, par_chg;
  vector<double> par_slope_covxx,par_slope_covyy,par_slope_covxy;//NB: slope c.v. derivable from mom vec
  vector<double> par_gho, par_chi;
  vector<int> par_typ;
  vector<int> par_PVind;

  //initialize the vectors for track<-->PV association
  std::vector<double> trkPV_pz, trkPV_gho;
  std::vector<int> trkPV_num;
  trkPVassociator(trkPV_pz,trkPV_gho,trkPV_num,"velo");

  LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
  typedef std::vector<const LHCb::Track*> TrackContainer ;
  int VEL_maxnum=250;
  for( LHCb::Tracks::const_iterator it = tracks->begin() ; it != tracks->end(); ++it) {
    const Track* track=(*it);
    if (track->type()!=1 && track->type()!=2) continue;//skip tracks != Velo or VeloR
    Gaudi::XYZPoint track_pos;
    Gaudi::XYZVector track_mom;
    Gaudi::SymMatrix6x6 track_posmom_cov;
    track->positionAndMomentum(track_pos,track_mom,track_posmom_cov);
    //pos e cov matrix
    par_pos_x.push_back(track_pos.x());
    par_pos_y.push_back(track_pos.y());
    par_pos_z.push_back(track_pos.z());
    par_pos_covxx.push_back(track_posmom_cov(0,0));
    par_pos_covyy.push_back(track_posmom_cov(1,1));
    par_pos_covxy.push_back(track_posmom_cov(0,1));
    
    //momentum and charge
    par_px.push_back(track_mom.x());
    par_py.push_back(track_mom.y());
    par_pz.push_back(track_mom.z());
    par_pt.push_back(track->pt());
    par_p.push_back(track->p());
    par_chg.push_back(track->charge());
    
    Gaudi::XYZVector track_slope;
    Gaudi::SymMatrix3x3 track_slope_cov;
    track->slopes(track_slope,track_slope_cov);
    par_slope_covxx.push_back(track_slope_cov(0,0));
    par_slope_covyy.push_back(track_slope_cov(1,1));
    par_slope_covxy.push_back(track_slope_cov(0,1));
    
    //other info
    par_gho.push_back(track->ghostProbability());
    par_chi.push_back(track->chi2PerDoF());
    par_typ.push_back(track->type());

    //PV association
    par_PVind.push_back(getAssociatedPV(track,trkPV_pz,trkPV_gho,trkPV_num));
  }
  
  test &= tuple->farray( "VEL_par_pos_x", par_pos_x.begin(), par_pos_x.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pos_y", par_pos_y.begin(), par_pos_y.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pos_z", par_pos_z.begin(), par_pos_z.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pos_covxx", par_pos_covxx.begin(), par_pos_covxx.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pos_covyy", par_pos_covyy.begin(), par_pos_covyy.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pos_covxy", par_pos_covxy.begin(), par_pos_covxy.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_px", par_px.begin(), par_px.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_py", par_py.begin(), par_py.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pz", par_pz.begin(), par_pz.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_pt", par_pt.begin(), par_pt.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_p", par_p.begin(), par_p.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_chg", par_chg.begin(), par_chg.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_gho", par_gho.begin(), par_gho.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_chi", par_chi.begin(), par_chi.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_typ", par_typ.begin(), par_typ.end(), "VEL_num", VEL_maxnum );
  test &= tuple->farray( "VEL_par_PVind", par_PVind.begin(), par_PVind.end(), "VEL_num", VEL_maxnum );

  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}//end fillVeloTracksInfo

StatusCode TupleToolMuonVariables::fillUpstreamTracksInfo(Tuples::Tuple& tuple){
  bool test=true;
  vector<double> par_pos_x, par_pos_y, par_pos_z;
  vector<double> par_pos_covxx,par_pos_covyy,par_pos_covxy;

  vector<double> par_px, par_py, par_pz, par_pt, par_p, par_chg;
  vector<double> par_slope_covxx,par_slope_covyy,par_slope_covxy;//NB: slope c.v. derivable from mom vec
  vector<double> par_gho, par_chi;
  vector<int> par_typ;
  vector<double> par_NNe,par_NNmu,par_NNpi,par_NNk,par_NNp,par_NNghost;//MR: PID ProbNN
  vector<double> par_DLLe,par_DLLmu,par_DLLk,par_DLLp;//MR: PID DLL
  vector<double> par_isMuon;//MR: isMuon
  vector<int> par_PVind;

  //initialize the vectors for track<-->PV association
  std::vector<double> trkPV_pz, trkPV_gho;
  std::vector<int> trkPV_num;
  trkPVassociator(trkPV_pz,trkPV_gho,trkPV_num,"upstream");
  
  std::string m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  LHCb::ProtoParticles * protos = getIfExists<ProtoParticles>(m_protoPath);
  int US_maxnum=200;
  for ( ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto ){
    const LHCb::Track* track=(*iProto)->track();
    
    if (track->type()!=4) continue;//skip tracks != Upstream
    Gaudi::XYZPoint track_pos;
    Gaudi::XYZVector track_mom;
    Gaudi::SymMatrix6x6 track_posmom_cov;
    track->positionAndMomentum(track_pos,track_mom,track_posmom_cov);
    //pos e cov matrix
    par_pos_x.push_back(track_pos.x());
    par_pos_y.push_back(track_pos.y());
    par_pos_z.push_back(track_pos.z());
    par_pos_covxx.push_back(track_posmom_cov(0,0));
    par_pos_covyy.push_back(track_posmom_cov(1,1));
    par_pos_covxy.push_back(track_posmom_cov(0,1));
    
    //momentum and charge
    par_px.push_back(track_mom.x());
    par_py.push_back(track_mom.y());
    par_pz.push_back(track_mom.z());
    par_pt.push_back(track->pt());
    par_p.push_back(track->p());
    par_chg.push_back(track->charge());
    
    Gaudi::XYZVector track_slope;
    Gaudi::SymMatrix3x3 track_slope_cov;
    track->slopes(track_slope,track_slope_cov);
    par_slope_covxx.push_back(track_slope_cov(0,0));
    par_slope_covyy.push_back(track_slope_cov(1,1));
    par_slope_covxy.push_back(track_slope_cov(0,1));
    
    //other info
    par_gho.push_back(track->ghostProbability());
    par_chi.push_back(track->chi2PerDoF());
    par_typ.push_back(track->type());
    
    //PID info
    par_NNe.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNe,-9999));
    par_NNmu.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNmu,-9999));
    par_NNpi.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNpi,-9999));
    par_NNk.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNk,-9999));
    par_NNp.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNp,-9999));
    par_NNghost.push_back((*iProto)->info(LHCb::ProtoParticle::ProbNNghost,-9999));
    par_DLLe.push_back((*iProto)->info(LHCb::ProtoParticle::CombDLLe,-9999));
    par_DLLmu.push_back((*iProto)->info(LHCb::ProtoParticle::CombDLLmu,-9999));
    par_DLLk.push_back((*iProto)->info(LHCb::ProtoParticle::CombDLLk,-9999));
    par_DLLp.push_back((*iProto)->info(LHCb::ProtoParticle::CombDLLp,-9999));
    int ismuon_flag=0;
    if ((*iProto)->muonPID()) ismuon_flag=(*iProto)->muonPID()->IsMuon()?1:0;
    par_isMuon.push_back(ismuon_flag);

    //PV association
    par_PVind.push_back(getAssociatedPV(track,trkPV_pz,trkPV_gho,trkPV_num));
  }

  test &= tuple->farray( "US_par_pos_x", par_pos_x.begin(), par_pos_x.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pos_y", par_pos_y.begin(), par_pos_y.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pos_z", par_pos_z.begin(), par_pos_z.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pos_covxx", par_pos_covxx.begin(), par_pos_covxx.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pos_covyy", par_pos_covyy.begin(), par_pos_covyy.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pos_covxy", par_pos_covxy.begin(), par_pos_covxy.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_px", par_px.begin(), par_px.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_py", par_py.begin(), par_py.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pz", par_pz.begin(), par_pz.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_pt", par_pt.begin(), par_pt.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_p", par_p.begin(), par_p.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_chg", par_chg.begin(), par_chg.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_gho", par_gho.begin(), par_gho.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_chi", par_chi.begin(), par_chi.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_typ", par_typ.begin(), par_typ.end(), "US_num", US_maxnum );

  test &= tuple->farray( "US_par_NNe", par_NNe.begin(), par_NNe.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_NNmu", par_NNmu.begin(), par_NNmu.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_NNpi", par_NNpi.begin(), par_NNpi.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_NNk", par_NNk.begin(), par_NNk.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_NNp", par_NNp.begin(), par_NNp.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_NNghost", par_NNghost.begin(), par_NNghost.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_DLLe", par_DLLe.begin(), par_DLLe.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_DLLmu", par_DLLmu.begin(), par_DLLmu.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_DLLk", par_DLLk.begin(), par_DLLk.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_DLLp", par_DLLp.begin(), par_DLLp.end(), "US_num", US_maxnum );
  test &= tuple->farray( "US_par_isMuon", par_isMuon.begin(), par_isMuon.end(), "US_num", US_maxnum );

  test &= tuple->farray( "US_par_PVind", par_PVind.begin(), par_PVind.end(), "US_num", US_maxnum );
  if(test) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE; 

}//end fillUpstreamTracksInfo

void TupleToolMuonVariables::trkPVassociator(std::vector<double>& trkPV_pz, std::vector<double>& trkPV_gho,std::vector<int>& trkPV_num,std::string trk_type){
  if (trkPV_pz.size()!=trkPV_gho.size() || trkPV_pz.size()!=trkPV_num.size())
    { cout<<"ERROR in TupleToolMuonVariables::trkPVassociator: trkPV_pz, trkPV_gho, trkPV_num siz ="<<trkPV_pz.size()<<" "<<trkPV_gho.size()<<" " <<trkPV_num.size()<<endl; return; }
  if (trkPV_pz.size()>0) return;//the vectors have been already filled
  
  //Get the list of primary vertices and loop over them
  int pv_index(0);
  LHCb::RecVertex::Container* verts = NULL;
  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)) verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  LHCb::RecVertex::Container::const_iterator iv;
  for ( iv = verts->begin(); iv != verts->end(); iv++) {
    //get the tracks associated to the PV and loop over them
    for (SmartRefVector< LHCb::Track >::const_iterator it_ptrk=(*iv)->tracks().begin(); it_ptrk!=(*iv)->tracks().end(); ++it_ptrk){
      const Track* track=(*it_ptrk);
      int type=track->type();
      if (!( (trk_type=="velo" && (type==1 || type==2)) ||
	     (trk_type=="long" && type==3) ||
	     (trk_type=="upstream" && type==4) )) continue; 
      trkPV_pz.push_back(track->momentum().z());
      trkPV_gho.push_back(track->ghostProbability());
      trkPV_num.push_back(pv_index);
    }//end it_ptrk!=(*iv)->tracks.end() loop
    pv_index++;
  }//end iv!=verts->end()
  

  return;
}//end trkPVassociator

int TupleToolMuonVariables::getAssociatedPV(const LHCb::Track* track, const std::vector<double>& trkPV_pz, const std::vector<double>& trkPV_gho, const std::vector<int>& trkPV_num){
  int output=0;
  if (trkPV_pz.size()!=trkPV_gho.size() || trkPV_pz.size()!=trkPV_num.size())
    { cout<<"ERROR in TupleToolMuonVariables::trkPVassociator: trkPV_pz, trkPV_gho, trkPV_num siz ="<<trkPV_pz.size()<<" "<<trkPV_gho.size()<<" " <<trkPV_num.size()<<endl; return output; }
  double pz_trk=track->momentum().z();
  double gho_trk=track->ghostProbability();
  for (int i=0;i<(int)trkPV_pz.size();i++){
    if (trkPV_pz.at(i)==pz_trk && trkPV_gho.at(i)==gho_trk){ 
      output+=pow(2,trkPV_num.at(i));
      //cout<<"match: pz = "<<pz_trk<<" trkPV_num.at(i) = "<<trkPV_num.at(i)<<" output = "<<output<<endl;
    }
  }
  return output;
}//end getAssociatedPV
