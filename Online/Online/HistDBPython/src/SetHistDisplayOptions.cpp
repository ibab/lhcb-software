// $Id: SetHistDisplayOptions.cpp,v 1.2 2009-03-30 13:19:00 nchiapol Exp $

#include "SetHistDisplayOptions.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(SetHistDisplayOptions)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
SetHistDisplayOptions::SetHistDisplayOptions(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
 
  declareProperty( "HistoBase"            , m_histoBase             = "TestMon"); 
  declareProperty( "HistoNames"           , m_histoNames);
  
  //declareProperty( "OptionsList"          , m_dispOptsList);
  declareProperty( "UnsetOptions"         , m_unsetOptions          = true);
  declareProperty( "intOptions"           , m_intOptions);
  declareProperty( "floatOptions"         , m_doubleOptions);
  declareProperty( "stringOptions"        , m_strOptions);

  declareProperty( "xLabels"              , m_xLabels);
  declareProperty( "yLabels"              , m_yLabels);
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode SetHistDisplayOptions::initialize() 
{  
  m_statusCode = GaudiHistoAlg::initialize(); // must be executed first
  // MF: ../src/SetHistDisplayOptions.cpp:36:8: 
  // MF: warning: variable ¡initOk¢ set but not used [-Wunused-but-set-variable]
  //bool initOk = true;

/// get password from file
  FILE *pFile;
  char cstr_password[20];
      
  pFile = fopen(m_passwordFile.c_str(),"r");
  if (pFile == NULL) {
    error() << "could not open password file" << endmsg;
  } else {
    fgets(cstr_password, 20, pFile);
    if (ferror(pFile)) {
      error() << "could not read password" << endmsg;
    }
    fclose(pFile);
  }
  
/// connect to DB
  OnlineHistDB *HistDB = new OnlineHistDB(cstr_password,"HIST_WRITER","HISTDB");
//   OnlineHistDB *HistDB = new OnlineHistDB();
  
  if ( HistDB ) {
    info() << "------------------------------------------------------------------" << endmsg;
    info() << " opened connection with HISTOGRAMDB : " << endmsg;
    info() << " canwrite = " << HistDB->canwrite() << endmsg;
    info() << " there is " << HistDB->nHistograms()<<" Histograms "
           << HistDB->nPages() << " Pages "
           << HistDB->nPageFolders() << " PageFolders "<<endmsg;
    info() << "------------------------------------------------------------------" << endmsg;
  } else {
    info() << "initalizing HistDB failed!" << endmsg;
    //initOk = false;
  }

  
/// loop over all histograms in m_histoNames
  std::map<std::string, int>::iterator histIt;
  std::map<std::string, int>::iterator histItEnd = m_histoNames.end();
  for( histIt = m_histoNames.begin() ; histIt != histItEnd ; histIt++) {
    std::ostringstream histo_name;
    histo_name << m_histoBase << (*histIt).first;

    info() << "processing " << histo_name.str() << endmsg;
    OnlineHistogram* histo = HistDB->getHistogram(histo_name.str());

/// unset all display options 
    if (histo) {
      if ( m_unsetOptions ) {
	histo->unsetAllDisplayOptions();
      }

  /// set the display options according to the configutration file
      if ( !m_intOptions.empty() ) {
        debug() << "*) intOptions:"<< endmsg;
        std::map<std::string, int>::iterator optIt;
        std::map<std::string, int>::iterator optItEnd = m_intOptions.end();
        for( optIt = m_intOptions.begin() ; optIt != optItEnd ; optIt++) {
          debug() << "setting " << (*optIt).first << " to " << (*optIt).second << endmsg;
          histo->setDisplayOption( (*optIt).first, (void*) &((*optIt).second) );
        }
      } else {
        debug() << "No intOptions to edit"<< endmsg;
      }
      
      if ( !m_doubleOptions.empty() ) {
        debug() << "*) floatOptions:"<< endmsg;
        std::map<std::string, double>::iterator optIt;
        std::map<std::string, double>::iterator optItEnd = m_doubleOptions.end();
        for( optIt = m_doubleOptions.begin() ; optIt != optItEnd ; optIt++) {
          debug() << "setting " << (*optIt).first << " to " << (*optIt).second << endmsg;
          float tempFloat = (float) ((*optIt).second);
          histo->setDisplayOption( (*optIt).first, (void*) &tempFloat );
        }
      } else {
        debug() << "No floatOptions to edit"<< endmsg;
      }
      
      if ( !m_strOptions.empty() ) {
        debug() << "*) strOptions:"<< endmsg;
        std::map<std::string, std::string>::iterator optIt;
        std::map<std::string, std::string>::iterator optItEnd = m_strOptions.end();
        for( optIt = m_strOptions.begin() ; optIt != optItEnd ; optIt++) {
          debug() << "setting " << (*optIt).first << " to " << (*optIt).second << endmsg;
          histo->setDisplayOption( (*optIt).first, (void*) &((*optIt).second) );
        }
      } else { 
        debug() << "No strOptions to edit"<< endmsg;
      }



  /// set custom labels for x and y axis
      debug() << "setting labels" << endmsg;
      std::vector<std::string> *xlabels = NULL;
      std::vector<std::string> *ylabels = NULL;
      if ( !m_xLabels.empty() ) {
	debug() << "xLabels found" << endmsg;
	xlabels = &m_xLabels;
      }
      if ( !m_yLabels.empty() ) {
	debug() << "yLabels found" << endmsg;
	ylabels = &m_yLabels;
      }
      
      if ( (xlabels != NULL) || (ylabels != NULL) ) {
	
        if ( msgLevel( MSG::DEBUG ) ) {
          if (xlabels != NULL) {
	    debug() << "*) xLabels" << endmsg;
            std::vector< std::string >::iterator It;
            for( It = xlabels->begin() ; It < xlabels->end() ; It++) {
              debug() << "  " << *It << endmsg;
            }
	  }
          if (ylabels != NULL) {
	    debug() << "*) yLabels" << endmsg;
            std::vector< std::string >::iterator It;
            for( It = ylabels->begin() ; It < ylabels->end() ; It++) {
              debug() << "  " << *It << endmsg;
            }
	  }
        }

        int nh = 0;	
	std::vector<OnlineHistogram*> setList;
	if ((*histIt).second) {
	  std::string histSet = histo->hsname();
	  debug() << "setName: " << histSet << endmsg;
	  nh = HistDB->getHistogramsBySet(m_histoBase + "/" + histSet, &setList);
	} else {
	  setList.push_back(histo);
	}
	debug() << "number of histograms in set: " << nh << endmsg;
	
        bool labelSuccess = true;
	std::vector<OnlineHistogram*>::iterator setIt;
	std::vector<OnlineHistogram*>::iterator setItEnd = setList.end();
	for (setIt = setList.begin(); setIt < setItEnd; setIt++) {
	  debug() << "setting labels for " << (*setIt)->hname() << endmsg;
	  if ( !(*setIt)->setBinLabels(xlabels, ylabels) ) {
            labelSuccess &= false;
	  }
	} 
	if ( !labelSuccess ) {
	  error() << "setting labels failed for histogram " << (*histIt).first << endmsg;
	}
      }

  /// saving changes
      if ((*histIt).second) {
        debug() << "saving Set"<< endmsg;
        histo->saveHistoSetDisplayOptions();
      } else {
        debug() << "saving normaly" << endmsg;
        debug() << histo->saveDisplayOptions() << endmsg;
      }
    }
    
    HistDB->commit();
    
  } //  the loop over all histograms
  
  return StatusCode::SUCCESS;
}



//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
StatusCode SetHistDisplayOptions::execute() 
{
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
// Finalize
//------------------------------------------------------------------------------
StatusCode SetHistDisplayOptions::finalize() 
{
  
  debug() << "finalizing...." << endmsg;
  
  m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
  
  return StatusCode::SUCCESS;
}
