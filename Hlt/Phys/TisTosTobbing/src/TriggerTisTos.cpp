// $Id: TriggerTisTos.cpp,v 1.4 2008-06-18 01:18:31 tskwarni Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TriggerTisTos.h"

#include "HltBase/HltUtils.h"
#include "Event/HltSummary.h"
#include "Event/Particle.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTisTos
//
// 2007-08-20 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerTisTos );

const std::vector< std::string > TriggerTisTos::m_empty_selections = std::vector< std::string >();

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerTisTos::TriggerTisTos( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TriggerSelectionTisTos ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

  m_InputSelectionsToIgnore.push_back("PV2D");  // Not really signal selection
  // optional input selection (contaminates upstream with huge TOB fraction)
  m_InputSelectionsToIgnore.push_back("MuonTAndTConfirmed"); 
  declareProperty("InputTriggerSelectionsToIgnore",
                   m_InputSelectionsToIgnore);

  m_error_count=0;

  m_hltANNSvc = 0;
}
//=============================================================================
// Destructor
//=============================================================================
TriggerTisTos::~TriggerTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTisTos::initialize() {
  StatusCode sc = TriggerSelectionTisTos::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  setOfflineInput();
  setTriggerInput();
  
  debug() << " InputTriggerSelectionsToIgnore=";
  for( std::vector<std::string>::const_iterator iTsel=m_InputSelectionsToIgnore.begin();
       iTsel!=m_InputSelectionsToIgnore.end(); ++iTsel ){
    debug() << *iTsel << " ";
  }
  debug() << endmsg;
  
 
  return StatusCode::SUCCESS;
}

//=============================================================================

void TriggerTisTos::getAlleyExitSelections()
{
  // done before ?
  if( m_alleyExitSelections.size() !=0 ){ return; }
  getHltSummary();
  // given by input selections of "HltAlleys" if exists ...
  m_alleyExitSelections = inputTriggerSelectionNames("HltAlleys");
  if( m_alleyExitSelections.size() !=0 ){ return; }
  // ... use old algorithm otherwise 
  // exit selection has InputSelection's but is not an InputSelection to anything 
  const std::vector< std::string > keys = m_hltconf->keys();
  for( std::vector< std::string >::const_iterator ikey=keys.begin();ikey!=keys.end();++ikey){
    std::string::size_type ll = ikey->find("/InputSelections");
    if(  ll!=std::string::npos ){
      std::string selName( *ikey, 0, ll );
      // remove any PreTriggers since some alleys leave them stranded
      if( selName.find("PreTrigger") != std::string::npos ){ continue;}      
      bool feedsTo(false);
      for( std::vector< std::string >::const_iterator ikey2=keys.begin();ikey2!=keys.end();++ikey2){
        std::string::size_type ll = ikey2->find("/InputSelections");
        if(  ll!=std::string::npos ){
          std::vector< std::string > inputs2 = m_hltconf->retrieve< std::vector<std::string> >(*ikey2);
          if( std::find( inputs2.begin(),inputs2.end(), selName ) != inputs2.end() ){
            feedsTo = true;
            break;
          }
        }
      }
      if( !feedsTo ){ m_alleyExitSelections.push_back( selName ); }
    }
  }
  if( m_alleyExitSelections.size() == 0 ){
    if( m_error_count++ < 50 ){
      warning() << " No Alley Exit Selections found " << endmsg;
      if( m_error_count==50 ){
        warning() << " No more warnings will be printed " << endmsg;
      }
    } 
  }
}

