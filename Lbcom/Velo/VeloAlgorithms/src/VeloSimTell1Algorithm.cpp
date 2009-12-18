// $Id: VeloSimTell1Algorithm.cpp,v 1.3 2009-12-18 08:12:33 szumlat Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "VeloSimTell1Algorithm.h"

// standard lib
#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSimTell1Algorihtm
//
// 2009-08-02 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloSimTell1Algorithm );

using namespace boost::assign;
using namespace VeloTELL1;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSimTell1Algorithm::VeloSimTell1Algorithm( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_evtNumber ( 0 ),
    m_validationRun ( false ),
    m_dbConfig ( false ),
    m_inputDataLoc ( ),
    m_outputDataLoc ( ),
    m_algorithmName ( ),
    m_condPath ( "VeloCondDB" ),
    m_srcIdList ( ),
    m_dataBuffer(VeloTELL1::ALL_STRIPS),
    m_rawEvent ( 0 ),
    m_rawEventLoc ( LHCb::RawEventLocation::Default ),
    m_detElement ( 0 ),
    m_inputData ( 0 ),
    m_outputData ( 0 ),
    m_procInfos ( 0 ),
    m_procInfosLoc ( VeloProcessInfoLocation::Default ),
    m_algorithmType ( ),
    m_isEnable ( false ),
    m_isInitialized ( false )
{
  declareProperty("ForceEnable", m_forceEnable=false);
  declareProperty("ConvLimit", m_convergenceLimit=0);
}
//=============================================================================
// Destructor
//=============================================================================
VeloSimTell1Algorithm::~VeloSimTell1Algorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloSimTell1Algorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_detElement=getDet<DeVelo>(DeVeloLocation::Default);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSimTell1Algorithm::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSimTell1Algorithm::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::getData()
{
  debug()<< " ==> getData() " <<endmsg;
  //
  if(!exist<LHCb::VeloTELL1Datas>(m_inputDataLoc)){
    debug()<< " ==> There is no input data: "
           << m_inputDataLoc <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    // get data banks from default TES location
    m_inputData=get<LHCb::VeloTELL1Datas>(m_inputDataLoc);
    debug()<< " ==> The data have been read-in from location: "
           << m_inputDataLoc
           << ", size of the data container: "
           << m_inputData->size() <<endmsg;
    // vector for output data if output location defined
    if(!m_outputDataLoc.empty()){
      m_outputData=new LHCb::VeloTELL1Datas();
    }
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::INIT()
{
  debug()<< " ==> INIT() " <<endmsg;
  //
  if(!m_isInitialized){
    // build list of source IDs
    if(m_srcIdList.empty()){
      if ( !exist<LHCb::VeloTELL1Datas>(LHCb::VeloTELL1DataLocation::SimADCs) ) {
        debug() << " --> Input NZS data stream is EMPTY!" << endmsg;
        return StatusCode::FAILURE;
      }
      LHCb::VeloTELL1Datas* lDat=
        get<LHCb::VeloTELL1Datas>(LHCb::VeloTELL1DataLocation::SimADCs);
      LHCb::VeloTELL1Datas::const_iterator datIT=lDat->begin();
      for( ; datIT!=lDat->end(); ++datIT){
        const unsigned int tell1=static_cast<unsigned int>((*datIT)->key());
        m_srcIdList.push_back(tell1);
      }
    }

    bool bit=false;
    if(m_tell1Process==VeloTELL1::CLUSTER_MAKER){
      m_isInitialized=true;
      bit=true;
    }

    if(bit){
      setIsEnable(true);
      info()<< " ==> " << m_algorithmName << " Configured ";
      if(!m_algorithmType.empty()){
        info()<< " type: " << m_algorithmType <<endmsg;
      }else{
        info()<<endmsg;
      }
      return ( StatusCode::SUCCESS );
    }else{
      setIsEnable(false);
      info()<< " ==> " << m_algorithmName << " Configured (disabled)" <<endmsg;
      return ( StatusCode::SUCCESS );
    }
  }else{
    return ( StatusCode::SUCCESS );
  }
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::writeData()
{
  debug()<< " ==> writeData() " <<endmsg;
  //
  if(m_outputData!=0){
    put(m_outputData, m_outputDataLoc);
    return ( StatusCode::SUCCESS );
  }else{
    error()<< " ==> empty output container! " <<endmsg;
    return ( StatusCode::FAILURE );
  }
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::cloneData()
{
  debug()<< " ==> cloneData() " <<endmsg;
  // if CMS algorithm is disabled copy the data for next algo
  LHCb::VeloTELL1Datas::const_iterator sensIt=m_inputData->begin();
  for( ; sensIt!=m_inputData->end(); ++sensIt){
    const int sens=(*sensIt)->key();
    LHCb::VeloTELL1Data* out=new LHCb::VeloTELL1Data(sens, VeloTELL1::VeloFull);
    // check and propagate Reordering informations
    if((*sensIt)->isReordered()){
        this->propagateReorderingInfo(*sensIt, out);
    }
    VeloTELL1::sdataVec replica=(*sensIt)->data();
    out->setDecodedData(replica);
    m_outputData->insert(out);
  }
  StatusCode writer=writeData();
  return ( writer );
}
//=============================================================================
LHCb::VeloTELL1Datas* VeloSimTell1Algorithm::inputData() const
{
  debug()<< " ==> inputData() " <<endmsg;
  //
  return ( m_inputData );
}
//=============================================================================
void VeloSimTell1Algorithm::outputData(LHCb::VeloTELL1Data* inData)
{
  debug()<< " ==> inputData() " <<endmsg;
  //
  m_outputData->insert(inData);
}
//=============================================================================
LHCb::VeloTELL1Datas* VeloSimTell1Algorithm::outputData()
{
  return ( m_outputData );
}
//=============================================================================
void VeloSimTell1Algorithm::setInputDataLoc(const std::string inValue)
{
  m_inputDataLoc=inValue;
}
//=============================================================================
void VeloSimTell1Algorithm::setOutputDataLoc(const std::string inValue)
{
  m_outputDataLoc=inValue;
}
//=============================================================================
void VeloSimTell1Algorithm::setTELL1Process(const unsigned int inValue)
{
  m_tell1Process=inValue;
}
//=============================================================================
void VeloSimTell1Algorithm::setAlgoName(const std::string inValue)
{
  m_algorithmName=inValue;
}
//=============================================================================
const std::string VeloSimTell1Algorithm::algoName() const
{
  return ( m_algorithmName );
}
//=============================================================================
void VeloSimTell1Algorithm::setAlgoType(const std::string inValue)
{
  m_algorithmType=inValue;
}
//=============================================================================
const std::string VeloSimTell1Algorithm::algoType() const
{
  return ( m_algorithmType );
}
//=============================================================================
bool VeloSimTell1Algorithm::isEnable() const
{
  return ( m_isEnable );
}
//=============================================================================
void VeloSimTell1Algorithm::setIsEnable(bool flag)
{
   m_isEnable=flag;
}
//=============================================================================
unsigned int VeloSimTell1Algorithm::convergenceLimit() const
{
  return ( m_convergenceLimit );
}
//=============================================================================
bool VeloSimTell1Algorithm::isInitialized() const
{
  return ( m_isInitialized );
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::readDataFromFile(
                             const char* fileName, VeloTELL1::sdataVec& dataVec)
{
  debug()<< " readDataFromFile() " <<endmsg;
  //
  VeloTELL1::sdataVec localStorage;
  std::ifstream inData(fileName);
  signed int aValue=0;
  if(!inData){
    Error(" Can't open the specified file! ");
    info()<< " ===> " << fileName << " <=== " <<endmsg;
    Error(" Mind the input and output file paths ");
    return ( StatusCode::FAILURE );
  }else{
    info()<< " Reading the file " << fileName <<endmsg;
    for(int chan=0; chan<VeloTELL1::ALL_STRIPS; ++chan){
      inData>>aValue;
      localStorage.push_back(aValue);
    }
    inData.close();
  }
  //
  dataVec=localStorage;
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::writeDataToFile(
                             const char* fileName, VeloTELL1::sdataVec& dataVec)
{
  debug()<< " writeDataToFile() " <<endmsg;
  //
  std::ofstream outData(fileName);
  VeloTELL1::sdatIt datIT=dataVec.begin();
  if(outData.bad()){
    Error(" Cannot open file to write ");
    return ( StatusCode::FAILURE );
  }else{
    for( ; datIT!=dataVec.end(); ++datIT){
      outData<<(*datIT)<<std::endl;
    }
    outData.close();
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void VeloSimTell1Algorithm::prepareData(bool insertAtEnd=false)
{
  debug()<< " ==> prepareData() " <<endmsg;
  //
  VeloTELL1::sdataVec::iterator start, stop;
  LHCb::VeloTELL1Datas::const_iterator sensIt;
  sensIt=inputData()->begin();

  for( ; sensIt!=inputData()->end(); ++sensIt){

    const VeloTELL1::sdataVec& data=(*sensIt)->data();

    if(!insertAtEnd){

      for (unsigned int proc=0; proc<VeloTELL1::NumberOfPPFPGA; ++proc){

        start=m_dataBuffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
        std::copy(data.begin()+proc*STRIPS_IN_PPFPGA,
                  data.begin()+(proc+1)*STRIPS_IN_PPFPGA, start);
        start=m_dataBuffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
              STRIPS_IN_PPFPGA;
        stop=m_dataBuffer.begin()+(proc+1)*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
        std::fill(start, stop, 0);

      } 

    }else{

      std::copy(data.begin(), data.end(), m_dataBuffer.begin());
      std::fill(m_dataBuffer.begin()+NumberOfPPFPGA*STRIPS_IN_PPFPGA,
                m_dataBuffer.end(), 512);

    }

    // check if there is proper number of strips
    (*sensIt)->setDecodedData(m_dataBuffer);

  }

}
//=============================================================================
void VeloSimTell1Algorithm::prepareData(VeloTELL1::sdataVec& inVec) const
{
  debug()<< " ==> prepareData(sdataVec) " <<endmsg;
  //

  inVec.resize(inVec.size()+VeloTELL1::NumberOfPPFPGA*VeloTELL1::DUMMY_STRIPS);
  VeloTELL1::sdataVec::iterator start, stop, target;
  for (unsigned int proc=0; proc<VeloTELL1::NumberOfPPFPGA; ++proc) {
    target = inVec.begin() + (VeloTELL1::NumberOfPPFPGA-1-proc)*(VeloTELL1::STRIPS_IN_PPFPGA+VeloTELL1::DUMMY_STRIPS);
    start  = inVec.begin() + (VeloTELL1::NumberOfPPFPGA-1-proc)*VeloTELL1::STRIPS_IN_PPFPGA;
    stop   = start + VeloTELL1::STRIPS_IN_PPFPGA;
    std::copy(start,stop,target);
    start  = target + VeloTELL1::STRIPS_IN_PPFPGA;
    stop   = start + VeloTELL1::DUMMY_STRIPS;
    std::fill(start,stop,0);
  }  
}
//=============================================================================
DeVelo* VeloSimTell1Algorithm::deVelo() const
{
  debug()<< " ==> deVelo() "<<endmsg;
  return ( m_detElement );
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::inputStream(LHCb::VeloTELL1Datas* inVec) const
{
  debug()<< " ==> dataStatus() " <<endmsg;
  if(inVec->empty()){
    Error("Input stream is empty!");
    return ( StatusCode::FAILURE );
  }else{
    return ( StatusCode::SUCCESS);
  }
}
//=============================================================================
void VeloSimTell1Algorithm::propagateReorderingInfo(
     const LHCb::VeloTELL1Data* inObj, LHCb::VeloTELL1Data* outObj)
{
  debug()<< " ==> propagateReorderingInfo() " <<endmsg;
  //
  outObj->setIsReordered(true);
  outObj->setSensorType(inObj->sensorType());
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::i_cacheConditions()
{
  debug()<< " ==> cacheConditions() " <<endmsg;
  // do something interesting inside derived classes only
  return ( StatusCode::SUCCESS );
}
//=============================================================================
VeloTELL1::AListPair VeloSimTell1Algorithm::getSrcIdList() const
{
  debug()<< " ==> getSourceIds " <<endmsg;
  VeloTELL1::AListPair aPair;
  aPair.first=m_srcIdList.begin();
  aPair.second=m_srcIdList.end();
  //
  return ( aPair );
}
//=============================================================================
void VeloSimTell1Algorithm::setSrcIdList(std::vector<unsigned int> inVec)
{
  m_srcIdList=inVec;
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::i_cacheSrcIdList()
{
  debug()<< " --> getSrcIdListFromDB() " <<endmsg;
  std::vector<unsigned int> srcIdList;
  std::string tell1Path=m_condPath+"/Tell1Map";
  Condition* tell1Cond=getDet<Condition>(tell1Path);
  std::vector<int> tempList=tell1Cond->param<std::vector<int> >("tell1_map");
  if(tempList.empty())
    return ( Error(" --> Cannot retrieve Tell1 Map!", StatusCode::FAILURE) );
  std::vector<int>::const_iterator iT=tempList.begin();
  for( ; iT!=tempList.end(); ++iT){
    srcIdList.push_back(static_cast<unsigned int>(*iT));
  }
  this->setSrcIdList(srcIdList);
  //
  return ( StatusCode::SUCCESS );
}
//--
