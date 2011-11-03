// Framework includes
#include "Framework/Monitor.h"
#include "Framework/IExecutor.h"
#include "Framework/EventContext.h"

// C/C++ include files
#include <sys/time.h>
#include <map>

// ROOT includes
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TROOT.h"

using namespace Framework;
using namespace std;

namespace {
  struct Entry {
    Long64_t data[4];
    char name[255];
  };
  struct Summary {
    Long64_t id;
    Long64_t instances;
    Long64_t threads;
    Long64_t parallel;
    Long64_t calls;
    Double_t data[2];
    char     name[255];
  };
  static Entry e;
  static Summary s;
}

namespace Framework  {
  struct MonitorConfig {
  public:
    MonitorConfig() : file(0), tree(0), sum_tree(0) {}
    TFile*    file;
    TTree*    tree, *sum_tree;
  };
}

#define USE_SPLITTING 1

/// Initializing constructor
Monitor::Monitor(const std::string& histogram_fname, bool update) : m_fileName(histogram_fname)  {
  m_cfg = new MonitorConfig();
  if ( !m_fileName.empty() ) {
    m_cfg->file = new TFile(m_fileName.c_str(),update ? "UPDATE" : "RECREATE","Multi-thread analysis");
    m_cfg->tree     = update ? ((TTree*)m_cfg->file->Get("Data"))    : new TTree("Data","Data",99);
    m_cfg->sum_tree = update ? ((TTree*)m_cfg->file->Get("Summary")) : new TTree("Summary","Data",99);
    if ( !update ) {
#ifdef USE_SPLITTING
      m_cfg->tree->Branch("name",e.name,"name/C");
      m_cfg->tree->Branch("id",&e.data[0],"id/L");
      m_cfg->tree->Branch("serial",&e.data[1],"serial/L");
      m_cfg->tree->Branch("event",&e.data[2],"event/L");
      m_cfg->tree->Branch("time",&e.data[3],"time/L");

      m_cfg->sum_tree->Branch("name",      s.name,"name/C");
      m_cfg->sum_tree->Branch("id",       &s.id,"id/L");
      m_cfg->sum_tree->Branch("parallel", &s.parallel,"parallel/L");
      m_cfg->sum_tree->Branch("threads",  &s.threads,"threads/L");
      m_cfg->sum_tree->Branch("calls",    &s.calls,"calls/L");
      m_cfg->sum_tree->Branch("instances",&s.instances,"instances/L");
      m_cfg->sum_tree->Branch("mean",     &s.data[0],"mean/D");
      m_cfg->sum_tree->Branch("sigma",    &s.data[1],"sigma/D");

#else
      m_cfg->tree->Branch("DATA",&e,"id/L:serial/L:event/L:time/L:name/C");
      m_cfg->sum_tree->Branch("SUMMARY",&s,"id/L:instances/L:threads/L:parallel/L:calls/L:mean/D:sigma/D:name/C");
#endif
    }
    else {
      m_cfg->tree->GetBranch("name")->SetAddress(e.name);
      m_cfg->tree->GetBranch("id")->SetAddress(&e.data[0]);
      m_cfg->tree->GetBranch("serial")->SetAddress(&e.data[1]);
      m_cfg->tree->GetBranch("event")->SetAddress(&e.data[2]);
      m_cfg->tree->GetBranch("time")->SetAddress(&e.data[3]);

      m_cfg->sum_tree->GetBranch("name")->SetAddress(s.name);
      m_cfg->sum_tree->GetBranch("id")->SetAddress(&s.id);
      m_cfg->sum_tree->GetBranch("parallel")->SetAddress(&s.parallel);
      m_cfg->sum_tree->GetBranch("threads")->SetAddress(&s.threads);
      m_cfg->sum_tree->GetBranch("calls")->SetAddress(&s.calls);
      m_cfg->sum_tree->GetBranch("instances")->SetAddress(&s.instances);
      m_cfg->sum_tree->GetBranch("mean")->SetAddress(&s.data[0]);
      m_cfg->sum_tree->GetBranch("sigma")->SetAddress(&s.data[1]);
    }
  }
}

/// Default destructor
Monitor::~Monitor()   {
  if ( m_cfg )  {
    if ( m_cfg->sum_tree ) m_cfg->sum_tree->Write();
    if ( m_cfg->tree ) m_cfg->tree->Write();
    m_cfg->file->Close();
    delete m_cfg->file;
    delete m_cfg;
  }
  m_cfg = 0;
}

/// Monitor executor
void Monitor::monitor(Executor* exec, EventContext* context, const struct timeval& start, const struct timeval& stop) {
  if ( exec )   {
    ::strncpy(e.name,exec->factory().name().c_str(),sizeof(e.name));
    e.name[sizeof(e.name)] = 0;
    e.data[0] = exec->id();
    e.data[1] = exec->serial();
    e.data[2] = context->id();
    if ( stop.tv_usec > start.tv_usec )   {
      struct timeval t = { stop.tv_sec-start.tv_sec, stop.tv_usec-start.tv_usec };
      e.data[3] = 1000000*t.tv_sec + t.tv_usec;
    }
    else {
      struct timeval t = { 1+stop.tv_sec-start.tv_sec, 1000000+stop.tv_usec-start.tv_usec };
      e.data[3] = 1000000*t.tv_sec + t.tv_usec;
    }
    //m_cfg->tree->Fill();
  }
}

void Monitor::add(const string& name, long id, long event, const struct timeval& start, const struct timeval& stop) {
  ::strncpy(e.name,name.c_str(),sizeof(e.name));
  e.name[sizeof(e.name)] = 0;
  e.data[0] = id;
  e.data[1] = 0;
  e.data[2] = event;
  if ( stop.tv_usec > start.tv_usec )   {
    struct timeval t = { stop.tv_sec-start.tv_sec, stop.tv_usec-start.tv_usec };
    e.data[3] = 1000000*t.tv_sec + t.tv_usec;
  }
  else {
    struct timeval t = { 1+stop.tv_sec-start.tv_sec, 1000000+stop.tv_usec-start.tv_usec };
    e.data[3] = 1000000*t.tv_sec + t.tv_usec;
  }
  //if ( m_cfg && m_cfg->tree ) m_cfg->tree->Fill();
}

void Monitor::saveSummary(const std::string& name, size_t id, size_t parallel, size_t threads, size_t instances, const Timing& timing) {
  ::strncpy(s.name,name.c_str(),sizeof(s.name));
  s.data[0]   = timing.mean();
  s.data[1]   = timing.sigma();
  s.calls     = timing.numCalls();
  s.instances = instances;
  s.threads   = threads;
  s.parallel  = parallel;
  s.id        = id;
  if ( m_cfg && m_cfg->sum_tree ) m_cfg->sum_tree->Fill();
}

