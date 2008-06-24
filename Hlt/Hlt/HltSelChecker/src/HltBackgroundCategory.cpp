// $Id: HltBackgroundCategory.cpp,v 1.13 2008-06-24 11:05:59 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// LHCb
#include "Event/HltSummary.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IPrintDecayTreeTool.h"            // Interface
#include "MCInterfaces/IPrintMCDecayTreeTool.h"            // Interface
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IANNSvc.h"

// local
#include "HltBackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltBackgroundCategory
//
// 2008-02-04 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltBackgroundCategory );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltBackgroundCategory::HltBackgroundCategory( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Hlt2StatisticsBase ( name , pSvcLocator )
  ,   m_bkg()
    , m_print()
    , m_printMC()
{
  declareProperty("PrintTree",m_printTree=false );
  declareProperty("FillAll",m_fillAll=false );
}
//=============================================================================
// Destructor
//=============================================================================
HltBackgroundCategory::~HltBackgroundCategory() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltBackgroundCategory::initialize() {
  StatusCode sc = Hlt2StatisticsBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by Hlt2StatisticsBase

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_bkg         = tool<IBackgroundCategory>("BackgroundCategory",this);
  if (m_printTree ){
    m_print       = tool<IPrintDecayTreeTool>("PrintDecayTreeTool",this);
    m_printMC     = tool<IPrintMCDecayTreeTool>("PrintMCDecayTreeTool",this);
  }
  
  const std::map<int,std::string>& catMap = m_bkg->getCategoryMap() ;

  strings cats ;
  for ( std::map<int, std::string>::const_iterator i = catMap.begin() ; 
        i!= catMap.end() ; ++i) {
    verbose() << "Looking at " << i->first << " " << i->second << endmsg ;
    cats.push_back(i->second);
  }

  sc =  m_algoCorr->algorithms(cats);
  if (!sc) return sc;
  sc =  m_algoCorr->algorithmsRow(getSelections());

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltBackgroundCategory::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  if ( !(m_summaryTool->decision())) return StatusCode::SUCCESS ;

  strings sels =  m_summaryTool->selections() ;
  Particle2MCLinker* linker  = 0 ;
  if (m_printTree) linker = new Particle2MCLinker( this, Particle2MCMethod::Links, "");
  
  for ( strings::const_iterator is = sels.begin() ; 
        is!=sels.end() ; ++is){

    if (msgLevel(MSG::VERBOSE)) verbose() << *is <<  " " 
                                          << svc<IANNSvc>("HltANNSvc")->value("Hlt2SelectionID",*is)->first  
                                          <<  " says " << m_summaryTool->decision() 
                                          << " has selection : " 
                                          << m_summaryTool->hasSelection(*is) 
                                          << " summary : " 
                                          << m_summaryTool->summary().hasSelectionSummary(*is) << endmsg ;
    if (!m_summaryTool->selectionDecision(*is)) Exception("selectionDecision false for "+*is);
    if (!m_summaryTool->hasSelection(*is)) Exception("hasSelection false for "+*is);
     
    if (msgLevel(MSG::VERBOSE)) verbose() << *is << " found " 
                                      << m_summaryTool->selectionParticles(*is).size() 
                                      << " Particles" << endmsg ;
    if ( m_summaryTool->selectionParticles(*is).empty()) continue ;
    const LHCb::Particle::ConstVector  parts =  m_summaryTool->selectionParticles(*is);

    if (parts.empty()) {
      err() << "Selection " << *is << " found no particles" << endmsg  ;
      continue ;
    }

    IBackgroundCategory::categories mincat = IBackgroundCategory::LastGlobal ;
    const std::map<int,std::string>& catMap = m_bkg->getCategoryMap() ;
    for ( LHCb::Particle::ConstVector::const_iterator ip =  
            parts.begin() ; ip != parts.end() ; ++ip){
      
      // bkg category
      IBackgroundCategory::categories cat = m_bkg->category(*ip);
 
      if ((msgLevel(MSG::DEBUG)) || m_printTree ) {
        std::map<int, std::string>::const_iterator scat = catMap.find(cat) ;
        info() << *is << " has a candidate " 
               << (*ip)->particleID().pid() << " " 
               << (*ip)->momentum() 
               << " category: " 
               << (*scat).first << " " << (*scat).second  << endmsg ;
      }
      
      if ( m_printTree ){
        m_print->printTree(*ip,linker);
        if (0!=m_bkg->origin(*ip)) m_printMC->printTree(m_bkg->origin(*ip));
      }
      
      if (m_fillAll){
        std::map<int, std::string>::const_iterator scat = catMap.find(cat) ;
        if (msgLevel(MSG::DEBUG)) debug() << "Filling " << (*scat).second << " for " << *is << endmsg ;
        if (!m_algoCorr->fillResult((*scat).second, true )) return StatusCode::FAILURE;
      } else if ( cat < mincat ) {
        if (msgLevel(MSG::DEBUG)) debug() << "New minimum " << cat << " for " << *is << endmsg ;
        mincat = cat ;   
      }
    }
    if (!m_fillAll){
      if ( mincat == IBackgroundCategory::LastGlobal ) mincat = IBackgroundCategory::Undefined ;
      std::map<int, std::string>::const_iterator scat = catMap.find(mincat) ;
      if (msgLevel(MSG::DEBUG)) debug() << "Best category is " << (*scat).second 
                                        << " for " << *is << endmsg ;
      if (!m_algoCorr->fillResult((*scat).second, true )) return StatusCode::FAILURE;
    }  
    if (!m_algoCorr->fillResult(*is, true )) return StatusCode::FAILURE; 
    if (msgLevel(MSG::DEBUG)) debug() << "End for " << *is << endmsg ;
    if (!m_algoCorr->endEvent()) return StatusCode::FAILURE ;
  }
  if (0!=linker) delete linker ;
  
  return  StatusCode::SUCCESS ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltBackgroundCategory::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return Hlt2StatisticsBase::finalize();  // must be called after all other actions
}

//=============================================================================
