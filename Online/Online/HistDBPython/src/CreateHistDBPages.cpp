// $Id: CreateHistDBPages.cpp,v 1.3 2010-08-09 15:29:03 mtobin Exp $

#include "CreateHistDBPages.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(CreateHistDBPages)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
CreateHistDBPages::CreateHistDBPages(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
  
  declareProperty( "DumpPageInfo"         , m_dumpPageInfo          = false );

  declareProperty( "PageBase"             , m_pageBase              = "TestPages");
  declareProperty( "PageNames"            , m_pageNames);
  declareProperty( "PageLayout"           , m_pageLayout);
  declareProperty( "PageDoc"              , m_pageDoc);
  
  declareProperty( "HistoBase"            , m_histoBase             = "TestMon");
  declareProperty( "HistoNames"           , m_histoNames);
  declareProperty( "HistoLinks"           , m_histoLinks);
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode CreateHistDBPages::initialize() 
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
    m_histoLinksEndIterator = m_histoLinks.end();
    
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Link map:" << endmsg;
      std::map< std::string, std::string >::iterator linkIt;
      for( linkIt = m_histoLinks.begin() ; linkIt != m_histoLinksEndIterator ; linkIt++) {
	debug() << "  " << linkIt->first << " : " << linkIt->second << endmsg;
      }
    }
	
	
    //   int max = m_histoNames.size(); // old line - this should be a bug!
    int max = m_pageNames.size();
    for( int i=0 ; i < max ; i++ ){
    
      // checking layout
      if (numHistInLayout != m_histoNames[i].size()) {
        Error("Layout does not agree with Number of Histograms!\n Not processing page:"+m_pageNames[i]);
        break; 
      }

      
      // creating Pages
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
StatusCode CreateHistDBPages::execute() 
{
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
// Finalize
//------------------------------------------------------------------------------
StatusCode CreateHistDBPages::finalize() 
{
  
  debug() << "finalizing...." << endmsg;
  
  m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
  
  return StatusCode::SUCCESS;
}




/**
 * FUNCTIONS
 */
StatusCode CreateHistDBPages::createPage(OnlineHistDB *HistDB, int pageNr) 
{
  StatusCode funcStatus = StatusCode::SUCCESS;
  bool  ok = true;
  
  /// getting infos for this page
  std::string *pageName = &(m_pageNames[pageNr]);
  std::vector<std::string> *histoNames = &(m_histoNames[pageNr]);

  /// preparing page
  OnlineHistPage*  page =  HistDB->declarePage(m_pageBase + *pageName);
  page ->removeAllHistograms();
  
  /// add page documentation, if required
  if ( ! m_pageDoc.empty() ) {
    std::string *thisDoc;
    if ( m_pageDoc.size() == m_pageNames.size() ) {
      std::string pageComment = "";
      std::vector<std::string>::iterator iDoc=m_pageDoc[pageNr].begin();
      for(; iDoc != m_pageDoc[pageNr].end(); ++iDoc) {
	pageComment += (*iDoc) + "\n";
      }
      thisDoc = &pageComment;
      debug() << "Page number " << pageNr << ":\n" << *thisDoc << endmsg;
    } else {
      debug() << "Using first description." << endmsg;
      thisDoc = &(m_pageDoc[0][0]);
    }
    ok &= page->setDoc(*thisDoc);
  }
  
  /// loop over all histograms on page
  std::vector<std::string>::iterator hNamesIt;
  std::vector<std::string>::iterator hNamesEndIt = (*histoNames).end();
  std::vector<std::vector<double> >::iterator layoutIt = m_pageLayout.begin();
  for( hNamesIt = (*histoNames).begin() ; hNamesIt < hNamesEndIt ; hNamesIt++) {
    debug() << "processing: " << *hNamesIt << endmsg;

    /// adding histogram to page at the correct position
    tmpActiveHisto = HistDB->getHistogram(m_histoBase + *hNamesIt);
    if ( tmpActiveHisto ) {
      if ( ((*layoutIt)[0] < 0) && tmpLastHisto ) { 
	      ok &= (NULL != page->declareOverlapHistogram(
		                      tmpActiveHisto, tmpLastHisto
                	           ));
      } else {
	      tmpLastHisto = page->declareHistogram(
		                      tmpActiveHisto,(*layoutIt)[0],(*layoutIt)[1],(*layoutIt)[2],(*layoutIt)[3]
				   );
	      std::map< std::string, std::string >::iterator linkPair = m_histoLinks.find( *hNamesIt ); 
	      if ( linkPair != m_histoLinksEndIterator ) {
		debug() << "linking " << linkPair->first << " to " << linkPair->second << endmsg;
		tmpLastHisto->setPage2display(linkPair->second);
	      }
	      ok &= (NULL != tmpLastHisto);
      }
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
