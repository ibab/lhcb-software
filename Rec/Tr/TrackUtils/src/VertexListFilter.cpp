/** @class VertexListFilter VertexListFilter.h
 *
 *  Algorithm to filter events in which a track list is not empty
 *
 *  @author W. Hulsbergen
 *  @date   2008
 */

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

// track interfaces
#include "Event/RecVertex.h"

class VertexListFilter: public GaudiAlgorithm 
{
public:
  // Constructors and destructor
  VertexListFilter(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~VertexListFilter();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
};


DECLARE_ALGORITHM_FACTORY( VertexListFilter )


VertexListFilter::VertexListFilter(const std::string& name,
				   ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "InputLocation",  
		   m_inputLocation  = LHCb::RecVertexLocation::Primary ) ;
}

VertexListFilter::~VertexListFilter()
{
}

StatusCode VertexListFilter::execute()
{
  LHCb::RecVertex::Range vertices = get<LHCb::RecVertex::Range>(m_inputLocation);
  setFilterPassed( !vertices.empty() ) ;
  return  StatusCode::SUCCESS ;
}
