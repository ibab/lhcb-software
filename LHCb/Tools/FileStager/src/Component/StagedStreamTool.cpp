// $Id:
// Include files
#include <iostream>

// STL
#include <vector>

// boost
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

// from Gaudi
#include <GaudiKernel/SmartIF.h>
#include <GaudiKernel/ISvcLocator.h>
#include <GaudiKernel/DataStreamTool.h>
#include <GaudiKernel/EventSelectorDataStream.h>
#include <GaudiUtils/IFileCatalog.h>

// FileStager
#include <FileStager/IFileStagerSvc.h>

// local
#include "StagedStreamTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StagedStreamTool
//
// 2006-09-21 : Andres Felipe Osorio Oliveros
//-----------------------------------------------------------------------------

using std::string;
using std::vector;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StagedStreamTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StagedStreamTool::StagedStreamTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
: DataStreamTool( type, name , parent )
{
   m_stagerSvc = 0;
   m_catalog = 0;

   m_regex.assign("(?:DATA(?:FILE)?)='((?:((?:PFN)|(?:LFN)):)((?:\\w+:)*/{0,2}[a-zA-Z0-9_/:?&\\.=\\-+]+))'");
   declareProperty( "CatalogSvc", m_catalogSvc = "Gaudi::MultiFileCatalog/FileCatalog" );
}

//=============================================================================
// Destructor
//=============================================================================
StagedStreamTool::~StagedStreamTool()
{

}

//=============================================================================
StatusCode StagedStreamTool::initialize()
{
   MsgStream logger( msgSvc(), name() );

   StatusCode status = DataStreamTool::initialize();
   if( !status.isSuccess() )  {
      logger << MSG::FATAL << "Error. Cannot initialize base class." << endmsg;
      return status;
   }

   // Get a reference to the FileStagerSvc
   m_stagerSvc = serviceLocator()->service("FileStagerSvc");
   if( !m_stagerSvc.isValid() )  {
      logger << MSG::FATAL << "Error retrieving FileStagerSvc." << endmsg;
      return StatusCode::FAILURE;
   }

   // Get a reference to the IFileCatalog
   m_catalog = serviceLocator()->service( m_catalogSvc );
   if( !m_catalog.isValid() )  {
      logger << MSG::FATAL << "Error retrieving Catalog." << endmsg;
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode StagedStreamTool::addStreams(const StreamSpecs & inputs) {

   StatusCode status = StatusCode::SUCCESS;
   MsgStream logger( msgSvc(), name() );

   BOOST_FOREACH( const string& input, inputs ) {
      status = addStream( input );
      if ( !status.isSuccess() ) break;
   }

   vector< Descriptor > descriptors;
   BOOST_FOREACH( const std::string& descriptor, inputs ) {
      try {
         Descriptor d = extractFilename( descriptor );
         descriptors.push_back( d );
      } catch ( const std::exception& ) {
         return StatusCode::FAILURE;
      }
   }
   
   vector< string > files;
   BOOST_FOREACH( const Descriptor& descriptor, descriptors ) {
      if ( descriptor.type() == LFN ) {
         string file = descriptor.descriptor();
         string lfn = descriptor.descriptor().substr( 4 );
         const string& fid = m_catalog->lookupLFN( lfn );
         if ( !fid.empty() ) {
            // the lfn is in the catalog
            Gaudi::IFileCatalog::Files entries;
            m_catalog->getPFN( fid, entries );
            if ( !entries.empty() ) {
               // We'll stage the PFN
               file = entries[ 0 ].first;
            } 
         }
         logger << MSG::DEBUG << "Adding " << file << " to files to be staged " 
                << " for lfn: " << lfn << endmsg;
         files.push_back( file );
      } else {
         // Try with whatever we have
         logger << MSG::DEBUG << "Adding: " << descriptor.descriptor() 
                << " to the list of files to be staged." << endmsg;
         files.push_back( descriptor.descriptor() );
      }
   }

   if ( status.isSuccess() ) {
      status = m_stagerSvc->addFiles( files );
   }

   return status;
}

//=============================================================================
StatusCode StagedStreamTool::clear()
{
   StatusCode sc = m_stagerSvc->clearFiles();
   if ( sc.isSuccess() ) {
      return DataStreamTool::clear();
   } else {
      return sc;
   }
}

//=============================================================================
StagedStreamTool::Descriptor
StagedStreamTool::extractFilename( const std::string& descriptor )
{
   MsgStream logger( msgSvc(), name() );

   boost::smatch match;
   boost::match_flag_type flags = boost::match_default;
   if ( boost::regex_search( descriptor.begin(), descriptor.end(), match, m_regex, flags ) ) {
      if ( match.size() >= 1 ) {
         if ( match.str( 2 ) == "PFN" ) {
            return Descriptor( PFN, match.str( 1 ) );
         } else if ( match.str( 2 ) == "LFN" ) {
            return Descriptor( LFN, match.str( 1 ) );
         }
      } else {
         logger << MSG::ERROR << "could not extract filename from descriptor: "
                << descriptor << endmsg;
         throw std::exception();
      }
   } else {
      logger << MSG::ERROR << "bad descriptor: " << descriptor << endmsg;
      throw std::exception();
   }
   // To get rid of warning, will never be reached.
   return Descriptor( NONE, "" );
}
