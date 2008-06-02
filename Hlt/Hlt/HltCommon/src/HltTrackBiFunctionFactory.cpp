// $Id: HltTrackBiFunctionFactory.cpp,v 1.3 2008-06-02 19:57:57 graven Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltTrackBiFunctionFactory.h"
#include "HltFunctions.h"
#include "boost/algorithm/string/regex.hpp"

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

  std::string name = boost::algorithm::erase_regex_copy(fn, boost::regex(",.*"));
  Hlt::TrackBiFunction* fun = 
           name == "DOCA"       ? new Hlt::DOCA()       :
           name == "DimuonMass" ? new Hlt::DimuonMass() :
           name == "SumPT"      ? new Hlt::SumPT()      :
           (Hlt::TrackBiFunction*)0;
  
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


