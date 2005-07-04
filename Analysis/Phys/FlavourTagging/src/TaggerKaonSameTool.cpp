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

  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.4 );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 4.0 );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS = 2.5 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS = 1.1 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS = 1.0 );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS  = 1.5 );
  declareProperty( "KaonS_upstreamTrack_LCS_cut", m_lcs_kSu = 2.5 );

}
TaggerKaonSameTool::~TaggerKaonSameTool() {}; 

//=====================================================================
StatusCode TaggerKaonSameTool::initialize() { 

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }

return StatusCode::SUCCESS; 
}

//=====================================================================
ParticleVector TaggerKaonSameTool::taggers( Particle* AXB0, 
					    Vertex* RecVert, 
					    std::vector<Particle*> vtags ){

  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vkaonS(0);
  Particle* kaonS=0;
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

      HepLorentzVector ptotB = AXB0->momentum();
      double B0mass= ptotB.m()/GeV;
      double B0the = ptotB.theta();
      double B0phi = ptotB.phi();
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
	TrStoredTrack* track = proto->track();
	if((track->measurements()).size() > 5)
	  lcs = track->lastChiSq()/((track->measurements()).size()-5);
	if(     track->forward()   ) trtyp = 1;
	else if(track->isUpstream()) trtyp = 3;
      }
      debug()<< "      trtyp=" << trtyp << " lcs=" << lcs << endreq; 
      if( trtyp==1 || (trtyp==3 && lcs< m_lcs_kSu ) ) {
	if( Pt > ptmaxkS ) { 
	  kaonS = (*ipart);
	  ptmaxkS = Pt;
	}
      }
    }
  } 
  if( kaonS ) vkaonS.push_back( kaonS );

  return vkaonS;
}
//====================================================================
void TaggerKaonSameTool::calcIP( Particle* axp, 
				 Vertex* RecVert, 
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
//==========================================================================
StatusCode TaggerKaonSameTool::finalize() { return StatusCode::SUCCESS; }

