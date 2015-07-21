#ifndef HLT2MONITORING_COMMON_H
#define HLT2MONITORING_COMMON_H 1

namespace Monitoring {

using TCK = unsigned int;
using HistId = long unsigned int;
using RunNumber = unsigned int;
using BinContent = unsigned long long;
using Seconds = std::size_t;

const std::string s_Known{"KNOWN"};
const std::string s_Unknown{"UNKNOWN"};

const std::string s_Rate{"RATE"};
const std::string s_Histo1D{"HISTO1D"};

const std::string s_RunInfo{"RUNINFO"};
const std::string s_HistoInfo{"HISTOINFO"};
   
}

#endif // HLT2MONITORING_COMMON_H
