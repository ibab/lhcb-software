// $Id: MCTupleToolReconstructed.cpp,v 1.6 2009-05-14 13:40:44 pkoppenb Exp $
// Include files 
#include "gsl/gsl_sys.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "MCTupleToolReconstructed.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/MCParticle.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "MCInterfaces/IMCReconstructed.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolReconstructed
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolReconstructed );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolReconstructed::MCTupleToolReconstructed( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_recible(0)
  , m_rected(0)
  , m_pCPPAsct(0)
  , m_pNPPAsct(0)
{
  declareInterface<IMCParticleTupleTool>(this);
  declareProperty("Associate",m_associate=true,"Fill associated protoparticle");
  declareProperty("FillPID",m_pid=true,"Fill PID");
}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolReconstructed::~MCTupleToolReconstructed() {} 

//=============================================================================
// initialize
//=============================================================================

StatusCode MCTupleToolReconstructed::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_recible = tool<IMCReconstructible>("MCReconstructible");
  m_rected  = tool<IMCReconstructed>("MCReconstructed");

  if (m_associate){  
    m_pCPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::ChargedPP, LHCb::ProtoParticleLocation::Charged);
    m_pNPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::NeutralPP, LHCb::ProtoParticleLocation::Neutrals);
  }
  
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolReconstructed::fill( const LHCb::MCParticle* 
                                     , const LHCb::MCParticle* mcp
                                     , const std::string& head
                                     , Tuples::Tuple& tuple ){
  
  bool test = true;
  int catted = -10 ;
  int catible = -10 ;

  // pointer is ready, prepare the values:
  if( 0!=mcp ) {
    catted  = m_rected->reconstructed(mcp);
    catible = m_recible->reconstructible(mcp);
  }

  // fill the tuple:
  test &= tuple->column( head+"_Reconstructible", catible );  
  test &= tuple->column( head+"_Reconstructed", catted );  
  std::vector<double> PX, PY, PZ, Weights, dlle, dllmu, dllk, dllp, pchi2;

  if ( (0!=mcp) && m_associate && isStable(mcp)){
    std::vector<std::pair<const LHCb::ProtoParticle*,double> > ppv = getProtos(mcp);
    for ( std::vector<std::pair<const LHCb::ProtoParticle*,double> >::const_iterator ppp = ppv.begin() ; 
          ppp != ppv.end() ; ++ppp){
      const LHCb::ProtoParticle* proto = ppp->first;
      double w = ppp->second;
      /// @todo There's plenty more that can be added here. Like PID for instance.
      if (0!=proto->track()){
        Gaudi::XYZVector mom = proto->track()->momentum();
        PX.push_back(mom.X());
        PY.push_back(mom.Y());
        PZ.push_back(mom.Z());
        pchi2.push_back(proto->track()->probChi2());
      }
      Weights.push_back(w);
      if (m_pid){
        dlle.push_back(proto->info( ProtoParticle::CombDLLe,  -999.0 ));
        dllmu.push_back(proto->info( ProtoParticle::CombDLLmu,  -999.0 ));
        dllk.push_back(proto->info( ProtoParticle::CombDLLk,  -999.0 ));
        dllp.push_back(proto->info( ProtoParticle::CombDLLp,  -999.0 ));
      }
    }
  }
  if (isStable(mcp)){
    const unsigned int maxPP = 20 ;
    test &= tuple->farray(  head+"_PP_PX", PX,  head+"_ProtoParticles" , maxPP );
    test &= tuple->farray(  head+"_PP_PY", PY,  head+"_ProtoParticles" , maxPP );
    test &= tuple->farray(  head+"_PP_PZ", PZ,  head+"_ProtoParticles" , maxPP );
    test &= tuple->farray(  head+"_PP_Weight", Weights,  head+"_ProtoParticles" , maxPP );
    test &= tuple->farray(  head+"_PP_tr_pchi2", pchi2,  head+"_ProtoParticles" , maxPP );
    if (m_pid){
      test &= tuple->farray(  head+"_PP_DLLe", dlle,  head+"_ProtoParticles" , maxPP );
      test &= tuple->farray(  head+"_PP_DLLk", dllk,  head+"_ProtoParticles" , maxPP );
      test &= tuple->farray(  head+"_PP_DLLp", dllp,  head+"_ProtoParticles" , maxPP );
      test &= tuple->farray(  head+"_PP_DLLmu", dllmu,  head+"_ProtoParticles" , maxPP );
    }
  }
  
  return StatusCode(test);
}

//=========================================================================
///  Protoparticles list @todo return weight as well
//========================================================================
std::vector<std::pair<const LHCb::ProtoParticle*,double> > 
MCTupleToolReconstructed::getProtos(const LHCb::MCParticle* mcp) const {
  ProtoParticle2MCLinker* asct = (mcp->particleID().threeCharge()==0)?m_pNPPAsct:m_pCPPAsct;
  if (0==asct) Exception("Null PP asociator");
  std::vector<std::pair<const LHCb::ProtoParticle*,double> > ppv ;
  double w=0;
  const LHCb::ProtoParticle* pp = asct->firstP(mcp,w);
  while( pp ){
    ppv.push_back(std::pair<const LHCb::ProtoParticle*,double>(pp,w));
    pp = asct->nextP(w);
  }
  return ppv;
}
