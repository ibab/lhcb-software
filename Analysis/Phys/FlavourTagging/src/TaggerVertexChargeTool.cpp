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

  declareProperty( "SecondaryVertexName",
                   m_SecondaryVertexToolName = "SVertexTool" );

  declareProperty( "CombTech",     m_CombinationTechnique = "NNet" );
  declareProperty( "AverageOmega", m_AverageOmega         = 0.40 );
  declareProperty( "PowerK",       m_PowerK               = 0.35 );
  declareProperty( "MinimumCharge",m_MinimumCharge        = 0.15 );
  declareProperty( "P0",           m_P0                   = 5.280128e-01 );
  declareProperty( "P1",           m_P1                   =-2.464036e-01 );
  declareProperty( "Gt075",        m_P2                   = 0.35 );
  declareProperty( "TracksEq2",    m_wSameSign2           = 0.465074 );
  declareProperty( "TracksGt2",    m_wSameSignMoreThan2   = 0.404992 );

  m_svtool = 0;

}
TaggerVertexChargeTool::~TaggerVertexChargeTool() {}; 

//=====================================================================
StatusCode TaggerVertexChargeTool::initialize() {

  m_svtool = tool<ISecondaryVertexTool> ("SVertexTool", 
                                         m_SecondaryVertexToolName, this);
  if(! m_svtool) {
    warning()<< "*** No Vertex Charge tag will be used! " 
             << m_SecondaryVertexToolName << endreq;
  }

  return StatusCode::SUCCESS;
}

//=====================================================================
Tagger TaggerVertexChargeTool::tag( const Particle* AXB0, 
                                    const RecVertex* RecVert,
                                    Vertex::ConstVector& allVtx, 
                                    Particle::ConstVector& vtags ) {
  Tagger tVch;
  if(!RecVert) return tVch;
  if(vtags.empty()) return tVch;

  ///--- Inclusive Secondary Vertex ---
  //look for a secondary Vtx due to opposite B
  std::vector<Vertex> vvec(0);
  if(m_svtool) vvec = m_svtool -> buildVertex( *RecVert, vtags );
  if(vvec.empty()) return tVch;
  allVtx.push_back(&vvec.at(0));
  if(vvec.size()>1) allVtx.push_back(&vvec.at(1));

  double Vch = 0, norm = 0;
  Particle::ConstVector::const_iterator ip;
  Particle::ConstVector Pfit = allVtx.at(0)->outgoingParticlesVector();
  //if Vertex does not contain any daughters, exit
  if(Pfit.size()<1) return tVch;

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
      omega = m_P0 + m_P1*fabs(Vch) ;
    if( fabs(Vch)>0.75 ) omega = m_P2;
    if( fabs(Vch)>0.99 ) { // tracks have all the same charge
      if(Pfit.size()==2) omega = m_wSameSign2;
      if(Pfit.size() >2) omega = m_wSameSignMoreThan2;
    }
  }

  verbose()<<"B pt= "<< AXB0->pt() <<endreq;
  debug() <<" VtxCh= "<< Vch <<" with "<< Pfit.size() <<" parts"
          <<" omega= "<< omega <<endreq;
  if(omega>0.5){
    omega = 1-omega;
    tVch.setDecision( Vch>0 ? 1 : -1 );
    tVch.setOmega( omega );
  }else{
    tVch.setDecision( Vch>0 ? -1 : 1 );
    tVch.setOmega( omega );
  }
  tVch.setType( Tagger::VtxCharge ); 
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) tVch.addTaggerPart(**ip);

  return tVch;
}
//====================================================================
StatusCode TaggerVertexChargeTool::finalize() { return StatusCode::SUCCESS; }

