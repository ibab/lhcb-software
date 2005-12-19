// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "STSelectFromCluster.h"
#include "Kernel/STDetSwitch.h"

static const ToolFactory<STSelectFromCluster>  s_factory;
const IToolFactory& STSelectFromClusterFactory = s_factory;


STSelectFromCluster::STSelectFromCluster(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent)
{

 // interfaces
 this->declareProperty("dataLocation", m_dataLocation = LHCb::STClusterLocation::TTClusters);
  
 declareProperty("detType", m_detType = "TT");
 declareInterface<ISTDigitSelector>(this);
}

STSelectFromCluster::~STSelectFromCluster(){
  // destructer
}

StatusCode STSelectFromCluster::initialize(){

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_dataLocation);

  return StatusCode::SUCCESS;
}

StatusCode STSelectFromCluster::initializeEvent(){

  // intialize once per event

  // retrieve clusters
  m_dataCont = get<LHCb::STClusters>(m_dataLocation);
  
  m_cachedSector = -1;
  m_cachedIter = m_dataCont->begin();
 
  return StatusCode::SUCCESS;
}

bool STSelectFromCluster::execute(const LHCb::STDigit* aDigit){

  // search for digit in cluster container
   
  LHCb::STChannelID chan = aDigit->channelID();
  int iSectorID = (int)chan.uniqueSector();

  if (iSectorID != m_cachedSector){
    updateCache(iSectorID);
  }


  LHCb::STClusters::iterator iterC = m_cachedIter;
  bool found = false;
  while ((iterC != m_dataCont->end())
    &&((int)(*iterC)->channelID().uniqueSector() <= iSectorID )&&(found == false)){
    SmartRefVector<LHCb::STDigit>::const_iterator iterDigit = (*iterC)->digits().begin();
    for ( ; iterDigit != (*iterC)->digits().end(); ++iterDigit){ 
      if ((*iterDigit)->channelID() == chan){
        found = true;
      }  
    } // iterDigit
   ++iterC;
  } // iterC

  return found;
}

void STSelectFromCluster::updateCache(const int iTest){
 
  LHCb::STClusters::iterator iterC = m_cachedIter;
  while ((iterC != m_dataCont->end())&&((int)(*iterC)->channelID().uniqueSector() < iTest)){
    ++iterC;
  }  // iterC
  m_cachedIter = iterC;
  m_cachedSector = iTest;

}
