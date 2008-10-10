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

  declareProperty( "CombTech",     m_CombinationTechnique = "Probability" );
  declareProperty( "AverageOmega", m_AverageOmega         = 0.41 );

  declareProperty( "PowerK",       m_PowerK               = 0.35 );
  declareProperty( "MinimumCharge",m_MinimumCharge        = 0.15 );
  declareProperty( "P0",           m_P0                   = 5.259631e-01 );
  declareProperty( "P1",           m_P1                   =-3.256825e-01 );
  declareProperty( "Gt075",        m_Gt075                = 0.35 );
  declareProperty( "TracksEq2",    m_wSameSign2           = 0.420701 );
  declareProperty( "TracksGt2",    m_wSameSignMoreThan2   = 0.352641 );

  declareProperty( "ProbMin",      m_ProbMin              = 0.52);

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
Tagger TaggerVertexChargeTool::tag( const Particle* /*AXB0*/, 
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
  Particle::ConstVector Pfit = allVtx.at(0)->outgoingParticlesVector();
  //if Vertex does not contain any daughters, exit
  if(Pfit.size()<1) return tVch;

  //if a particle appears twice in vtags and the same was used
  //for the inclusive vertexing than skip
  Particle::ConstVector::const_iterator ip, jp, kp;
//   for( ip = vtags.begin(); ip != vtags.end(); ip++) {
//     const ProtoParticle* proto = (*ip)->proto();
//     for( jp=ip+1; jp!=vtags.end(); jp++) {
//       if( (*ip) == (*jp) ) { //ip is a tagger
// 	debug()<<"found a used tagger"<<(*ip)->pt()<<endreq;
// 	for( kp=Pfit.begin(); kp!=Pfit.end(); kp++) {
// 	  if( proto == (*kp)->proto() ) {//ip is in SV
// 	    debug()<<"Secondary Vertex uses OS tagger. Skip."<<endreq;
// 	    return tVch;
// 	  }
// 	}
//       }
//     }
//   }

  //calculate vertex charge
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) { 
    double a = pow((*ip)->pt(), m_PowerK);
    Vch += (*ip)->charge() * a;
    norm+= a;
  }
  if(norm) Vch = Vch / norm;
  if(fabs(Vch) < m_MinimumCharge ) return tVch;
 
  //calculate omega
  double omega = m_AverageOmega;
  if(m_CombinationTechnique == "Probability") {
    if( fabs(Vch)<0.75 ) omega = m_P0 + m_P1*fabs(Vch) ;
    if( fabs(Vch)>0.75 ) omega = m_Gt075;
    if( fabs(Vch)>0.99 ) { // tracks have all the same charge
      if(Pfit.size()==2) omega = m_wSameSign2;
      if(Pfit.size() >2) omega = m_wSameSignMoreThan2;
    }
  }

  debug() <<" VtxCh= "<< Vch <<" with "<< Pfit.size() <<" parts"
          <<" omega= "<< omega <<endreq;

  if( 1-omega < m_ProbMin ) return tVch;
  if(   omega > m_ProbMin ) return tVch;

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

