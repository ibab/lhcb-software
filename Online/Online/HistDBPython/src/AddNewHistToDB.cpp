// $Id: AddNewHistToDB.cpp,v 1.4 2009-04-07 18:04:52 nchiapol Exp $

#include "AddNewHistToDB.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(AddNewHistToDB)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
AddNewHistToDB::AddNewHistToDB(const std::string& name, ISvcLocator* ploc)
  : GaudiHistoAlg(name, ploc) 
{
  declareProperty( "PasswordFile"         , m_passwordFile          = "password");
  
  declareProperty( "TaskName"             , m_taskName              = "TestMon");
  declareProperty( "AlgorithmName"        , m_algoName              = "TestAlg");
  
  declareProperty( "HistoNames"           , m_histoNames);
  declareProperty( "HistoType"            , m_histTypeName          = "H2D");
}


//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
StatusCode AddNewHistToDB::initialize() 
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
    
  /// convert HistoTypeName into 
    OnlineHistDBEnv::HistType type = OnlineHistDBEnv::H1D;
    bool typeOK = false;
    int it;
    for (it=0; it< OnlineHistDBEnv_constants::NHTYPES; it++) {
      if ( m_histTypeName == std::string(OnlineHistDBEnv_constants::HistTypeName[it])) {
	type=(OnlineHistDBEnv::HistType)it;
	typeOK = true;
      }
    }
    if (!typeOK) {
      error() << "Given HistoType does not exist" << endmsg;
      return StatusCode::SUCCESS;
    }
    
  /// loop over all histograms on page
    std::vector< std::string >::iterator hNamesIt;
    std::vector< std::string >::iterator hNamesEndIt = m_histoNames.end();    
    for( hNamesIt = m_histoNames.begin() ; hNamesIt < hNamesEndIt ; hNamesIt++) {
        HistDB->declareHistogram(m_taskName, m_algoName, (*hNamesIt), type );
    }    
  }

  HistDB->commit();
  
  return StatusCode::SUCCESS;
}



//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
StatusCode AddNewHistToDB::execute() 
{
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
// Finalize
//------------------------------------------------------------------------------
  StatusCode AddNewHistToDB::finalize() 
  {
    m_statusCode = GaudiHistoAlg::finalize(); // must be called after all others actions
    
    return StatusCode::SUCCESS;
  }

