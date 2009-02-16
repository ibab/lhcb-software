// $Id: STSetHistDisplayOptions.cpp,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $

#include "STSetHistDisplayOptions.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(STSetHistDisplayOptions);

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
STSetHistDisplayOptions::STSetHistDisplayOptions(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
  declareProperty( "TaskName"             , m_taskName              = "ITTESTmon");
  declareProperty( "AlgoBase"             , m_algoBase              = "STZSMonitor");
  
  declareProperty( "HistosToEdit"         , m_histosToEdit);
  
  declareProperty( "OptionsList"          , m_dispOptsList);
  declareProperty( "intOptions"           , m_intOptions);
  declareProperty( "floatOptions"         , m_doubleOptions);
  declareProperty( "stringOptions"        , m_strOptions);
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode STSetHistDisplayOptions::initialize() 
{  
  m_statusCode = GaudiHistoAlg::initialize(); // must be executed first
  bool initOk = true;

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
    initOk = false;
  }

  
/// loop over all histograms in m_histosToEdit
  std::map<std::string, int>::iterator histIt;
  std::map<std::string, int>::iterator histItEnd = m_histosToEdit.end();
  for( histIt = m_histosToEdit.begin() ; histIt != histItEnd ; histIt++) {
    std::ostringstream histo_name;
    histo_name << m_taskName << "/" << m_algoBase << (*histIt).first;

    info() << "processing " << histo_name.str() << endmsg;
    OnlineHistogram* histo = HistDB->getHistogram(histo_name.str());

  /// unset all display options 
    if (histo) {
      if ( !m_dispOptsList.empty() ) {
        debug() << "*) resetting all Options"<< endmsg;
        std::vector<std::string>::iterator optIt;
        std::vector<std::string>::iterator optItEnd = m_dispOptsList.end();
        for( optIt = m_dispOptsList.begin() ; optIt != optItEnd ; optIt++) {
          histo->unsetDisplayOption( (*optIt) );
        }
      } else {
        debug() << "OptionList is empty"<< endmsg;
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
StatusCode STSetHistDisplayOptions::execute() 
{
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
// Finalize
//------------------------------------------------------------------------------
StatusCode STSetHistDisplayOptions::finalize() 
{
  
  debug() << "finalizing...." << endmsg;
  
  m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
  
  return StatusCode::SUCCESS;
}
