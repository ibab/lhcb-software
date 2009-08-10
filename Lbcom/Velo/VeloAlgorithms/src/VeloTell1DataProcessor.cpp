// $Id: VeloTell1DataProcessor.cpp,v 1.1 2009-08-10 08:48:02 szumlat Exp $
// Include files
// from STL
#include <vector>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "VeloTell1DataProcessor.h"

// TELL1 Kernel
#include "compareSensor.h"
#include "Digitalizer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloTell1DataProcessor
//
// 2006-05-25 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloTell1DataProcessor );

using namespace VeloTELL1;
using namespace LHCb;

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
    m_adcScale ( 1024 ),
    m_elScale ( 800000. )
{ }
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

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloTell1DataProcessor::execute() {
  debug() << "==> Execute" << endmsg;
  //
  if(getData()){
    writeDataForSensors();
    writeOutput();
  }
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloTell1DataProcessor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloTell1DataProcessor::getData()
{
  debug()<< " ==> getData() " <<endmsg;
  // get simulated front end signals
  if(!exist<LHCb::MCVeloFEs>(m_fesContainer)){
    info()<< " ==> There is no MCVeloFE at: "
           << m_fesContainer <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    // get data banks from default TES location
    m_fes=get<LHCb::MCVeloFEs>(m_fesContainer);
    debug()<< " ==> The simulated FEs have been read-in from location: "
           << m_fesContainer
           << ", size of the FEs container: "
           << m_fes->size() <<endmsg;
    m_adcs=new VeloTELL1Datas();
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloTell1DataProcessor::writeDataForSensors()
{
  debug()<< " ==> writeDataForSensors() " <<endmsg;
  //
  feIt firstOnCurrentSens, firstOnNextSens;
  feIt iT;
  int currentSens;
  //
  for(iT=m_fes->begin(); iT!=m_fes->end(); ++iT){
    firstOnCurrentSens=iT;
    // get sensor number -> use ConditionsDB
    // to translate it to TELL1 number
    currentSens=(*firstOnCurrentSens)->sensor();
    sdataVec adcCharge, pedCharge;
    VeloTELL1Data* adcs=new VeloTELL1Data(currentSens, VeloFull);
    // find first fe object on the next sensor
    firstOnNextSens=std::find_if(iT, m_fes->end(),
                    compareSensor<LHCb::MCVeloFE*>(currentSens));
    debug()<< " channels number (should be 2048 for NZS data): " 
          << distance(firstOnCurrentSens, firstOnNextSens) 
          << ", " << currentSens <<endmsg;
    for( ; iT!=firstOnNextSens; ++iT){
      adcCharge.push_back(static_cast<signed int>((*iT)->charge()));
    }
    // write adc and pedestal data, prior to this digitize them
    std::transform(adcCharge.begin(), adcCharge.end(),
                   adcCharge.begin(),
                   Digitalizer<signed int>(m_adcScale, m_elScale));
    debug()<< " ped value: " << (*(pedCharge.begin())) <<endmsg;
    adcs->setDecodedData(adcCharge);
    m_adcs->insert(adcs);
    iT--;
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void VeloTell1DataProcessor::writeOutput()
{
  debug()<< " ==> writeOutput() " <<endmsg;
  //
  put(m_adcs, m_simADCs);
}
//
