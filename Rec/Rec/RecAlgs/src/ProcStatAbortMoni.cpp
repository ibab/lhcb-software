// local
#include "ProcStatAbortMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProcStatAbortMoni
//
// 2010-07-16 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProcStatAbortMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProcStatAbortMoni::ProcStatAbortMoni( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_h           ( NULL )
{
  using namespace boost::assign;
  std::vector<std::string> tmp =  list_of
                   ("Overall")("Hlt")("VELO")("TT")("IT")("OT")("Tracking")("Vertex")
                   ("RICH")("CALO")("MUON")("PROTO");
  declareProperty( "SubSystems", m_subSystems = tmp);
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = LHCb::ProcStatusLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
ProcStatAbortMoni::~ProcStatAbortMoni() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ProcStatAbortMoni::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  

  // book the histo
  m_h = bookProfile1D( "aborts", "Processing Abort Rates (%)",
                       -0.5, m_subSystems.size()-0.5, 
                       m_subSystems.size() ); 
  // Set the bin labels
  const bool ok = Gaudi::Utils::Histos::setBinLabels( m_h, m_subSystems );

  // return
  return ok ? sc : StatusCode::FAILURE ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProcStatAbortMoni::execute()
{

  // Load the ProcStatus object
  const LHCb::ProcStatus * proc = get<LHCb::ProcStatus>( m_procStatLocation );

  // Loop over sub-systems and fill plot
  int index(0);
  for ( std::vector<std::string>::const_iterator iSystem = m_subSystems.begin();
        iSystem != m_subSystems.end(); ++iSystem, ++index )
  {
    if ( *iSystem == "Overall"  ) 
    { 
      m_h->fill( index, proc->aborted() ? 100.0 : 0.0 );
    }
    else
    {
      m_h->fill( index, proc->subSystemAbort(*iSystem) ? 100.0 : 0.0 );
    }
  }

  // Debug printout if aborted
  if ( proc->aborted() && msgLevel(MSG::DEBUG) ) { debug() << *proc << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
