// $Id: RootDataConnection.cpp,v 1.10 2010-08-24 13:21:01 frankb Exp $
#include "RootDataConnection.h"
#include "RootUtils.h"

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TBranch.h"
#include "TTreePerfStats.h"

using namespace Gaudi;
using namespace std;
typedef const string& CSTR;

static string s_empty;
static string s_local = "<localDB>";

#ifdef __POOL_COMPATIBILITY
#include "PoolTool.h"
#endif
#include "RootTool.h"

/// Standard constructor
RootConnectionSetup::RootConnectionSetup() : refCount(1), m_msgSvc(0)
{
}

/// Standard destructor      
RootConnectionSetup::~RootConnectionSetup() {
  if ( m_msgSvc ) delete m_msgSvc;
  m_msgSvc = 0;
}

/// Increase reference count
void RootConnectionSetup::addRef() {
  ++refCount;
}

/// Decrease reference count
void RootConnectionSetup::release() {
  int tmp = --refCount;
  if ( tmp <= 0 ) {
    delete this;
  }
}

/// Set message service reference
void RootConnectionSetup::setMessageSvc(MsgStream* m) {
  MsgStream* tmp = m_msgSvc;
  m_msgSvc = m;
  if ( tmp ) delete tmp;
}

/// Standard constructor
RootDataConnection::RootDataConnection(const IInterface* owner, CSTR fname, RootConnectionSetup* setup)
  : IDataConnection(owner,fname), m_setup(setup), m_statistics(0), m_tool(0)
{ //               01234567890123456789012345678901234567890
  // Check if FID: A82A3BD8-7ECB-DC11-8DC0-000423D950B0
  if ( fname.length() == 36 && fname[8]=='-'&&fname[13]=='-'&&fname[18]=='-'&&fname[23]=='-' ) {
    m_name = "FID:"+fname;
  }
  m_setup->addRef();
  m_age  = 0;
  m_file = 0;
  m_refs = 0;
}

/// Standard destructor      
RootDataConnection::~RootDataConnection()   {
  m_setup->release();
  releasePtr(m_tool);
}

/// Save TTree access statistics if required
void RootDataConnection::saveStatistics(CSTR statisticsFile) {
  if ( m_statistics ) {
    m_statistics->Print();
    if ( !statisticsFile.empty() ) {
      m_statistics->SaveAs(statisticsFile.c_str());
    }
    delete m_statistics;
    m_statistics = 0;
  }
}

/// Enable TTreePerStats
void RootDataConnection::enableStatistics(CSTR section) {
  if ( 0 == m_statistics ) {
    TTree* t=getSection(section,false);
    if ( t ) {
      m_statistics = new TTreePerfStats((section+"_ioperf").c_str(),t);
      return;
    }
    msgSvc() << MSG::WARNING << "Failed to enable perfstats for tree:" << section << endmsg;
    return;
  }
  msgSvc() << MSG::INFO << "Perfstats are ALREADY ENABLED." << endmsg;
}

/// Create file access tool to encapsulate POOL compatibiliy
RootDataConnection::Tool* RootDataConnection::makeTool()   {
  releasePtr(m_tool);
  if ( !m_refs ) m_refs = (TTree*)m_file->Get("Refs");
  if ( m_refs )
    m_tool = new RootTool(this);
#ifdef __POOL_COMPATIBILITY
  else if ( m_file->Get("##Links") != 0 )
    m_tool = new PoolTool(this);
#endif
  return m_tool;
}

/// Connect the file in READ mode
StatusCode RootDataConnection::connectRead()  {
  m_file = TFile::Open(m_pfn.c_str());
  if ( m_file && !m_file->IsZombie() )   {
    StatusCode sc = StatusCode::FAILURE;
    msgSvc() << MSG::DEBUG << "Opened file " << m_pfn << " in mode READ. [" << m_fid << "]" << endmsg << MSG::DEBUG;    
    if ( msgSvc().isActive() ) m_file->ls();
    msgSvc() << MSG::VERBOSE;
    if ( msgSvc().isActive() ) m_file->Print();
    if ( makeTool() ) sc = m_tool->readRefs();
    if ( sc.isSuccess() ) {
      bool need_fid = m_fid == m_pfn;
      string fid = m_fid;
      for(size_t i=0, n=m_params.size(); i<n; ++i) {
	if ( m_params[i].first == "FID" && m_params[i].second != m_fid )    {
	  if ( m_fid == m_pfn ) {
	    m_fid = m_params[i].second;
	    continue;
	  }
	}
      }
      if ( !need_fid && fid != m_fid ) {
	msgSvc() << MSG::ERROR << "FID mismatch:" << fid << "(Catalog) != " << m_fid << "(file)" << endmsg;
	return StatusCode::FAILURE;
      }
      msgSvc() << MSG::DEBUG << "Using FID " << m_fid << " from params table...." << endmsg;
      return sc;
    }
  }
  else if ( m_file ) {
    delete m_file;
    m_file = 0;
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::connectWrite(IoType typ)  {
  msgSvc() << MSG::DEBUG;
  switch(typ)  {
  case CREATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"CREATE","Root event data");
    m_refs = new TTree("Refs","Root reference data");
    msgSvc() << "Opened file " << m_pfn << " in mode CREATE. [" << m_fid << "]" << endmsg;
    m_params.push_back(make_pair("PFN",m_pfn));
    if ( m_fid != m_pfn ) {
      m_params.push_back(make_pair("FID",m_fid));
    }
    makeTool();
    break;
  case RECREATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"RECREATE","Root event data");
    msgSvc() << "Opened file " << m_pfn << " in mode RECREATE. [" << m_fid << "]" << endmsg;
    m_refs = new TTree("Refs","Root reference data");
    m_params.push_back(make_pair("PFN",m_pfn));
    if ( m_fid != m_pfn ) {
      m_params.push_back(make_pair("FID",m_fid));
    }
    makeTool();
    break;
  case UPDATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"UPDATE","Root event data");
    msgSvc() << "Opened file " << m_pfn << " in mode UPDATE. [" << m_fid << "]" << endmsg;
    if ( m_file && !m_file->IsZombie() )  {
      if ( makeTool() ) return m_tool->readRefs();
      TDirectory::TContext ctxt(m_file);
      m_refs = new TTree("Refs","Root reference data");
      makeTool();
      return StatusCode::SUCCESS;
    }
    break;
  default:
    m_refs = 0;
    m_file = 0;
    return StatusCode::FAILURE;
  }
  return 0==m_file ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

