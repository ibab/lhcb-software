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
  declareProperty( "JetSame_dR_cut", m_dR_cut_jetS = 0.8 );

}
TaggerJetSameTool::~TaggerJetSameTool() {}; 

//=====================================================================
StatusCode TaggerJetSameTool::initialize() { return StatusCode::SUCCESS; }

//=====================================================================
ParticleVector TaggerJetSameTool::taggers( const Particle* AXB0, 
					   const Vertex* RecVert, 
					   const ParticleVector& vtags ){
  //select jetS sameside 
  ParticleVector vjetS(0);
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    Particle* axp = (*ipart);
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
      vjetS.push_back( axp );
      debug() << " JetS Pt="<< Pt << " dR=" << dR <<endreq;
      verbose() << " VTXz=" << RecVert->position().z() <<endreq;
    }
  }
  return vjetS;
}
//==========================================================================
StatusCode TaggerJetSameTool::finalize() { return StatusCode::SUCCESS; }

