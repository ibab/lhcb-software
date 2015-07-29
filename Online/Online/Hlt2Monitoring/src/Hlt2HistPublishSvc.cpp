// Include files

//AIDA
#include <AIDA/IHistogram1D.h>

// ROOT
#include <TBufferFile.h>
#include <TClass.h>

// local
#include "Hlt2HistPublishSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2HistPublishSvc
//
// 2015-07-28 : Roel Aaij
//-----------------------------------------------------------------------------

namespace {
   using std::string;
   namespace fs = boost::filesystem;
}

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2HistPublishSvc)

//=============================================================================
Hlt2HistPublishSvc::Hlt2HistPublishSvc(const string& name, ISvcLocator* loc)
 : Hlt2MonBaseSvc(name, loc)
{
   declareProperty("DataConnection", m_dataCon);
   declareProperty("InitialFile", m_initialFile);
   declareProperty("HistogramDirectory", m_histoDir);
}

//=============================================================================
Hlt2HistPublishSvc::~Hlt2HistPublishSvc()
{
}

//===============================================================================
StatusCode Hlt2HistPublishSvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

   // MonitorSvc
   IMonitorSvc* monSvc;
   sc = service("MonitorSvc", monSvc, false); // do not create
   if (!sc.isSuccess()) {
      error() << "Could not retrieve MonitorSvc." << endmsg;
      return sc;
   }
   m_monitorSvc = monSvc;

   // HistogramSvc

   IHistogramSvc* histoSvc;
   sc  = service("HistogramSvc", histoSvc, false); // do not create
   if (!sc.isSuccess()) {
      error() << "Could not retrieve HistogramSvc." << endmsg;
      return sc;
   }
   m_histoSvc = histoSvc;

   Monitoring::histos_t histos;
   if (!m_initialFile.empty()) {
      fs::path file{m_initialFile};
      boost::system::error_code ec;
      bool good = fs::exists(file, ec);
      good &= !ec;
      if (good) {
         info() << "Reading initial histograms from " << m_initialFile << endmsg;
         Monitoring::loadSavedHistograms(histos, file, 0);
      } else {
         warning() << "Initial file " << m_initialFile
                   << " does not exist or is not accessible." << endmsg;
      }
   }

   for (const auto& entry : histos) {
      if (entry.dir == m_histoDir) {
         declareHisto(entry.dir, entry.histo.get());
      }
   }

   if (m_dataCon.empty()) {
      warning() << "Connections not correctly configured, "
                << "updates to Hlt2 histogram publisher disabled." << endmsg;
      disable();
   }
   return sc;
}

//===============================================================================
void Hlt2HistPublishSvc::declareHisto(string, TH1D* histo) {

   // string histoPath = dir + "/" + histo->GetName();
   string histoPath = histo->GetName();
   if (m_gaudiHistos.count(histoPath)) {
      return;
   }

   // Book IHistogram1D
   auto axis = histo->GetXaxis();

   info() << "Publish " << histoPath << " " << histo->GetTitle()
           << " " << histo->GetNbinsX() << " " << axis->GetXmin()
           << " " << axis->GetXmax() << endmsg;
   auto gaudiHisto = m_histoSvc->book(histoPath, histo->GetName(),
                                      histo->GetNbinsX(), axis->GetXmin(),
                                      axis->GetXmax());
   m_gaudiHistos.emplace(histoPath, gaudiHisto);

   // Declare info to monitor svc
   string description = string{"HLT2 "} + histo->GetName();
   m_monitorSvc->declareInfo(histoPath, gaudiHisto, description, this);
}

//===============================================================================
void Hlt2HistPublishSvc::function()
{
   zmq::socket_t data{context(), ZMQ_SUB};
   data.connect(m_dataCon.c_str());
   data.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected data socket to: " << m_dataCon << endmsg;

   // Clean up queue
   zmq::message_t msg;
   while (data.recv(&msg, ZMQ_DONTWAIT)) {
      msg.rebuild();
   }

   // Control socket
   zmq::socket_t control{context(), ZMQ_SUB};
   control.bind(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { data, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;

   while (true) {
      //  Process messages from both sockets
      zmq::message_t message;

      zmq::poll(&items[0], 2, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         auto cmd = receiveString(control);
         if (cmd == "TERMINATE") {
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (cmd == "PAUSE") {
            debug() << name() << " paused." << endmsg;
            paused = true;
         } else if (cmd == "RESUME") {
            debug() << name() << " resumed." << endmsg;
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Receive data
         auto key = receiveRunAndId(data);

         // Receive type
         auto type = receiveString(data);

         // Receive directory
         auto dir = receiveString(data);

         // Receive ROOT histogram
         data.recv(&message);

         // ROOT uses char buffers, ZeroMQ uses size_t
         auto factor = sizeof( size_t ) / sizeof( char );

         // Create a buffer and insert the message
         TBufferFile buffer(TBuffer::kRead);
         buffer.SetBuffer(message.data(), factor * message.size(), kFALSE);

         // Grab the object from the buffer
         auto cl = TClass::GetClass(typeid(TH1D));
         std::unique_ptr<TH1D> histo{static_cast<TH1D*>(buffer.ReadObject(cl))};
         histo->SetDirectory(nullptr);
         histo->SetTitle(type.c_str());

         debug() << "Received ROOT histogram " << key.first << " " << key.second
                 << " " << dir << " " << histo->GetName() << endmsg;

         if (dir == m_histoDir) {
            declareHisto(dir, histo.get());
         }
      }
   }
}
