// $Id: HltBackgroundCategory.cpp,v 1.21 2009-11-07 21:40:59 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IPrintDecayTreeTool.h"            // Interface
#include "MCInterfaces/IPrintMCDecayTreeTool.h"            // Interface
#include "Kernel/IANNSvc.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

// local
#include "HltBackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltBackgroundCategory
//
// 2008-02-04 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  HltBackgroundCategory::HltBackgroundCategory( const std::string& name,
                                                ISvcLocator* pSvcLocator)
    : Hlt2StatisticsBase ( name , pSvcLocator )
    , m_bkg()
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
StatusCode HltBackgroundCategory::initialize()
{
  StatusCode sc = Hlt2StatisticsBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by Hlt2StatisticsBase

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_bkg         = tool<IBackgroundCategory>("BackgroundCategory",this);
  if (m_printTree ){
    m_print       = tool<IPrintDecayTreeTool>("PrintDecayTreeTool",this);
    m_printMC     = tool<IPrintMCDecayTreeTool>("PrintMCDecayTreeTool",this);
    m_linker      = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper",this);
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
  
  const LHCb::HltDecReports* decReports = getIfExists<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default);
  
  if( NULL==decReports)
  {
    return Warning("No Hlt decision",StatusCode::SUCCESS,1);
  }
  
  if (!(decReports->decReport("Hlt2Global"))) return StatusCode::SUCCESS ;
  const LHCb::HltSelReports* selReports = getIfExists<LHCb::HltSelReports>( LHCb::HltSelReportsLocation::Default );
  if( NULL==selReports)
  {
    return Warning("No Hlt sel Reports",StatusCode::SUCCESS,1);
  }

  Particle2MCLinker* linker = 0;
  if (m_printTree) linker = m_linker->linker( Particle2MCMethod::Links );

  for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
      it!=decReports->end();++it){
    if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first
                                       << " decision= " << it->second.decision() << endmsg;
    if ( !it->second.decision() ) continue ;
    LHCb::Particle::ConstVector parts;
    const LHCb::HltObjectSummary* sum = selReports->selReport(it->first);
    if (0==sum) {
      Warning("No summary for "+it->first,StatusCode::SUCCESS,1);
      continue ;
    }
    if (msgLevel(MSG::DEBUG)) debug() <<  it->first << " summarised obj = " << sum->summarizedObject()
                                      << ", vector " << sum->substructure().size() << endmsg ;
    for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator s = sum->substructure().begin() ;
          s != sum->substructure().end() ; ++s){
      if (msgLevel(MSG::DEBUG)) debug() << it->first << " substructure has " << (*s)->summarizedObject() << " and "
                                        << (*s)->substructure().size() << endmsg ;
      if (0==(*s)->summarizedObject()) continue ;
      const LHCb::Particle* p = dynamic_cast<const LHCb::Particle*>((*s)->summarizedObject());
      if ( 0==p ) Warning(it->first+" has no particles",StatusCode::SUCCESS,1).ignore();
      else parts.push_back(p);
    }
    if (parts.empty()) {
      err() << "Selection " << it->first << " found no particles" << endmsg  ;
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
        info() << it->first   << " has a candidate "
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
        if (msgLevel(MSG::DEBUG)) debug() << "Filling " << (*scat).second << " for " << it->first << endmsg ;
        if (!m_algoCorr->fillResult((*scat).second, true )) return StatusCode::FAILURE;
      } else if ( cat < mincat ) {
        if (msgLevel(MSG::DEBUG)) debug() << "New minimum " << cat << " for " << it->first << endmsg ;
        mincat = cat ;
      }
    }
    if (!m_fillAll){
      if ( mincat == IBackgroundCategory::LastGlobal ) mincat = IBackgroundCategory::Undefined ;
      std::map<int, std::string>::const_iterator scat = catMap.find(mincat) ;
      if (msgLevel(MSG::DEBUG)) debug() << "Best category is " << (*scat).second
                                        << " for " << it->first << endmsg ;
      if (!m_algoCorr->fillResult((*scat).second, true )) return StatusCode::FAILURE;
    }
    if (!m_algoCorr->fillResult(it->first, true )) return StatusCode::FAILURE;
    if (msgLevel(MSG::DEBUG)) debug() << "End for " << it->first << endmsg ;
    if (!m_algoCorr->endEvent()) return StatusCode::FAILURE ;
  }

  return  StatusCode::SUCCESS ;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltBackgroundCategory )
