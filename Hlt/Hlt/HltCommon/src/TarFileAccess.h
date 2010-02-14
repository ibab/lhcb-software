#ifndef TARFILEACCESS_H
#define TARFILEACCESS_H
#include "GaudiKernel/IFileAccess.h"
#include "IDirectoryAccess.h"
#include <string>
#include <memory>
#include <iostream>
#include <map>

namespace TarFileAccess_details { class TarFile; };

class TarFileAccess : public IFileAccess, IDirectoryAccess {
public:
    TarFileAccess();
    ~TarFileAccess();
    std::auto_ptr<std::istream> open(const std::string &url);
    std::vector<std::string> listdir(const std::string &url);
    const std::vector<std::string>& protocols() const;
private:
    std::pair<TarFileAccess_details::TarFile*,std::string> resolve(const std::string& url);
    typedef std::map<std::string,TarFileAccess_details::TarFile*> container_t;
    container_t m_tarFiles;
};
#endif
