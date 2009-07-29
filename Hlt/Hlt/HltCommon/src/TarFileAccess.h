#ifndef TARFILEACCESS_H
#define TARFILEACCESS_H
#include "GaudiKernel/IFileAccess.h"
#include <string>
#include <memory>
#include <iostream>
#include <map>

namespace TarFileAccess_details { class TarFile; };

class TarFileAccess : public IFileAccess {
public:
    TarFileAccess();
    ~TarFileAccess();
    std::auto_ptr<std::istream> open(const std::string &url);
    const std::vector<std::string>& protocols() const;
private:
    typedef std::map<std::string,TarFileAccess_details::TarFile*> container_t;
    container_t m_tarFiles;
};
#endif
