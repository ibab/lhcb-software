// Include files 
#include "TaggerMuonTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerMuonTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerMuonTool>          s_factory ;
const        IToolFactory& TaggerMuonToolFactory = s_factory ; 

//====================================================================
TaggerMuonTool::TaggerMuonTool( const std::string& type,
				const std::string& name,
				const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "Muon_Pt_cut", m_Pt_cut_muon = 1.2 );
  declareProperty( "Muon_P_cut",  m_P_cut_muon  = 5.0 );
  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "AverageOmega", m_AverageOmega = 0.33 );
  m_nnet = 0;
  m_Geom = 0;
}
TaggerMuonTool::~TaggerMuonTool() {}; 

//=====================================================================
StatusCode TaggerMuonTool::initialize() { 

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ("NNetTool", this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }

return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerMuonTool::tag( const Particle* AXB0, 
			    std::vector<const Vertex*>& allVtx, 
			    ParticleVector& vtags ){
  Tagger tmu;
  const Vertex *RecVert=0, *SecVert=0;
  std::vector<const Vertex*>::const_iterator iv;
  for( iv=allVtx.begin(); iv!=allVtx.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ) RecVert = (*iv);
    if( (*iv)->type() == Vertex::Kink    ) SecVert = (*iv);
  } 
  if(!RecVert) return tmu;

  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle* imuon=0;
  double ptmaxm = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 13 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_muon ) continue;
    double P = (*ipart)->p()/GeV;
    if( P  < m_P_cut_muon ) continue;
    debug() << " Muon P="<< P <<" Pt="<< Pt <<endreq;
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
    HepLorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.theta();
    double B0p    = ptotB.vect().mag()/GeV;
    double rnet, IP, IPerr, ip, iperr, IPT=0.;

    calcIP(imuon, RecVert, IP, IPerr); //calculate IP
    if(SecVert) {
      calcIP(imuon, SecVert, ip, iperr); //calculate IPT
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    //calculate the result of neural net, the higher rnet is,
    //the more reliable is the tagger:
    rnet = m_nnet->MLPm(B0p, B0the, vtags.size(), 100, 
			imuon->p()/GeV, imuon->pt()/GeV, IP/IPerr, IPT);

    //pn is the probability that the tagger is giving the
    //correct answer for the B flavour. the hard-coded numbers
    //are obtained by fitting the ratio of the 'rnet' distribution
    //for right muon tags and wrong muon tags:
    pn = 1 - pol3(rnet, 1.2939, -2.0406, 0.90781); //1-omega
  }
  tmu.setOmega( 1-pn );
  tmu.setType( Tagger::OS_Muon ); 

  return tmu;
}
//==========================================================================
double TaggerMuonTool::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}
//==========================================================================
StatusCode TaggerMuonTool::calcIP( Particle* axp, 
				   const Vertex* RecVert, 
				   double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
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

