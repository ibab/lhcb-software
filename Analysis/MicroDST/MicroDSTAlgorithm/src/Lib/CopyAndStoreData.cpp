// $Id: CopyAndStoreData.cpp,v 1.3 2007-10-15 13:54:39 jpalac Exp $
// Include files 

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/KeyedContainer.h>
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
  // Remove any leading "/Event" in input TES location.
  getNiceLocationName( this->inputTESLocation() );
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
StatusCode CopyAndStoreData::copyAndStoreObject( const std::string& from,
                                                 const std::string& to    )

{
  verbose() << "try to get data container" << endmsg;

  if (exist<T>( from ) ) {
    const T* data = get<T>( from );
    verbose() << "now copy information" << endmsg;
    T* newData = new T(*data);
    put (newData, to );
    verbose() << "Data values set to\n" << *newData << "\n" << endmsg;
  } else {
    Warning("No data container found at "+ from, StatusCode::SUCCESS);
  } // if exist

  return StatusCode::SUCCESS;
}
//=============================================================================
template <class T>
StatusCode CopyAndStoreData::copyAndStoreContainer( const std::string& from,
                                                    const std::string& to   )
{
  debug() << "now store container for location " << from << endmsg;

  if (!exist<T>(from)) {
    debug() << "Container location does not exist" << endmsg;
    return StatusCode::FAILURE;    
  } else {
    const T* data = get<T>( from );
    // do stuff here!
    if (!data) {
      return StatusCode::FAILURE;
    }
    verbose() << "got # elements in container: " << data->size() << endmsg;
    T* clones = getOutputContainer<T>();
    return cloneContainer<T>(data, clones);
  } // if !exist
}
//=============================================================================
template <class T> 
StatusCode CopyAndStoreData::cloneContainer(const T* data, T* clones) 
{

  for (typename T::const_iterator i = data->begin(); 
       i != data->end(); 
       ++i) {
    typename T::value_type item = clones->object( (*i)->key() );
    if (!item) {
      item = (*i)->clone();
      clones->insert(item, (*i)->key());
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
template<class T> 
T* CopyAndStoreData::getOutputContainer()
{
  const std::string& location = fullOutputTESLocation();
  
  if ( !exist<T>( location ) ) {
    T* container = new T();
    put(container, location);
  }

  return get<T>( location );      
  
}
//=============================================================================
void CopyAndStoreData::getNiceLocationName(std::string& location)
{
  const std::string tmpString = "/Event/";
  const std::string::size_type loc       = location.find(tmpString);
  if ( loc != std::string::npos) {
    location.replace(loc, tmpString.length(), "");
    verbose() << "TES location ID is now " << location << endmsg;
  }
}
//=============================================================================
