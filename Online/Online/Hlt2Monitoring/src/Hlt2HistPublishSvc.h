#ifndef HLT2HISTPUBLISHSVC_H
#define HLT2HISTPUBLISHSVC_H 1

// Include files
#include <string>
#include <unordered_map>
#include <chrono>

// boost
#include <boost/unordered_map.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/HistoDef.h>
#include <GaudiKernel/IIncidentListener.h>
#include <GaudiKernel/IMonitorSvc.h>
#include <GaudiKernel/IHistogramSvc.h>

// Local
#include "Hlt2Monitoring/Common.h"
#include "Hlt2MonBaseSvc.h"
#include "Utilities.h"

class TFile;
class TH1D;

/** @class Hlt2Saver Hlt2Saver.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-13
 */
class Hlt2HistPublishSvc : public Hlt2MonBaseSvc {
public:

   /// Standard constructor
   Hlt2HistPublishSvc(const std::string& name, ISvcLocator* sl);

   virtual ~Hlt2HistPublishSvc( ); ///< Destructor

   // Service pure virtual member functions
   virtual StatusCode initialize();

   virtual void function() override;

private:

   void declareHisto(std::string dir, TH1D* histo);

   // properties
   std::string m_dataCon;
   std::string m_initialFile;
   std::string m_histoDir;

   // data members
   std::unordered_map<std::string, AIDA::IHistogram1D*> m_gaudiHistos;

   SmartIF<IMonitorSvc> m_monitorSvc;
   SmartIF<IHistogramSvc> m_histoSvc;
   
};
#endif // HLT2HISTPUBLISHSVC_H
