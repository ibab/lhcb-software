// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltFilters.h"
#include "HltTFilter.icpp"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFilters
//
// 2007-11-27 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackFilter );
DECLARE_ALGORITHM_FACTORY( HltVertexFilter );

DECLARE_ALGORITHM_FACTORY( HltTrackPrepare );
DECLARE_ALGORITHM_FACTORY( HltVertexPrepare );

HltTrackFilter::HltTrackFilter( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  HltTFilter<LHCb::Track>(name,pSvcLocator) {
  m_functionFactoryName = "HltTrackFunctionFactory";
}

HltVertexFilter::HltVertexFilter( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  HltTFilter<LHCb::RecVertex>(name,pSvcLocator) {
  m_functionFactoryName = "HltVertexFunctionFactory";
}


HltTrackPrepare::HltTrackPrepare( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  HltTFilter<LHCb::Track>(name,pSvcLocator) {
  m_functionFactoryName = "HltTrackFunctionFactory";
  m_tesInput = true;
}

HltVertexPrepare::HltVertexPrepare( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  HltTFilter<LHCb::RecVertex>(name,pSvcLocator) {
  m_functionFactoryName = "HltVertexFunctionFactory";
  m_tesInput = true;
}
