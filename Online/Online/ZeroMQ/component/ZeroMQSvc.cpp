// Include files
// Gaudi
#include <GaudiKernel/StateMachine.h>
#include <GaudiKernel/GaudiException.h>

// local
#include "ZeroMQSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ZeroMQSvc
//
// 2015-06-22 :
//-----------------------------------------------------------------------------

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(ZeroMQSvc)

//=============================================================================
ZeroMQSvc::ZeroMQSvc(const std::string& name, ISvcLocator* sl)
   : base_class(name, sl),
     m_context{nullptr}
{
   declareProperty("NThreads", m_nThreads = 1);
}

//=============================================================================
ZeroMQSvc::~ZeroMQSvc()
{
   delete m_context;
   m_context = nullptr;
}


//=============================================================================
StatusCode ZeroMQSvc::initialize()
{
   return Service::initialize();
}

//=============================================================================
StatusCode ZeroMQSvc::start()
{
   return Service::start();
}

//=============================================================================
StatusCode ZeroMQSvc::finalize()
{
   return Service::finalize();
}

//=============================================================================
zmq::context_t& ZeroMQSvc::context() const
{
   if (!m_context) {
      if (FSMState() < Gaudi::StateMachine::INITIALIZED) {
         throw GaudiException("First call to ZeroMQSvc::context must be after initialize",
                              "", StatusCode::FAILURE);
      }
      m_context = new zmq::context_t{m_nThreads};
   }
   return *m_context;
}
