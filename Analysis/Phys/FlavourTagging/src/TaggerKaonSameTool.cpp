// Include files 
#include "TaggerKaonSameTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerKaonSameTool>          s_factory ;
const        IToolFactory& TaggerKaonSameToolFactory = s_factory ; 

//====================================================================
TaggerKaonSameTool::TaggerKaonSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.4 );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 4.0 );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS = 2.5 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS = 1.1 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS = 1.0 );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS  = 1.5 );
  declareProperty( "KaonS_upstreamTrack_LCS_cut", m_lcs_kSu = 2.5 );
  declareProperty( "AverageOmega",    m_AverageOmega = 0.336 );
  m_nnet = 0;
  m_Geom = 0;
}
TaggerKaonSameTool::~TaggerKaonSameTool() {}; 

//=====================================================================
StatusCode TaggerKaonSameTool::initialize() { 

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
Tagger TaggerKaonSameTool::tag( const Particle* AXB0, 
				std::vector<const Vertex*>& allVtx, 
				ParticleVector& vtags ){
  Tagger tkaonS;
  const Vertex *RecVert=0, *SecVert=0;
  std::vector<const Vertex*>::const_iterator iv;
  for( iv=allVtx.begin(); iv!=allVtx.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ) RecVert = (*iv);
    if( (*iv)->type() == Vertex::Kink    ) SecVert = (*iv);
  } 
  if(!RecVert) return tkaonS;

  HepLorentzVector ptotB = AXB0->momentum();
  double B0mass= ptotB.m()/GeV;
  double B0the = ptotB.theta();
  double B0phi = ptotB.phi();

  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle* ikaonS=0;
  double ptmaxkS = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 321 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_kaonS )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_kaonS )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    debug() << " KaoS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
            << " IP=" << IP <<endreq;

    if(IPsig < m_IP_cut_kaonS) {
      double deta  = fabs(log(tan(B0the/2.)/tan(asin(Pt/P)/2.)));
      double dphi  = fabs((*ipart)->momentum().phi() - B0phi); 
      if(dphi>3.1416) dphi=6.2832-dphi;
      double dQ    = (ptotB+(*ipart)->momentum()).m()/GeV - B0mass;
      debug()<< "      deta=" << deta << " dphi=" << dphi 
             << " dQ=" << dQ << endreq; 
      if(dphi > m_phicut_kaonS) continue;
      if(deta > m_etacut_kaonS) continue;
      if(dQ   > m_dQcut_kaonS ) continue;

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
      if( trtyp==1 || (trtyp==3 && lcs< m_lcs_kSu ) ) {
        if( Pt > ptmaxkS ) { 
          ikaonS  = (*ipart);
          ptmaxkS = Pt;
        }
      }
    }
  } 
  if( ! ikaonS ) return tkaonS;

  tkaonS.addTaggerPart(*ikaonS);
  tkaonS.setDecision(ikaonS->charge()>0 ? 1: -1);

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    double rnet, IP, IPerr, ip, iperr, IPT=0.;
    double B0p = ptotB.vect().mag()/GeV;
    double ang = asin((ikaonS->pt()/GeV)/(ikaonS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ikaonS->momentum().phi()-B0phi), 
			  6.283-fabs(ikaonS->momentum().phi()-B0phi));
    double dQ  = (ptotB+ikaonS->momentum()).m()/GeV - B0mass;
    calcIP(ikaonS, RecVert, IP, IPerr);
    if(SecVert) {
      calcIP(ikaonS, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 
    rnet = m_nnet->MLPkS(B0p, B0the, vtags.size(), 100, 
			 ikaonS->p()/GeV, ikaonS->pt()/GeV,IP/IPerr, IPT,
			 deta, dphi, dQ);
    pn = 1.0-pol2(rnet, 1.0007, -1.0049);
  }
  tkaonS.setOmega( 1-pn );
  tkaonS.setType( Tagger::SS_Kaon ); 

  return tkaonS;
}
//====================================================================
void TaggerKaonSameTool::calcIP( const Particle* axp, 
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
double TaggerKaonSameTool::pol2(double x, double a0, double a1) { 
  return a0+a1*x; 
}
//==========================================================================
StatusCode TaggerKaonSameTool::finalize() { return StatusCode::SUCCESS; }

