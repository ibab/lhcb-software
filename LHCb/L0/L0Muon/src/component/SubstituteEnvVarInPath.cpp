#include "SubstituteEnvVarInPath.h"
#include "GaudiKernel/System.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

namespace L0MuonUtils {

std::string SubstituteEnvVarInPath(const std::string& in) {
    boost::filesystem::path path(in);
    boost::filesystem::path::iterator i = path.begin();
    boost::filesystem::path out;
    while (i != path.end()) {
       if ( *(i->c_str())=='$' ) {
            std::string x = System::getEnv( i->c_str()+1 );
            out /= x.empty() ? *i : x ;
       } else {
            out /= *i;
       }
       ++i;
    }
    return out.string();
}

}
