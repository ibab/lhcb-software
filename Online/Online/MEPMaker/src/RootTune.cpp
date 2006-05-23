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
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TTree.h"
#include <cstdio>


class RootTune : public Algorithm {
private:
    MsgStream *m_log;
    Long64_t m_size;
    int m_meg;
public:

	/// Constructor: A constructor of this form must be provided.
    RootTune(const std::string& name, ISvcLocator* pSvcLocator)
	: Algorithm(name, pSvcLocator) { 
	    declareProperty("ROOTTREESIZE", m_meg = 4000);	    
    };
    /// Standard Destructor
    virtual ~RootTune() { }
    /// Initialize
    virtual StatusCode initialize() {
	MsgStream log(msgSvc(), name());
	m_size = 0x100000 * m_meg;
	TTree::SetMaxTreeSize((Long64_t) 0x100000 * (Long64_t) m_meg);
	log << MSG::INFO << "Set TTree Max size to " << m_meg <<  " Megabytes " << endmsg;
	std::printf("RootTune: MaxTreeSize is %llx\n", TTree::GetMaxTreeSize());
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
