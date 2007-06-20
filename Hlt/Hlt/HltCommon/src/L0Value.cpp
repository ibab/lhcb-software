// $Id: L0Value.cpp,v 1.1 2007-06-20 12:17:38 hernando Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

// local
#include "L0Value.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0Value
//
// 2007-06-20 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0Value );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Value::L0Value( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_CaloDataLocation ( LHCb::L0ProcessorDataLocation::Calo ),
    m_MuonDataLocation ( LHCb::L0ProcessorDataLocation::Muon ),
    m_PileUpDataLocation ( LHCb::L0ProcessorDataLocation::PileUp ),
    m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  )
{
  declareInterface<IL0Value>(this);
  
  declareProperty("L0DUReportLocation", m_L0DUReportLocation );
  declareProperty("ADCconvert", m_ADCconvert = true );  
};

//=============================================================================
// Destructor
//=============================================================================
L0Value::~L0Value() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Value::initialize() {

  m_CaloCandidate   = tool<IL0Candidate>("L0CandidateTool",
                                         "L0CaloCandidate",this);
  m_MuonCandidate   = tool<IL0Candidate>("L0CandidateTool",
                                         "L0MuonCandidate",this);
  m_PileUpCandidate = tool<IL0Candidate>("L0CandidateTool",
                                         "L0PileUpCandidate",this);

  m_CaloCandidate   -> setUp(m_CaloDataLocation     , m_ADCconvert);
  m_MuonCandidate   -> setUp(m_MuonDataLocation     , m_ADCconvert);
  m_PileUpCandidate -> setUp(m_PileUpDataLocation   , m_ADCconvert);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
double L0Value::value( int id) {

  double val = 0.;

  if (id == IL0Value::EtTotal) {
    val =  m_CaloCandidate->value( L0DUBase::Sum::Et);
  } else if ( id == IL0Value::SPDMult) {
    val =  m_CaloCandidate->value( L0DUBase::Spd::Mult);
  } else if ( id == IL0Value::PuHits) {
    val  =  m_PileUpCandidate->value(L0DUBase::PileUp::Hits);
  } else if (id == IL0Value::PuPeak2) {
    val =  m_PileUpCandidate->value(L0DUBase::PileUp::Peak2);
  } else if (id == IL0Value::EtEle) {
    val =  m_CaloCandidate->value( L0DUBase::Electron::Et);
  } else if (id == IL0Value::EtGamma) {
    val  =  m_CaloCandidate->value( L0DUBase::Photon::Et);
  } else if (id == IL0Value::EtHad) {
    val =  m_CaloCandidate->value( L0DUBase::Hadron::Et);
  } else if (id == IL0Value::EtPi0Global) {
    val =  m_CaloCandidate->value( L0DUBase::Pi0Global::Et);
  } else if (id == IL0Value::EtPi0Local) {
    val =  m_CaloCandidate->value( L0DUBase::Pi0Local::Et);
  } else {
    std::vector<double> ptmus;
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon1::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon2::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon3::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon4::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon5::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon6::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon7::Pt));
    ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon8::Pt));
    
    std::sort(ptmus.begin(),ptmus.end());
    double ptmu = ptmus[7];
    double ptdimu = ptmus[6]+ptmus[7];

    if (id == IL0Value::PtMu) val = ptmu;
    else if (id == IL0Value::PtDiMu) val = ptdimu;
  }

  debug() << " L0Value " << id << " = " << val << endreq;
  return val;
}

