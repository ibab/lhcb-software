// $Id: RemovePages.cpp,v 1.1 2009-04-06 12:58:54 nchiapol Exp $

#include "RemovePages.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(RemovePages)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
RemovePages::RemovePages(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
  
  declareProperty( "StartFolders"         , m_startFolders);
  declareProperty( "DryRun"               , m_dryRun                = true);
  declareProperty( "ProtectTopLevel"      , m_protectTopLevel       = true);
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode RemovePages::initialize() 
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
    
    
  /// loop over all pages
    std::vector< std::string >::iterator sfIt;
    std::vector< std::string >::iterator sfEndIt = m_startFolders.end();    
    for( sfIt = m_startFolders.begin() ; sfIt < sfEndIt ; sfIt++) {
        if (sfIt->find_first_of('/') != 0) {
	  error() << "absolute path needed - ignoring: " << *sfIt << endmsg;
	  continue;
	}
        if (sfIt->find_last_of('/') == sfIt->length()-1) {
	  error() << "remove trailing slashes - ignoring: " << *sfIt << endmsg;
	  continue;
	}
        if (m_protectTopLevel && sfIt->find_last_of('/') == 0) { 
	  error() << "removing top level directories is blocked as a safety measure: " << *sfIt << endmsg;
	  continue;
	}
	
	// This should be unnecessary due to the first two if statements
	// but it is in here just to be save.
	if (sfIt->compare("/") == 0) {
	    error() << "No, I am not going to delete the whole page tree!" << endmsg;
	    break;
	}
	
	m_folderNames.push_back(*sfIt);
	if (parseTree(HistDB, *sfIt, 0) == StatusCode::FAILURE) {
	  break;
	}
      }
     
      info() << "** processing pages:" << endmsg;
      std::vector< std::string >::iterator pIt;
      std::vector< std::string >::iterator pEndIt = m_pageNames.end();    
      for( pIt = m_pageNames.begin() ; pIt < pEndIt ; pIt++) {
	info() << *pIt << endmsg;
	OnlineHistPage*  page =  HistDB->getPage(*pIt);
	if ( !(HistDB->removePage(page))) {
	  error() << "failed to remove page: " << *pIt << endmsg;
	}
      }
      
      info() << "** processing folders:" << endmsg;
      std::vector< std::string >::reverse_iterator fIt;
      std::vector< std::string >::reverse_iterator fEndIt = m_folderNames.rend();    
      for( fIt = m_folderNames.rbegin() ; fIt < fEndIt ; fIt++) {
	info() << *fIt << endmsg;
	if ( !(HistDB->removePageFolder(*fIt))) {
	  error() << "failed to remove folder: " << *fIt << endmsg;
	}
      }
    
      if ( !m_dryRun ) {
	HistDB->commit();
      }
      
    }
    return StatusCode::SUCCESS;
  }



  //------------------------------------------------------------------------------
  // Execute
  //------------------------------------------------------------------------------
  StatusCode RemovePages::execute() 
  {
    return StatusCode::SUCCESS;
  }


  //------------------------------------------------------------------------------
  // Finalize
  //------------------------------------------------------------------------------
    StatusCode RemovePages::finalize() 
    {
      m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
      
      return StatusCode::SUCCESS;
    }


  StatusCode RemovePages::parseTree(OnlineHistDB *HistDB, std::string startPoint, int level) 
  {
    debug() << "parseTree " << startPoint << " - level: " << level << endmsg;
    if (level > 20) {
      error() << "To many sublevels!" << endmsg;
      return StatusCode::FAILURE;
    } else {
      level++;
    }
    
    std::vector<std::string> pages;
    int np  = HistDB->getPageNamesByFolder(startPoint, pages);
    debug() << "number of pages: " << np << endmsg;

    std::vector<std::string>::iterator pIt;
    std::vector<std::string>::iterator pEndIt = pages.end();
    for( pIt = pages.begin() ; pIt < pEndIt ; pIt++) {
      m_pageNames.push_back(*pIt);
    }
    
    std::vector<std::string> subfolders;
    int nf = HistDB->getPageFolderNames(subfolders, startPoint);
    debug() << "number of subfolders: " << nf << endmsg;
     
    std::vector<std::string>::iterator It;
    std::vector<std::string>::iterator EndIt = subfolders.end();
    for( It = subfolders.begin() ; It < EndIt ; It++) {
      m_folderNames.push_back(*It);
      if (parseTree(HistDB, *It, level) == StatusCode::FAILURE) {
	return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }
