// Include files
// from STL
#include <vector>
#include <algorithm>

// local
#include "VeloTell1DataProcessor.h"

// TELL1 Kernel
#include "compareSensor.h"
#include "Digitalizer.h"

//boost
#include <boost/assign/std/vector.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloTell1DataProcessor
//
// 2006-05-25 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloTell1DataProcessor )

using namespace VeloTELL1;
using namespace LHCb;
using namespace boost::assign;

typedef std::pair<unsigned int, unsigned int> BOUNDARIES;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloTell1DataProcessor::VeloTell1DataProcessor( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fesContainer ( LHCb::MCVeloFELocation::Default ),
    m_fes ( 0 ),
    m_simADCs ( VeloTELL1DataLocation::SimADCs ),
    m_adcs ( 0 ),
    m_buffer ( ALL_STRIPS ),
    m_isDebug ( msgLevel( MSG::DEBUG ) ),
    m_innerBuffer ( INNER_STR_ALL ),
    m_outerBuffer ( SENSOR_STRIPS - INNER_STR_ALL ),
    m_innerStrips ( ),
    m_outerStrips ( ),
    m_innerDummy ( ),
    m_outerDummy ( )
{
  // make the ADC in e- = (ADC_max)/(e_max) parameters tunable
  // default is set to e- per ADC = 601. tuned to 2009 data
  declareProperty("ElectronsFullScale", m_elScale = 76950.);
  declareProperty("ADCsFullScale", m_adcScale = 128 );
}
//=============================================================================
// Destructor
//=============================================================================
VeloTell1DataProcessor::~VeloTell1DataProcessor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloTell1DataProcessor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(m_isDebug) debug() << "==> Initialize" << endmsg;

  m_innerStrips+=INNER_STR_2, INNER_STR_2, INNER_STR_1, INNER_STR_2;
  m_outerStrips+=OUTER_STR_1, OUTER_STR_1, OUTER_STR_2, OUTER_STR_1;
  m_innerDummy+=INNER_DUMMY_1, INNER_DUMMY_1, INNER_DUMMY_2, INNER_DUMMY_1;
  m_outerDummy+=OUTER_DUMMY_2, OUTER_DUMMY_2, OUTER_DUMMY_1, OUTER_DUMMY_2;  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloTell1DataProcessor::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  //
  if(getData()){
    writeDataForSensors();
    //if(m_isDebug) checkStripsOrdering(15);
    writeOutput();
  }
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloTell1DataProcessor::finalize() {

  if(m_isDebug) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloTell1DataProcessor::getData()
{
  if(m_isDebug) debug()<< " ==> getData() " <<endmsg;
  // get simulated front end signals
  m_fes=getIfExists<LHCb::MCVeloFEs>(m_fesContainer);
  if( NULL == m_fes ){
    info()<< " ==> There is no MCVeloFE at: "
           << m_fesContainer <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    if(m_isDebug)
           debug()<< " ==> The simulated FEs have been read-in from: "
           << m_fesContainer
           << ", size of the FEs container: "
           << m_fes->size() <<endmsg;
    m_adcs=new VeloTELL1Datas();
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void VeloTell1DataProcessor::writeDataForSensors()
{
  if(m_isDebug) debug()<< " ==> writeDataForSensors() " <<endmsg;
  //
  feIt firstOnCurrentSens, firstOnNextSens;
  feIt iT;
  int currentSens=-999;
  bool rType=false;
  //
  for(iT=m_fes->begin(); iT!=m_fes->end(); ++iT){
    firstOnCurrentSens=iT;
    // get sensor number -> use ConditionsDB
    // to translate it to TELL1 number
    currentSens=(*firstOnCurrentSens)->sensor();
    rType=((*firstOnCurrentSens)->channelID().isRType())||
          ((*firstOnCurrentSens)->channelID().isPileUp());
    sdataVec adcCharge, pedCharge;
    VeloTELL1Data* adcs=new VeloTELL1Data(currentSens, VeloFull);
    // find first fe object on the next sensor
    firstOnNextSens=std::find_if(iT, m_fes->end(),
                    compareSensor<LHCb::MCVeloFE*>(currentSens));
    if(m_isDebug) debug()<< " found: " 
          << distance(firstOnCurrentSens, firstOnNextSens) 
          << " VeloMCFE objects on sensor: " << currentSens <<endmsg;
    sdataVec data(2048, 0);
    unsigned int strip=0;
    signed int charge=0;
    for( ; iT!=firstOnNextSens; ++iT){
      //adcCharge.push_back(static_cast<signed int>((*iT)->charge()));
      strip=static_cast<unsigned int>((*iT)->strip());
      charge=static_cast<signed int>((*iT)->charge());
      if(m_isDebug) debug()<< " --> found MCVeloFE object: "
                           << "     strip: " << strip << ", charge: "
                           << charge <<endmsg;
      data[strip]=charge;
    }
    // write adc and pedestal data, prior to this digitize them
    std::transform(data.begin(), data.end(), data.begin(),
                   Digitalizer<signed int>(m_adcScale, m_elScale));
    rType ? adcs->setSensorType(R_SENSOR) : adcs->setSensorType(PHI_SENSOR);
    orderStripsTopo(data, rType);
    adcs->setDecodedData(m_buffer);
    adcs->setIsReordered(true);
    m_adcs->insert(adcs);
    iT--;
  }
  //
  return;
}
//=============================================================================
void VeloTell1DataProcessor::writeOutput()
{
  if(m_isDebug) debug()<< " ==> writeOutput() " <<endmsg;
  //
  put(m_adcs, m_simADCs);
}
//=============================================================================
void VeloTell1DataProcessor::orderStripsTopo(const sdataVec& inData,
                                             const bool type)
{
  if(m_isDebug) debug()<< " ==> orderStrips() " <<endmsg;

  /*
     -> here the type argument may be a bit misleading, actually
        the type is true when we work with a R sensor and false
        when we process a Phi sensor
   */

  sdataVec::iterator start, stop;

  if(type){    

    for(unsigned int proc=0; proc<NumberOfPPFPGA; ++proc){

      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      std::copy(inData.begin()+proc*STRIPS_IN_PPFPGA,
                inData.begin()+(proc+1)*STRIPS_IN_PPFPGA, start);
      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
        STRIPS_IN_PPFPGA;
      stop=m_buffer.begin()+(proc+1)*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      std::fill(start, stop, 0);

    }

  }else{
    
    // divide the input strips into inner and outer respectively
    start=m_innerBuffer.begin();
    std::copy(inData.begin(), inData.begin()+INNER_STR_ALL, start);
    start=m_outerBuffer.begin();
    std::copy(inData.begin()+INNER_STR_ALL, inData.end(), start);
    
    // build topologically reordered Phi strips container
    BOUNDARIES beg_end;

    for(unsigned int proc=0; proc<NumberOfPPFPGA; ++proc){
      
      // -> inner strips
      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS);
      beg_end=findBoundary(proc, m_innerStrips);
      std::copy(m_innerBuffer.begin()+beg_end.first,
                m_innerBuffer.begin()+beg_end.second, start);
      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc];
      stop=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
           m_innerStrips[proc]+m_innerDummy[proc];
      std::fill(start, stop, 0);

      // -> outer strips
      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc];
      beg_end=findBoundary(proc, m_outerStrips);
      std::copy(m_outerBuffer.begin()+beg_end.first,
                m_outerBuffer.begin()+beg_end.second, start);
      start=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc]+m_outerStrips[proc];
      stop=m_buffer.begin()+proc*(STRIPS_IN_PPFPGA+DUMMY_STRIPS)+
            m_innerStrips[proc]+m_innerDummy[proc]+m_outerStrips[proc]+
            m_outerDummy[proc];
      std::fill(start, stop, 0);

    }

  }
  //
  return;

}
//=============================================================================
void VeloTell1DataProcessor::checkStripsOrdering(const unsigned int sens)
{
  if(m_isDebug) debug()<< " --> checkStripsOrdering()" <<endmsg;
  //
  info()<< " --> NZS data stream size: " << (m_adcs->size()) <<endmsg;
  LHCb::VeloTELL1Data* check=m_adcs->object(sens);

  if(NULL!=check){
    int stripADC=0;

    for(unsigned int strip=0; strip<2048; ++strip){

      stripADC=check->stripADC(strip);
      
      if(stripADC>1) info()<< " -> digitized charge: " << stripADC
                        << " at strip: " << strip <<endmsg;
    }   
    
  }

  return;

} 
//=============================================================================
BOUNDARIES VeloTell1DataProcessor::findBoundary(const unsigned int proc,
                                                const TOPO_CONT& cont)
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
//
