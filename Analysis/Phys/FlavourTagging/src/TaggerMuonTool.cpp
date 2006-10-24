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
  GaudiTool ( type, name, parent ), m_eventSvc(0), m_Geom(0) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",    m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName     = "NNetTool_v1" );
  declareProperty( "Muon_Pt_cut", m_Pt_cut_muon  = 1.2 );
  declareProperty( "Muon_P_cut",  m_P_cut_muon   = 5.0 );
  declareProperty( "AverageOmega",m_AverageOmega = 0.33 );
  declareProperty( "muonDLL_cut", m_muonDLL_cut  = -99. );
  declareProperty( "muonNSH_cut", m_muonNSH_cut  = 0 );
  m_nnet = 0;
}
TaggerMuonTool::~TaggerMuonTool() {}; 

//=====================================================================
StatusCode TaggerMuonTool::initialize() { 

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
  }
  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
//   m_muonIDdll = tool<IMuonIDDLLTool>("MuonIDDLLTool");
//   if(! m_muonIDdll) {
//     fatal() << "Unable to retrieve MuonIDDLLTool"<< endreq;
//     return StatusCode::FAILURE;
//   }
//   m_muonIDnsh = tool<IMuonIDNSharedHitsTool>("MuonIDNSharedHitsTool");
//   if(! m_muonIDnsh ) {
//     fatal() << "Unable to retrieve MuonIDNSharedHitsTool"<< endreq;
//     return StatusCode::FAILURE;
//   }
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
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_muon ) continue;
    double P = (*ipart)->p()/GeV;
    if( P  < m_P_cut_muon ) continue;
    debug() << " Muon P="<< P <<" Pt="<< Pt <<endreq;

    // Load muonPid results
    double muonDLL=-999.;
    int  muonNSH  = -1;
    bool muonData = false;
//     SmartDataPtr<MuonIDs> muonpids ( m_eventSvc, MuonIDLocation::Default );//xxx
//     if( !muonpids ) {
//       info()<<"Failed to retrieve any MuonIDs "<<endreq;
//     } else {
//       muonData = true;
//       debug()<<"Successfully retrieved MuonIDs "<< muonpids->size()<<endreq;
//     }
//     const ProtoParticle* proto = (*ipart)->proto();
//     if ( muonData ) 
//       if( proto ) {
//       const Track* trackMu = proto->track();
//       /// Look at the number of shared hits
//       MuonIDs::const_iterator iMuon;
//       for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
// 	const Track* MuonIDtrack = (*iMuon)->idTrack();
// 	if( MuonIDtrack == trackMu ) {
// 	  // When MuonDet is capable of identifying it add its result
// 	  if( (*iMuon)->IsMuon() ) {
// 	    m_muonIDdll->calcMuonDLL( *iMuon, muonDLL );
// 	    m_muonIDnsh->calcSharedHits( *iMuon, muonNSH );
// 	  }
// 	}
//       }
//     }
//     debug() << "muonDLL= " <<muonDLL<<"  muonNSH= "<<muonNSH<< endreq;
//     if( muonDLL < m_muonDLL_cut ) continue;
//     if( muonNSH > m_muonNSH_cut ) continue;

    if( Pt > ptmaxm ) { //Pt ordering
      imuon = (*ipart);
      ptmaxm = Pt;
    }   
  }
  if( ! imuon ) return tmu;

  tmu.addTaggerPart(*imuon);
  tmu.setDecision(imuon->charge()>0 ? -1: 1);

  //calculate omega
  double pn = 1 - m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    Gaudi::LorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.Theta();
    double B0p    = ptotB.P()/GeV;
    double IP, IPerr, ip, iperr, IPT=0.;

    calcIP(imuon, RecVert, IP, IPerr); //calculate IP
    if(SecVert) {
      calcIP(imuon, SecVert, ip, iperr); //calculate IPT
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> inputs(8);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = vtags.size();
    inputs.at(3) = 100;
    inputs.at(4) = imuon->p()/GeV;
    inputs.at(5) = imuon->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    
    //calculate the result of neural net, the higher rnet is,
    //the more reliable is the tagger:
    //pn is the probability that the tagger is giving the
    //correct answer for the B flavour. the hard-coded numbers
    //are obtained by fitting the ratio of the 'rnet' distribution
    //for right muon tags and wrong muon tags:

    pn = m_nnet->MLPm( inputs );

  }
  tmu.setOmega( 1-pn );
  tmu.setType( Tagger::OS_Muon ); 

  return tmu;
}
//==========================================================================
StatusCode TaggerMuonTool::calcIP( const Particle* axp,
                                   const Vertex* RecVert,
                                   double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ;
    iperr= iperr;
  }
  return sc;
}
//==========================================================================
StatusCode TaggerMuonTool::calcIP( const Particle* axp,
                                   const RecVertex* RecVert,
                                   double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ;
    iperr= iperr;
  }
  return sc;
}
//====================================================================
StatusCode TaggerMuonTool::finalize() { return StatusCode::SUCCESS; }

