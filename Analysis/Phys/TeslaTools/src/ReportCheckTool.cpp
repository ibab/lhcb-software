// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "ReportCheckTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReportCheckTool: ReportCheckTool.cpp
// Author: Sean Benson
// 05/06/2008
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ReportCheckTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReportCheckTool::ReportCheckTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool( type, name , parent )
{
  declareInterface<IReportCheck>(this);
  
  // Associate versions with saved particle properties
  version_map_particle.emplace("1#Particle.measuredMass",1);
  version_map_particle.emplace("9#Particle.massErr",2);
  // Associate versions with saved track properties
  version_map_track.emplace("6#Track.chi2PerDoF",1);
  version_map_track.emplace("8#Track.Likelihood",2);
  // Associate versions with saved vertex properties
  version_map_vertex.emplace("0#Vertex.chi2",2);
  // Associate versions with saved rpid properties
  version_map_rpid.emplace("4#Rich.DLLK",2);
}

//===========================================================================
/// ReportCheckTool::VersionTopLevel finds report version from HltSelReports.
//===========================================================================
int ReportCheckTool::VersionTopLevel(string trigger,string RepLoc)
{
  if (msgLevel(MSG::DEBUG)) debug() << "SelReports version check requested" << endmsg;
  
  int vnum = 99;

  const LHCb::HltSelReports* selReports;
  if ( exist<LHCb::HltSelReports>( RepLoc.c_str() ) ) {
    selReports = get<LHCb::HltSelReports>( RepLoc.c_str() );
  }
  else{
    warning() << "Sel. reports do not exist!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  const LHCb::HltObjectSummary * MotherRep = selReports->selReport(trigger.c_str());
  if(MotherRep){
    
    debug() << "Examining mother report" << endmsg;

    SmartRefVector <LHCb::HltObjectSummary> substructure = MotherRep->substructure();
    const LHCb::HltObjectSummary * Obj = substructure.begin()->target();
    // What keys are present:
    std::vector<string> infoList;
    std::vector<string> vec_keys = Obj->numericalInfoKeys();

    if( Obj->summarizedObjectCLID() == LHCb::Particle::classID() ){
      
      debug() << "Mother report is of a particle" << endmsg;

      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_particle.find( key )->second == 1 ) vnum = 1;
      }
      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_particle.find( key )->second == 2 ) vnum = 2;
      }
      debug() << "ReportCheckTool finds verision " << vnum << endmsg;
      if(vnum==99){
        warning() << "Please check how HltSelReports were created" << endmsg;
        return 0;
      }
      return vnum;
    }
    else if( Obj->summarizedObjectCLID() == LHCb::Track::classID() ){
      
      debug() << "Mother report is of a track" << endmsg;
      
      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_track.find( key )->second == 1 ) vnum = 1;
      }
      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_track.find( key )->second == 2 ) vnum = 2;
      }
      debug() << "ReportCheckTool finds verision " << vnum << endmsg;
      if(vnum==99){
        warning() << "Please check how HltSelReports were created" << endmsg;
        return 0;
      }
      return vnum;
    }
  }
  return vnum;
}

//===========================================================================
/// ReportCheckTool::checkBankVersion finds report version from HltSelReports.
//===========================================================================
int ReportCheckTool::checkBankVersion(){
  LHCb::RawEvent* raw = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Trigger);
  if(!raw) raw = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  if(!raw) {
    debug() << "Cannot get raw event, returning 0" << endmsg;
    return 0;
  }
  for(int j=0; j<256; ++j)  
  {
    LHCb::RawBank::BankType i = LHCb::RawBank::BankType(j);
    if(i!=LHCb::RawBank::HltSelReports) continue;
    else {
      debug() << "Found selection report banks" << endmsg;
      const std::vector<LHCb::RawBank*>& b = raw->banks(i);
      if ( b.size() > 0 ) return b[0]->version();
    }
  }
  return 0;
}    

//===========================================================================
/// ReportCheckTool::getTCK finds TCK from raw bank
//===========================================================================
unsigned int ReportCheckTool::getTCK() {
  LHCb::RawEvent* raw = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Trigger);
  if(!raw) raw = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  if(!raw) {
    debug() << "Cannot get raw event, returning 0" << endmsg;
    return 0u;
  }
  const auto& rawbanks = raw->banks( LHCb::RawBank::HltDecReports );
  auto rb = rawbanks.front();
  if ( rb && rb->magic() == LHCb::RawBank::MagicPattern && rb->version() > 0 && rb->size()>0 )  {
    auto content = rb->begin<unsigned int>();
    return *content;
  }
  return 0u;
}
