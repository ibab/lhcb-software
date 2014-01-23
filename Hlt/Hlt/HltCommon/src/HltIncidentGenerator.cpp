// $Id: HltIncidentGenerator.cpp,v 1.1 2009-08-17 08:42:14 graven Exp $
// Include files
#include <string>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiAlg/GaudiAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltIncidentGenerator
//
//-----------------------------------------------------------------------------

class HltIncidentGenerator : public GaudiAlgorithm
{
  public:
    HltIncidentGenerator( const std::string& name, ISvcLocator* pSvcLocator );
    ~HltIncidentGenerator() {};
    StatusCode initialize();
    StatusCode execute();

  private:
    std::string m_incident;
    IIncidentSvc* m_incidentSvc;
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltIncidentGenerator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltIncidentGenerator::HltIncidentGenerator( const std::string& name,
                                            ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator ), m_incidentSvc( 0 )
{
    declareProperty( "Incident", m_incident );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltIncidentGenerator::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    m_incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );

    return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltIncidentGenerator::execute()
{
    if ( !m_incident.empty() )
        m_incidentSvc->fireIncident( Incident( name(), m_incident ) );
    return StatusCode::SUCCESS;
};
