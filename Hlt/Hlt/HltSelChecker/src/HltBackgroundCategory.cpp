 // $Id: HltBackgroundCategory.cpp,v 1.10 2008-06-17 19:12:34 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// LHCb
#include "Event/Particle.h"
#include "Kernel/IHltSummaryTool.h"
#include "Event/HltSummary.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IAlgorithmCorrelations.h"            // Interface
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
  : GaudiAlgorithm ( name , pSvcLocator )
  ,   m_summaryTool()
  ,   m_bkg()
  ,   m_algoCorr()
    , m_print()
    , m_printMC()
    , m_linker()
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_summaryTool = tool<IHltSummaryTool>("HltSummaryTool",this);
  m_bkg         = tool<IBackgroundCategory>("BackgroundCategory",this);
  m_algoCorr    = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);
  if (m_printTree ){
    m_print       = tool<IPrintDecayTreeTool>("PrintDecayTreeTool",this);
    m_printMC     = tool<IPrintMCDecayTreeTool>("PrintMCDecayTreeTool",this);
    m_linker      = new Particle2MCLinker( this, Particle2MCMethod::Links, "");
  }
  
  const std::map<int,std::string>& catMap = m_bkg->getCategoryMap() ;

  std::vector<std::string> cats ;
  for ( std::map<int, std::string>::const_iterator i = catMap.begin() ; 
        i!= catMap.end() ; ++i) {
    verbose() << "Looking at " << i->first << " " << i->second << endmsg ;
    cats.push_back(i->second);
  }

  IANNSvc *ann = svc<IANNSvc>("HltANNSvc");
  std::vector<IANNSvc::minor_value_type> selections2 = ann->items("Hlt2SelectionID");
  std::vector<std::string> selections ;

  for (std::vector<IANNSvc::minor_value_type>::const_iterator i =
         selections2.begin(); i!=selections2.end(); ++i) {
    selections.push_back(i->first) ;
  }
  
  sc =  m_algoCorr->algorithms(cats);
  if (!sc) return sc;
  sc =  m_algoCorr->algorithmsRow(selections);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltBackgroundCategory::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  if ( !(m_summaryTool->decision())) return StatusCode::SUCCESS ;

  std::vector<std::string> sels =  m_summaryTool->selections() ;
  
  for ( std::vector<std::string>::const_iterator is = sels.begin() ; 
        is!=sels.end() ; ++is){

    if (msgLevel(MSG::VERBOSE)) verbose() << *is <<  " " 
                                          << svc<IANNSvc>("HltANNSvc")->value("SelectionID",*is)->first  
                                          <<  " says " << m_summaryTool->decision() 
                                          << " has selection : " 
                                          << m_summaryTool->hasSelection(*is) 
                                          << " summary : " 
                                          << m_summaryTool->summary().hasSelectionSummary(*is) << endmsg ;
    if (!m_summaryTool->selectionDecision(*is)) continue ;
    if (!m_summaryTool->hasSelection(*is)) continue ;  // ???
     
    const LHCb::HltSelectionSummary& sum = m_summaryTool->summary().selectionSummary(*is); // waiting for Particles method

    /// @todo does not work yet
    /*
    if (msgLevel(MSG::VERBOSE)) verbose() << *is << " found " 
                                      << m_summaryTool->selectionParticles(*is).size() 
                                      << " Particles" << endmsg ;
    if ( m_summaryTool->selectionParticles(*is).empty()) continue ;
    std::vector< LHCb::Particle * > parts =  m_summaryTool->selectionParticles(*is);
    */

    if (msgLevel(MSG::VERBOSE)) verbose() << *is << " gets " 
                                          << sum.particles().size() << " particles" << endmsg ;
    
    
    const SmartRefVector<LHCb::Particle>&  parts = sum.particles() ;

    if (parts.empty()) continue ;

    IBackgroundCategory::categories mincat = IBackgroundCategory::LastGlobal ;
    const std::map<int,std::string>& catMap = m_bkg->getCategoryMap() ;
    for ( SmartRefVector<LHCb::Particle>::const_iterator ip =  
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
        m_print->printTree(*ip,m_linker);
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
      if (msgLevel(MSG::DEBUG)) debug() << "Best category is " << (*scat).second << " for " << *is << endmsg ;
      if (!m_algoCorr->fillResult((*scat).second, true )) return StatusCode::FAILURE;
    }  
    if (!m_algoCorr->fillResult(*is, true )) return StatusCode::FAILURE; 
    if (msgLevel(MSG::DEBUG)) debug() << "End for " << *is << endmsg ;
    if (!m_algoCorr->endEvent()) return StatusCode::FAILURE ;
  }

  return  StatusCode::SUCCESS ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltBackgroundCategory::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  sc = m_algoCorr->printTable() ;
  if (!sc) return sc;
  //  sc = m_algoCorr->printList() ;
  //  if (!sc) return sc;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
