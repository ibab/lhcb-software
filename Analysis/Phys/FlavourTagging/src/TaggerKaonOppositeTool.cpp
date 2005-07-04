// Include files 
#include "TaggerKaonOppositeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonOppositeTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerKaonOppositeTool>          s_factory ;
const        IToolFactory& TaggerKaonOppositeToolFactory = s_factory ; 

//====================================================================
TaggerKaonOppositeTool::TaggerKaonOppositeTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "Kaon_Pt_cut",  m_Pt_cut_kaon = 0.4 );
  declareProperty( "Kaon_P_cut",   m_P_cut_kaon  = 3.0 );
  declareProperty( "Kaon_IP_cut",  m_IP_cut_kaon   = 3.5 );
  declareProperty( "Kaon_forwardTrack_LCS_cut", m_lcs_kf = 2.5 );
  declareProperty( "Kaon_matchTrack_LCS_cut",   m_lcs_km = 1.4 );
  declareProperty( "Kaon_upstreamTrack_LCS_cut",m_lcs_ku = 2.5 );
  declareProperty( "Kaon_forwardTrack_IP_cut",  m_IP_kf  = 2.0 );
  declareProperty( "Kaon_matchTrack_IP_cut",    m_IP_km  = 1.0 );
  declareProperty( "Kaon_upstreamTrack_IP_cut", m_IP_ku  = 1.0 );
}
TaggerKaonOppositeTool::~TaggerKaonOppositeTool() {}; 

//=====================================================================
StatusCode TaggerKaonOppositeTool::initialize() { 

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
ParticleVector TaggerKaonOppositeTool::taggers( const Particle* AXB0, 
                                                const Vertex* RecVert, 
                                                const ParticleVector& vtags ){
  verbose()<< "B pt=" << AXB0->pt()/GeV <<endreq;

  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vkaon(0);
  Particle* kaon=0;
  double ptmaxk = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    if( (*ipart)->particleID().abspid() != 321 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_kaon )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_kaon )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    debug() << " Kaon P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
            << " IP=" << IP <<endreq;

    if(IPsig > m_IP_cut_kaon ) {
      long   trtyp= 0;
      double lcs  = 1000.;
      ContainedObject* contObj = (*ipart)->origin();
      if (contObj) {
        ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
        if ( proto) {
          TrStoredTrack* track = proto->track();
          if((track->measurements()).size() > 5)
            lcs = track->lastChiSq()/((track->measurements()).size()-5);
          if(     track->forward()   ) trtyp = 1;
          else if(track->match()     ) trtyp = 2;
          else if(track->isUpstream()) trtyp = 3;
        }
      }
      
      debug()<< "      trtyp=" << trtyp << " lcs=" << lcs << endreq; 
      if((trtyp==1 && lcs<m_lcs_kf && fabs(IP)<m_IP_kf) ||
         (trtyp==2 && lcs<m_lcs_km && fabs(IP)<m_IP_km) ||
         (trtyp==3 && lcs<m_lcs_ku && fabs(IP)<m_IP_ku) ) {
        if( Pt > ptmaxk ) { 
          kaon = (*ipart);
          ptmaxk = Pt;
        }
      }
    }
  } 
  if( kaon ) vkaon.push_back( kaon );

  return vkaon;
}
//====================================================================
void TaggerKaonOppositeTool::calcIP( const Particle* axp, 
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
//==========================================================================
StatusCode TaggerKaonOppositeTool::finalize() { return StatusCode::SUCCESS; }

