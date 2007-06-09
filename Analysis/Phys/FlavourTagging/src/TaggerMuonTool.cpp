// Include files 
#include "TaggerMuonTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerMuonTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerMuonTool );

//====================================================================
TaggerMuonTool::TaggerMuonTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent ) :
  GaudiTool ( type, name, parent ), m_eventSvc(0) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",    m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName     = "NNetTool_v1" );
  declareProperty( "Muon_Pt_cut", m_Pt_cut_muon   = 1.1 *GeV );
  declareProperty( "Muon_P_cut",  m_P_cut_muon    = 0.0 *GeV );
  declareProperty( "Muon_lcs_cut",m_lcs_cut_muon  = 4.0 );
  declareProperty( "AverageOmega",m_AverageOmega  = 0.33 );
  m_nnet = 0;
  m_util = 0;
}
TaggerMuonTool::~TaggerMuonTool() {}; 

//=====================================================================
StatusCode TaggerMuonTool::initialize() { 

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
  }
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerMuonTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                            std::vector<const Vertex*>& allVtx, 
                            Particle::ConstVector& vtags ){
  Tagger tmu;
  if(!RecVert) return tmu;
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);

  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* imuon=0;
  double ptmaxm = -99.0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 13 ) continue;

    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_muon ) continue;

    double P = (*ipart)->p();
    if( P  < m_P_cut_muon ) continue;

    const ProtoParticle* proto = (*ipart)->proto();
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    debug() << " Muon P="<< P <<" Pt="<< Pt <<" muonNSH="<<muonNSH<<endreq;
    if( muonNSH != 0 ) continue;

    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    if(lcs>m_lcs_cut_muon) continue;

    if( Pt > ptmaxm ) { //Pt ordering
      imuon = (*ipart);
      ptmaxm = Pt;
    }
  }
  if( ! imuon ) return tmu;

  //calculate omega
  double pn = 1 - m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    Gaudi::LorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.Theta();
    double B0p    = ptotB.P()/GeV;
    double IP, IPerr, ip, iperr, IPT=0.;

    m_util->calcIP(imuon, RecVert, IP, IPerr); //calculate IP
    if(SecVert) {
      m_util->calcIP(imuon, SecVert, ip, iperr); //calculate IPT
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> inputs(8);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = m_util->countTracks(vtags);
    inputs.at(3) = 100;
    inputs.at(4) = imuon->p()/GeV;
    inputs.at(5) = imuon->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    
    pn = m_nnet->MLPm( inputs );

  }

  if(pn<0.5){
    pn = 1-pn;
    tmu.setOmega( 1-pn );
    tmu.setDecision(imuon->charge()>0 ? 1: -1);
  } else {
    tmu.setOmega( 1-pn );
    tmu.setDecision(imuon->charge()>0 ? -1: 1);
  }
  tmu.setType( Tagger::OS_Muon ); 
  tmu.addTaggerPart(*imuon);

  return tmu;
}

//====================================================================
StatusCode TaggerMuonTool::finalize() { return StatusCode::SUCCESS; }

