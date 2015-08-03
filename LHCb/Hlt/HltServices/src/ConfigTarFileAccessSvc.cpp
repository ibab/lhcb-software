#include "ConfigTarFileAccessSvc.h"

#include "GaudiKernel/System.h"
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
    declareProperty( "File", m_name = "" );
    declareProperty( "Mode", m_mode = "ReadOnly" );
    declareProperty("CompressOnWrite", m_compress = true );
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::finalize()
{
    m_file.reset( nullptr ); // close file if still open
    return ConfigArchiveAccessSvc::finalize();
}


IArchive* ConfigTarFileAccessSvc::file() const
{
    if ( !m_file ) {
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
        if ( m_name.empty() ) {
            std::string def( System::getEnv( "HLTTCKROOT" ) );
            if ( !def.empty() ) {
                def += "/config.tar";
            } else {
               throw GaudiException("Environment variable HLTTCKROOT not specified and no explicit "
                                    "filename given; cannot obtain location of config.tar.",
                                    name(), StatusCode::FAILURE);
            }
            m_name = def;
        }
        info() << " opening " << m_name << " in mode " << m_mode << endmsg;
        m_file.reset( new TarFile( m_name, mode, m_compress ) );
        if ( !*m_file ) {
            error() << " Failed to open " << m_name << " in mode " << m_mode
                    << endmsg;
            error() << std::string( strerror( errno ) ) << endmsg;
            m_file.reset( nullptr );
        }
    }
    return m_file.get();
}

