// Include files 
#include "TaggerPionSameTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerPionSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerPionSameTool>          s_factory ;
const        IToolFactory& TaggerPionSameToolFactory = s_factory ; 

//====================================================================
TaggerPionSameTool::TaggerPionSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_v1" );
  declareProperty( "ProbMin", m_ProbMin = 0.52);
  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS = 0.2 );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS  = 2.0 );
  declareProperty( "PionSame_IP_cut", m_IP_cut_pionS = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS  = 3.0 );
  declareProperty( "PionS_upstreamTrack_LCS_cut", m_lcs_pSu = 2.5 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.44 );
  m_nnet = 0;
  m_Geom = 0;
}
TaggerPionSameTool::~TaggerPionSameTool() {}; 

//=====================================================================
StatusCode TaggerPionSameTool::initialize() { 

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
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
Tagger TaggerPionSameTool::tag( const Particle* AXB0, 
				std::vector<const Vertex*>& allVtx, 
				ParticleVector& vtags ){
  Tagger tpionS;
  const Vertex *RecVert=0, *SecVert=0;
  std::vector<const Vertex*>::const_iterator iv;
  for( iv=allVtx.begin(); iv!=allVtx.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ) RecVert = (*iv);
    if( (*iv)->type() == Vertex::Kink    ) SecVert = (*iv);
  } 
  if(!RecVert) return tpionS;

  HepLorentzVector ptotB = AXB0->momentum();
  double B0mass = ptotB.m()/GeV;

  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle* ipionS=0;
  double ptmaxpS = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 211 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_pionS )  continue;
    double P = (*ipart)->p()/GeV;
    if( P  < m_P_cut_pionS )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    debug() << " PioS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
            << " IP=" << IP <<endreq;

    if(IPsig < m_IP_cut_pionS) {
      double dQ    = (ptotB+(*ipart)->momentum()).m()/GeV - B0mass;
      debug() << "      dQ=" << dQ << endreq; 
      if(dQ > m_dQcut_pionS ) continue;

      long   trtyp= 0;
      double lcs  = 1000.;
      ContainedObject* contObj = (*ipart)->origin();
      if (contObj) {
        ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
        if ( proto ) {
          TrStoredTrack* track = proto->track();
          if((track->measurements()).size() > 5)
            lcs = track->lastChiSq()/((track->measurements()).size()-5);
          if(     track->forward()   ) trtyp = 1;
          else if(track->isUpstream()) trtyp = 3;
        }
      }
      debug()<< "      trtyp=" << trtyp << " lcs=" << lcs << endreq; 
      if(trtyp==1 || (trtyp==3 && lcs<m_lcs_pSu)) {
        if( Pt > ptmaxpS ) { 
          ipionS = (*ipart);
          ptmaxpS = Pt;
        }
      }
    }
  } 
  if( !ipionS ) return tpionS;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double rnet, IP, IPerr, ip, iperr, IPT=0.;
    double B0p  = ptotB.vect().mag()/GeV;
    double B0the= ptotB.theta();
    double B0phi= ptotB.phi();
    double ang = asin((ipionS->pt()/GeV)/(ipionS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ipionS->momentum().phi()-B0phi), 
			  6.283-fabs(ipionS->momentum().phi()-B0phi));
    double dQ  = (ptotB+ipionS->momentum()).m()/GeV - B0mass;
    calcIP(ipionS, RecVert, IP, IPerr);
    if(SecVert) {
      calcIP(ipionS, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> inputs(12);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = vtags.size();
    inputs.at(3) = 100;
    inputs.at(4) = ipionS->p()/GeV;
    inputs.at(5) = ipionS->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    inputs.at(8) = 0.;
    inputs.at(9) = deta;
    inputs.at(10)= dphi;
    inputs.at(11)= dQ;
    
    rnet = m_nnet->MLPpS( inputs );

    pn = 1.0-pol2(rnet, 1.0772, -1.1632); 
    if( pn < m_ProbMin ) return tpionS;
  }

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( AXB0->particleID().hasUp() ) tagdecision = -tagdecision; //is Bu
  tpionS.setDecision( tagdecision );
  tpionS.setOmega( 1-pn );
  tpionS.setType( Tagger::SS_Pion ); 
  tpionS.addTaggerPart(*ipionS);

  return tpionS;
}
//====================================================================
void TaggerPionSameTool::calcIP( const Particle* axp, 
                                 const Vertex* RecVert, 
                                 double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  StatusCode sc =  m_Geom->calcImpactPar(*axp, *RecVert, ip,
                                         iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
}
double TaggerPionSameTool::pol2(double x, double a0, double a1) { 
  return a0+a1*x; 
}
//==========================================================================
StatusCode TaggerPionSameTool::finalize() { return StatusCode::SUCCESS; }

