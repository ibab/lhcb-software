#include "ConfigZipFileAccessSvc.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>


#include "GaudiKernel/System.h"

#include "zip.h"

using namespace std;

using ConfigZipFileAccessSvc_details::ZipFile;

// Factory implementation
DECLARE_COMPONENT( ConfigZipFileAccessSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigZipFileAccessSvc::ConfigZipFileAccessSvc( const string& name,
                                                ISvcLocator* pSvcLocator )
    : ConfigArchiveAccessSvc( name, pSvcLocator ), m_file{ nullptr }
{
    declareProperty( "File", m_name = "" );
    declareProperty( "Mode", m_mode = "ReadOnly" );
}

IArchive* ConfigZipFileAccessSvc::file() const
{
    if ( !m_file ) {
        if ( m_mode != "ReadOnly" && m_mode != "ReadWrite" &&
             m_mode != "Truncate" ) {
            error() << "invalid mode: " << m_mode << endmsg;
            return nullptr;
        }
        ios::openmode mode = ( m_mode == "ReadWrite" )
                                 ? ( ios::in | ios::out | ios::ate )
                                 : ( m_mode == "Truncate" )
                                       ? ( ios::in | ios::out | ios::trunc )
                                       : ios::in;
        if ( m_name.empty() ) {
            std::string def( System::getEnv( "HLTTCKROOT" ) );
            if ( !def.empty() ) {
                def += "/config.zip";
            } else {
               throw GaudiException("Environment variable HLTTCKROOT not specified and no explicit "
                                    "filename given; cannot obtain location of config.zip.",
                                    name(), StatusCode::FAILURE);
            }
            m_name = def;
        }
        info() << " opening " << m_name << " in mode " << m_mode << endmsg;
        m_file.reset( new ZipFile( m_name, mode ) );
        if ( !*m_file ) {
            error() << " Failed to open " << m_name << " in mode " << m_mode
                    << endmsg;
            error() << string( strerror( errno ) ) << endmsg;
            m_file.reset( nullptr );
        }
    }
    return m_file.get();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigZipFileAccessSvc::finalize()
{
    m_file.reset( nullptr ); // close file if still open
    return ConfigArchiveAccessSvc::finalize();
}

