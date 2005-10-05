// Include files 
#include "TaggerElectronTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerElectronTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerElectronTool>          s_factory ;
const        IToolFactory& TaggerElectronToolFactory = s_factory ; 

//====================================================================
TaggerElectronTool::TaggerElectronTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "TrVeloChargeName",m_veloChargeName = "TrVeloCharge" );
  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.0 );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 5.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin  = 21  );
  declareProperty( "VeloChargeMax",m_VeloChMax  = 32  );
  declareProperty( "EoverP",       m_EoverP     = 0.8 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.30 );
  m_nnet = 0;
  m_Geom = 0;
}
TaggerElectronTool::~TaggerElectronTool() {}; 

//=====================================================================
StatusCode TaggerElectronTool::initialize() { 

  m_veloCharge= tool<ITrVeloCharge> (m_veloChargeName, this);
  if ( ! m_veloCharge ) {   
    fatal() << " Unable to retrieve " << m_veloChargeName << endreq;
    return StatusCode::FAILURE;
  }
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
Tagger TaggerElectronTool::tag( const Particle* AXB0, 
				std::vector<const Vertex*>& allVtx,
				ParticleVector& vtags ){
  Tagger tele;
  const Vertex *RecVert=0, *SecVert=0;
  std::vector<const Vertex*>::const_iterator iv;
  for( iv=allVtx.begin(); iv!=allVtx.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ) RecVert = (*iv);
    if( (*iv)->type() == Vertex::Kink    ) SecVert = (*iv);
  } 
  if(!RecVert) return tele;

  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vele(0);
  Particle* iele=0;
  double ptmaxe = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 11 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_ele )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_ele )  continue;

    long   trtyp= 0;
    double Emeas= -1;
    double lcs  = 1000.;
    ContainedObject* contObj = (*ipart)->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      if ( proto ) {
        SmartRefVector<CaloHypo>& vcalo = proto->calo();
        if(vcalo.size()==1) Emeas = (*(vcalo.begin()))->e()/GeV;
        TrStoredTrack* track = proto->track();
        if((track->measurements()).size() > 5)
          lcs = track->lastChiSq()/((track->measurements()).size()-5);
        if(track->forward()) trtyp = 1;
      }
    }
    
    debug() << " Elec P="<<P <<" Pt="<<Pt 
            << " trtyp=" << trtyp << " Emeas/P=" << Emeas/P <<endreq;

    if(Emeas/P > m_EoverP || Emeas<0) {
      if(trtyp == 1) {
	double veloch = m_veloCharge->calculate(*ipart);
	debug() << "      veloch=" <<veloch << endreq;
	if(veloch > m_VeloChMin && veloch < m_VeloChMax ) {
          if( Pt > ptmaxe ) { 
            iele = (*ipart);
            ptmaxe = Pt;
          }
        }
      } 
    }
  }
  if( !iele ) return tele;
  tele.addTaggerPart(*iele);
  tele.setDecision(iele->charge()>0 ? -1: 1);

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    HepLorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.theta();
    double B0p    = ptotB.vect().mag()/GeV;
    double rnet, IP, IPerr, ip, iperr, IPT=0.;
 
    calcIP(iele, RecVert, IP, IPerr);
    if(SecVert) {
      calcIP(iele, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 
    rnet = m_nnet->MLPe(B0p, B0the, vtags.size(), 100, 
			iele->p()/GeV, iele->pt()/GeV, IP/IPerr, IPT);
    pn = 1.0-pol4(rnet, 0.4933, -0.6766, 1.761, -1.587); 
  }
  tele.setOmega( 1-pn );
  tele.setType( Tagger::OS_Electron ); 

  return tele;
}
//==========================================================================
StatusCode TaggerElectronTool::calcIP( Particle* axp, 
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
double TaggerElectronTool::pol4(double x, double a0, double a1, double a2, double a3){
  return a0+a1*x+a2*x*x+a3*pow(x,3);
}
//====================================================================
StatusCode TaggerElectronTool::finalize() { return StatusCode::SUCCESS; }

