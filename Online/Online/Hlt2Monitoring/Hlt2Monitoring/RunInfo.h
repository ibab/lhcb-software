#ifndef HLT2MONITORING_RUNINFO_H
#define HLT2MONITORING_RUNINFO_H 1

// Include files

namespace Monitoring {
/** @class HltRunInfo HltRunInfo.h Hlt2Monitoring/HltRunInfo.h
 *
 *  @author Roel Aaij
 *  @date   2015-07-09
 */
class RunInfo {
public:

   RunInfo() = default;
   RunInfo(const RunInfo&) = default;
   RunInfo(RunInfo&&) = default;
   RunInfo& operator=(const RunInfo&) = default;
   RunInfo& operator=(RunInfo&&) = default;
   
   RunInfo(const std::vector<double> pars, Monitoring::RunNumber r,
           size_t s)
      : lumiPars{pars},
        run{r},
        start{s} { }
   
   virtual ~RunInfo( ) {}; ///< Destructor

   std::vector<double> lumiPars;
   Monitoring::RunNumber run;
   size_t start;
   
};
}
#endif // HLT2MONITORING_RUNINFO_H
