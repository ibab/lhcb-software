// Include files 

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
DECLARE_ALGORITHM_FACTORY( VeloSimTell1Algorithm )

using namespace boost::assign;
using namespace VeloTELL1;

typedef std::pair<unsigned int, unsigned int> BOUNDARIES;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSimTell1Algorithm::VeloSimTell1Algorithm( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_evtNumber ( 0 ),
    m_dbConfig ( false ),
    m_inputDataLoc ( ),
    m_outputDataLoc ( ),
    m_algorithmName ( ),
    m_condPath ( "Conditions/HardwareProperties/Velo" ),
    m_srcIdList ( ),
    m_isDebug ( msgLevel(MSG::DEBUG) ),
    m_dataBuffer ( VeloTELL1::ALL_STRIPS ),
    m_thresholds ( VeloTELL1::ALL_STRIPS ),
    m_innerTh ( INNER_STR_ALL ),
    m_outerTh ( SENSOR_STRIPS - INNER_STR_ALL ),
    m_rawEventLoc ( LHCb::RawEventLocation::Default ),
    m_detElement ( 0 ),
    m_inputData ( 0 ),
    m_outputData ( 0 ),
    m_procInfosLoc ( VeloProcessInfoLocation::Default ),
    m_algorithmType ( ),
    m_tell1Process ( 0 ),
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

  if(m_isDebug) debug()<< "==> Initialize" <<endmsg;

  // build list of source IDs for all possible sensors (also PU)
  if(m_srcIdList.empty()){

    // -> build counters map for the full VELO
    for(unsigned int rsens=R_SENS_START; rsens<=R_SENS_END; ++rsens){

      m_srcIdList.push_back(rsens);

    }

    for(unsigned int psens=PHI_SENS_START; psens<=PHI_SENS_END; ++psens){

      m_srcIdList.push_back(psens);
 
    }

    for(unsigned int pusens=PU_SENS_START; pusens<=PU_SENS_END; ++pusens){

      m_srcIdList.push_back(pusens);

    }
      
  }

  m_detElement=getDet<DeVelo>(DeVeloLocation::Default);

  m_innerStrips+=INNER_STR_2, INNER_STR_2, INNER_STR_1, INNER_STR_2;
  m_outerStrips+=OUTER_STR_1, OUTER_STR_1, OUTER_STR_2, OUTER_STR_1;
  m_innerDummy+=INNER_DUMMY_1, INNER_DUMMY_1, INNER_DUMMY_2, INNER_DUMMY_1;
  m_outerDummy+=OUTER_DUMMY_2, OUTER_DUMMY_2, OUTER_DUMMY_1, OUTER_DUMMY_2; 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSimTell1Algorithm::execute() {

  if(m_isDebug) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSimTell1Algorithm::finalize() {

  if(m_isDebug) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::getData()
{
  if(m_isDebug) debug()<< " ==> getData() " <<endmsg;
  //
  m_inputData=getIfExists<LHCb::VeloTELL1Datas>(m_inputDataLoc);
  if( NULL == m_inputData ){
    if(m_isDebug) debug()<< " ==> There is no input data: "
           << m_inputDataLoc <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    // get data banks from default TES location
    if(m_isDebug) debug()<< " ==> The data have been read-in from location: "
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
  if(m_isDebug) debug()<< " ==> INIT() " <<endmsg;
  //
  if(!m_isInitialized){

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
  if(m_isDebug) debug()<< " ==> writeData() " <<endmsg;
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
  if(m_isDebug) debug()<< " ==> cloneData() " <<endmsg;
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
  if(m_isDebug) debug()<< " ==> inputData() " <<endmsg;
  //
  return ( m_inputData );
}
//=============================================================================
void VeloSimTell1Algorithm::outputData(LHCb::VeloTELL1Data* inData)
{
  if(m_isDebug) debug()<< " ==> inputData() " <<endmsg;
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
  if(m_isDebug) debug()<< " readDataFromFile() " <<endmsg;
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
  if(m_isDebug) debug()<< " writeDataToFile() " <<endmsg;
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
  if(m_isDebug) debug()<< " ==> prepareData() " <<endmsg;
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
  if(m_isDebug) debug()<< " ==> prepareData(sdataVec) " <<endmsg;
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
  if(m_isDebug) debug()<< " ==> deVelo() "<<endmsg;
  return ( m_detElement );
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::inputStream(LHCb::VeloTELL1Datas* inVec) const
{
  if(m_isDebug) debug()<< " ==> dataStatus() " <<endmsg;
  if(inVec->empty()){
    if(m_isDebug) debug() << "Input stream is empty!" << endmsg;
    return StatusCode::FAILURE; // no data but this is not always an error
  }else{
    return ( StatusCode::SUCCESS);
  }
}
//=============================================================================
void VeloSimTell1Algorithm::propagateReorderingInfo(
     const LHCb::VeloTELL1Data* inObj, LHCb::VeloTELL1Data* outObj)
{
  if(m_isDebug) debug()<< " ==> propagateReorderingInfo() " <<endmsg;
  //
  outObj->setIsReordered(true);
  outObj->setSensorType(inObj->sensorType());
}
//=============================================================================
StatusCode VeloSimTell1Algorithm::i_cacheConditions()
{
  if(m_isDebug) debug()<< " ==> cacheConditions() " <<endmsg;
  // do something interesting inside derived classes only
  return ( StatusCode::SUCCESS );
}
//=============================================================================
VeloTELL1::AListPair VeloSimTell1Algorithm::getSrcIdList() const
{
  if(m_isDebug) debug()<< " ==> getSourceIds " <<endmsg;
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
BOUNDARIES VeloSimTell1Algorithm::findBoundary(const unsigned int proc,
                                               const TH_CONT& cont)
{
    if(m_isDebug) debug()<< " --> findStartOfSector()" <<endmsg;
    //
    unsigned int pos_1=0, pos_2=0;
   
    if(0==proc){
      pos_2+=cont[proc];
    }else{
      for(unsigned int pos=0; pos<proc; ++pos){
        pos_1+=cont[pos];
      }
      for(unsigned int pos=0; pos<proc+1; ++pos){
        pos_2+=cont[pos];
      }
    }

    if(m_isDebug) debug()<< " pos1: " << pos_1 << ", pos2" << pos_2 <<endmsg;

    BOUNDARIES bound=std::make_pair(pos_1, pos_2);
     
    return ( bound );

}
//=============================================================================
void VeloSimTell1Algorithm::addProcThresholds(const sdataVec& inData,
                                              const bool rtype)
{
  if(m_isDebug) debug()<< " ==> orderStrips() " <<endmsg;

  /*
      Add processing thresholds - the one used for the dummy strips
      Note! - type means sensor type, R=0, Phi=1
      Note! - for the processing strips use max threshold = 127
   */

  sdataVec::iterator start, stop;

  if(rtype){    

    for(unsigned int proc=0; proc<NumberOfPPFPGA; ++proc){

      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      std::copy(inData.begin()+proc*STRIPS_IN_PPFPGA,
                inData.begin()+(proc+1)*STRIPS_IN_PPFPGA, start);
      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
        STRIPS_IN_PPFPGA;
      stop=m_thresholds.begin()+(proc+1)*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      std::fill(start, stop, 127);

    }

  }else{

    // divide the input strips into inner and outer respectively
    start=m_innerTh.begin();
    std::copy(inData.begin(), inData.begin()+INNER_STR_ALL, start);
    start=m_outerTh.begin();
    std::copy(inData.begin()+INNER_STR_ALL, inData.end(), start);
   
    // build topologically reordered Phi strips container
    BOUNDARIES beg_end;

    for(unsigned int proc=0; proc<NumberOfPPFPGA; ++proc){
     
      // -> inner strips
      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      beg_end=findBoundary(proc, m_innerStrips);
      std::copy(m_innerTh.begin()+beg_end.first,
                m_innerTh.begin()+beg_end.second, start);
      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc];
      stop=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
           m_innerStrips[proc]+m_innerDummy[proc];
      std::fill(start, stop, 127);

      // -> outer strips
      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc];
      beg_end=findBoundary(proc, m_outerStrips);
      std::copy(m_outerTh.begin()+beg_end.first,
                m_outerTh.begin()+beg_end.second, start);
      start=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc]+m_outerStrips[proc];
      stop=m_thresholds.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc]+m_outerStrips[proc]+
            m_outerDummy[proc];
      std::fill(start, stop, 127);
    
   }
    
  }

  //
  return;

}
//--
