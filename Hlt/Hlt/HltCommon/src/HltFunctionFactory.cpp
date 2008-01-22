// $Id: HltFunctionFactory.cpp,v 1.20 2008-01-22 10:31:48 hernando Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltFunctionFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctionFactory
//
// 2007-12-09 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltFunctionFactory );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltFunctionFactory::HltFunctionFactory( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  //  declareInterface<IPyHltFunctionFactory>(this);

}
//=============================================================================
// Destructor
//=============================================================================
HltFunctionFactory::~HltFunctionFactory() {} 

//=============================================================================

StatusCode HltFunctionFactory::initialize(){ 

  m_trackFactory = 
    tool< IFunctionFactory<LHCb::Track> >("HltTrackFunctionFactory",this);
  
  m_vertexFactory = 
    tool< IFunctionFactory<LHCb::RecVertex> >("HltVertexFunctionFactory",this);
  
  m_trackBiFactory = 
    tool< IBiFunctionFactory<LHCb::Track, LHCb::Track> >("HltTrackBiFunctionFactory",this);
    
  return StatusCode::SUCCESS;
}

zen::function<LHCb::Track>* 
HltFunctionFactory::trackFunction(const std::string& name)
{return m_trackFactory->function(name);}   

zen::filter<LHCb::Track>* 
HltFunctionFactory::trackFilter(const std::string& name)
{return m_trackFactory->filter(name);} 
  
zen::function<LHCb::RecVertex>* 
HltFunctionFactory::vertexFunction(const std::string& name)
{return m_vertexFactory->function(name);}

zen::filter<LHCb::RecVertex>* 
HltFunctionFactory::vertexFilter(const std::string& name)
{return m_vertexFactory->filter(name);}
  
zen::bifunction<LHCb::Track,LHCb::Track>* 
HltFunctionFactory::trackBiFunction(const std::string& name)
{return m_trackBiFactory->function(name);}
