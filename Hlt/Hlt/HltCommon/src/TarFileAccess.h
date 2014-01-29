#ifndef TARFILEACCESS_H
#define TARFILEACCESS_H
#include "GaudiKernel/IFileAccess.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include <string>
#include <memory>
#include <iostream>
#include <map>

namespace TarFileAccess_details
{
class TarFile;
}

class TarFileAccess : public extends1<AlgTool, IFileAccess>
{
  public:
    TarFileAccess( const std::string& type, const std::string& name,
                   const IInterface* parent );

    ~TarFileAccess();
    std::auto_ptr<std::istream> open( const std::string& url );
    // std::vector<std::string> listdir(const std::string &url);
    const std::vector<std::string>& protocols() const;

  private:
    MsgStream& verbose() const
    {
        return msg( MSG::VERBOSE );
    }
    MsgStream& debug() const
    {
        return msg( MSG::DEBUG );
    }
    MsgStream& info() const
    {
        return msg( MSG::INFO );
    }
    MsgStream& warning() const
    {
        return msg( MSG::WARNING );
    }
    MsgStream& error() const
    {
        return msg( MSG::ERROR );
    }
    MsgStream& fatal() const
    {
        return msg( MSG::FATAL );
    }
    MsgStream& always() const
    {
        return msg( MSG::ALWAYS );
    }

    MsgStream& msg( MSG::Level level ) const;

    mutable std::unique_ptr<MsgStream> m_msg;

    std::pair<TarFileAccess_details::TarFile*, std::string>
    resolve( const std::string& url );
    typedef std::map<std::string, TarFileAccess_details::TarFile*> container_t;
    container_t m_tarFiles;
};
#endif
