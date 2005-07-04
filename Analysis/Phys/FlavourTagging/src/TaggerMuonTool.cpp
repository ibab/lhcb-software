// Include files 
#include "TaggerMuonTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerMuonTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerMuonTool>          s_factory ;
const        IToolFactory& TaggerMuonToolFactory = s_factory ; 

//====================================================================
TaggerMuonTool::TaggerMuonTool( const std::string& type,
				const std::string& name,
				const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "Muon_Pt_cut", m_Pt_cut_muon = 1.2 );
  declareProperty( "Muon_P_cut",  m_P_cut_muon  = 5.0 );
}
TaggerMuonTool::~TaggerMuonTool() {}; 

//=====================================================================
StatusCode TaggerMuonTool::initialize() { return StatusCode::SUCCESS; }

//=====================================================================
ParticleVector TaggerMuonTool::taggers( const Particle* AXB0, 
					const Vertex* RecVert, 
					const ParticleVector& vtags ){
  verbose() << "B pt=" << AXB0->pt()/GeV 
	    << " VTXz="<< RecVert->position().z() <<endreq;

  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vmu(0);
  Particle* mu=0;
  double ptmaxm = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 13 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_muon ) continue;
    double P = (*ipart)->p()/GeV;
    if( P  < m_P_cut_muon ) continue;
    debug() << " Muon P="<< P <<" Pt="<< Pt <<endreq;
    if( Pt > ptmaxm ) { //Pt ordering
      mu = (*ipart);
      ptmaxm = Pt;
    }   
  }
  if( mu ) vmu.push_back( mu );

  return vmu;
}
//====================================================================
StatusCode TaggerMuonTool::finalize() { return StatusCode::SUCCESS; }

