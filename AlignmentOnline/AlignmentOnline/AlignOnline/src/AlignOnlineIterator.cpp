/*
 * AligOnlIterator.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */
#include <fstream>

#include "GaudiKernel/Service.h"
#include "GaudiAlg/GaudiTool.h"

#include "OnlineAlign/IAlignSys.h"
#include "OnlineAlign/IAlignUser.h"
#include "RTL/rtl.h"

#include "ASDCollector.h"

#include "GaudiKernel/ToolHandle.h"
#include "AlignmentInterfaces/IAlignUpdateTool.h"
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"
#include "AlignmentMonitoring/CompareConstants.h"

#include "AlignOnlineXMLCopier.h"
#include <boost/filesystem.hpp>

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "DetDesc/RunChangeIncident.h"
#include "EventBuilder/PublishSvc.h"

//#include <stdio.h>
//#include <stdlib.h>

namespace {
   using std::string;
   using std::vector;
   using std::map;
   using std::to_string;
   using std::ofstream;
   using std::make_pair;
   using std::endl;
}

class AlignOnlineIterator: public GaudiTool, virtual public LHCb::IAlignIterator
{
  public:
    AlignOnlineIterator(const string & type, const string & name,
        const IInterface * parent);
    //      StatusCode init(string, string);
    StatusCode i_start();
    StatusCode i_run();
    StatusCode initialize();
    StatusCode finalize();
    StatusCode stop();
    //      StatusCode de_init();
    StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  private:
    StatusCode writeWrappers() const ;
    StatusCode writeWrapper(const std::string& det) const ;
    LHCb::IAlignDrv *m_parent;
    lib_rtl_thread_t m_thread;
    string m_PartitionName;
    string m_runType;
    ToolHandle<Al::IAlignUpdateTool> m_alignupdatetool;
    ToolHandle<IWriteAlignmentConditionsTool> m_xmlwriter;
    unsigned int m_maxIteration;
    ASDCollector m_asdCollector;
    unsigned int m_iteration;
    string m_onlinexmldir;
    string m_alignworkdir; // directory where all jobs run
    string m_runningdir ;  // sub directory where this job runs
    string m_runningxmldir ;  // directory where the iterator stores the xml
    bool m_keepIterXml ;
    int m_refRunNr;
    vector<AlignOnlineXMLCopier*> m_xmlcopiers;
    IPublishSvc *m_PubSvc;
    map<string, string*> m_condmap;
    string m_ServInfix;
    vector<string> m_subDets;
    IUpdateManagerSvc* m_updateMgrSvc ; ///< Pointer to update manager svc
    unsigned int m_wrnLo;
    unsigned int m_wrnHi;
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

AlignOnlineIterator::AlignOnlineIterator(const string & type,
    const string & name, const IInterface * parent) :
    GaudiTool(type, name, parent), m_alignupdatetool("Al::AlignUpdateTool"), m_xmlwriter(
        "WriteMultiAlignmentConditionsTool")
{
  declareInterface<LHCb::IAlignIterator>(this);
  declareProperty("PartitionName", m_PartitionName = "LHCbA");
  declareProperty("MaxIteration", m_maxIteration = 10);
  declareProperty("ASDDir", m_asdCollector.m_dir);
  declareProperty("ASDFilePattern", m_asdCollector.m_filePatt);
  declareProperty("OnlineXmlDir", m_onlinexmldir = "/group/online/alignment");
  declareProperty("AlignXmlDir", m_alignworkdir = "/group/online/AligWork");
  declareProperty("ServiceInfix", m_ServInfix = "TrackerAlignment/");
  declareProperty("SubDetectors", m_subDets);
  declareProperty("KeepIterXml", m_keepIterXml = true ) ;
  declareProperty("RunType", m_runType = "NotDefined");
  declareProperty("ReferenceRunNr",m_refRunNr);

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
  debug() << "ReferenceRunNumber  :" << m_refRunNr <<endreq;

  StatusCode sc = GaudiTool::initialize();
  if (sc.isSuccess())
  {
    sc = m_alignupdatetool.retrieve();
    if (!sc.isSuccess())
      error() << "Cannot retrieve alignupdatetool" << endreq;
  }
  if (m_refRunNr == -1)
  {
    error() << "reference Run Number not defined. seems that there are no runs specified in the RunInfo" <<endreq;
  }
  sc = service("LHCb::PublishSvc", m_PubSvc, true);
  if (!sc.isSuccess())
  {
    error() << "cannot retrieve the Publishing Service" << endmsg;
  }
  sc = service("UpdateManagerSvc",m_updateMgrSvc, false);
  if (!sc.isSuccess())  return Error("==> Failed to retrieve UpdateManagerSvc",  StatusCode::FAILURE);

  if (sc.isSuccess())
  {
    sc = m_xmlwriter.retrieve();
    if (!sc.isSuccess())
      error() << "Cannot retrieve xmlwriter" << endreq;
    if (m_xmlwriter->FSMState() < Gaudi::StateMachine::INITIALIZED) m_xmlwriter->initialize();
    IProperty* prop = 0;
    sc = m_xmlwriter->queryInterface(IProperty::interfaceID(), (void**)&prop);
    if (sc.isSuccess()) {
       prop->setProperty("OnlineMode", "True");
    }
  }

  // instantiate the objects that will take care of copying and versioning files
  m_runningdir = m_alignworkdir + "/running" ;
  if (!boost::filesystem::exists(m_runningdir))
    boost::filesystem::create_directory(m_runningdir);
  m_runningxmldir = m_runningdir + "/xml" ;
  if (!boost::filesystem::exists(m_runningxmldir))
    boost::filesystem::create_directory(m_runningxmldir);

  vector<string> condnames;
  for (const auto& sd: m_subDets) {
     condnames.push_back(sd + "/" + sd + "Global");
     condnames.push_back(sd + "/" + sd + "Modules");
  }
  for (const auto& i : condnames) {
    string *s = new string("");
    m_condmap.insert(make_pair(i, s));
    AlignOnlineXMLCopier* acpy = new AlignOnlineXMLCopier(m_onlinexmldir, m_runningxmldir, i, &info());
    m_PubSvc->declarePubItem(m_ServInfix + i, *s);
    m_xmlcopiers.push_back(acpy);
  }
  fflush(stdout);

  // set reference base
  m_parent->setReferenceBase(0);

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

  int runnr = m_refRunNr;
  IDetDataSvc* detDataSvc(0);
  sc = service("DetectorDataSvc", detDataSvc, false);
  if (sc.isFailure())
  {
    error() << "Could not retrieve DetectorDataSvc" << endmsg;
    return sc;
  }
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");

  Al::IAlignUpdateTool::ConvergenceStatus convergencestatus = Al::IAlignUpdateTool::Unknown;

  // Constants
  std::map<std::string, double> initAlConsts, finalAlConsts;

  while (m_iteration <= m_maxIteration)
  {
    debug() << "wait for analyzers" << endreq;
    m_parent->waitRunOnce();

    debug() << "Iteration " << m_iteration << endreq;
    // 4. read ASDs and compute new constants
    debug() << "Collecting ASD files" << endreq;
    Al::Equations equations(0);
    m_asdCollector.collectASDs(equations);
    debug() << "Collected ASDs: numevents = " << equations.numEvents()
        << endreq;
//    runnr = equations.firstRun();

    // FIXME: fire incident in the run changehandler to read the xml:
    // otherwise it will just use the geometry from the snapshot
    // Don't know if we need all of this ... Need to think.
    // Update 2015/06/11: we definitely need the 'newEvent'.
    detDataSvc->setEventTime(equations.lastTime());
    incSvc->fireIncident(RunChangeIncident(name(), runnr));
    m_updateMgrSvc->newEvent() ;

    // Now call the update tool
    debug() << "Calling AlignUpdateTool" << endreq;
    sc = m_alignupdatetool->process(equations, convergencestatus);
    if (!sc.isSuccess())
    {
      error() << "Error calling alignupdate tool" << endreq;
      break;
    }

    // Store alignment constants
    if (m_iteration == 1) // first iteration store the initial
      initAlConsts = m_alignupdatetool->getAlignmentConstants(false);

    // copy the xml directory to a directory with the iteration name
    if( m_keepIterXml ) {
      const std::string xmliterdir = m_runningdir + "/Iter" + to_string(m_iteration);
      boost::system::error_code ec;
      boost::filesystem::rename(m_runningxmldir, xmliterdir,ec);
      if (ec.value() != 0)
      {
        error() << "Rename from " << m_runningxmldir << " to " << xmliterdir << "failed: Return"
            << ec.value()<<" " << ec.message()<<". Retrying..."<< endreq;
        boost::filesystem::remove_all(xmliterdir,ec);
        boost::filesystem::rename(m_runningxmldir, xmliterdir,ec);
        if (ec.value() != 0)
          error() << "Rename from " << m_runningxmldir << " to " << xmliterdir << "failed again: Return"
              << ec.value()<<" " <<ec.message() << "giving up..."<<endreq;
      }
    }

    // write the xml
    debug() << "writing xml files" << endreq;
    sc = m_xmlwriter->write("run" + to_string(runnr) ) ;
    if (!sc.isSuccess())
    {
      error() << "Error writing xml files" << endreq;
      break;
    }

    // Write the wrappers for the generated conditions
    if (sc.isSuccess()) sc = writeWrappers() ;

    // writes a little file with number of iteration step
    debug() << "calling parent->writeReference()" << endreq;
    m_parent->writeReference();

    // break the loop if converged or maximum number of iterations reached
    if (convergencestatus == Al::IAlignUpdateTool::Converged || m_iteration > m_maxIteration)
    {
      break;
    }
    ++m_iteration;
    // start the analyzers and wait
    m_asdCollector.setTime();
    m_parent->doContinue();
  }
  string *s;
  if (sc.isSuccess() && m_iteration > 1 && convergencestatus == Al::IAlignUpdateTool::Converged)
  {
    // if converged, check whether alignment needs to be updated by comparing the constants
    //
    // get the latest constants
    finalAlConsts = m_alignupdatetool->getAlignmentConstants(true);
    // compare them to initial
    Alignment::AlignmentMonitoring::CompareConstants cmp(initAlConsts,finalAlConsts);
    cmp.Compare();
    if ( msgLevel(MSG::DEBUG) ) cmp.PrintWarnings(1);
    if ( !(cmp.GetNumWarnings(3) || cmp.GetNumWarnings(2)>3) ) { // no significant change
      warning() << "Alignment converged in more than one iteration but constants didn't change significantly." 
		<< endmsg;
      if (cmp.GetNumWarnings(2))
	cmp.PrintWarnings(2);
    } else {
      // after last update, if more than one iteration and successfully converged
      for (auto& i : m_xmlcopiers)
	{
	  StatusCode thissc = i->copyToOnlineArea();
	  info() << "Condition: " << i->condition() << " Filename: "
		 << i->copybackfilename() << endmsg;
	  auto j = m_condmap.find(i->condition());
	  s = (j->second);
	  //*s = to_string(runnr) + " " + i->copybackfilename();
	  *s = to_string(runnr) + " v" + to_string(i->version());
	  
	  if (!thissc.isSuccess())
	    {
	      error() << "Error copying file to online area" << endmsg;
	    }
	}
    } // end of check on the constants
  }
  else
  {
    if (sc.isSuccess() && m_iteration <= 1)
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
      //*s = to_string(runnr) + " " + i->onlinefilename();
      *s = to_string(runnr) + " v" + to_string(i->version());

    }
  }
  m_PubSvc->updateAll();
  fflush(stdout);

