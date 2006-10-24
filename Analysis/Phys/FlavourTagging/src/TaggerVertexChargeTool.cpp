// Include files
#include "TaggerVertexChargeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerVertexChargeTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerVertexChargeTool );

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
Tagger TaggerVertexChargeTool::tag( const Particle* AXB0, const RecVertex* RecVert,
				    std::vector<const Vertex*>& allVtx, 
				    Particle::ConstVector& vtags ){
  Tagger tVch;
  if(!RecVert) return tVch;
  if(vtags.empty()) return tVch;
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);
  if(!SecVert) return tVch;

  double Vch = 0, norm = 0;
  Particle::ConstVector::const_iterator ip;
  Particle::ConstVector Pfit = SecVert->outgoingParticlesVector();
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) { 
    double a = pow((*ip)->pt(), m_PowerK);
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
//====================================================================
StatusCode TaggerVertexChargeTool::finalize() { return StatusCode::SUCCESS; }

