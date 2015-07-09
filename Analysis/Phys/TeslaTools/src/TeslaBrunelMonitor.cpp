// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/Aida2ROOT.h"

 // from ROOT
#include "TH2D.h"


// local
#include "TeslaBrunelMonitor.h"

// local 
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : TeslaBrunelMonitor
//
// 2013-09-04 : Lucio Anderlini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TeslaBrunelMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TeslaBrunelMonitor::TeslaBrunelMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator )
  , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
{
  declareProperty("HybridFactory", m_factory,
                  "Type/Name for C++/Python Hybrid Factory");

  declareProperty("Code", m_code,
                  "LoKi code of the Variable to plot");

  declareProperty("HistTitle", m_histTitle = "TeslaBrunel Monitor",
                  "Histogram Title" );

  declareProperty("HistName", m_histName = "TeslaBrunelMonitor",
                  "Histogram Name" );

  declareProperty("TeslaMatcher", m_teslaMatcherName = "TeslaMatcher:PUBLIC",
                  "Tool name for ITeslaMatcher interface" );

  declareProperty("Particle", m_particle,
                  "Particle or decay desciptor list" );

  declareProperty("Name", m_name,
                  "Variable Name" );

  declareProperty("Max", m_max,
                  "Maximum" );

  declareProperty("Folder", m_folder = "TeslaBrunelMonitor", 
                  "Name of the TDirectory storing histograms");

  declareProperty("Preambulo", m_preambulo = "", 
                  "LoKi Preambulo");

  declareProperty("Cut", m_cut, 
                  "LoKi requirement to the entries included in the histogram");


  declareProperty ( "MatchLocations" , m_matching_locations );                   

}
//=============================================================================
// Destructor
//=============================================================================
TeslaBrunelMonitor::~TeslaBrunelMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TeslaBrunelMonitor::initialize() {
  StatusCode sc = DaVinciHistoAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_matcher = tool<ITeslaMatcher>(m_teslaMatcherName, this);
  if (m_matcher == NULL)
    return Error("Cannot load TeslaMatcher tool", StatusCode::FAILURE);

  m_nVariables = m_code.size();
  bool status = true;

  setHistoDir ( m_folder );

  status &= ( m_nVariables == m_max.size()      );
  status &= ( m_nVariables == m_code.size()     );
  status &= ( m_nVariables == m_particle.size() );
  status &= ( m_nVariables == m_cut.size()      );
  status &= ( m_nVariables == m_name.size()     );

  if (status == false)
  {
    error() << "Misconfigured algorithm. Inconsistent number of variables."
            << endmsg;

    return StatusCode::FAILURE;
  }

  m_histTitle += ";";
  for (size_t iVar = 0; iVar < m_nVariables; ++iVar)
  {
    m_vars . push_back ( Variable() );
    m_vars . back() . m_max   = m_max   [ iVar ];
    m_vars . back() . m_code  = m_code  [ iVar ];
    m_vars . back() . m_name  = m_name  [ iVar ];

    info () << "Adding variable " << m_name [ iVar ] << endmsg;
  }

  AIDA::IHistogram2D * h(NULL);
  if ( produceHistos() )
  {
    // retrieve or book the histogram
    h = histo2D ( m_histName ) ;
    if ( NULL == h )   
      h = book2D (m_histName, m_histTitle, 0, m_nVariables, m_nVariables,
                                          -1., 1., 1000.); 

    for (size_t iVar = 0; iVar < m_nVariables; ++iVar)
    {
      std::stringstream s;
      s <<  m_vars[iVar].m_name << "/" << m_vars[iVar].m_max;

      Gaudi::Utils::Aida2ROOT::aida2root(h)->GetXaxis()
        ->SetBinLabel(iVar+1, s.str().c_str());
    }
  }

  initVariables ();
  initTESlocations ();

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode TeslaBrunelMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;

  const LHCb::Particle::Range particleArray = particles();

  for ( auto part : particleArray )
  {
    for (size_t iVar = 0; iVar < m_nVariables; ++iVar)
    {
      const Variable& var = m_vars[iVar];

      if (part == NULL) continue; 
      if (var.m_cut ( part ) == 0.) continue; 

      const int apid = part->particleID().abspid(); 

      if (m_parsed_locations.count(apid) == 0)
      {
        if (msgLevel(MSG::WARNING)) 
          warning() << "Required matching for " 
                    << m_particle[iVar]
                    << " but no TES location was given for PID "
                    << part->particleID().abspid()
                    << endmsg;

        continue;
      }


      const LHCb::Particle* matched;
      sc = sc&& m_matcher->findBestMatch(part,matched,m_parsed_locations[apid]);
      if (!sc || !matched) continue;


      double diff = (var.m_fun(part) - var.m_fun(matched));
      double normdiff = diff/var.m_max;
      if (normdiff >= 1.) normdiff =  1.-1e-2;
      if (normdiff < -1.) normdiff= -1.+1e-2;

//      info()  << iVar << ": " 
//              << "on:  " << var.m_fun(part) 
//              << ", off: " << var.m_fun(matched) 
//              << ", diff: " << diff 
//              << endmsg;

      plot2D((double)iVar , normdiff,
         m_histName, m_histTitle,
         0, m_nVariables, -1, 1., m_nVariables, 1000);

      plot1D(diff,
              m_histName + "_" + var.m_name,
              var.m_name,
              -var.m_max, var.m_max, 1000);

      counter ("Entries") ++;
    }
  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TeslaBrunelMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciHistoAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


StatusCode TeslaBrunelMonitor::initVariables ()
{
  // get the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
  //
  //
  info() << "DecayDescriptor: " << getDecayDescriptor() << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  
  const size_t nVars = m_nVariables;
  for (size_t iVar = 0; iVar < nVars; iVar++)
  {
    sc = sc&& factory->get(
        std::string("switch(DECTREE('") + m_particle[iVar] +
        "') & (" + m_cut[iVar] + "),1,0)", m_vars[iVar].m_cut, m_preambulo);
      
    if (sc.isFailure())
      return Error("Check DecayDescriptor " + m_particle[iVar], sc) ;

    sc = factory->get(m_vars[iVar].m_code, m_vars[iVar].m_fun, m_preambulo);

    if (sc.isFailure())
      return Error("Unable to decode " + m_vars[iVar].m_code, sc) ;

  }
  //
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}


StatusCode TeslaBrunelMonitor::initTESlocations ()
{
#define SET_TES(part,tes) \
  if (m_matching_locations.count(part) == 0)\
    m_matching_locations [part] = tes

  SET_TES("pi+", "Phys/StdAllNoPIDsPions/Particles");
  SET_TES("K+",  "Phys/StdAllNoPIDsKaons/Particles");
  SET_TES("p+",  "Phys/StdAllNoPIDsProtons/Particles");
  SET_TES("e+",  "Phys/StdAllNoPIDsElectrons/Particles");
  SET_TES("mu+", "Phys/StdAllNoPIDsMuons/Particles");
#undef SET_TES

  // Gets the ParticleProperty service
  LHCb::IParticlePropertySvc*  ppSvc = 
    this -> template svc<LHCb::IParticlePropertySvc> 
                            ( "LHCb::ParticlePropertySvc" , true) ;

  std::map<std::string, std::string>::const_iterator matchLocation;

  for (matchLocation = m_matching_locations.begin();
       matchLocation != m_matching_locations.end();
       matchLocation++)
  {
    const LHCb::ParticleProperty* property = ppSvc->find(matchLocation->first);
    if (property)
      m_parsed_locations [ abs(property->pdgID().pid()) ] = matchLocation->second;
  }

  return StatusCode::SUCCESS;
}




