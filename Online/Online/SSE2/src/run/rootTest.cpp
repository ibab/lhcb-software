// C/C++ include files
#include <sys/time.h>
#include <map>

// ROOT includes
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TROOT.h"

namespace Framework {}

using namespace Framework;
using namespace std;

namespace Framework {
  struct Entry {
    Long64_t  id;
    Long64_t  time;
  };

  struct MonitorConfig {
  public:
    MonitorConfig() : file(0), tree(0) {}
    TFile*    file;
    TTree*    tree;
    vector<Entry*> entries;
  };
}

#if defined(NO_ACLIK)
int main(int , char**) {
#else
int rootTest() {
#endif
  MonitorConfig* cfg = new MonitorConfig();
  cfg->file = new TFile("test.root","RECREATE","Multi-thread analysis");
  cfg->tree = new TTree("Data","Data",99);

  Entry e;
  Long64_t id=0, time=0;
  TBranch* b1 = cfg->tree->GetBranch("id");
  TBranch* b2 = cfg->tree->GetBranch("time");
  if ( 0 == b1 )  {
    b1 = cfg->tree->Branch("id",(void*)&id,"id/L");
    b2 = cfg->tree->Branch("time",(void*)&time,"time/L");
  }
  for(int i=0; i<1000;++i) {
    Entry* q = new Entry();
    q->id = q->time = i;
    cfg->entries.push_back(q);
  }
  for(vector<Entry*>::iterator i=cfg->entries.begin(); i!=cfg->entries.end();++i) {
    id = (*i)->id;
    time = (*i)->time;
    delete *i;
    cfg->tree->Fill();
  }
  cfg->entries.clear();

  cfg->tree->Write();
  cfg->file->Close();
  delete cfg->file;
  delete cfg;
  cfg = 0;
  return 1;
}
