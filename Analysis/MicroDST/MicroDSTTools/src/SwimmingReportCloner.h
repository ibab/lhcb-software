// $Id: SwimmingReportCloner.h,v 1.1 2010-08-11 12:48:36 jpalac Exp $
#ifndef SWIMMINGREPORTCLONER_H 
#define SWIMMINGREPORTCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/Functors.hpp>
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneSwimmingReport.h>

namespace LHCb {
   class SwimmingReport;
}

/** @class SwimmingReportCloner SwimmingReportCloner.h
 *  
 *  MicroDSTTool that clones an LHCb::SwimmingReport for storage on the MicroDST.
 *
 *  @author Roel Aaij
 *  @date   2010-10-09
 */
class SwimmingReportCloner : public extends1<MicroDSTTool, ICloneSwimmingReport>
{
public: 
   /// Standard constructor
   SwimmingReportCloner( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

   virtual ~SwimmingReportCloner( ); ///< Destructor

   StatusCode initialize();

private:

   LHCb::SwimmingReport* operator() (const LHCb::SwimmingReport* report);

   typedef MicroDST::BasicItemCloner<LHCb::SwimmingReport> Cloner;

   LHCb::SwimmingReport* clone (const LHCb::SwimmingReport* report);

};
#endif // SWIMMINGREPORTCLONER_H
