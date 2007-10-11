// $Id: CopyAndStoreData.cpp,v 1.1 2007-10-11 17:56:56 jpalac Exp $
// Include files 

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/KeyedContainer.h>
// fromLHCb
//#include "Event/ODIN.h"
// local
#include "MicroDST/CopyAndStoreData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyAndStoreData
//
// 2007-10-11 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyAndStoreData::CopyAndStoreData( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  //  m_inputTESLocation(LHCb::ODINLocation::Default),
  m_inputTESLocation(""),
  m_outputPrefix("microDST")
{
  declareProperty( "InputLocation", m_inputTESLocation );
  declareProperty( "OutputPrefix",  m_outputPrefix  );
}
//=============================================================================
// Destructor
//=============================================================================
CopyAndStoreData::~CopyAndStoreData() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyAndStoreData::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  this->fullOutputTESLocation() = "/Event/"+ this->outputPrefix() + 
    "/" + this->inputTESLocation();

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CopyAndStoreData::execute() {

  debug() << "==> Execute" << endmsg;

  //  return copyAndStoreData<LHCb::ODIN>( inputTESLocation(),
  //                                       fullOutputTESLocation() );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyAndStoreData::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
template <class T>
StatusCode CopyAndStoreData::copyAndStoreData( const std::string& fromLocation,
                                               const std::string& toLocation   )

{
  verbose() << "try to get data container" << endmsg;

  if (exist<T>( fromLocation ) ) {
    const T* data = get<T>( fromLocation );
    verbose() << "now copy information" << endmsg;
    T* newData = new T(*data);
    put (newData, toLocation );
    verbose() << "Data values set to\n" << *newData << "\n" << endmsg;
  } else {
    Warning("No data container found at "+ fromLocation, 
            StatusCode::SUCCESS);
  } // if exist

  return StatusCode::SUCCESS;
}
//=============================================================================
template<class T> 
T* CopyAndStoreData::getContainer(std::string locTES)
{
  
  // do some string manipulaitons
  getNiceLocationName(locTES);
  // Check if the container is already there
  if ( containerMap().find(locTES) !=  containerMap().end() ) {
    verbose() << "return existing container" << endmsg;
    return dynamic_cast<T*>(containerMap()[locTES]);
  } else {
    verbose() << "container does not exisit yet, create" << endmsg;
    std::string containerLocation =  locTES;
    // now insert identifier for microDST after "/Event/"
    containerLocation.insert(0,"/Event/"+ outputPrefix() + "/");
    verbose() << "location of container in TES " 
              << containerLocation << endmsg;
    // create new container and store into TES
    T* container = new T();
    put (container, containerLocation);     
    containerMap()[locTES] = dynamic_cast<ObjectContainerBase*>(container);
    return dynamic_cast<T*>( containerMap()[locTES] );
  }
  

}
//=============================================================================
void CopyAndStoreData::getNiceLocationName(std::string& location)
{
  const std::string tmpString = "/Event/";
  const std::string::size_type loc       = location.find(tmpString);
  if ( loc != std::string::npos) {
    location.replace(loc, tmpString.length(), "");
    verbose() << "TES location ID is now " << location << endmsg;
  } //if loc
}
//=============================================================================