void TriggerTisTos::getTriggerNames()
{
  // done before ?
  if( m_triggerNames.size() !=0 ){ return; }

  // use HltANNSvc to get Hlt1, then Hlt2 names
  std::vector<IANNSvc::minor_value_type> selIDs = m_hltANNSvc->items("SelectionID");
  for(std::vector<IANNSvc::minor_value_type>::const_iterator i =
          selIDs.begin(); i!=selIDs.end(); ++i) {
    if( find( m_triggerNames.begin(), m_triggerNames.end(), i->first )
            == m_triggerNames.end() ){
          m_triggerNames.push_back(i->first);
    }
  }
  {    
  std::vector<IANNSvc::minor_value_type> selIDs2 = m_hltANNSvc->items("Hlt2SelectionID");
  for(std::vector<IANNSvc::minor_value_type>::const_iterator i =
          selIDs2.begin(); i!=selIDs2.end(); ++i) {
    if( find( m_triggerNames.begin(), m_triggerNames.end(), i->first )
            == m_triggerNames.end() ){
          m_triggerNames.push_back(i->first);
    }
  }
  }

  // for now also add any trigger-names which appear to have input selections in hltConf
  getHltSummary();
  const std::vector< std::string > keys = m_hltconf->keys();
  for( std::vector< std::string >::const_iterator ikey=keys.begin();ikey!=keys.end();++ikey){
    std::string::size_type ll = ikey->find("/InputSelections");
    if(  ll!=std::string::npos ){
      std::string selName( *ikey, 0, ll );
      if( find( m_triggerNames.begin(), m_triggerNames.end(),selName )
            == m_triggerNames.end() ){
          m_triggerNames.push_back(selName);
      }
    }
  }  
  
  if( m_triggerNames.size()==0 ){
    warning() << "No known trigger names found" << endmsg;
  }
  
}
void TriggerTisTos::refreshTriggerStructure()
{
  m_alleyExitSelections.clear();
  m_triggerNames.clear();  
}

void TriggerTisTos::setTriggerInput()
{
  m_triggerInput_Selections.clear();
}

void TriggerTisTos::addToTriggerInput( const std::string & selectionNameWithWildChar,
				       bool alleyExitsOnly)
{
  unsigned int sizeAtEntrance( m_triggerInput_Selections.size() );
  if( alleyExitsOnly ){ 
    getAlleyExitSelections();
    for( std::vector< std::string >::const_iterator inpt=m_alleyExitSelections.begin();inpt!=m_alleyExitSelections.end();++inpt){
      if( wildcmp( selectionNameWithWildChar.c_str(),inpt->c_str()) ){ 
        if( find( m_triggerInput_Selections.begin(), m_triggerInput_Selections.end(), *inpt ) 
            == m_triggerInput_Selections.end() ){
          m_triggerInput_Selections.push_back(*inpt);
        }
      }
    }
  } else {
    getTriggerNames();
    for( std::vector< std::string >::const_iterator inpt=m_triggerNames.begin();inpt!=m_triggerNames.end();++inpt){
        if( wildcmp( selectionNameWithWildChar.c_str(), inpt->c_str()) ){ 
          if( find( m_triggerInput_Selections.begin(), m_triggerInput_Selections.end(), *inpt ) 
              == m_triggerInput_Selections.end() ){
            m_triggerInput_Selections.push_back(*inpt);
          }
        }
    }
  }
  if( m_triggerInput_Selections.size()==sizeAtEntrance ){
    if( m_error_count++ < 50 ){
      warning() << " addToTriggerInput called with selectionNameWithWildChar="<< selectionNameWithWildChar 
                << " alleyExitsOnly=" << int(alleyExitsOnly) 
                << " added no selection to the Trigger Input, which has size=" << m_triggerInput_Selections.size() << endmsg;
      if( m_error_count==50 ){
        warning() << " No more warnings will be printed " << endmsg;
      }
    }
  }
}
 