StatusCode RootDataConnection::disconnect()    {
  if ( m_file ) {
    if ( !m_file->IsZombie() )   {
      if ( m_file->IsWritable() ) {
	msgSvc() << MSG::DEBUG;
	TDirectory::TContext ctxt(m_file);
	if ( m_refs ) {
	  m_tool->saveRefs().ignore();
	  m_refs->Write();
	}
	for(Sections::iterator i=m_sections.begin(); i!= m_sections.end();++i) {
	  if ( (*i).second ) {
	    (*i).second->Write();
	    msgSvc() << "Disconnect section " << (*i).first << " " << (*i).second->GetName() << endmsg;
	  }
	}
	m_sections.clear();
      }
      msgSvc() << MSG::DEBUG;
      if ( msgSvc().isActive() ) m_file->ls();
      msgSvc() << MSG::VERBOSE;
      if ( msgSvc().isActive() ) m_file->Print();
      m_file->Close();
    }
    msgSvc() << MSG::DEBUG << "Disconnected file " << m_pfn << " " << m_file->GetName() << endmsg;
    delete m_file;
    m_file = 0;
    releasePtr(m_tool);
  }
  return StatusCode::SUCCESS;
}

TTree* RootDataConnection::getSection(CSTR section, bool create) {
  TTree* t = m_sections[section];
  if ( !t ) {
    t = (TTree*)m_file->Get(section.c_str());
    if ( !t && create ) {
      TDirectory::TContext ctxt(m_file);
      t = new TTree(section.c_str(),"Root Event data");
    }
    if ( t ) {
      if ( create ) {
	//t->SetAutoFlush(100);
      }
      if ( m_setup->cacheSize>-2 )  {
	t->SetCacheSize(m_setup->cacheSize);
	t->SetCacheLearnEntries(m_setup->learnEntries);
	msgSvc() << MSG::DEBUG;
	if ( create ) {
	  msgSvc() << "Tree:" << section << "Setting up tree cache:" << m_setup->cacheSize << endmsg;
	}
	else {
	  msgSvc() << "Tree:" << section << " Setting up tree cache:" << m_setup->cacheSize << " Add all branches." << endmsg;
	  msgSvc() << "Tree:" << section << " Learn for " << m_setup->learnEntries << " entries." << endmsg;
	  t->AddBranchToCache("*",kTRUE);
	}
      }
      m_sections[section] = t;
    }
  }
  return t;
}

// Access data branch by name: Get existing branch in write mode
TBranch* RootDataConnection::getBranch(CSTR section, CSTR n, TClass* cl) {
  TTree* t = getSection(section,true);
  TBranch* b = t->GetBranch(n.c_str());
  if ( !b && m_file->IsWritable() ) {
    void* ptr = 0;
    b = t->Branch(n.c_str(),cl->GetName(),&ptr);
  }
  if ( b )   {
    b->SetAutoDelete(kFALSE);
  }
  return b;
}

int RootDataConnection::makeLink(CSTR p) {
  int cnt = 0;
  StringVec::iterator ip;
  for(ip=m_links.begin();ip!=m_links.end();++ip,++cnt) {
    if( (*ip) == p ) return cnt;
  }
  m_links.push_back(p);
  return m_links.size()-1;
}

// Access database/file name from saved index
CSTR RootDataConnection::getDb(int which) const {
  if ( (which>=0) && (size_t(which)<m_dbs.size()) )  {
    if ( *(m_dbs.begin()+which) == s_local ) return m_fid;
    return *(m_dbs.begin()+which);
  }
  return s_empty;
}

CSTR RootDataConnection::empty() const { 
  return s_empty;
}

