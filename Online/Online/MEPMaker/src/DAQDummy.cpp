//	====================================================================
//  DAQDummy.cpp
//	--------------------------------------------------------------------
//
//	Package   : MEPMaker
//
//	Author    : Niko Neufeld
//
//	====================================================================

// Framework include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Algorithm.h"  // Required for inheritance



class DAQdummy : public Algorithm {
private:
    MsgStream *m_log;
    std::vector<std::string> m_items;
public:
	/// Constructor: A constructor of this form must be provided.
    DAQdummy(const std::string& name, ISvcLocator* pSvcLocator)
	: Algorithm(name, pSvcLocator) { 
	    declareProperty("DUMMYITEMS", m_items);
    };
    /// Standard Destructor
    virtual ~DAQdummy() { }
    /// Initialize
    virtual StatusCode initialize() {
	m_log = new MsgStream(msgSvc(), name());
	return StatusCode::SUCCESS;
    }
    virtual StatusCode execute() {
	for(std::vector<std::string>::const_iterator i = m_items.begin(); 
	    i != m_items.end(); ++i )  {
	    SmartDataPtr<DataObject> item(eventSvc(),*i);
	    if ( item == 0 )  {
		*m_log << MSG::DEBUG << *i << " = NULL"<< endmsg;
	    }
	}
	return StatusCode::SUCCESS;
    }
    /// Finalize
    virtual StatusCode finalize() {
	return StatusCode::SUCCESS;
    }
};

static const AlgFactory<DAQdummy>  Factory;
const IFactory& DAQdummyFactory = Factory;
