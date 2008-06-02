// $Id: HltFunctionFactory.cpp,v 1.22 2008-06-02 19:57:57 graven Exp $
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

Hlt::TrackFunction* 
HltFunctionFactory::trackFunction(const std::string& name)
{return m_trackFactory->function(name);}   

Hlt::TrackFilter* 
HltFunctionFactory::trackFilter(const std::string& name)
{return m_trackFactory->filter(name);} 
 

Hlt::VertexFunction* 
HltFunctionFactory::vertexFunction(const std::string& name)
{return m_vertexFactory->function(name);}

Hlt::VertexFilter* 
HltFunctionFactory::vertexFilter(const std::string& name)
{return m_vertexFactory->filter(name);}
 

Hlt::TrackBiFunction* 
HltFunctionFactory::trackBiFunction(const std::string& name)
{return m_trackBiFactory->function(name);}
