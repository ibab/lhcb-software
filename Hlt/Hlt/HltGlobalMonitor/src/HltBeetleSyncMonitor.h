#ifndef HLTBEETLESYNCMONITOR_H 
#define HLTBEETLESYNCMONITOR_H 1

#include "GaudiKernel/HistoDef.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "HltBase/HltBaseAlg.h"

namespace AIDA {
   class IHistorgram2D;
}

/** @class HltBeetleSyncMonitor HltBeetleSyncMonitor.h
 *
 *  functionality:
 *        monitor the BeetleSync Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Roel Aaij
 *  @date   2011-10-13
 */
class HltBeetleSyncMonitor : public HltBaseAlg {
public: 

   /// Standard constructor
   HltBeetleSyncMonitor(const std::string& name, ISvcLocator* pSvcLocator);

   virtual ~HltBeetleSyncMonitor(); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode execute   ();    ///< Algorithm execution

private:

   template < typename T > T* fetch(const std::string& location)
   {
      T* t = this->exist<T>( location ) ?  this->get<T>( location ) : 0;
      if ( t == 0 && this->msgLevel( MSG::WARNING ) ) {
         Warning( " Could not retrieve " + location, StatusCode::SUCCESS, 10 );
      }
      return t;
   }

   void monitor();

   std::string m_ODINLocation;
   std::string m_VeloLiteClusterLocation;
   std::string m_ITLiteClusterLocation;
   std::string m_L0DUReportLocation;
   
   AIDA::IHistogram2D* m_SPDIT;
   AIDA::IHistogram2D* m_SPDVelo;

};
#endif // HLTBEETLESYNCMONITOR_H
