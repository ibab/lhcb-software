// $Id: HltTrackBiFunctionFactory.cpp,v 1.1 2008-01-22 09:56:37 hernando Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackBiFunctionFactory.h"
#include "HltBase/EParser.h"
#include "HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackBiFunctionFactory
//
// 2007-03-22 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackBiFunctionFactory );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackBiFunctionFactory::HltTrackBiFunctionFactory( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : HltBaseTool( type, name , parent )
{
  declareInterface< IBiTrackFactory >(this);
}

StatusCode HltTrackBiFunctionFactory::initialize() {
  
  return HltBaseTool::initialize();
}

bool HltTrackBiFunctionFactory::command(const std::string& command,
                                      const std::string & value) {
  info() << " command " << command << " value " << value << endreq;
  return true;
}


Hlt::TrackBiFunction* 
HltTrackBiFunctionFactory::function(const std::string& fn) 
{
  debug() << " creating function " << fn << endreq;
  Hlt::TrackBiFunction* fun = 0;

  std::vector<std::string> cromos = EParser::parse(fn,",");  
  std::string name = cromos[0];

  if (name == "DOCA"){
    fun = new Hlt::DOCA();
  } else if (name == "DimuonMass") {
    fun = new Hlt::DimuonMass();  
  } else if (name == "SumPT") {
    fun = new Hlt::SumPT();
  }
  
  Assert( fun != 0, " function() not able to create function" + name);
  debug() << " created function " << name << " fun " << fun << endreq;

  return fun;
}

Hlt::TrackBiFilter* 
HltTrackBiFunctionFactory::filter(const std::string& name) {

  info() << " not able to create filter " << name << endreq;

  return 0;
  
}

HltTrackBiFunctionFactory::~HltTrackBiFunctionFactory() {} 


