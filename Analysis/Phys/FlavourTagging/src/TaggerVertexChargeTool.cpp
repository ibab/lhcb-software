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

  declareProperty( "AverageOmega", m_AverageOmega = 0.40 );
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
  verbose() << " B Pt=" << AXB0->pt()/GeV << endreq;

  double Vch = 0;
  std::vector<const Particle*>::const_iterator ip;
  std::vector<const Particle*> Pfit = toStdVector(SecVert->products());;
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) Vch += (*ip)->charge();
  debug() <<" VtxCh = "<<Vch << " with "<<Pfit.size() << " parts" <<endreq;
  if(!Vch) return tVch;
 
  tVch.setDecision( Vch>0 ? -1 : 1 );
  tVch.setOmega( m_AverageOmega );
  tVch.setType( Tagger::VtxCharge ); 
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) tVch.addTaggerPart(**ip);

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

