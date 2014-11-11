/*
 * AligOnlIterator.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#include "GaudiKernel/Service.h"
#include "GaudiAlg/GaudiTool.h"

#include "OnlineAlign/IAlignSys.h"
#include "OnlineAlign/IAlignUser.h"
#include "RTL/rtl.h"

#include "ASDCollector.h"

#include "GaudiKernel/ToolHandle.h"
#include "AlignmentInterfaces/IAlignUpdateTool.h"
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

#include "AlignOnlineXMLCopier.h"
#include <boost/filesystem.hpp>

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "DetDesc/RunChangeIncident.h"
#include "EventBuilder/PublishSvc.h"

//#include <stdio.h>
//#include <stdlib.h>

class AlignOnlineIterator: public GaudiTool, virtual public LHCb::IAlignIterator
{
  public:
    AlignOnlineIterator(const std::string & type, const std::string & name,
        const IInterface * parent);
    //      StatusCode init(std::string, std::string);
    StatusCode i_start();
    StatusCode i_run();
    StatusCode initialize();
    StatusCode finalize();
    StatusCode stop();
    //      StatusCode de_init();
    StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  private:
    LHCb::IAlignDrv *m_parent;
    lib_rtl_thread_t m_thread;
    std::string m_PartitionName;
    ToolHandle<Al::IAlignUpdateTool> m_alignupdatetool;
    ToolHandle<IWriteAlignmentConditionsTool> m_xmlwriter;
    unsigned int m_maxIteration;
    ASDCollector m_asdCollector;
    unsigned int m_iteration;
    std::string m_onlinexmldir;
    std::string m_alignxmldir;
    std::vector<AlignOnlineXMLCopier*> m_xmlcopiers;
    IPublishSvc *m_PubSvc;
    std::map<std::string, std::string*> m_condmap;
    std::string m_ServInfix;
};

DECLARE_TOOL_FACTORY(AlignOnlineIterator)

static AlignOnlineIterator *IteratorInstance;

extern "C"
{
  int AlignOnlineIteratorThreadFunction(void *t)
  {
    AlignOnlineIterator *f = (AlignOnlineIterator*) t;
    IteratorInstance = f;
    f->i_run();
    return 1;
  }

}

AlignOnlineIterator::AlignOnlineIterator(const std::string & type,
    const std::string & name, const IInterface * parent) :
    GaudiTool(type, name, parent), m_alignupdatetool("Al::AlignUpdateTool"), m_xmlwriter(
        "WriteMultiAlignmentConditionsTool")
{
  declareInterface<LHCb::IAlignIterator>(this);
  declareProperty("PartitionName", m_PartitionName = "LHCbA");
  declareProperty("MaxIteration", m_maxIteration = 10);
  declareProperty("ASDDir", m_asdCollector.m_dir);
  declareProperty("ASDFilePattern", m_asdCollector.m_filePatt);
  declareProperty("OnlineXmlDir", m_onlinexmldir = "/group/online/alignment");
  declareProperty("AlignXmlDir", m_alignxmldir = "/group/online/AligWork");
  declareProperty("ServiceInfix", m_ServInfix = "TrackerAlignment/");

  m_iteration = 0;
  IInterface *p = (IInterface*) parent;
  StatusCode sc = p->queryInterface(LHCb::IAlignDrv::interfaceID(),
      (void**) (&m_parent));
}

StatusCode AlignOnlineIterator::initialize()
{
  debug() << "AlignOnlineIterator::initialize()" << endreq;
  debug() << "ASDDir         : " << m_asdCollector.m_dir << endreq;
  debug() << "ASDFilePattern : " << m_asdCollector.m_filePatt << endreq;
  debug() << "PartitionName  : " << m_PartitionName << endreq;
  debug() << "MaxIteration    : " << m_maxIteration << endreq;

  StatusCode sc = GaudiTool::initialize();
  if (sc.isSuccess())
  {
    sc = m_alignupdatetool.retrieve();
    if (!sc.isSuccess())
      error() << "Cannot retrieve alignupdatetool" << endreq;
  }
  sc = service("LHCb::PublishSvc", m_PubSvc, true);
  if (!sc.isSuccess())
  {
    error() << "cannot retrieve the Publishing Service" << endmsg;
  }
  if (sc.isSuccess())
  {
    sc = m_xmlwriter.retrieve();
    if (!sc.isSuccess())
      error() << "Cannot retrieve xmlwriter" << endreq;
  }

  // instantiate the objects that will take care of copying and versioning files
  const std::string runningdir = m_alignxmldir + "/running";
  if (!boost::filesystem::exists(runningdir))
    boost::filesystem::create_directory(runningdir);

  const std::vector<std::string> condnames =
  {
  { "Velo/VeloGlobal", "Velo/VeloModules", "TT/TTGlobal", "TT/TTModules",
      "IT/ITGlobal", "IT/ITModules", "OT/OTGlobal", "OT/OTModules" } };
  for (const auto& i : condnames)
  {
    std::string *s = new std::string("");
//    s->reserve(1024);
    m_condmap.insert(std::make_pair(i, s));
    auto j = m_condmap.find(i);
    AlignOnlineXMLCopier* acpy = new AlignOnlineXMLCopier(m_onlinexmldir,
        runningdir, i);
    m_PubSvc->declarePubItem(m_ServInfix + j->first, *s);
    m_xmlcopiers.push_back(acpy);
  }
  fflush(stdout);

  return sc;
}

StatusCode AlignOnlineIterator::finalize()
{
  m_PubSvc->undeclarePubAll();
  for (auto& j : m_condmap)
  {
    delete j.second;
  }
  m_condmap.clear();
  for (auto& i : m_xmlcopiers)
  {
    delete (i);
  }
  m_xmlcopiers.clear();
  m_alignupdatetool.release().ignore();
  m_xmlwriter.release().ignore();
  return GaudiTool::finalize();
}

StatusCode AlignOnlineIterator::i_run()
{
  StatusCode sc = StatusCode::SUCCESS;
  // only called once

  int runnr = 0;
  IDetDataSvc* detDataSvc(0);
  sc = service("DetectorDataSvc", detDataSvc, false);
  if (sc.isFailure())
  {
    error() << "Could not retrieve DetectorDataSvc" << endmsg;
    return sc;
  }
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  while (m_iteration < m_maxIteration)
  {
    m_parent->waitRunOnce();

    // 4. read ASDs and compute new constants
    debug() << "Collecting ASD files" << endreq;
    Al::IAlignUpdateTool::ConvergenceStatus convergencestatus;
    Al::Equations equations;
    m_asdCollector.collectASDs(equations);
    debug() << "Collected ASDs: numevents = " << equations.numEvents()
        << endreq;
    runnr = equations.firstRun();

    // FIXME: fire incident in the run changehandler to read the xml:
    // otherwise it will just use the geometry from the snapshot
    // Don't know if we need all of this ... Need to think.
    detDataSvc->setEventTime(equations.lastTime());
    incSvc->fireIncident(RunChangeIncident(name(), runnr));

    // Now call the update tool
    debug() << "Calling AlignUpdateTool" << endreq;
    StatusCode sc = m_alignupdatetool->process(equations, convergencestatus);
    if (!sc.isSuccess())
    {
      error() << "Error calling alignupdate tool" << endreq;
      break;
    }

    // write the xml
    debug() << "writing xml files" << endreq;
    sc = m_xmlwriter->write("run" + std::to_string(runnr) ) ;
    if (!sc.isSuccess())
    {
      error() << "Error writing xml files" << endreq;
      break;
    }

    // break the loop if converged
    if (convergencestatus == Al::IAlignUpdateTool::Converged)
    {
      break;
    }

    // writes a little file with number of iteration step
    debug() << "calling parent->writeReference()" << endreq;
    m_parent->writeReference();

    // start the analyzers and wait
    debug() << "wait for analyzers" << endreq;
    m_asdCollector.setTime();
    debug() << "Iteration " << ++m_iteration << endreq;
    m_parent->doContinue();
  }
  std::string *s;
  if (sc.isSuccess() && m_iteration > 1)
  {
    // after last update, if more than one iteration
    for (auto& i : m_xmlcopiers)
    {
      StatusCode thissc = i->copyToOnlineArea();
      info() << "Condition: " << i->condition() << " Filename: "
          << i->copybackfilename() << endmsg;
      auto j = m_condmap.find(i->condition());
      s = (j->second);
      *s = std::to_string(runnr) + " " + i->copybackfilename();
      if (!thissc.isSuccess())
      {
        error() << "Error copying file to online area" << endmsg;
      }
    }
  }
  else
  {
    if (sc.isSuccess())
    {
      warning() << "Alignment didn't change." << endmsg;
    }
    else
    {
      warning() << "Alignment procedure failed." << endmsg;
    }
    for (auto& i : m_xmlcopiers)
    {
      info() << "Condition: " << i->condition() << " Filename: "
          << i->onlinefilename() << endmsg;
      auto j = m_condmap.find(i->condition());
      s = (j->second);
      *s = std::to_string(runnr) + " " + i->onlinefilename();
    }
  }
  m_PubSvc->updateAll();
  fflush(stdout);

// move the 'running' dir to a dirname with current run
  std::string rundir = m_alignxmldir + "/run" + std::to_string(runnr);
  if (boost::filesystem::exists(rundir))
    boost::filesystem::remove_all(rundir);
  boost::filesystem::rename(m_alignxmldir + "/running", rundir);

//fflush(stdout);
  m_parent->doStop();
  return sc;
}

StatusCode AlignOnlineIterator::i_start()
{
  StatusCode sc = StatusCode::SUCCESS;
// called only once
// don't touch this
  debug() << "Iteration " << ++m_iteration << endreq;

// 1. writes a little file with number of iteration step
  debug() << "calling parent->writeReference()" << endreq;
  m_parent->writeReference();

// 2. copy the files from the online area. if this doesn't work,
// dump the current database.
  for (auto& i : m_xmlcopiers)
  {
    StatusCode thissc = i->copyFromOnlineArea();
    if (!thissc.isSuccess())
    {
      error() << "Cannot find input xml file \'" << i->onlinefilename() << "\'"
          << endmsg;
      sc = StatusCode::FAILURE;
    }
  }
  fflush(stdout);

// if some of the input files are missing, bootstrap this by writing
// from the database.
  if (!sc.isSuccess())
  {
    debug() << "writing xml files in i_start to bootstrap alignment" << endreq;
    sc = m_xmlwriter->write();
    if (!sc.isSuccess())
      error() << "Error writing xml files" << endreq;
  }

// 3. start the analyzers and wait
  debug() << "wait for analyzers" << endreq;
  m_asdCollector.setTime();
  ::lib_rtl_start_thread(AlignOnlineIteratorThreadFunction, this, &m_thread);
  return sc;
}

StatusCode AlignOnlineIterator::stop()
{
// called only once
  ::lib_rtl_delete_thread(m_thread);
  return StatusCode::SUCCESS;
}

StatusCode AlignOnlineIterator::queryInterface(const InterfaceID& riid,
    void** ppvIF)
{
  if (LHCb::IAlignIterator::interfaceID().versionMatch(riid))
  {
    *ppvIF = (IAlignIterator*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return GaudiTool::queryInterface(riid, ppvIF);
}

