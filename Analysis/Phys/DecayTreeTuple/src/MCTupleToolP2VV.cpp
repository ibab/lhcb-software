// $Id: MCTupleToolP2VV.cpp,v 1.1 2009-01-19 18:07:44 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MCTupleToolP2VV.h"
#include "Kernel/IP2VVMCPartAngleCalculator.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

using namespace LHCb ;
//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolP2VV
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolP2VV );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolP2VV::MCTupleToolP2VV( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_angleTool()
{
  declareInterface<IMCParticleTupleTool>(this);

  declareProperty( "Calculator", m_calculator = "MCBd2KstarMuMuAngleCalculator" );
}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolP2VV::~MCTupleToolP2VV() {} 

//=============================================================================
StatusCode MCTupleToolP2VV::initialize( ){
   if( ! GaudiTool::initialize() ) return StatusCode::FAILURE; 
   m_angleTool = tool<IP2VVMCPartAngleCalculator>(m_calculator,this);
   return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolP2VV::fill( const LHCb::MCParticle* 
                                  , const LHCb::MCParticle* mcp
                                  , const std::string& head
                                  , Tuples::Tuple& tuple ){
  
  bool test = true;
  
  //Helicity
  double thetaL(9999), thetaK(9999), phi(9999);
  
  StatusCode sc_hel = false;
  if (mcp) sc_hel = m_angleTool->calculateAngles( mcp, thetaL, thetaK, phi);
  
  if ( !sc_hel ) {
    thetaL=9999.;
    thetaK=9999.;
    phi=9999.;      
  }
  
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Three true helicity angles are theta_L : " 
            << thetaL 
            << " K: "<< thetaK
            << " phi: " << phi << endmsg ;
  }
  
  test &= tuple->column( head+"_TRUEThetaL", thetaL );
  test &= tuple->column( head+"_TRUEThetaK", thetaK );
  test &= tuple->column( head+"_TRUEPhi",    phi  );
  
  //Transversity
  double Theta_tr(9999), Phi_tr(9999), Theta_V(9999);
  
  StatusCode sc_tr = false;
  if (mcp) sc_tr = m_angleTool->calculateTransversityAngles( mcp,Theta_tr,Phi_tr,Theta_V );
  
  if ( !sc_tr ) {
    Theta_tr=9999.;
    Phi_tr=9999.;
    Theta_V=9999.;
  }
  
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Three true transversity angles are Theta_tr : " 
            << Theta_tr 
            << " Phi_tr: " << Phi_tr
            << " Theta_phi_tr: " << Theta_V 
            << endmsg ;
  }
  
  test &= tuple->column( head+"_TRUEThetaTr", Theta_tr );
  test &= tuple->column( head+"_TRUEPhiTr", Phi_tr );
  test &= tuple->column( head+"_TRUEThetaVtr", Theta_V  );
  
  return StatusCode(test);
}
