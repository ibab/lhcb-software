// $Id: $
// Include files
#include <iostream>

#include "LoKi/IHybridFactory.h"
#include "LoKi/Functions.h"

#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// local
#include "B2KShhBDTSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : B2KShhBDTSelection
//
// 2013-03-27 :Rafael Coutinho, Thomas Latham and David Dossett 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( B2KShhBDTSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2KShhBDTSelection::B2KShhBDTSelection( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
	: base_class(type,name,parent),
	  m_dist(0),
	  m_dva(0),
	  m_BDTReader(0),
	  m_nVars(0),
	  m_values(0),
	  m_cut(-1.0),
	  m_BPVIPCHI2(m_dist, 0),
          m_BPVVDCHI2(0)	  
{
	declareProperty("BDTCut", m_cut, "BDT cut value");
	declareProperty("WeightsFile", m_weightsFile, "Weights file");
	declareProperty("typeKS", m_typeKS, "DD, LD or LL Ks");
}

//=============================================================================
// Destructor
//=============================================================================
B2KShhBDTSelection::~B2KShhBDTSelection()
{
	delete []m_values;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode B2KShhBDTSelection::initialize()
{
	// initialize the base class  (the first action)
	StatusCode sc = GaudiTool::initialize();
	if(sc.isFailure()) return sc; 
	
	//====================================================================
	// Initialize DVAlg, etc
	//====================================================================
	m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc() ) ;
	if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
	                           StatusCode::FAILURE);
	
	m_dist = m_dva->distanceCalculator();
	if( !m_dist )
	{
		Error("Unable to retrieve the IDistanceCalculator tool");
		return StatusCode::FAILURE;
	}

	std::vector<const char*> variables;
	variables.reserve(7);

	variables.push_back("B0_PT");
	variables.push_back("log10(B0_FDCHI2_OWNPV)");
	variables.push_back("log10(B0_IPCHI2_OWNPV)");
	variables.push_back("log10(B0_ENDVERTEX_CHI2)");
	variables.push_back("log10(h3_ENDVERTEX_CHI2)");
	variables.push_back("log10(h1_IPCHI2_OWNPV+h2_IPCHI2_OWNPV+h3_IPCHI2_OWNPV)");
	
	if( m_typeKS == "LL" ){
		variables.push_back("log10(h3_FDCHI2_OWNPV)");
	}

	//====================================================================
	// Initialize TMVA reader with some magic values
	//====================================================================  
	m_BDTReader = new TMVA::Reader( "Silent" );
	
	TString varName;
	m_nVars = variables.size();
	m_values = new float[m_nVars];

	for(unsigned int index = 0; index < m_nVars ; index++)
	{
		varName = variables[index];
		m_BDTReader->AddVariable( varName, &m_values[index] );
	}
	
	std::string methodName = "BDT method";
	m_BDTReader->BookMVA( methodName, m_weightsFile );
	
	return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode B2KShhBDTSelection::finalize()
{
	delete m_BDTReader; 
	m_BDTReader = 0;
	return GaudiTool::finalize();
}


//====================================================================
// BDT cut
//====================================================================
bool B2KShhBDTSelection::operator()(const LHCb::Particle *p) const 
{
	if(0 == p) {
		Error("LHCb::Particle* points to NULL, return false");
		return false ;
	}
	
	if(!set(p)) return false;
	
	std::string methodName = "BDT method"; 
	
	double BDTResValue = m_BDTReader->EvaluateMVA(methodName);
	
	if(msgLevel(MSG::DEBUG)) debug() << " BDT value of this candidate is: " << BDTResValue << endmsg ;
	
	if(BDTResValue > m_cut) return true;
	
	return false;  
}

//====================================================================
// All variables for BDT
//====================================================================
bool B2KShhBDTSelection::set (const LHCb::Particle *p) const
{  
	if(0 == p) return false;

	std::vector< const LHCb::Particle* > daughterVec;
	daughterVec.reserve(3);

	daughterVec.push_back( LoKi::Child::child( p, 1 ) );
	daughterVec.push_back( LoKi::Child::child( p, 2 ) );
	daughterVec.push_back( LoKi::Child::child( p, 3 ) );

	const LHCb::Particle *ph1(0);
	const LHCb::Particle *ph2(0);
	const LHCb::Particle *pKs(0);
	
	const int absKsID = 310;
	const int absPiID = 211;
	const int absKID = 321;

	for( int i=0; i<3; i++ )
	{
		const LHCb::Particle *daughter = daughterVec[i];
		const int id = (daughter->particleID()).pid();
		const int absID = abs(id);

		if( absID==absKsID )
		{
			pKs = daughter;
		}
		else if( absID==absKID )
		{
			ph1 = daughter;
		}
		else if( absID==absPiID )
		{
			ph2 = daughter;
		}
		else
		{
			Error("LHCb::Particle* has Unknown ID");
			return false ;
		}
	}


	if( !ph1 || !ph2 || !pKs )
	{
		Error("Can't get all the daughters, check the inputs!");
		return false;
	}
	
	if(msgLevel(MSG::DEBUG)) debug() << " Ks PID: " << pKs->particleID() 
	                                 << ", h1 PID: " << ph1->particleID() 
	                                 << ", h2 PID: " << ph2->particleID()
	                                 << endmsg;	
	
	// BPV for B mother 
	const LHCb::VertexBase *BPV_B  = m_dva->bestVertex(p);
	const LHCb::VertexBase *BPV_Ks = m_dva->bestVertex(pKs);

	// FD setup for B
	m_BPVVDCHI2.setVertex( BPV_B );
	// IPCHI2 setup for B
	m_BPVIPCHI2.setTool(m_dist);
	m_BPVIPCHI2.setVertex(BPV_B);
	
	// Now get the values from the Loki Functors and some calculations.	
	m_values[0] = p->pt()/Gaudi::Units::MeV;
	m_values[1] = m_BPVVDCHI2( p );
	m_values[2] = m_BPVIPCHI2( p );
	m_values[3] = log10(p->endVertex()->chi2());
	m_values[4] = log10(pKs->endVertex()->chi2());
	m_values[5] = log10( m_BPVIPCHI2(ph1) + m_BPVIPCHI2(ph2) + m_BPVIPCHI2(pKs));
	
	// Set the vertex to be the Ks bestVertex
	m_BPVVDCHI2.setVertex(BPV_Ks);
	if( m_typeKS == "LL" ){
		m_values[6] = log10( m_BPVVDCHI2(pKs) );
	}
	
	return true;
}
