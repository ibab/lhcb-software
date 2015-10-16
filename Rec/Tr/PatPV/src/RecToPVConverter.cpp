#include "Event/PrimaryVertex.h"
#include "Event/RecVertex.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class RecToPVConverter : public GaudiAlgorithm
{
public:
  RecToPVConverter( const std::string&, ISvcLocator*) ;
  StatusCode execute() ;
private:
  std::string m_inputName ;
  std::string m_outputName ;
  bool m_doFit ;
} ;

DECLARE_ALGORITHM_FACTORY( RecToPVConverter )

//-----------------------------------------------------------------------------
// Implementation file for class : PackVertices
//
// 2015-09-25 : Wouter Hulsbergen
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecToPVConverter::RecToPVConverter( const std::string& name,
						  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation" , m_inputName  = LHCb::RecVertexLocation::Primary );
  declareProperty( "OutputLocation", m_outputName = LHCb::PrimaryVertexLocation::Default );
  declareProperty( "DoFit", m_doFit = false ) ;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode RecToPVConverter::execute() 
{
  LHCb::RecVertex::Range recvertices = get<LHCb::RecVertex::Range>(m_inputName) ;
  auto slimvertices = new LHCb::PrimaryVertex::Container() ;
  for( const auto& recpv: recvertices )
    slimvertices->insert( new LHCb::PrimaryVertex( *recpv, m_doFit ) );
  put(slimvertices,m_outputName) ;

  return StatusCode::SUCCESS ;
}
