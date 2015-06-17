// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h"


// local
#include "TeslaMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TeslaMonitor
//
// 2013-09-04 : Lucio Anderlini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TeslaMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TeslaMonitor::TeslaMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator )
  , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
  , m_decayDescFunc(LoKi::BasicFunctors<const LHCb::Particle*>::Constant ( 0 ))
{
  declareProperty("HybridFactory", m_factory,
                  "Type/Name for C++/Python Hybrid Factory");

  declareProperty("Code", m_code,
                  "LoKi code of the Variable to plot");

  declareProperty("Title", m_title,
                  "Histogram Title" );

  declareProperty("Name", m_name,
                  "Histogram Name" );

  declareProperty("Min", m_min,
                  "Minimum" );

  declareProperty("Max", m_max,
                  "Maximum" );

  declareProperty("nBins", m_nBins,
                  "NumberOfBins");

  declareProperty("Folder", m_folder = "TeslaMonitor", 
                  "Name of the TDirectory storing histograms");

  declareProperty("Preambulo", m_preambulo = "", 
                  "LoKi Preambulo");

  declareProperty("Cut", m_cut = "(ALL)", 
                  "LoKi requirement to the entries included in the histogram");

}
//=============================================================================
// Destructor
//=============================================================================
TeslaMonitor::~TeslaMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TeslaMonitor::initialize() {
  StatusCode sc = DaVinciHistoAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  const size_t nVars = m_code.size();
  bool status = true;

  setHistoDir ( m_folder );

  status &= ( nVars == m_min.size()   );
  status &= ( nVars == m_max.size()   );
  status &= ( nVars == m_code.size()  );
  status &= ( nVars == m_nBins.size() );

  if (status == false)
  {
    error() << "Misconfigured algorithm. Inconsistent number of variables."
            << endmsg;

    return StatusCode::FAILURE;
  }

  for (size_t iVar = 0; iVar < nVars; ++iVar)
  {
    m_vars . push_back ( Variable() );
    m_vars . back() . m_min   = m_min   [ iVar ];
    m_vars . back() . m_max   = m_max   [ iVar ];
    m_vars . back() . m_code  = m_code  [ iVar ];
    m_vars . back() . m_nBins = m_nBins [ iVar ];
  }

  initVariables ();

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode TeslaMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;

  const LHCb::Particle::Range particleArray = particles();
  LHCb::Particle::Range::const_iterator im;
  Variables::const_iterator var;

  for ( im = particleArray.begin(); im != particleArray.end(); ++im)
  {
    if (m_decayDescFunc ( *im ) == 0.) continue;
    counter ("Entries") ++;

    switch (m_vars.size())
    {
      case 1: 
        plot(m_vars[0].m_fun(*im) ,
          m_name, m_title,
          m_vars[0].m_min , m_vars[0].m_max , m_vars[0].m_nBins);
        break;
      case 2:
        plot2D(m_vars[0].m_fun(*im) , m_vars[1].m_fun(*im),
          m_name, m_title,
          m_vars[0].m_min , m_vars[0].m_max , 
          m_vars[1].m_min , m_vars[1].m_max , 
          m_vars[0].m_nBins, m_vars[1].m_nBins);
        break;
      case 3:
        plot3D(m_vars[0].m_fun(*im) , m_vars[1].m_fun(*im), m_vars[2].m_fun(*im),
          m_name, m_title,
          m_vars[0].m_min , m_vars[0].m_max , 
          m_vars[1].m_min , m_vars[1].m_max , 
          m_vars[2].m_min , m_vars[2].m_max , 
          m_vars[0].m_nBins, m_vars[1].m_nBins, m_vars[2].m_nBins);
          break;
      default:
          return Error ("Unexpected number of variables.",StatusCode::FAILURE);
    }

  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TeslaMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciHistoAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


StatusCode TeslaMonitor::initVariables ()
{
  // get the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
  //
  //
  info() << "DecayDescriptor: " << getDecayDescriptor() << endmsg;

  StatusCode sc = factory->get(
      std::string("switch(DECTREE('") + getDecayDescriptor() +
      "') & (" + m_cut + "),1,0)", m_decayDescFunc, m_preambulo);
      
  if (sc.isFailure())
    return Error("Check DecayDescriptor " + getDecayDescriptor(), sc) ;

  Variables::iterator var;
  for ( var = m_vars.begin() ; var != m_vars.end() ; ++var )
  {
    sc = factory->get(var->m_code, var->m_fun, m_preambulo);

    if (sc.isFailure())
      return Error("Unable to decode " + var->m_code, sc) ;
  }

  //
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}