// Save object of a given class to section and container
pair<int,unsigned long> 
RootDataConnection::saveObj(CSTR section, CSTR cnt, TClass* cl, DataObject* pObj) {
  DataObjectPush push(pObj);
  return save(section,cnt,cl,pObj);
}

// Save object of a given class to section and container
pair<int,unsigned long> 
RootDataConnection::save(CSTR section, CSTR cnt, TClass* cl, void* pObj) {
  TBranch* b = getBranch(section, cnt, cl);
  if ( b ) {
    b->SetAddress(&pObj);
    long evt = b->GetEntries();
    if ( evt > 0 && (evt%m_setup->autoFlush)==0 ) {
      if ( cnt == "/Event" ) {
	if ( evt == m_setup->autoFlush ) {
	  b->GetTree()->SetAutoFlush(m_setup->autoFlush);
	  b->GetTree()->SetEntries(evt);
	  b->GetTree()->OptimizeBaskets(40000000,1.,"");
	}
	else   {
	  b->GetTree()->FlushBaskets();
	}
      }
    }
    return make_pair(b->Fill(),evt);
  }
  msgSvc() << MSG::ERROR << "Failed to access branch " << m_name << "/" << cnt << endmsg;
  return make_pair(-1,~0);
}

// Load object
int RootDataConnection::loadObj(CSTR section, CSTR cnt, unsigned long entry, DataObject*& pObj) {
  TBranch* b = getBranch(section,cnt);
  if ( b ) {
    TClass* cl = gROOT->GetClass(b->GetClassName(),kTRUE);
    if ( cl ) {
      pObj = (DataObject*)cl->New();
      DataObjectPush push(pObj);
      b->SetAddress(&pObj);
      TTree* t = b->GetTree();
      if ( Long64_t(entry) != t->GetReadEntry() ) {
	t->LoadTree(Long64_t(entry));
      }
      int nb = b->GetEntry(entry);
      msgSvc() << MSG::VERBOSE;
      if ( msgSvc().isActive() ) {
	msgSvc() << "Load [" << entry << "] --> " << section 
		 << ":" << cnt << "  " << nb << " bytes." 
		 << endmsg;
      }
      return nb;
    }
  }
  return -1;
}

// Access link section for single container and entry
pair<const RootRef*,const RootDataConnection::ContainerSection*> 
RootDataConnection::getMergeSection(const string& container, int entry) const {
  MergeSections::const_iterator i=m_mergeSects.find(container);
  if ( i != m_mergeSects.end() ) {
    size_t cnt = 0;
    const ContainerSections& s = (*i).second;
    for(ContainerSections::const_iterator j=s.begin(); j != s.end(); ++j,++cnt) {
      const ContainerSection& c = *j;
      if ( entry >= c.start && entry < (c.start+c.length) ) {
	if ( m_linkSects.size() > cnt ) {
	  return make_pair(&(m_linkSects[cnt]), &c);
	}
      }
    }
  }
  msgSvc() << MSG::ERROR << "Return INVALID MergeSection:  [" << entry << "]" << endmsg;
  return make_pair((const RootRef*)0,(const ContainerSection*)0);
}

// Create reference object from registry entry
void RootDataConnection::makeRef(IRegistry* pR, RootRef& ref) {
  IOpaqueAddress* pA = pR->address();
  makeRef(pR->name(),pA->clID(),pA->svcType(),pA->par()[0],pA->par()[1],-1,ref);
}

// Create reference object from values
void RootDataConnection::makeRef(CSTR name, long clid, int tech, CSTR dbase, CSTR cnt, int entry, RootRef& ref) {
  string db(dbase);
  int cdb=-1, ccnt=-1, clnk=-1;
  StringVec::iterator idb, icnt, ilnk;
  if ( db == m_fid ) {
    db = s_local;
  }
  ref.entry = entry;
  if ( !db.empty() ) {
    for(cdb=0,idb=m_dbs.begin(); idb!=m_dbs.end();++idb,++cdb)
      if( (*idb) == db ) break;
    if ( idb == m_dbs.end() ) {
      cdb = m_dbs.size();
      m_dbs.push_back(db);
    }
  }
  if ( !cnt.empty() ) {
    for(ccnt=0,icnt=m_conts.begin(); icnt!=m_conts.end();++icnt,++ccnt)
      if( (*icnt) == cnt ) break;
    if ( icnt == m_conts.end() ) {
      ccnt = m_conts.size();
      m_conts.push_back(cnt);
    }
  }
  if ( !name.empty() ) {
    for(clnk=0,ilnk=m_links.begin(); ilnk!=m_links.end();++ilnk,++clnk)
      if( (*ilnk) == name ) break;
    if ( ilnk == m_links.end() ) {
      clnk = m_links.size();
      m_links.push_back(name);
    }
  }
  ref.dbase     = cdb;
  ref.container = ccnt;
  ref.link      = clnk;
  ref.clid      = clid;
  ref.svc       = tech;
  if ( ref.svc == POOL_ROOT_StorageType || 
       ref.svc == POOL_ROOTKEY_StorageType || 
       ref.svc == POOL_ROOTTREE_StorageType ) {
    ref.svc = ROOT_StorageType;
  }
}

