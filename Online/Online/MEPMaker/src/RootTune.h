//	====================================================================
//  RootTune.cpp
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
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "TTree.h"


class RootTune : public Algorithm {
private:
    MsgStream *m_log;
    Long64_t m_size;
public:
	/// Constructor: A constructor of this form must be provided.
    RootTune(const std::string& name, ISvcLocator* pSvcLocator)
	: Algorithm(name, pSvcLocator) { 
	    declareProperty("ROOTTREESIZE", m_size);
    };
    /// Standard Destructor
    virtual ~RootTune() { }
    /// Initialize
    virtual StatusCode initialize() {
	m_log = new MsgStream(msgSvc(), name());
	SetMaxTreeSize(m_size);
	*m_log << MSG::INFO << "Set TTree Max size to " << m_size << endmsg;
	return StatusCode::SUCCESS;
    }
    virtual StatusCode execute() {
	return StatusCode::SUCCESS;
    }
    /// Finalize
    virtual StatusCode finalize() {
	return StatusCode::SUCCESS;
    }
};

static const AlgFactory<RootTune>  Factory;
const IFactory& RootTuneFactory = Factory;
