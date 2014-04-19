#include "ConfigTarFileAccessSvc.h"

#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;


#include "GaudiKernel/SvcFactory.h"
#include "tar.h"


using ConfigTarFileAccessSvc_details::TarFile;

// Factory implementation
DECLARE_SERVICE_FACTORY( ConfigTarFileAccessSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigTarFileAccessSvc::ConfigTarFileAccessSvc( const std::string& name,
                                                ISvcLocator* pSvcLocator )
    : ConfigArchiveAccessSvc( name, pSvcLocator ), m_file{ nullptr }
{
    std::string def( System::getEnv( "HLTTCKROOT" ) );
    if ( !def.empty() ) def += "/config.tar";
    declareProperty( "File", m_name = def );
    declareProperty( "Mode", m_mode = "ReadOnly" );
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::finalize()
{
    m_file.reset( nullptr ); // close file if still open
    return ConfigArchiveAccessSvc::finalize();
}

//=============================================================================
// Destructor
//=============================================================================
ConfigTarFileAccessSvc::~ConfigTarFileAccessSvc()
{
}



IArchive* ConfigTarFileAccessSvc::file() const
{
    if ( !m_file.get() ) {
        if ( m_mode != "ReadOnly" && m_mode != "ReadWrite" &&
             m_mode != "Truncate" ) {
            error() << "invalid mode: " << m_mode << endmsg;
            return 0;
        }
        std::ios::openmode mode = ( m_mode == "ReadWrite" )
                                 ? ( std::ios::in | std::ios::out | std::ios::ate )
                                 : ( m_mode == "Truncate" )
                                       ? ( std::ios::in | std::ios::out | std::ios::trunc )
                                       : std::ios::in;

        info() << " opening " << m_name << " in mode " << m_mode << endmsg;
        m_file.reset( new TarFile( m_name, mode, true ) );
        if ( !*m_file ) {
            error() << " Failed to open " << m_name << " in mode " << m_mode
                    << endmsg;
            error() << std::string( strerror( errno ) ) << endmsg;
            m_file.reset( nullptr );
        }
    }
    return m_file.get();
}

