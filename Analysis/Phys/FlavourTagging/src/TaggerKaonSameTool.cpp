// Include files 
#include "TaggerKaonSameTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerKaonSameTool );

//====================================================================
TaggerKaonSameTool::TaggerKaonSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_v1" );
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
  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerKaonSameTool::tag( const Particle* AXB0, const RecVertex* RecVert,
				std::vector<const Vertex*>& allVtx, 
				Particle::ConstVector& vtags ){
  Tagger tkaonS;
  if(!RecVert) return tkaonS;
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);

  Gaudi::LorentzVector ptotB = AXB0->momentum();
  double B0mass= ptotB.M()/GeV;
  double B0the = ptotB.Theta();
  double B0phi = ptotB.Phi();

  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaonS=0;
  double ptmaxkS = -99.0;
  Particle::ConstVector::const_iterator ipart;
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
      debug() << " passa" <<endreq; 
      double deta  = fabs(log(tan(B0the/2.)/tan(asin(Pt/P)/2.)));
      double dphi  = fabs((*ipart)->momentum().Phi() - B0phi); 
      if(dphi>3.1416) dphi=6.2832-dphi;
      double dQ    = (ptotB+(*ipart)->momentum()).M()/GeV - B0mass;
      debug()<< "      deta=" << deta << " dphi=" << dphi 
             << " dQ=" << dQ << endreq; 
      if(dphi > m_phicut_kaonS) continue;
      if(deta > m_etacut_kaonS) continue;//xxx
      if(dQ   > m_dQcut_kaonS ) continue;

      long   trtyp= 3; //sometimes trtyp remains =0??? put default=3
//      long   trtyp= 0;
      double lcs  = 1000.;
      const ProtoParticle* proto = (*ipart)->proto();
      if ( proto ) {
	const Track* track = proto->track();
        lcs = track->chi2PerDoF();
	if( track->type() == Track::Long ) trtyp = 1;
	else if( track->type() == Track::Upstream ) trtyp = 3;
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
  if( !ikaonS ) return tkaonS;

  tkaonS.addTaggerPart(*ikaonS);
  tkaonS.setDecision(ikaonS->charge()>0 ? 1: -1);

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    double IP, IPerr, ip, iperr, IPT=0.;
    double B0p = ptotB.P()/GeV;
    double ang = asin((ikaonS->pt()/GeV)/(ikaonS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ikaonS->momentum().Phi()-B0phi), 
			  6.283-fabs(ikaonS->momentum().Phi()-B0phi));
    double dQ  = (ptotB+ikaonS->momentum()).M()/GeV - B0mass;
    calcIP(ikaonS, RecVert, IP, IPerr);
    if(SecVert) {
      calcIP(ikaonS, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    //all NNets have same inputs here, so dont change
    //the structure. Just add variables, if needed.
    std::vector<double> inputs(12);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = vtags.size();
    inputs.at(3) = 100;//tampering placeholder
    inputs.at(4) = ikaonS->p()/GeV;
    inputs.at(5) = ikaonS->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    inputs.at(8) = 0.;
    inputs.at(9) = deta;
    inputs.at(10)= dphi;
    inputs.at(11)= dQ;    
   
    pn = m_nnet->MLPkS( inputs );

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
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =  m_Geom->calcImpactPar(*axp, *RecVert, ip,
                                         iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
}
//====================================================================
void TaggerKaonSameTool::calcIP( const Particle* axp, 
                                 const RecVertex* RecVert, 
                                 double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =  m_Geom->calcImpactPar(*axp, *RecVert, ip,
                                         iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
}
//==========================================================================
StatusCode TaggerKaonSameTool::finalize() { return StatusCode::SUCCESS; }

