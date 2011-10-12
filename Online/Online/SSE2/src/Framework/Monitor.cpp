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

namespace Framework {

  struct Entry {
    Long64_t data[4];
    char name[255];
  };
  struct Branches {
    TBranch *b0, *b1, *b2, *b3, *b4;
  };
  struct MonitorConfig {
  public:
    MonitorConfig() : file(0), tree(0) {}
    TFile*    file;
    TTree*    tree;
    Branches  branches;
  };
}
static Entry e;// = new Entry();

#define USE_SPLITTING

/// Initializing constructor
Monitor::Monitor(const std::string& histogram_fname) : m_fileName(histogram_fname)  {
  m_cfg = new MonitorConfig();
  if ( !m_fileName.empty() ) {
    m_cfg->file = new TFile(m_fileName.c_str(),"RECREATE","Multi-thread analysis");
    m_cfg->tree = new TTree("Data","Data",99);
    Branches& br = m_cfg->branches;
#ifdef USE_SPLITTING
    br.b0 = m_cfg->tree->GetBranch("name");
    if ( 0 == br.b0 )   {
      br.b0 = m_cfg->tree->Branch("name",e.name,"name/C");
    }
    br.b1 = m_cfg->tree->GetBranch("id");
    if ( 0 == br.b1 )   {
      br.b1 = m_cfg->tree->Branch("id",&e.data[0],"id/L");
    }
    br.b2 = m_cfg->tree->GetBranch("serial");
    if ( 0 == br.b2 )   {
      br.b2 = m_cfg->tree->Branch("serial",&e.data[1],"serial/L");
    }
    br.b3 = m_cfg->tree->GetBranch("event");
    if ( 0 == br.b3 )   {
      br.b3 = m_cfg->tree->Branch("event",&e.data[2],"event/L");
    }
    br.b4 = m_cfg->tree->GetBranch("time");
    if ( 0 == br.b4 )   {
      br.b4 = m_cfg->tree->Branch("time",&e.data[3],"time/L");
    }
#else
    m_cfg->branches.b0 = m_cfg->tree->Branch("DATA",&e,"id/L:serial/L:event/L:time/L:name/C");
#endif
  }
}

/// Default destructor
Monitor::~Monitor()   {
  if ( m_cfg )  {
    m_cfg->tree->Write();
    m_cfg->file->Close();
    delete m_cfg->file;
    delete m_cfg;
  }
  m_cfg = 0;
}


void Monitor::save() {
}

/// Monitor executor
void Monitor::monitor(Executor* exec, EventContext* context, const struct timeval& start, const struct timeval& stop) {
  if ( exec )   {
    ::strncpy(e.name,exec->factory()->name().c_str(),sizeof(e.name));
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
    m_cfg->tree->Fill();
  }
}
