// $Id: TupleToolMCTruth.cpp,v 1.9 2008-10-02 14:44:31 ahicheur Exp $
// Include files
#include "gsl/gsl_sys.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
// local
#include "TupleToolMCTruth.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

// kernel
#include "Kernel/IP2VVMCPartAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCTruth );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCTruth::TupleToolMCTruth( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_pLink(0)
  , m_pComp(0)
  , m_pChi2(0)
  , m_linkerTool_Links(0)
  , m_linkerTool_Chi2(0)
  , m_linkerTool_Composite(0)
{
  declareInterface<IParticleTupleTool>(this);
  
  // Associator input location. Empty should be fine for most of the case
  declareProperty( "InputLocations", m_assocInputs = std::vector<std::string>(1,"") );

  // Use the chi2 associator instead of the link and composite associator
  declareProperty( "UseChi2Method", m_useChi2Method=false );

  // Store the number of associations seen
  declareProperty( "StoreAssociationNumbers", m_storeNumberOfAssoc=false );

  // Store kinetic information from the associated candidate
  declareProperty( "StoreKineticInfo",  m_storeKinetic = false );

  // Store the end and origin true vertex information
  declareProperty( "StoreVertexInfo",  m_storeVertexes = false );
  
  // Store the propertime information for associated composite particle
  declareProperty( "StorePropertimeInfo", m_storePT = true );

  // Store the true angular information 
  declareProperty( "FillAngles", m_fillangles = false );
  declareProperty( "Calculator", m_calculator = "MCBd2KstarMuMuAngleCalculator" );
}

//=============================================================================

StatusCode TupleToolMCTruth::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_linkerTool_Links = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Links",this);
  m_linkerTool_Chi2 = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Chi2",this);
  m_linkerTool_Composite = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Composite",this);

   if (m_fillangles) m_angleTool  = tool<IP2VVMCPartAngleCalculator>(m_calculator,this);

  return StatusCode::SUCCESS;
}

StatusCode TupleToolMCTruth::fill( const LHCb::Particle* 
				 , const LHCb::Particle* P
				 , const std::string& head
				 , Tuples::Tuple& tuple ){

  m_pLink = m_linkerTool_Links->linker(Particle2MCMethod::Links,m_assocInputs); 
  m_pComp = m_linkerTool_Composite->linker(Particle2MCMethod::Composite,m_assocInputs);
  m_pChi2 = m_linkerTool_Chi2->linker(Particle2MCMethod::Chi2,m_assocInputs);

  Assert( ( !m_useChi2Method && m_pLink && m_pComp ) 
	  ||
	  ( m_useChi2Method || m_pChi2 )
	  , "One of your associator hasn't been initialized!");
  
  int assignedPid = 0;
  
  int mcPid = 0;
  int nbAss = 0;
  double mcTau = -1;

  Gaudi::XYZVector endVertex, originVertex;
  Gaudi::LorentzVector trueP;

  const MCParticle* mcp(0);
  bool test = true;

  if( P ){
    assignedPid = P->particleID().pid();
    if ( m_useChi2Method ){
      double w=0;
      nbAss = m_pChi2->associatedMCP( P );
      mcp = m_pChi2->firstMCP( P, w );
      
      test &= tuple->column( head+"_MC_CHI2WHEIGHT", w );

    } else {
      if( P->isBasicParticle() ) mcp = m_pLink->firstMCP( P );
      else                       mcp = m_pComp->firstMCP( P );
    }
  }
  
  // pointer is ready, prepare the values:
  if( mcp ) {

    mcPid = mcp->particleID().pid();

    trueP = mcp->momentum();

    const SmartRefVector< LHCb::MCVertex > & endVertices = mcp->endVertices();
    endVertex = endVertices.front()->position(); // the first item, the other are discarded.
    originVertex = mcp->originVertex()->position();

    // lifetime
    if( m_storePT ){
      Gaudi::XYZVector dist = endVertex - originVertex;
      // copied from DecayChainNTuple // 
      mcTau = trueP.M() * dist.Dot( trueP.Vect() ) / trueP.Vect().mag2();
      mcTau /= Gaudi::Units::picosecond * Gaudi::Units::c_light;
    }
  }

  // fill the tuple:
  test &= tuple->column( head+"_TRUEID", mcPid );  

  if( m_storeNumberOfAssoc )
    test &= tuple->column( head + "_MC_ASSOCNUMBER", nbAss );

  if( m_storeKinetic )
    test &= tuple->column( head + "_TRUEP_", trueP );

  if( m_storeVertexes ){
    test &= tuple->column( head + "_TRUEORIGINVERTEX_", originVertex );
    test &= tuple->column( head + "_TRUEENDVERTEX_", endVertex );
  }

  if( m_storePT )
    test &= tuple->column( head + "_TRUETAU", mcTau );

  // true angles information:
  if(m_fillangles) {
    // only for Bs or Bd (patch, not meant to be elegant)
    if (abs(assignedPid) == 511 || abs(assignedPid) == 531) {
      
    //Helicity
    double thetaL(9999), thetaK(9999), phi(9999);

    StatusCode sc_hel = false;
    if (mcp) sc_hel = m_angleTool->calculateAngles( mcp, thetaL, thetaK, phi);

    if ( !sc_hel ) {
      thetaL=9999.;
      thetaK=9999.;
      phi=9999.;      
    }
    
      
    if (msgLevel(MSG::DEBUG)) debug() << "Three true helicity angles are theta_L : " 
                                      << thetaL 
                                      << " K: "<< thetaK
                                      << " phi: " << phi << endmsg ;
      
      
    
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
    

      if (msgLevel(MSG::DEBUG)) debug() << "Three true transversity angles are Theta_tr : " 
					<< Theta_tr 
					<< " Phi_tr: " << Phi_tr
					<< " Theta_phi_tr: " << Theta_V 
					<< endmsg ;
      
      
      

          test &= tuple->column( head+"_TRUEThetaTr", Theta_tr );
          test &= tuple->column( head+"_TRUEPhiTr", Phi_tr );
          test &= tuple->column( head+"_TRUEThetaVtr", Theta_V  );
       
    }
    
    
  }
  

  return StatusCode(test);
}

