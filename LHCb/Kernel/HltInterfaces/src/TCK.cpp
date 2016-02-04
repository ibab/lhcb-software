#include "Kernel/TCK.h"
#include <string>
#include "boost/regex.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

TCK& 
TCK::set(const std::string& s) {
    static const boost::regex e("^(0x[0-9a-fA-F]{8})$");
    boost::smatch what;
    if(!boost::regex_match(s, what, e)) {
        throw GaudiException("Invalid TCK format",s,StatusCode::FAILURE);
        return *this;
    }
    m_unsigned = std::stoul(what[1],nullptr,0);
    m_stringRep = s;
    // canonical rep is lower case...
    boost::algorithm::to_lower(m_stringRep);
    return *this;
}
