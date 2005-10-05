// Include files 
#include "TaggerVertexChargeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerVertexChargeTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerVertexChargeTool>          s_factory ;
const        IToolFactory& TaggerVertexChargeToolFactory = s_factory ; 

//====================================================================
TaggerVertexChargeTool::TaggerVertexChargeTool( const std::string& type,
						const std::string& name,
						const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",     m_CombinationTechnique = "NNet" );
  declareProperty( "AverageOmega", m_AverageOmega         = 0.40 );
  declareProperty( "PowerK",       m_PowerK               = 0.4 );
  declareProperty( "MinimumCharge",m_MinimumCharge        = 0.3 );
  declareProperty( "P0",           m_P0                   = 0.47018 );
  declareProperty( "P1",           m_P1                   = 0.02891 );
  declareProperty( "P2",           m_P2                   = 0.49702 );
}
TaggerVertexChargeTool::~TaggerVertexChargeTool() {}; 

//=====================================================================
StatusCode TaggerVertexChargeTool::initialize() {return StatusCode::SUCCESS;}

//=====================================================================
Tagger TaggerVertexChargeTool::tag( const Particle* AXB0, 
				    std::vector<const Vertex*>& allVtx, 
				    ParticleVector& vtags ){
  Tagger tVch;
  const Vertex *RecVert=0, *SecVert=0;
  std::vector<const Vertex*>::const_iterator iv;
  for( iv=allVtx.begin(); iv!=allVtx.end(); iv++){
    if( (*iv)->type() == Vertex::Primary ) RecVert = (*iv);
    if( (*iv)->type() == Vertex::Kink    ) SecVert = (*iv);
  } 
  if(!SecVert) return tVch;
  if(vtags.empty()) return tVch;

  double Vch = 0, norm = 0;
  std::vector<const Particle*>::const_iterator ip;
  std::vector<const Particle*> Pfit = toStdVector(SecVert->products());;
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) { 
    double a = std::pow((*ip)->pt(), m_PowerK);
    Vch += (*ip)->charge() * a;
    norm+= a;
  }
  if(norm) Vch = Vch / norm;
  if(fabs(Vch) < m_MinimumCharge ) return tVch;
 
  //calculate omega
  double omega = m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
     if( fabs(Vch)<0.75 ) 
       omega = m_P0 + m_P1*fabs(Vch) - m_P2*Vch*Vch ;
     if( fabs(Vch)>0.75 ) omega = 0.20;
     if( fabs(Vch)>0.99 ) { // tracks have all the same charge
       if(Pfit.size()==2) omega = 0.372;
       if(Pfit.size() >2) omega = 0.309;
     }
  }

  verbose()<<"B pt= "<< AXB0->pt() <<endreq;
  debug() <<" VtxCh= "<< Vch <<" with "<< Pfit.size() <<" parts"
	  <<" omega= "<< omega <<endreq;
  tVch.setDecision( Vch>0 ? -1 : 1 );
  tVch.setType( Tagger::VtxCharge ); 
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) tVch.addTaggerPart(**ip);
  tVch.setOmega( omega );

  return tVch;
}
//==========================================================================
std::vector<const Particle*> TaggerVertexChargeTool::toStdVector( const SmartRefVector<Particle>& refvector ){
  std::vector<const Particle*>  tvector;
  for( SmartRefVector<Particle>::const_iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//====================================================================
StatusCode TaggerVertexChargeTool::finalize() { return StatusCode::SUCCESS; }

