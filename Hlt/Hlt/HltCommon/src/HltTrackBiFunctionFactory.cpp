// $Id: HltTrackBiFunctionFactory.cpp,v 1.5 2010-08-17 08:47:19 graven Exp $
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
  declareInterface< IBiFunctionFactory<LHCb::Track,LHCb::Track> >(this);
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
           name == "DOCA"         ? new Hlt::DOCA()          :
           name == "DimuonMass"   ? new Hlt::DimuonMass()    :
           name == "DikaonMass"   ? new Hlt::DikaonMass()    :
           name == "SumPT"        ? new Hlt::SumPT()         :
           name == "CosThetaStar" ? new Hlt::CosThetaStar()  :
           (Hlt::TrackBiFunction*)0;
  
  Assert( fun != 0, " function() not able to create function" + name);
  debug() << " created function " << name << " fun " << fun << endreq;

  return fun;
}

Hlt::TrackBiFilter* 
HltTrackBiFunctionFactory::filter(const std::string& name) {
  error() << " not able to create filter " << name << endreq;
  return 0;
  
}

HltTrackBiFunctionFactory::~HltTrackBiFunctionFactory() {} 