  // move the 'running' dir to a dirname with current run
  string rundir = m_alignworkdir + "/"+ m_runType+"/run" + to_string(runnr);
  if (! (boost::filesystem::exists(m_alignworkdir + "/"+ m_runType)) )
    boost::filesystem::create_directory(m_alignworkdir + "/"+ m_runType);
  if (boost::filesystem::exists(rundir))
    boost::filesystem::remove_all(rundir);
  boost::filesystem::rename(m_runningdir,rundir);

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
      warning() << "Cannot find input xml file \'" << i->onlinefilename() << "\'" << endmsg;
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

  // Write the wrappers for the generated conditions, this only needs to happen once.
  if (sc.isSuccess()) sc = writeWrappers() ;

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

StatusCode AlignOnlineIterator::writeWrappers() const
{
  StatusCode sc = StatusCode::SUCCESS ;
  for (const auto& sd: m_subDets) {
    sc = writeWrapper(sd);
    if (!sc.isSuccess()) {
      error() << "Error writing wrapper file for " << sd << endreq;
      break;
    }
  }
  return sc ;
}

StatusCode AlignOnlineIterator::writeWrapper(const string& sd) const
{
   string filename = m_runningxmldir + "/" + sd + ".xml";
   ofstream xml(filename);
   if (xml.fail() ){
      return Warning(string("Failed to open wrapper file ") + filename, StatusCode::FAILURE);
   }
   // XML header and links
   xml << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl
       << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\"" << endl
       << "[<!ENTITY " << sd << "GlobalLabel SYSTEM \"" << sd << "/" << sd << "Global.xml\">" << endl
       << " <!ENTITY " << sd << "ModLabel SYSTEM \"" << sd << "/" << sd << "Modules.xml\">]" << endl
       << ">" << endl
   // DDDB open
       << "<DDDB>" << endl
   // labels
       << "&" << sd << "GlobalLabel;" << endl
       << "&" << sd << "ModLabel;" << endl
   // DDDB close
       << "</DDDB>" << endl;
   return StatusCode::SUCCESS;
}
