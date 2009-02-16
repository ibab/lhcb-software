// $Id: STCreateHistDBPages.cpp,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $

#include "STCreateHistDBPages.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(STCreateHistDBPages);

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
STCreateHistDBPages::STCreateHistDBPages(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
  
  declareProperty( "DumpPageInfo"         , m_dumpPageInfo          = false );

  declareProperty( "PageBase"             , m_pageBase              = "/ST Commissioning/Nicola");
  declareProperty( "PageNames"            , m_pageNames);
  declareProperty( "PageLayout"           , m_pageLayout);
  
  declareProperty( "TaskName"             , m_taskName              = "ITTESTmon");
  declareProperty( "AlgoBase"             , m_algoBase              = "STZSMonitor");
  declareProperty( "HistoNames"           , m_histoNames);
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode STCreateHistDBPages::initialize() 
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
  
  if ( HistDB ) {
    info() << "------------------------------------------------------------------" << endmsg;
    info() << " opened connection with HISTOGRAMDB : " << endmsg;
    info() << " canwrite = " << HistDB->canwrite() << endmsg;
    info() << " there is " << HistDB->nHistograms()<<" Histograms "
           << HistDB->nPages() << " Pages "
           << HistDB->nPageFolders() << " PageFolders "<<endmsg;
    info() << "------------------------------------------------------------------" << endmsg;
  } else {
    error() << "initalizing HistDB failed!" << endmsg;
    initOk = false;
  }

  if ( initOk ) {
  /// loop over all pages in m_pageNames
    unsigned int numHistInLayout = m_pageLayout.size();
  //   int max = m_histoNames.size(); // old line - this should be a bug!
    int max = m_pageNames.size();
    for( int i=0 ; i < max ; i++ ){
    
      /// checking layout
      if (numHistInLayout != m_histoNames[i].size()) {
        Error("Layout does not agree with Number of Histograms!\n Not processing page:"+m_pageNames[i]);
        break; // same effect as initOK = false;
      }
      
      /// creating Pages
      StatusCode pageStatus;
      
      pageStatus = createPage(HistDB, i);
      
      if (pageStatus.isSuccess()) {
        info() << "page " << m_pageNames[i] << " in " << m_pageBase << " successfully created" << endmsg;
      } else {
        info() << "page " << m_pageNames[i] << " could not be created" << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}



//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
StatusCode STCreateHistDBPages::execute() 
{
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
// Finalize
//------------------------------------------------------------------------------
StatusCode STCreateHistDBPages::finalize() 
{
  
  debug() << "finalizing...." << endmsg;
  
  m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
  
  return StatusCode::SUCCESS;
}




/**
 * FUNCTIONS
 */
StatusCode STCreateHistDBPages::createPage(OnlineHistDB *HistDB, int pageNr) 
{
  StatusCode funcStatus = StatusCode::SUCCESS;
  bool  ok = true;
  
  /// getting infos for this page
  std::string *pageName = &(m_pageNames[pageNr]);
  std::vector<std::string> *histoNames = &(m_histoNames[pageNr]);

  /// preparing page
  OnlineHistPage*  page =  HistDB->declarePage(m_pageBase + '/' + *pageName);
  page ->removeAllHistograms();
  
  /// loop over all histograms on page
  std::vector<std::string>::iterator hNamesIt;
  std::vector<std::string>::iterator hNamesEndIt = (*histoNames).end();
  std::vector<std::vector<double> >::iterator layoutIt = m_pageLayout.begin();
  for( hNamesIt = (*histoNames).begin() ; hNamesIt < hNamesEndIt ; hNamesIt++) {
    debug() << "processing: " << *hNamesIt << endmsg;

    /// adding histogram to page at the correct position
    tmpOnlineHisto = HistDB->getHistogram(m_taskName + '/' + m_algoBase + *hNamesIt);
    if ( tmpOnlineHisto ) {
      ok &= (NULL != page->declareHistogram(tmpOnlineHisto,(*layoutIt)[0],(*layoutIt)[1],(*layoutIt)[2],(*layoutIt)[3]) );
    }

    layoutIt++;
  }
  
  /// saving changes
  if( ok ) {
    page->save();
    if ( m_dumpPageInfo ) page->dump();
  } else {
    funcStatus = StatusCode::FAILURE;
  }
  HistDB->commit();

  return funcStatus; 
}
