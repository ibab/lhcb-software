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

  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS = 0.2 );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS  = 2.0 );
  declareProperty( "PionSame_IP_cut", m_IP_cut_pionS = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS  = 3.0 );
  declareProperty( "PionS_upstreamTrack_LCS_cut", m_lcs_pSu = 2.5 );

}
TaggerPionSameTool::~TaggerPionSameTool() {}; 

//=====================================================================
StatusCode TaggerPionSameTool::initialize() { 

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
return StatusCode::SUCCESS; 
}

//=====================================================================
ParticleVector TaggerPionSameTool::taggers( Particle* AXB0, 
					    Vertex* RecVert, 
					    std::vector<Particle*> vtags ){

  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vpionS(0);
  Particle* pionS=0;
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

      HepLorentzVector ptotB = AXB0->momentum();
      double B0mass= ptotB.m()/GeV;
      double dQ    = (ptotB+(*ipart)->momentum()).m()/GeV - B0mass;
      debug() << "      dQ=" << dQ << endreq; 
      if(dQ > m_dQcut_pionS ) continue;

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
      if(trtyp==1 || (trtyp==3 && lcs<m_lcs_pSu)) {
	if( Pt > ptmaxpS ) { 
	  pionS = (*ipart);
	  ptmaxpS = Pt;
	}
      }
    }
  } 
  if( pionS ) vpionS.push_back( pionS );

  return vpionS;
}
//====================================================================
void TaggerPionSameTool::calcIP( Particle* axp, 
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
StatusCode TaggerPionSameTool::finalize() { return StatusCode::SUCCESS; }

