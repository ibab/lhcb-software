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

   virtual ~RunInfo( ) {}; ///< Destructor

   std::vector<double> lumiPars;
   Monitoring::RunNumber run;

};
}
#endif // HLT2MONITORING_RUNINFO_H
