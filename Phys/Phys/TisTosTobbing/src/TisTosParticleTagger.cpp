// $Id: TisTosParticleTagger.cpp,v 1.2 2010-08-09 14:17:47 tskwarni Exp $
// Include files

// local
#include "TisTosParticleTagger.h"

#include "Event/Particle.h"
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TisTosParticleTagger
//
// 2010-07-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TisTosParticleTagger )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TisTosParticleTagger::TisTosParticleTagger( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty("TisTosSpecs",m_tistosSpecs);
  declareProperty("PassOnAll",m_passOnAll=false);
  declareProperty("NoRegex",m_noRegex=false);
  declareProperty("TriggerTisTosName",m_TriggerTisTosName="");

  declareProperty("HltDecReportsInputLocation", m_decReportsLocation = "" );
  declareProperty("HltSelReportsInputLocation", m_selReportsLocation = "" );
  declareProperty("HltSplitDecReportsLocation", m_split_decReportsLocation = "" );

  // pass these to the TisTos tool
  declareProperty("ProjectTracksToCalo", m_projectTracksToCalo=true );
  declareProperty("CaloClustForCharged", m_caloClustForCharged=true );
  declareProperty("CaloClustForNeutral", m_caloClustForNeutral=true );

  declareProperty("CompositeTPSviaPartialTOSonly", m_compositeTPSviaPartialTOSonly=false );

  declareProperty("NoHitTypes", m_noHitTypes = false);
  // if empty use defaults (vector of pairs <kHitType,value>) type 0=Velo,1=T,2=TT,3=Mu,4=Ecal,5=Hcal
  declareProperty("TOSFrac",m_TOSFrac);
  declareProperty("TISFrac",m_TISFrac);

  // this will affect execution only in non-zero tag-keys are used
  declareProperty("SatisfiedOnFirstSpec",m_SatisfiedOnFirstSpec =false);


}
//=============================================================================
// Destructor
//=============================================================================
TisTosParticleTagger::~TisTosParticleTagger() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TisTosParticleTagger::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_split_checkDecReport=true;

  if( m_TriggerTisTosName != "" )
  {
    // Specific TriggerTisTos tool
    verbose() << " Allocating TriggerTisTos tool specified by user=" << m_TriggerTisTosName << endmsg;
    m_tistostool = tool<ITriggerTisTos>(  m_TriggerTisTosName, m_TriggerTisTosName+"Tool" ,this);
    if( m_TriggerTisTosName.find("TES") == 0 ){
      m_checkDecReport=false;
      m_checkSelReport=false;
      m_split_checkDecReport=false;
    } else {
      m_checkDecReport=true;
      if( m_TriggerTisTosName.find("L0") == 0 ){
        m_decReportLoc="HltLikeL0/DecReports";
        m_checkSelReport=true;
        m_selReportLoc="HltLikeL0/SelReports";
	m_split_checkDecReport=false;
      } else {
        m_decReportLoc=HltDecReportsLocation::Default;
        m_checkSelReport=false;
      }
    }
  } else {
    verbose() << " Allocating TriggerTisTos tool " << endmsg;
    m_tistostool = tool<ITriggerTisTos>(  "TriggerTisTos" , "TriggerTisTosTool" ,this);
    m_checkDecReport=true;
    m_decReportLoc=HltDecReportsLocation::Default;
    //m_decReportLoc1="Hlt1/DecReports";
    m_decReportLoc2="Hlt2/DecReports";
    m_checkSelReport=true;
    m_selReportLoc=HltSelReportsLocation::Default;
    m_selReportLoc1="Hlt1/SelReports";
    m_selReportLoc2="Hlt2/SelReports";
  }
  if( !m_tistostool )return Error("Could not allocate TriggerTisTos tool", StatusCode::FAILURE);

  if( m_decReportsLocation != "" ){
    if( m_decReportsLocation == "None" ){
      m_checkDecReport=false;
    } else {
      m_checkDecReport=true;
      m_decReportLoc=m_decReportsLocation;
    }
  }
  if( m_selReportsLocation != "" ){
    if( m_selReportsLocation == "None" ){
      m_checkSelReport=false;
    } else {
      m_checkSelReport=true;
      m_selReportLoc=m_selReportsLocation;
    }
  }
  if( m_split_decReportsLocation != "" ){
    if( m_split_decReportsLocation == "None" ){
      m_split_checkDecReport=false;
    } else {
      //      m_split_checkDecReport=true;
    }
  } else {
    m_decReportLoc1="Hlt1/DecReports";
  }

  for( unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
       iTriggerStage < NTriggerStages; ++iTriggerStage )
  {
    m_tusSpecs[iTriggerStage].clear();
    m_tosSpecs[iTriggerStage].clear();
    m_tisSpecs[iTriggerStage].clear();
  }

  // pass trigger stage on first satisfied trigger spec
  m_fast = true;

  if( m_tistosSpecs.empty() ){
    error() <<
      "TisTosSpecs not set - example: = { 'L0.*Decision%TIS':12345 , 'Hlt1.*Hadron.*Decision%TUS':12346, 'Hlt2.*Decision%TOS':12347 }"
            << endmsg;
    return StatusCode::FAILURE;
  } else {
    for ( std::map<std::string,int>::const_iterator entry = m_tistosSpecs.begin();
          entry != m_tistosSpecs.end(); ++entry )
    {
      const std::string& spec = entry->first  ;
      if( entry->second != 0 ){
        Warning( "Non-zero trigger spec tag-key is used. This is permitted only in private analysis."
                 ,StatusCode::SUCCESS, 1 ).setChecked();
        if( !m_SatisfiedOnFirstSpec ){
          m_fast=false;
        } else {
          Warning(" All specs will always be evaluated unless you set SatisfiedOnFirstSpec=True."
                  ,StatusCode::SUCCESS, 1 ).setChecked();
        }
      }
      std::size_t i = spec.find("%");
      if( i == std::string::npos )
      {
        error() << " TisTosSpec contains no InputTrigger-spec separator" << spec << endmsg;
        return StatusCode::FAILURE;
      } else {
        std::string triggerInput = spec.substr(0,i);
        // unrecognizable trigger stage defaults to HLT1 (also used for TES selections)
        unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
        if( triggerInput.find("Hlt2")==0 ){
          iTriggerStage = (unsigned int)(HLT2);
        } else if( triggerInput.find("L0")==0 ){
          iTriggerStage = (unsigned int)(L0);
        }
        if( m_noRegex ){
          if( triggerInput.find(".*") != std::string::npos ){
            m_noRegex = false;
            Warning( "Regex characters found in trigger specs. NoRegex=True cannot be used; reset to False"
                     ,StatusCode::SUCCESS, 10 ).setChecked();
          }
        }
        std::string tisTosSpec = spec.substr(i+1);
        if( tisTosSpec == "TUS" || tisTosSpec == "tus" ){
          m_tusSpecs[iTriggerStage].insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else if( tisTosSpec == "TOS" || tisTosSpec == "tos" ){
          m_tosSpecs[iTriggerStage].insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else if( tisTosSpec == "TIS" || tisTosSpec == "tis" ){
          m_tisSpecs[iTriggerStage].insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else {
          error() << " unrecognized TisTos spec in " << spec << endmsg;
          return StatusCode::FAILURE;
        }
      }
    }
  }
  for( unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
       iTriggerStage<NTriggerStages; ++iTriggerStage )
  {
    m_NoSpecs[iTriggerStage] =
      m_tusSpecs[iTriggerStage].empty() && m_tosSpecs[iTriggerStage].empty() && m_tisSpecs[iTriggerStage].empty();
  }
  if( m_NoSpecs[HLT1] && m_NoSpecs[HLT2] && m_NoSpecs[L0] ){
    error() << "No TisTos specs definied " << endmsg;
    return StatusCode::FAILURE;
  }


  if( !m_NoSpecs[L0] ) {
    m_tistostoolL0 = tool<ITriggerTisTos>(  "L0TriggerTisTos" , "L0TriggerTisTosTool" ,this);
    if( !m_tistostoolL0 )return Error("Could not allocate L0TriggerTisTos tool", StatusCode::FAILURE);
  } else {
    m_tistostoolL0 = 0;
  }
  if( !m_NoSpecs[HLT1] ) {
    m_tistostoolHlt1 = tool<ITriggerTisTos>(  "Hlt1TriggerTisTos" , "Hlt1TriggerTisTosTool" ,this);
    if( !m_tistostoolHlt1 )return Error("Could not allocate Hlt1TriggerTisTos tool", StatusCode::FAILURE);
  } else {
    m_tistostoolHlt1 = 0;
  }
  if( !m_NoSpecs[HLT2] ) {
    m_tistostoolHlt2 = tool<ITriggerTisTos>(  "Hlt2TriggerTisTos" , "Hlt2TriggerTisTosTool" ,this);
    if( !m_tistostoolHlt2 )return Error("Could not allocate Hlt2TriggerTisTos tool", StatusCode::FAILURE);
  } else {
    m_tistostoolHlt2 = 0;
  }
  //m_tistostoolStage[HLT1] = m_tistostool;
  //m_tistostoolStage[HLT2] = m_tistostool;
  //m_tistostoolStage[L0]   = m_tistostoolL0;


  // configure TisTos tool
  m_tistostool->setProjectTracksToCalo(m_projectTracksToCalo);
  m_tistostool->setCaloClustForCharged(m_caloClustForCharged);
  m_tistostool->setCaloClustForNeutral(m_caloClustForNeutral);
  m_tistostool->setCompositeTPSviaPartialTOSonly(m_compositeTPSviaPartialTOSonly);
  m_tistostool->setNoHitTypes(m_noHitTypes);
  if( m_tistostoolL0 ){  m_tistostoolL0->setNoHitTypes(m_noHitTypes);  }
  if( m_tistostoolHlt1 ){  m_tistostoolHlt1->setNoHitTypes(m_noHitTypes);  }
  if( m_tistostoolHlt2 ){  m_tistostoolHlt2->setNoHitTypes(m_noHitTypes);  }
  for( std::map<int,double>::const_iterator entry=m_TOSFrac.begin();entry!=m_TOSFrac.end();++entry){
    m_tistostool->setTOSFrac( entry->first, entry->second );
    if( m_tistostoolL0 ){  m_tistostoolL0->setTOSFrac( entry->first, entry->second ); }
    if( m_tistostoolHlt1 ){  m_tistostoolHlt1->setTOSFrac( entry->first, entry->second ); }
    if( m_tistostoolHlt2 ){  m_tistostoolHlt2->setTOSFrac( entry->first, entry->second ); }
  }
  for( std::map<int,double>::const_iterator entry=m_TISFrac.begin();entry!=m_TISFrac.end();++entry){
    m_tistostool->setTISFrac( entry->first, entry->second );
    if( m_tistostoolL0 ){  m_tistostoolL0->setTISFrac( entry->first, entry->second ); }
    if( m_tistostoolHlt1 ){  m_tistostoolHlt1->setTISFrac( entry->first, entry->second ); }
    if( m_tistostoolHlt2 ){  m_tistostoolHlt2->setTISFrac( entry->first, entry->second ); }
  }

  if ( msgLevel(MSG::DEBUG) ) {
    if( m_noRegex ){
      debug() << "No regex expressions in trigger names allowed - regex patterns will be ignored " << endmsg;
    }
    if( m_fast ){
      debug() << "Pass a trigger stage on first satisfied trigger spec - do not evaluate others." << endmsg;
    }
    for( unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
         iTriggerStage<NTriggerStages;++iTriggerStage ){
      if( iTriggerStage==HLT1 ){
        debug() << " =========== HLT1 or undefined trigger stage ============== " << endmsg;
      } else if( iTriggerStage==HLT2 ){
        debug() << " =========== HLT2 ============== " << endmsg;
      } else if( iTriggerStage==L0 ){
        debug() << " =========== L0 ============== " << endmsg;
      }
      debug() << " Registered TUS conditions size=" << m_tusSpecs[iTriggerStage].size() ;
      for ( std::map<std::string,int>::const_iterator entry=m_tusSpecs[iTriggerStage].begin();
            entry!= m_tusSpecs[iTriggerStage].end();++entry){
        debug() << " , " << entry->first << " " << entry->second ;
      }
      debug() << endmsg;
      debug() << " Registered TOS conditions size=" << m_tosSpecs[iTriggerStage].size() ;
      for ( std::map<std::string,int>::const_iterator entry=m_tosSpecs[iTriggerStage].begin();
            entry!= m_tosSpecs[iTriggerStage].end();++entry){
        debug() << " , " << entry->first << " " << entry->second ;
      }
      debug() << endmsg;
      debug() << " Registered TIS conditions size=" << m_tisSpecs[iTriggerStage].size() ;
      for ( std::map<std::string,int>::const_iterator entry=m_tisSpecs[iTriggerStage].begin();
            entry!= m_tisSpecs[iTriggerStage].end();++entry){
        debug() << " , " << entry->first << " " << entry->second ;
      }
      debug() << endmsg;
    }
    if( m_passOnAll ){
      debug() << " always setFilterPassed(true); pass all input particles to output " << endmsg;
    } else {
      debug() << " Pass to output only particles which satisfy trigger specs. " << endmsg;
      debug() << " setFilterPassed(true) only if at least one particle saved to output. " << endmsg;
    }
    if( m_checkDecReport ) debug() << " We will check for presence of HltDecReports at " + m_decReportLoc << endmsg;
    if( m_checkSelReport ) debug() << " We will check for presence of HltSelReports at " + m_selReportLoc << endmsg;
  }

  return sc;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode TisTosParticleTagger::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //Get the particles from the local storage
  const LHCb::Particle::ConstVector& particles = this->i_particles();


  // determine if split reports
  bool split(false);
  if( m_split_checkDecReport ){
    split = exist<LHCb::HltDecReports>(m_decReportLoc1,false);    
    if( !split ) split= exist<LHCb::HltDecReports>(m_decReportLoc1);    
  }


  // Useful for MC: check if necessary reports exists
  if( m_checkDecReport ){
    if( split ){
      if( m_tistostoolHlt2 ){
	if( !exist<LHCb::HltDecReports>(m_decReportLoc2,false) ){
	  if( !exist<LHCb::HltDecReports>(m_decReportLoc2) ){
	    setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted.
	    if ( msgLevel(MSG::DEBUG) ) debug() << " No HltDecReport at " << m_decReportLoc2 << endmsg;
	    return StatusCode::SUCCESS;
	  }
	}
      }
    } else {
      if( !exist<LHCb::HltDecReports>(m_decReportLoc,false) ){
	if( !exist<LHCb::HltDecReports>(m_decReportLoc) ){
	  setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted.
	  if ( msgLevel(MSG::DEBUG) ) debug() << " No HltDecReport at " << m_decReportLoc << endmsg;
	  return StatusCode::SUCCESS;
	}
      }
    }
  }
  if( m_checkSelReport ){
    if( split ){
      if( m_tistostoolHlt1 ){
	if( !exist<LHCb::HltSelReports>(m_selReportLoc1,false) ){
	  if( !exist<LHCb::HltSelReports>(m_selReportLoc1) ){
	    setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted.
	    if ( msgLevel(MSG::DEBUG) ) debug() << " No HltSelReport at " << m_selReportLoc1 << endmsg;
	    return StatusCode::SUCCESS;
	  }
	}
      }
      if( m_tistostoolHlt2 ){
	if( !exist<LHCb::HltSelReports>(m_selReportLoc2,false) ){
	  if( !exist<LHCb::HltSelReports>(m_selReportLoc2) ){
	    setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted.
	    if ( msgLevel(MSG::DEBUG) ) debug() << " No HltSelReport at " << m_selReportLoc2 << endmsg;
	    return StatusCode::SUCCESS;
	  }
	}
      }
    } else {
      if( !exist<LHCb::HltSelReports>(m_selReportLoc,false) ){
	if( !exist<LHCb::HltSelReports>(m_selReportLoc) ){
	  setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted.
	  if ( msgLevel(MSG::DEBUG) ) debug() << " No HltSelReport at " << m_selReportLoc << endmsg;
	  return StatusCode::SUCCESS;
	}
      }
    }
  }

  if( split ){
    m_tistostoolStage[HLT1] = m_tistostoolHlt1;
    m_tistostoolStage[HLT2] = m_tistostoolHlt2;
    m_tistostoolStage[L0]   = m_tistostoolL0;
  } else {
    m_tistostoolStage[HLT1] = m_tistostool;
    m_tistostoolStage[HLT2] = m_tistostool;
    m_tistostoolStage[L0]   = m_tistostoolL0;
  }

  std::vector<const LHCb::Particle*> outparts;

  bool passed(m_passOnAll);

  bool firstL0(true);
  bool skipL0(false);

  for (LHCb::Particle::ConstVector::const_iterator particle = particles.begin();
       particle != particles.end(); ++particle)
  {
    if( !particleOK(*particle) ){
      if ( msgLevel(MSG::DEBUG) ) debug() << " Particle failed integrity check key = " << (*particle)->key() << endmsg;
      continue;
    }

    Particle* candi = const_cast<Particle*>(*particle);


    // This is a slow part of TisTos tool - do it only once
    //      only this tool is guaranteed to be allocated and configured
    m_tistostool->setOfflineInput(*candi);
    //      copying offline hits is fast
    if( split ){
      if( m_tistostoolHlt1 ){
	m_tistostoolHlt1->setOfflineInput( m_tistostool->offlineLHCbIDs() );
      }
      if( m_tistostoolHlt2 ){
	m_tistostoolHlt2->setOfflineInput( m_tistostool->offlineLHCbIDs() );
      }
    } 
    if( m_tistostoolL0 ){
	m_tistostoolL0->setOfflineInput( m_tistostool->offlineLHCbIDs() );
    }

    bool acceptStage[NTriggerStages] = { false, false, false };


    for(unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
        iTriggerStage < NTriggerStages; ++iTriggerStage){
      // if stage no present in specs it will be set to passed
      acceptStage[iTriggerStage] = m_NoSpecs[iTriggerStage];

      if ( msgLevel(MSG::DEBUG) ) {
        if( iTriggerStage==HLT1 ){
          debug() << " ====== HLT1 or undefined ====== " << endmsg;
        } else if( iTriggerStage==HLT2 ){
          debug() << " =========== HLT2 ============== " << endmsg;
        } else if( iTriggerStage==L0 ){
          debug() << " =========== L0 ================ " << endmsg;
        }
        if( m_NoSpecs[iTriggerStage] ) debug() << " No trigger specs " << endmsg;
      }

      if( m_fast && acceptStage[iTriggerStage] )continue;

      if( iTriggerStage == L0 ){
        if( skipL0 )continue;
        if( firstL0 ){
          firstL0 = false;
          // Useful for MC: check if necessary reports exists
          if( !exist<LHCb::HltDecReports>("HltLikeL0/DecReports") ){
            if ( msgLevel(MSG::DEBUG) ) debug() << " No HltDecReport at " << "HltLikeL0/DecReports" << endmsg;
            skipL0 = true;
            continue;
          }
          if( !exist<LHCb::HltSelReports>("HltLikeL0/SelReports") ){
            if ( msgLevel(MSG::DEBUG) ) debug() << " No HltSelReport at " << "HltLikeL0/SelReports" << endmsg;
            skipL0 = true;
            continue;
          }
        }
      }

      for ( std::map<std::string,int>::const_iterator entry = m_tusSpecs[iTriggerStage].begin();
            entry != m_tusSpecs[iTriggerStage].end(); ++entry ) {
        const std::string& triggerInput = entry->first  ;
        const int          index    = entry->second ;
        bool acc;
        if( m_noRegex) {
          acc = m_tistostoolStage[iTriggerStage]->tusSelection( triggerInput );
        } else {
          m_tistostoolStage[iTriggerStage]->setTriggerInput(triggerInput);
          acc=m_tistostoolStage[iTriggerStage]->tusTrigger();
        }
        if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TUS " << acc << endmsg;
        if( index )if( !storeTag(candi,index,acc) ){
            std::ostringstream mess;
            mess << triggerInput << " TUS index=" << index << " used for other types or illegal";
            Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
          }
        if( acc ){
          acceptStage[iTriggerStage] =true;
          if( m_fast )break;
        }
      }

      if( m_fast && acceptStage[iTriggerStage] )continue;
      for ( std::map<std::string,int>::const_iterator entry = m_tosSpecs[iTriggerStage].begin();
            entry != m_tosSpecs[iTriggerStage].end(); ++entry ) {
        const std::string& triggerInput = entry->first  ;
        const int          index    = entry->second ;
        bool acc;
        if( m_noRegex) {
          acc = m_tistostoolStage[iTriggerStage]->tosSelection( triggerInput );
        } else {
          m_tistostoolStage[iTriggerStage]->setTriggerInput(triggerInput);
          acc=m_tistostoolStage[iTriggerStage]->tosTrigger();
        }
        if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TOS " << acc << endmsg;
        if( index )if( !storeTag(candi,index,acc) ){
            std::ostringstream mess;
            mess << triggerInput << " TOS index=" << index << " used for other types or illegal";
            Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
          }
        if( acc ){
          acceptStage[iTriggerStage] =true;
          if( m_fast )break;
        }
      }

      if( m_fast && acceptStage[iTriggerStage] )continue;
      for ( std::map<std::string,int>::const_iterator entry = m_tisSpecs[iTriggerStage].begin();
            entry != m_tisSpecs[iTriggerStage].end(); ++entry ) {
        const std::string& triggerInput = entry->first  ;
        const int          index    = entry->second ;
        bool acc;
        if( m_noRegex) {
          acc = m_tistostoolStage[iTriggerStage]->tisSelection( triggerInput );
        } else {
          m_tistostoolStage[iTriggerStage]->setTriggerInput(triggerInput);
          acc=m_tistostoolStage[iTriggerStage]->tisTrigger();
        }
        if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TIS " << acc << endmsg;
        if( index )if( !storeTag(candi,index,acc) ){
            std::ostringstream mess;
            mess << triggerInput << " TIS index=" << index << " used for other types or illegal";
            Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
          }
        if( acc ){
          acceptStage[iTriggerStage] =true;
          if( m_fast )break;
        }
      }

      if( m_fast ){
        // advance to next stage only if satisifed this one
        if( acceptStage[iTriggerStage] )continue;
        // otherwise stop right away
        break;
      }
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      for(unsigned int iTriggerStage = (unsigned int)(defaultTriggerStage);
          iTriggerStage < NTriggerStages; ++iTriggerStage)
      {
        if( iTriggerStage==HLT1 ){
          debug() << " ==== HLT1 or undefined ======== ";
        } else if( iTriggerStage==HLT2 ){
          debug() << " =========== HLT2 ============== ";
        } else if( iTriggerStage==L0 ){
          debug() << " =========== L0 ================ ";
        }
        if( m_NoSpecs[iTriggerStage] ){
          debug() << " No Specs " << endmsg;
        } else if( acceptStage[iTriggerStage] ){
          debug() << " Specs satisfied " << endmsg;
        } else {
          debug() << " Specs failed or not evaluated " << endmsg;
        }
      }
    }

    bool accept(false);
    if( m_passOnAll ){
      accept=true;
    } else {
      accept = acceptStage[HLT1] && acceptStage[HLT2] && acceptStage[L0];
    }

    if( accept  ){
      outparts.push_back(candi);
      passed = true;
      if ( msgLevel(MSG::DEBUG) ) debug() << " Particle saved  key= " << candi->key() << endmsg;
    }
  }

  for( const auto particle : outparts ) {
     cloneAndMarkTree(particle);
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " Filter passed = " << passed << endmsg;
  setFilterPassed(passed);  // Mandatory. Set to true if event is accepted.

  return StatusCode::SUCCESS;
}


bool TisTosParticleTagger::particleOK(const Particle* po)
{
  if( !po )return false;
  const Particle & p = *po;
  //if( !(p.endVertex()) )return false;
  const SmartRefVector<Particle>& daughters = p.daughters();
  if ( !daughters.empty() )
  {
    for( SmartRefVector<Particle>::const_iterator pp=daughters.begin();
         pp != daughters.end(); ++pp )
    {
      if( !(pp->target()) )return false;
      const Particle* ppp= pp->target();
      if( !ppp )return false;
      if( !particleOK(ppp) )return false;
    }
  } else {
    if( !(p.proto()) )return false;
  }
  return true;
}

bool TisTosParticleTagger::storeTag(Particle* po, int index,bool passedCondition)
{
  if( !po )return false;
  double value = (passedCondition ? 1.0: 0.0);
  double svalue = po->info(index,-1.0);
  if( svalue == -1.0 ){
    return po->addInfo(index,value);
  } else if( passedCondition ){
    if( svalue == 0.0 ){
      po->eraseInfo(index);
      return po->addInfo(index,value);
    } else if( svalue == value ){
      return true;
    } else {
      return false;
    }
  } else if(  svalue == value ){
    return true;
  } else {
    return false;
  }
}

//=============================================================================