std::vector< std::string > TriggerTisTos::triggerSelectionNames(unsigned int decisionRequirement,
                                                                unsigned int tisRequirement,
                                                                unsigned int tosRequirement,
                                                                bool checkInputSelections )
{ 
  if( (decisionRequirement>=kAnything) && ( tisRequirement>=kAnything) && ( tosRequirement>=kAnything) )
  {
    return m_triggerInput_Selections;
  }
  std::vector< std::string > selections;  
  for( std::vector<std::string>::const_iterator iSel = m_triggerInput_Selections.begin();
       iSel != m_triggerInput_Selections.end(); ++iSel ){
    bool decision,tis,tos;
    selectionTisTos( *iSel,decision,tis,tos);
    if( ((decisionRequirement>=kAnything)||(decision==decisionRequirement))
        && ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
        && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
      if( checkInputSelections 
          && (   ((tisRequirement<kAnything)&&(tis==tisRequirement)) 
               ||((tosRequirement<kAnything)&&(tos==tosRequirement))  ) ){
        const std::vector< std::string > & inputs = inputTriggerSelectionNames( *iSel );
        bool skip(false);
        for( std::vector< std::string >::const_iterator inputTriggerSelection=inputs.begin(); 
           inputTriggerSelection!=inputs.end(); ++inputTriggerSelection ){
          recursiveInputTriggerTisTos( *inputTriggerSelection,tis,tos,true,true);
          if( ! ( ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
                  && 
                  ((tosRequirement>=kAnything)||(tos==tosRequirement)) ) )
          {
            skip = true;
            break;            
          }
        }
        if( skip )continue;
      }
      selections.push_back( *iSel );
    }
  }
  return selections;  
}


const std::vector< std::string > & TriggerTisTos::inputTriggerSelectionNames( const std::string & selectionNameWithNoWildChar )
{
  getHltSummary();
  std::string selInput = selectionNameWithNoWildChar + "/InputSelections";
  if( m_hltconf->has_key( selInput ) ){
    return m_hltconf->retrieve< std::vector<std::string> >(selInput);
  } else {
    return m_empty_selections;    
  }
}

void TriggerTisTos::triggerTisTos( bool & decision, bool & tis, bool & tos, bool checkInputSelections)
{
  decision = false; tis=false; tos=false;
  if( m_triggerInput_Selections.size()==0 ){
    error() << " triggerTisTos called with empty Trigger Input " << endmsg;
    return;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    bool decisionThis, tisThis, tosThis;
    selectionTisTos( *iTriggerSelection, decisionThis, tisThis, tosThis );
    decision = decision || decisionThis;
    if( checkInputSelections && ( ( !tis && tisThis ) || ( !tos && tosThis ) ) ){
      const std::vector< std::string > & inputs = inputTriggerSelectionNames( *iTriggerSelection );
      for( std::vector< std::string >::const_iterator inputTriggerSelection=inputs.begin(); 
           inputTriggerSelection!=inputs.end(); ++inputTriggerSelection ){
        recursiveInputTriggerTisTos( *inputTriggerSelection, tisThis, tosThis, tis, tos );
       if( ! ( ( !tis && tisThis ) || ( !tos && tosThis ) ) ){ break; }
      }
    }
    tis = tis || tisThis;
    tos = tos || tosThis;
    if( tis && tos ){ break; }
  }
}

void TriggerTisTos::recursiveInputTriggerTisTos( const std::string & selName, 
                                                 bool & tis,  bool & tos, bool previousTis, bool previousTos)
{
  //  ----- need to ignore e.g. Primary Vertex selection
  if( std::find( m_InputSelectionsToIgnore.begin(),m_InputSelectionsToIgnore.end(), selName ) 
      != m_InputSelectionsToIgnore.end() )return;

  //   --------- update tis,tos to include this input selection
  bool decisionThis, tisThis, tosThis;
  selectionTisTos( selName, decisionThis, tisThis, tosThis );
  //   if decisionThis==false then this selection summary was not saved and therefore tisThis,tosThis must be ignored
  if( decisionThis ){
    tis = tis && tisThis;
    tos = tos && tosThis;
  }

  // ----------- if still have a reason, check input selections of this selection
  bool usePrevious = !(previousTis&&previousTos);
  if( (!usePrevious) || ( !previousTis && tis ) || ( !previousTos && tos ) ){
    const std::vector< std::string > & inputs = inputTriggerSelectionNames( selName );
    for( std::vector< std::string >::const_iterator inputTriggerSelection=inputs.begin(); 
	   inputTriggerSelection!=inputs.end(); ++inputTriggerSelection ){
      recursiveInputTriggerTisTos( *inputTriggerSelection,  tis, tos, previousTis, previousTos );
      if( usePrevious && ! ( ( !previousTis && tis ) || ( !previousTos && tos ) ) ){ break; }
    }
  }

}
