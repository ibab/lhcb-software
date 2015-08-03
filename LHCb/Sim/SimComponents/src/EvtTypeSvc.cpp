// Include files 

// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"


// local
#include "EvtTypeSvc.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : EvtTypeSvc
//
// 2004-06-24 : Gloria CORTI
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(EvtTypeSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtTypeSvc::EvtTypeSvc( const std::string& name, ISvcLocator* svc ) 
  : Service( name, svc )
{
  //m_mcFinder=NULL; //don't make it unless needed
  
  // Default file to parse
  if( getenv("DECFILESROOT") != NULL ) {
    m_inputFile = std::string(getenv( "DECFILESROOT" )) +
      std::string( "/doc/table_event.txt");
  }
  else {
    m_inputFile = "";
  }
 
  declareProperty( "EvtTypesFile", m_inputFile );
  
}
//=============================================================================
// Destructor
//=============================================================================
EvtTypeSvc::~EvtTypeSvc() {}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode EvtTypeSvc::queryInterface( const InterfaceID& riid, 
                                       void** ppvInterface ) 
{
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( riid == IID_IEvtTypeSvc ) {
      *ppvInterface = static_cast<IEvtTypeSvc*>(this);
      sc = StatusCode::SUCCESS;
      addRef();
    }
    else
      sc = Service::queryInterface( riid, ppvInterface );    
  }
  return sc;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode EvtTypeSvc::initialize() 
{

  MsgStream msg( msgSvc(), name() );

  StatusCode status = Service::initialize();
  if( status.isFailure() ) return status;

  // Check if the input file is set
  if( "" == inputFile() ) {
    msg << MSG::FATAL
        << "==> You MUST set the input file containing the event type"
        << endmsg
        << "    the default is set to $DECFILESROOT/doc/table_event.txt"
        << endmsg
        << "    check if $DECFILESROOT is set"
        << endmsg;
    return StatusCode::FAILURE;
  }

  
  return parseFile( inputFile() );
  
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode EvtTypeSvc::finalize() 
{

  MsgStream msg( msgSvc(), name() );
  if( msg.level() <= MSG::DEBUG ) {
    msg << MSG::DEBUG << "==> Finalize" << endmsg;
    msg << MSG::DEBUG << "Table size before clean up" << m_evtTypeInfos.size()
        << endmsg;
  }
  
  // Clean up list of evttypes
  while( !m_evtTypeInfos.empty() ) {
    EvtTypeInfo* anEvtType = m_evtTypeInfos.back();
    m_evtTypeInfos.pop_back();
    delete anEvtType;
  }
  
  if( msg.level() <= MSG::DEBUG )
    msg << MSG::DEBUG << "Table size after clean up" << m_evtTypeInfos.size()
        << endmsg;
  
  return Service::finalize();
  
}


//=============================================================================
// Read data relative to event type from ASCII file
//=============================================================================
StatusCode EvtTypeSvc::parseFile( const std::string input ) 
{

  MsgStream msg( msgSvc(), name() );
  
  std::ifstream infile( input.c_str() );
  if ( !infile ) {
    msg << MSG::FATAL 
        << "File with event types information cannot be opened :" << input 
        << endmsg;
    return StatusCode::FAILURE;
  }
  msg << MSG::INFO 
      << "Opened file with the event types information : " << inputFile()
      << endmsg;

  std::string token = " | ";
  long evtCode;
  std::string line, sEvtCode, subLine, evtNick, evtDesc;
  std::string::size_type index0, index1;

  // Ignore the first line, since it is an header
  getline( infile, line, '\n' );

  // Now get all remaining lines in the file
  unsigned int nlines = 0;
  while( getline( infile, line, '\n' ) ) {

    nlines++;
    
    index0 = line.find_first_of(token);   
    if( (std::string::npos == index0) || (0 == index0) ) {       
      msg << MSG::ERROR << "Unexpected format" << endmsg;
      return StatusCode::FAILURE;
    }

    // This is the integer code
    sEvtCode = line.substr(0,index0);
    evtCode = atoi(sEvtCode.c_str());
    // Now get the nick name
    index0 = index0+token.size();
    index1 = line.find_first_of(token,index0);
    if( (std::string::npos == index1) || (0 == index1) ) {       
      msg << MSG::ERROR << "Unexpected format" << endmsg;
      return StatusCode::FAILURE;
    }
    evtNick = line.substr(index0,index1-index0);
    index1 = index1+token.size();
    evtDesc = line.substr(index1);
    EvtTypeInfo* entry = new EvtTypeInfo( evtCode, evtNick, evtDesc );
    m_evtTypeInfos.push_back(entry);
  }

  if( msg.level() <= MSG::DEBUG ) {
    msg << MSG::DEBUG << "Read   " << nlines << " entries in file " << endmsg;
    msg << MSG::DEBUG << "Stored " << m_evtTypeInfos.size() 
      << " entries in table" << endmsg;
    if( msg.level() <= MSG::VERBOSE ) {
      msg << MSG::VERBOSE 
          << "List of entries: EvtType, NickName, DecayDescriptor"
          << std::endl;
      for( EvtTypeInfos::const_iterator iEntry = m_evtTypeInfos.begin();
           m_evtTypeInfos.end() != iEntry; ++iEntry ) {
        msg << MSG::VERBOSE << (*iEntry)->evtCode() << token
            << (*iEntry)->nickName() << token
            << (*iEntry)->decayDescriptor() << std::endl;
      }
      msg << MSG::VERBOSE << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Given the integer code of an Event type checks that it exists in the
// list of used values 
//=============================================================================
bool EvtTypeSvc::typeExists( const int evtCode ) {

  EvtTypeInfos::const_iterator iEvtTypeInfo;
  iEvtTypeInfo = std::find_if( m_evtTypeInfos.begin(), 
                               m_evtTypeInfos.end(), 
                               EvtCodeEqual(evtCode) );
  if( m_evtTypeInfos.end() == iEvtTypeInfo ) {
    return false;
  }
  return true;

}

//=============================================================================
// Given the integer code of an Event type provides its Nick Name  
//=============================================================================
std::string EvtTypeSvc::nickName( const int evtCode ) {
  
  EvtTypeInfos::const_iterator iEvtTypeInfo;
  iEvtTypeInfo = std::find_if( m_evtTypeInfos.begin(), 
                               m_evtTypeInfos.end(), 
                               EvtCodeEqual(evtCode) );
  if( m_evtTypeInfos.end() == iEvtTypeInfo ) 
    {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::WARNING << evtCode << "not known" << endmsg;
      return "";
    }
  return (*iEvtTypeInfo)->nickName();
  
}  
 
//=============================================================================
// Given an EventType provide its ASCII decay descriptor
//=============================================================================
std::string EvtTypeSvc::decayDescriptor( const int evtCode ) {

  EvtTypeInfos::iterator iEvtTypeInfo;
  iEvtTypeInfo = std::find_if( m_evtTypeInfos.begin(), 
                               m_evtTypeInfos.end(), 
                               EvtCodeEqual(evtCode) );
  if( m_evtTypeInfos.end() == iEvtTypeInfo ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << evtCode << "not known" << endmsg;
    return "";
  }
  return (*iEvtTypeInfo)->decayDescriptor();

}


//=============================================================================
// Returns a LHCb::EventTypeSet of all known types
// this is a typedef of std::set<long unsigned int, std::greater<long unsigned int> >
// an ordered, unique, list
//=============================================================================
LHCb::EventTypeSet EvtTypeSvc::allTypes( void )
{

  LHCb::EventTypeSet types;

  for(EvtTypeInfos::const_iterator iEvtTypeInfo=m_evtTypeInfos.begin(); iEvtTypeInfo!=m_evtTypeInfos.end(); iEvtTypeInfo++)
    types.insert((*iEvtTypeInfo)->evtCode());
  
  return types;

}
