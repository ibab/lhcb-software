// $Id: ReadHltReport.cpp,v 1.3 2010-01-15 07:51:24 pkoppenb Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKErnel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// from HLT
// ============================================================================
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"
#include "Kernel/IANNSvc.h"
// ============================================================================
// local
// ============================================================================
#include "ReadHltReport.h"
// ============================================================================
//-----------------------------------------------------------------------------
// Implementation file for class : ReadHltReport
//
// 2007-03-29 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReadHltReport )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReadHltReport::ReadHltReport( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_decisionString("Decision")
{
  declareProperty("PrintParticles", m_printParticles = false, "Print out candidates");
  declareProperty("PrintDecisions", m_printDecisions = false, "Print out decisions");
  declareProperty("HltDecReportsLocation",
                  m_hltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("HltSelReportsLocation",
                  m_hltSelReportsLocation = LHCb::HltSelReportsLocation::Default);

  //  grep infoPersistent.insert $HLTRAWDATAROOT/src/HltSelReportsMaker.cpp |
  // grep # | awk -F"\(" '{print $2}' | awk -F, '{print $1}'
  // Bug #83271, HltSelReport fields are hard-coded
  m_infos["0#SelectionID"] = "ID";
  m_infos["0#Track.firstState.z"] = "Track z";
  m_infos["1#Track.firstState.x"] = "x";
  m_infos["2#Track.firstState.y"] = "y";
  m_infos["3#Track.firstState.tx"] = "tx";
  m_infos["4#Track.firstState.ty"] = "ty";
  m_infos["5#Track.firstState.qOverP"] = "q/P";
  m_infos["0#RecVertex.position.x"] = "Vx x";
  m_infos["1#RecVertex.position.y"] = "y";
  m_infos["2#RecVertex.position.z"] = "z";
  m_infos["0#Particle.particleID.pid"] = "Part pid";
  m_infos["1#Particle.measuredMass"] = "m";
  m_infos["2#Particle.referencePoint.z"] = "z";
  m_infos["3#Particle.referencePoint.x"] = "y";
  m_infos["4#Particle.referencePoint.y"] = "y";
  m_infos["5#Particle.slopes.x"] = "tx";
  m_infos["6#Particle.slopes.y"] = "ty";
  m_infos["7#Particle.1/p"] = "1/p";
  m_infos["0#CaloCluster.e"] = "Calo E";
  m_infos["1#CaloCluster.position.x"] = "x";
  m_infos["2#CaloCluster.position.y"] = "y";
  m_infos["3#CaloCluster.position.z"] = "z";

}
//=============================================================================
// Destructor
//=============================================================================
ReadHltReport::~ReadHltReport() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReadHltReport::execute() {

  StatusCode sc = StatusCode::SUCCESS ;
  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Execute" << endmsg;
  // code goes here
  const LHCb::HltDecReports* hltDecReports = getIfExists<LHCb::HltDecReports>(m_hltDecReportsLocation);
  if( NULL==hltDecReports )
  {
    return Warning("No Hlt decision",StatusCode::SUCCESS,1);
  }
  
  if ( hltDecReports->decReport("Hlt1Global")){
    counter("Has Hlt1")++;
    if (hltDecReports->decReport("Hlt1Global")->decision()){
      counter("Hlt1-yes")++;
    }
  }
  if ( hltDecReports->decReport("Hlt2Global")){
    counter("Has Hlt2")++;
    if (hltDecReports->decReport("Hlt2Global")->decision()){
      counter("Hlt2-yes")++;
    }
  }
  sc = readHltReport(hltDecReports);
  setFilterPassed(true) ;   // Mandatory. Set to true if event is accepted.
  return sc ;
}

//=============================================================================
//  Get HLT summary
//=============================================================================
StatusCode ReadHltReport::readHltReport(const LHCb::HltDecReports* decReports){

  StatusCode sc = StatusCode::SUCCESS ;
  std::vector<std::string> decisions;
  const LHCb::HltSelReports* selReports  = getIfExists<LHCb::HltSelReports>( m_hltSelReportsLocation );
  if ( NULL==selReports) return Warning("No HltDecReports at "+m_hltSelReportsLocation,StatusCode::FAILURE,1);
  
  for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
      it!=decReports->end();++it)
  {
    if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first
                                       << " decision= " << it->second.decision() << endmsg;
    if ( it->second.decision() ){
      if (m_printDecisions) decisions.push_back(it->first);
      counter("Selections")++ ;
      counter(it->first)++;
      if (m_printParticles && selReports && it->first!="Hlt1Global" && it->first!="Hlt2Global")
      {
        const LHCb::HltObjectSummary* sum = selReports->selReport(it->first);
        if (0==sum) {
          Warning("No summary for "+it->first,StatusCode::SUCCESS,1);
          continue ;
        } else {
          always() << "Selected objects for line " << it->first << " :" << endmsg ;
          sc = printObject(sum);
          if (!sc) return sc;
        }
      }
    }
  }
  if (!decisions.empty()) 
  {
    always() << "Decisions in this event : " ;
    for ( std::vector<std::string>::const_iterator s = decisions.begin() ; s!=decisions.end() ; ++s) always() << *s << " ";
    always() << endmsg;
  }
  return sc ;
}

//=========================================================================
// print candidates
//=========================================================================
StatusCode ReadHltReport::printObject(const LHCb::HltObjectSummary* sum, std::string increment){
  if (0==sum) return StatusCode::SUCCESS ;
  if (msgLevel(MSG::DEBUG)) debug() << "Summarised obj = " << sum->summarizedObject()
                                    << ", vector " << sum->substructure().size() << endmsg ;
  const LHCb::HltObjectSummary::Info info = sum->numericalInfoFlattened() ;
  always() << increment << "Info: " ;
  for ( LHCb::HltObjectSummary::Info::const_iterator i = info.begin() ; i != info.end() ; ++i){
    if ((m_infos.find(i->first))!=m_infos.end())
      always() << m_infos[i->first] << " = " << i->second << ", " ;
    else always() << i->first << " = " << i->second << ", " ;
  }
  always() << endmsg ;
  for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator s = sum->substructure().begin() ;
        s != sum->substructure().end() ; ++s){
    StatusCode sc = printObject(*s,increment+"   ");
    if (!sc) return sc;
  }
  return StatusCode::SUCCESS ;
}
