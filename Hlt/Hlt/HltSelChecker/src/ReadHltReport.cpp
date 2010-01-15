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
DECLARE_ALGORITHM_FACTORY( ReadHltReport );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReadHltReport::ReadHltReport( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_decisionString("Decision")
{
  declareProperty("PrintParticles", m_printParticles = false, "Print out candidates");
  declareProperty("MaxCandidates", m_maxCandidatesPrinted = 10 , 
                  "Maximum number of candidates printed per selection. Set to 0 for all");
  declareProperty("HltDecReportsLocation",
                  m_hltDecReportsLocation = LHCb::HltDecReportsLocation::Default);   
  declareProperty("HltSelReportsLocation",
                  m_hltSelReportsLocation = LHCb::HltSelReportsLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
ReadHltReport::~ReadHltReport() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ReadHltReport::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if (!sc) return sc;
  debug() << "==> Initialize" << endmsg;

  if (m_printParticles) info() << "Will print up to " << m_maxCandidatesPrinted 
                               << " particles " << endmsg ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReadHltReport::execute() {

  StatusCode sc = StatusCode::SUCCESS ;
  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Execute" << endmsg;
  // code goes here  
  if( !exist<LHCb::HltDecReports>( m_hltDecReportsLocation ) ){
    return Warning("No Hlt decision",StatusCode::SUCCESS,1);
  }
  const LHCb::HltDecReports* hltDecReports = get<LHCb::HltDecReports>(m_hltDecReportsLocation);
  if ( hltDecReports->decReport("Hlt1Global")){
    counter("Has Hlt1")++;
    if (hltDecReports->decReport("Hlt1Global")->decision()){
      counter("Hlt1-yes")++;
    }
    if ( hltDecReports->decReport("Hlt1IgnoringLumiDecision")){
      if (hltDecReports->decReport("Hlt1IgnoringLumiDecision")->decision()){
        counter("Hlt1noLumi-yes")++;
      }
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
//  Finalize
//=============================================================================
StatusCode ReadHltReport::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
//  Get HLT summary
//=============================================================================
StatusCode ReadHltReport::readHltReport(const LHCb::HltDecReports* decReports){
  
  StatusCode sc = StatusCode::SUCCESS ;
  if ( !exist<LHCb::HltSelReports>(m_hltSelReportsLocation )) return sc ;
  const LHCb::HltSelReports* selReports = get<LHCb::HltSelReports>( m_hltSelReportsLocation );

  for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
      it!=decReports->end();++it){
    if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first  
                                       << " decision= " << it->second.decision() << endmsg;
    if ( it->second.decision() ){
      counter("Selections")++ ;
      counter(it->first)++;
      const LHCb::HltObjectSummary* sum = selReports->selReport(it->first);
      if (0==sum) {
        Warning("No summary for "+it->first,StatusCode::SUCCESS,1);
        continue ;
      }
      if (msgLevel(MSG::DEBUG)) debug() <<  it->first << " summarised obj = " << sum->summarizedObject() 
                                        << ", vector " << sum->substructure().size() << endmsg ;
      LHCb::Particle::ConstVector parts;
      for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator s = sum->substructure().begin() ; 
            s != sum->substructure().end() ; ++s){
        if (msgLevel(MSG::DEBUG)) debug() << it->first << " substructure has " << (*s)->summarizedObject() << " and " 
                                          << (*s)->substructure().size() << endmsg ;
        if (0==(*s)->summarizedObject()) continue ;
        const LHCb::Particle* p = dynamic_cast<const LHCb::Particle*>((*s)->summarizedObject());
        if ( 0==p ) Warning(it->first+" has no particles",StatusCode::SUCCESS,1).ignore();
        else parts.push_back(p);
      }
      if (!parts.empty()) {
        info() << it->first << " found " << parts.size() << " particle(s)." << endmsg ;
        counter("Candidates") += parts.size() ;
        sc = printParticles(parts);
        if (!sc) return sc ;
      } else {
        if (msgLevel(MSG::DEBUG)) debug() << it->first << " stored no particles." << endmsg ;        
      }
      
    }
  }
  return sc ;
}

//=========================================================================
// print candidates
//=========================================================================
StatusCode ReadHltReport::printParticles(const LHCb::Particle::ConstVector& parts) const{
  unsigned int np = 0 ;
  for ( LHCb::Particle::ConstVector::const_iterator ip =  
          parts.begin(); ((ip!= parts.end()) && 
         ((0==m_maxCandidatesPrinted) || (np<m_maxCandidatesPrinted))) ; ++ip){
    np++ ;
    const LHCb::ParticleProperty* pp = ppSvc()->find ( (*ip)->particleID() ) ;
    if ( 0!=pp) info() << "  -> a " << pp->particle() ;
    else info() << "  -> an unknown PID " << (*ip)->particleID() ;
    info() << " P= " << (*ip)->momentum() ;
    const LHCb::VertexBase* bPV =  bestPV(*ip) ;
    if ( 0==(*ip)->endVertex()){
      double IP, IPe ;
      if ( 0!= bPV ){
        StatusCode sc = distanceCalculator()->distance((*ip), bPV, IP, IPe);
        if (!sc) return sc;
        info() << ", IP = " << IP/Gaudi::Units::micrometer << " +/- " 
               << IPe/Gaudi::Units::micrometer << " mum"  ;
      } 
    } else {
      info()  << ", M = " << (*ip)->measuredMass() ;
      if ( 0!= bPV ){
        double fp, chi2 ;
        StatusCode sc = distanceCalculator()
          ->distance(bPV, (*ip)->endVertex(), fp, chi2);
        if (!sc) return sc;
        double boost = ((*ip)->momentum().Beta())*((*ip)->momentum().Gamma())*Gaudi::Units::c_light ;
        info()  << ", FT = " << (fp/boost)/Gaudi::Units::picosecond << " ps, at "
                << chi2 << " chi2-separation";
      }
    }
    info() << endmsg ;
  }
  
  if ((0!=m_maxCandidatesPrinted) && (parts.size()>m_maxCandidatesPrinted))
    info() << "  ... and " << parts.size()-m_maxCandidatesPrinted 
           << " more candidates not printed." << endmsg ;
  return StatusCode::SUCCESS ;
}
