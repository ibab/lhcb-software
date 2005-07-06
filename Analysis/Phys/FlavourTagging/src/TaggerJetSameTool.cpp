// Include files 
#include "TaggerJetSameTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerJetSameTool
//
// Author: Julien Babel 
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerJetSameTool>          s_factory ;
const        IToolFactory& TaggerJetSameToolFactory = s_factory ; 

//====================================================================
TaggerJetSameTool::TaggerJetSameTool( const std::string& type,
				      const std::string& name,
				      const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "JetSame_Pt_cut", m_Pt_cut_jetS = 0.25 );
  declareProperty( "JetSame_dR_cut", m_dR_cut_jetS = 0.80 );
  declareProperty( "AverageOmega", m_AverageOmega  = 0.45 );

}
TaggerJetSameTool::~TaggerJetSameTool() {}; 

//=====================================================================
StatusCode TaggerJetSameTool::initialize() { return StatusCode::SUCCESS; }

//=====================================================================
Tagger TaggerJetSameTool::tag( const Particle* AXB0, 
			       std::vector<const Vertex*>& allVtx, 
			       ParticleVector& vtags ) {
  Tagger tjetS;
  double JetS = 0; 
  double aux  = 0;
  double norm = 0;
  double k    = 1.1;
  double Jetcut= 0.2;

  verbose() << " allVtxsize=" << allVtx.size() <<endreq;
  ParticleVector::const_iterator ip;
  for( ip = vtags.begin(); ip != vtags.end(); ip++ ) {
    Particle* axp = (*ip);
    double Pt = axp->pt()/GeV;
    if( Pt < m_Pt_cut_jetS )  continue;

    HepLorentzVector ptotB = AXB0->momentum();
    double etasig= -log(tan(ptotB.theta())/2);
    double eta   = -log(tan(axp->momentum().theta()/2));
    double dphi  = fabs(axp->momentum().phi() - ptotB.phi()); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    //cone condition
    double dR = sqrt(pow((eta-etasig),2)+pow(dphi,2));
    if ( dR < m_dR_cut_jetS) {
      // Construction of a Jet charge same side with no kaonS(pionS)
      //if ((*ip) != ikaonS || (*ip) != ipionS){
      aux  += pow((*ip)->pt()/GeV,k)*(*ip)->charge();
      norm += pow((*ip)->pt()/GeV,k);
      //}
      debug() << " JetS Pt="<< Pt << " dR=" << dR <<endreq;
    }
  }
  //build jet charge same side
  if (norm) if ( aux/norm<(-Jetcut) || aux/norm>Jetcut ) JetS = aux/norm;
  if(!JetS) return tjetS;
  
  tjetS.setDecision(JetS>0 ? 1: -1);
  tjetS.setOmega( m_AverageOmega );
  tjetS.setType( Tagger::SS_jetCharge ); 

  return tjetS;
}
//==========================================================================
StatusCode TaggerJetSameTool::finalize() { return StatusCode::SUCCESS; }

