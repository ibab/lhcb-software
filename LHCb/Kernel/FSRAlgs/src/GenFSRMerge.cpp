// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiAlg/IGenericTool.h"

// event model
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"
#include "Event/CrossSectionsFSR.h"
#include "Event/GenHeader.h"

//local
#include "GenFSRMerge.h"
#include "FSRNavigator.h"  

//-----------------------------------------------------------------------------       
// Implementation file for class : GenFSRMerge                                       
//                                                                                    
// 2015-06-25 : Davide Fazzini                                                           
//-----------------------------------------------------------------------------       

// Declaration of the Algorithm Factory                                               
DECLARE_ALGORITHM_FACTORY( GenFSRMerge )

//=============================================================================       
// Standard constructor, initializes variables                                        
//=============================================================================       

GenFSRMerge::GenFSRMerge(const std::string& name, ISvcLocator* pSvcLocator)
   :  GaudiAlgorithm( name, pSvcLocator),
      m_fileRecordSvc(NULL),
      m_navigatorTool(NULL),
      m_evtType(0),
      m_genName(""),
      m_method("")
{
  // expect the data to be written at LHCb::GenFSRLocation::Default        
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"   );
  declareProperty( "FSRName"            , m_FSRName           = "/GenFSR"       );
}

//=============================================================================       
// Destructor                                                                         
//=============================================================================       
GenFSRMerge::~GenFSRMerge() {}

//=============================================================================    
// Initialization                                                                  
//=============================================================================    
StatusCode GenFSRMerge::initialize() 
{  
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first          
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm     

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  // get the File Records service              
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);  

  // prepare navigator tool                    
  m_navigatorTool = tool<IFSRNavigator>("FSRNavigator", "FSRNavigator"); 

  return StatusCode::SUCCESS;

}

//=============================================================================       
// Main execution                                                                     
//=============================================================================       
StatusCode GenFSRMerge::execute() 
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================       
//  Finalize                                                                          
//=============================================================================       
StatusCode GenFSRMerge::finalize() 
{

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  info() << "========== Merging GenFSR ==========" << endmsg;

  // integrate all FSRs in one go                                                     
  const StatusCode mergeSC = merge();
 
  return (mergeSC && GaudiAlgorithm::finalize()); // must be called after all other actions
}

//=============================================================================         
//  merge the FSR data       
//=============================================================================         
StatusCode GenFSRMerge::merge() 
{

  // make an inventory of the FileRecord store         
  std::string fileRecordRoot = m_FileRecordName;
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  int nFSRs = 0;
  
  for(std::vector<std::string>::iterator iAddr = addresses.begin(); iAddr != addresses.end(); iAddr++)
  {
    debug() << "address: " << *iAddr << endmsg;
        
    if (iAddr->find(m_FSRName) != std::string::npos)
    {
      std::string fileRecordAddress = *iAddr;
      LHCb::GenFSR* theGenFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, fileRecordAddress);
      
      if(m_evtType==0) m_evtType = theGenFSR->getSimulationInfo("evtType", 0);
      else if(m_evtType != theGenFSR->getSimulationInfo("evtType", 0))
        throw GaudiException("Found a different Event Type! Merging is not allowed!",
                             "GenMergeFSR::finalize", StatusCode::FAILURE); 
      
      if(m_genName=="") m_genName = theGenFSR->getSimulationInfo("hardGenerator", "");        
      else if(m_genName != theGenFSR->getSimulationInfo("hardGenerator", ""))
        throw GaudiException("Found a different Hard Generator Name! Merging is not allowed!",
                             "GenMergeFSR::finalize", StatusCode::FAILURE);

      if(m_method=="") m_method = theGenFSR->getSimulationInfo("generationMethod", "");
      else if(m_method != theGenFSR->getSimulationInfo("generationMethod", ""))
        if(msgLevel(MSG::VERBOSE)) verbose() << "Found a different generation method" << endmsg;
      
      nFSRs += 1;
    }
  }

  if(nFSRs != 0)
  {
    LHCb::GenFSR* fsr = new LHCb::GenFSR();
    put(m_fileRecordSvc, fsr, fileRecordRoot + m_FSRName);
    
    fsr->initializeInfos();
  
    for(std::vector<std::string>::iterator iAddr = addresses.begin(); iAddr != addresses.end(); iAddr++)
    {
      if (iAddr->find(m_FSRName) != std::string::npos) 
      {
        std::string fileRecordAddress = *iAddr;
        
        LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, fileRecordAddress);
        
        if(genFSR == NULL)
        {
          if(msgLevel(MSG::VERBOSE)) error() << fileRecordAddress << " not found" << endmsg;
        }
        else
        {
          if(msgLevel(MSG::VERBOSE)) verbose() << fileRecordAddress << " found" << endmsg;
      
          // Fill the informations of the new FSR
          if(fsr->getSimulationInfo("hardGenerator", "") == "") 
          {
            fsr->setStringInfos(genFSR->stringInfos());
            fsr->setIntInfos(genFSR->intInfos());
          }
          
          // Update the FSR information and append to TS
          *fsr += *genFSR;
        }
      }
    }
  }
  
  // read back genFSR from TS                    
  if(nFSRs != 0)
  {
    LHCb::GenFSR* topGenFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, fileRecordRoot + m_FSRName);
    if (msgLevel(MSG::DEBUG)) debug() << *topGenFSR << endmsg;
    
    // clean up original FSRs              
    for(std::vector<std::string>::iterator iAddr = addresses.begin(); iAddr != addresses.end(); iAddr++)
    {
      // get FSR as keyed object and cleanup the original ones - this only cleans genFSRs   
      std::string fileRecordAddress = *iAddr;
      
      if(msgLevel(MSG::DEBUG)) debug() << "address in list: " << (*iAddr) << endmsg;
      
      LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, fileRecordAddress);
      if (NULL == genFSR) 
      {
        if (msgLevel(MSG::ERROR)) error() << fileRecordAddress << " not found" << endmsg;
      } 
      else 
      {
        if (msgLevel(MSG::VERBOSE)) verbose() << fileRecordAddress << " found" << endmsg;

        m_fileRecordSvc->unlinkObject(*iAddr).ignore(); // get the FSR out of TS            
        iAddr->erase(iAddr->end()-m_FSRName.length(),iAddr->end());
        m_fileRecordSvc->unlinkObject(*iAddr).ignore(); // get the FSR's directory out of TS
      }
    }
  }
  
  // make a new inventory of the FileRecord store              
  addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  for(std::vector<std::string>::iterator iAddr = addresses.begin(); iAddr != addresses.end(); iAddr++)
    if (msgLevel(MSG::DEBUG)) debug() << "address: " << *iAddr << endmsg;
  
  return StatusCode::SUCCESS;
  
}
