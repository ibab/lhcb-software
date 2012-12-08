// $Id: $
// Include files
#include "LoKi/IHybridFactory.h"

#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// local
#include "B2hhBDTSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : B2hhBDTSelection
//
// 2012-11-11 : Stefano Perazzini
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( B2hhBDTSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  B2hhBDTSelection::B2hhBDTSelection( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
    : base_class(type,name,parent),
      m_dist(NULL),
      m_dva(NULL),
      m_BDTReader(NULL),
      m_nVars(9),
      m_cut(-1.0),
      m_MIPDV(""),
      m_BPVIP(""),
      m_BPVVD(0)
{
  m_values = new float[m_nVars];

  declareProperty("BDTCut", m_cut, "BDT cut value");
  declareProperty("BDTWeightsFile", m_weightsFile, "Weights file");

}
//=============================================================================
// Destructor
//=============================================================================
B2hhBDTSelection::~B2hhBDTSelection()
{
  delete []m_values;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode B2hhBDTSelection::initialize()
{
  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  //====================================================================
  // Initialize DVAlg, etc
  //====================================================================
  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);

  m_dist = m_dva->distanceCalculator();
  if( !m_dist )
  {
    return Error("Unable to retrieve the IDistanceCalculator tool");
  }

  const char* m_variables[] = { "minPT", "minIP", "maxPT", "maxIP", "doca",
                                "vertexCHI2", "bPT", "bIP", "bFD" };

  //====================================================================
  // Initialize TMVA reader
  //====================================================================

  //m_BDTReader = new TMVA::Reader( "!Color:!Silent" );
  m_BDTReader = new TMVA::Reader( "Silent" );

  TString varName;
  for(unsigned int index = 0; index < m_nVars ; index++){
    varName = m_variables[index] ;
    m_BDTReader->AddVariable( varName, &m_values[index] );
  }

  const std::string methodName = "BDT method";

  m_BDTReader->BookMVA( methodName, m_weightsFile );

  return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode B2hhBDTSelection::finalize()
{
  delete m_BDTReader;
  m_BDTReader = NULL;
  return GaudiTool::finalize();
}

//====================================================================
// BDT cut
//====================================================================
bool B2hhBDTSelection::operator()(const LHCb::Particle* p) const
{
  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false").ignore();
    return false ;
  }

  if(!set(p)) return false;

  std::string methodName = "BDT method";

  double BDTResValue = m_BDTReader->EvaluateMVA(methodName);

  if(msgLevel(MSG::DEBUG))
    debug() << " BDT value of this candidate is: " << BDTResValue << " "
            << p->momentum().M() << " " << 0 << " "
            << m_values[0] << " " << m_values[1] << " "
            << m_values[2] << " " << m_values[3] << " "
            << m_values[4] << " " << m_values[5] << " "
            << m_values[6] << " " << m_values[7] << " "
            << m_values[8] << endmsg;

  if(BDTResValue > m_cut) return true;

  return false;
}

//====================================================================
// All variables for BDT
//====================================================================
bool B2hhBDTSelection::set (const LHCb::Particle* p) const{

  if(0 == p) return false;

  const LHCb::Particle* p1 = LoKi::Child::child( p, 1 );
  const LHCb::Particle* p2 = LoKi::Child::child( p, 2 );

  if( !p1 || !p2 || !p )
  {
    Error("Can't get all the particles, check the inputs!").ignore();
    return false;
  }

  if(msgLevel(MSG::DEBUG)) debug() << " p1 PID: " << p1->particleID()
                                   << ", p2 PID: " << p2->particleID()
                                   << ", B PID: " << p->particleID()
                                   << endmsg;

  const LHCb::VertexBase* BPV = m_dva->bestVertex(p);

  double p1PT = p1->pt()/1000; double p2PT = p2->pt()/1000;
  double p1IP = m_MIPDV(p1);   double p2IP = m_MIPDV(p2);
  double doca = 0.0;
  m_dist->distance(p1->proto()->track(),p2->proto()->track(),doca);

  m_BPVVD.setVertex(BPV);

  m_values[0]  = std::min(p1PT,p2PT);
  m_values[1]  = std::min(p1IP,p2IP);

  m_values[2]  = std::max(p1PT,p2PT);
  m_values[3]  = std::max(p1IP,p2IP);

  m_values[4]  = doca;
  m_values[5]  = p->endVertex()->chi2()/p->endVertex()->nDoF();
  m_values[6]  = p->pt()/1000;

  m_values[7]  = m_BPVIP(p);

  m_values[8] = m_BPVVD(p);

  return true;
}
