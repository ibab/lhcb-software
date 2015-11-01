// $Id: ParticleMonitor.cpp,v 1.6 2010-07-22 09:38:43 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
#include "Kernel/IPlotTool.h"

// local
#include "ParticleMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleMonitor
//
// 2008-12-04 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ParticleMonitor::ParticleMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
    ,   m_mother(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ))
    ,   m_peak(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ))
    ,   m_sideband(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( false ))
    ,   m_massPlotToolName("MassPlotTool")
{
  declareProperty( "MotherCut", m_motherCut = "ALL",
                   "The cut to be applied to all mother particle (default all)." )  ;
  // declareProperty( "FinalCut", m_finalCut = "ALL",
  //      "The cut to be applied to final state particle before plotting (default all)." )  ;
  declareProperty( "PeakCut", m_peakCut = "ALL",
                   "Selection of mass peak (default all)." )  ;
  declareProperty( "SideBandCut", m_sidebandCut = "NONE",
                   "Selection of sidebands (default none)." )  ;
  m_plotToolNames.push_back("MomentumPlotTool"); // default
  declareProperty( "PlotTools" , m_plotToolNames, "Names of plot tools");
}

//=============================================================================
// Destructor
//=============================================================================
ParticleMonitor::~ParticleMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleMonitor::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory>
    ("LoKi::Hybrid::Tool/HybridFactory:PUBLIC" , this ) ;

  if (! configureCut( factory, m_motherCut , m_mother  )) return StatusCode::FAILURE ;
  //  if (! configure ( factory, m_finalCut , m_final  )) return StatusCode::FAILURE ;
  if (! configureCut ( factory, m_peakCut , m_peak  )) return StatusCode::FAILURE ;
  if (! configureCut ( factory, m_sidebandCut , m_sideband  )) return StatusCode::FAILURE ;

  release(factory);

  if ( m_plotToolNames.empty() )
  {
    return Error( "No plot Tool defined. Will do nothing." ) ;
  }
  else
  {
    if (msgLevel(MSG::DEBUG)) debug() << "Using" ;
    m_plotTools.insert(std::pair<std::string,IPlotTool*>(m_massPlotToolName,
                                                         tool<IPlotTool>(m_massPlotToolName,this)));
    if (msgLevel(MSG::DEBUG)) debug() << " " << m_massPlotToolName << endmsg ;
    for ( std::vector<std::string>::const_iterator s = m_plotToolNames.begin() ;
          s != m_plotToolNames.end() ; ++s)
    {
      if ( *s == m_massPlotToolName ) {
        // CRJ : No need to print this
        // warning() <<  *s << " is mandatory. Is already added to list." << endmsg ;
        continue ; // already done
      }
      m_plotTools.insert(std::pair<std::string,IPlotTool*>(*s,tool<IPlotTool>(*s,this)));
      if (msgLevel(MSG::DEBUG)) debug() << " " << *s << endmsg ;
    }
    if (msgLevel(MSG::DEBUG)) debug() << endmsg ;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleMonitor::execute()
{

  // code goes here
  const LHCb::Particle::Range parts = particles();
  LHCb::Particle::Range::const_iterator m = parts.begin();
  LHCb::Particle::Range::const_iterator mEnd = parts.end();

  for ( ; m != mEnd; ++m) {
    counter("# Mothers")++ ;
    if ( !m_mother( *m ) )  { continue ; }   // discard particles with no cut

    std::string trail = "all";
    m_plotTools[m_massPlotToolName]->fillPlots(*m,trail); // full mass plot

    counter("# Accepted Mothers")++;
    const bool peak     = m_peak( *m ) ;
    const bool sideband = m_sideband( *m ) ;

    if (peak)
    {
      counter("# Accepted Mothers in Peak")++;
      trail = "peak";
    }
    else if (sideband)
    {
      counter("# Accepted Mothers in Sidebands")++;
      trail = "sideband";
    }
    else continue ;  // ignore events ouside of sidebands

    if (!fillPlots(*m,trail)) return StatusCode::FAILURE;
  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

StatusCode ParticleMonitor::fillPlots( const LHCb::Particle* d,
                                       const std::string & where )
{
  for ( std::map<std::string,IPlotTool*>::iterator s = m_plotTools.begin() ;
        s != m_plotTools.end() ; ++s )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << s->first << endmsg ;
    if (!s->second->fillPlots(d,where)) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ParticleMonitor::configureCut( LoKi::IHybridFactory* f,
                                          std::string & s,
                                          LoKi::Types::Cut& c )
{
  const StatusCode sc = f -> get ( s , c  ) ;
  if ( sc.isFailure () ) { return Error ( "Unable to  decode cut: " + s  , sc ) ; }
  if ( msgLevel(MSG::DEBUG)) debug () << "The decoded cut is: " << s << endmsg ;
  return sc;
}

//=============================================================================
