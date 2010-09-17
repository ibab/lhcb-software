// $Id: RootDataConnection.cpp,v 1.15 2010-09-17 09:40:02 frankb Exp $
//====================================================================
//	RootDataConnection.cpp
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootDataConnection.cpp,v 1.15 2010-09-17 09:40:02 frankb Exp $

// Framework include files
#include "RootDataConnection.h"
#include "RootUtils.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
// ROOT include files
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


static bool match_wild(const char *str, const char *pat)    {
  //
  // Credits: Code from Alessandro Felice Cantatore.
  // 
  static char table[256];
  static bool first = true;
  const char *s, *p;
  bool star = false;
  if ( first ) {
    for (int i = 0; i < 256; ++i) table[i] = char(i);
    first = false;
  }
 loopStart:
  for (s = str, p = pat; *s; ++s, ++p) {
    switch (*p) {
    case '?':
      if (*s == '.') goto starCheck;
      break;
    case '*':
      star = true;
      str = s, pat = p;
      do { ++pat; } while (*pat == '*');
      if (!*pat) return true;
      goto loopStart;
    default:
      if ( *(table+*s) != *(table+*p) )
	goto starCheck;
      break;
    } /* endswitch */
  } /* endfor */
  while (*p == '*') ++p;
  return (!*p);
  
 starCheck:
  if (!star) return false;
  str++;
  goto loopStart;
}

// Standard constructor
RootConnectionSetup::RootConnectionSetup() : refCount(1), m_msgSvc(0)
{
}

// Standard destructor      
RootConnectionSetup::~RootConnectionSetup() {
  deletePtr(m_msgSvc);
}

// Increase reference count
void RootConnectionSetup::addRef() {
  ++refCount;
}

// Decrease reference count
void RootConnectionSetup::release() {
  int tmp = --refCount;
  if ( tmp <= 0 ) {
    delete this;
  }
}

// Set message service reference
void RootConnectionSetup::setMessageSvc(MsgStream* m) {
  MsgStream* tmp = m_msgSvc;
  m_msgSvc = m;
  deletePtr(tmp);
}

// Standard constructor
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

// Standard destructor      
RootDataConnection::~RootDataConnection()   {
  m_setup->release();
  releasePtr(m_tool);
}

// Save TTree access statistics if required
void RootDataConnection::saveStatistics(CSTR statisticsFile) {
  if ( m_statistics ) {
    m_statistics->Print();
    if ( !statisticsFile.empty() )
      m_statistics->SaveAs(statisticsFile.c_str());
    deletePtr(m_statistics);
  }
}

// Enable TTreePerStats
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

// Create file access tool to encapsulate POOL compatibiliy
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

// Connect the file in READ mode
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
      m_mergeFIDs.clear();
      for(size_t i=0, n=m_params.size(); i<n; ++i) {
	if ( m_params[i].first == "FID" )  {
	  m_mergeFIDs.push_back(m_params[i].second);
	  if ( m_params[i].second != m_fid )    {
	    msgSvc() << MSG::DEBUG << "Check FID param:" << m_params[i].second << endmsg;
	    //if ( m_fid == m_pfn ) {
	    m_fid = m_params[i].second;
	    //}
	  }
	}
      }
      if ( !need_fid && fid != m_fid ) {
	msgSvc() << MSG::ERROR << "FID mismatch:" << fid << "(Catalog) != " << m_fid << "(file)" << endmsg
		 << "for PFN:" << m_pfn << endmsg;
	return StatusCode::FAILURE;
      }
      msgSvc() << MSG::DEBUG << "Using FID " << m_fid << " from params table...." << endmsg
	       << "for PFN:" << m_pfn << endmsg;
      return sc;
    }
  }
  else if ( m_file ) {
    deletePtr(m_file);
  }
  return StatusCode::FAILURE;
}

// Open data stream in write mode
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

// Release data stream and release implementation dependent resources
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
    deletePtr(m_file);
    releasePtr(m_tool);
  }
  return StatusCode::SUCCESS;
}

// Access TTree section from section name. The section is created if required.
TTree* RootDataConnection::getSection(CSTR section, bool create) {
  TTree* t = m_sections[section];
  if ( !t ) {
    t = (TTree*)m_file->Get(section.c_str());
    if ( !t && create ) {
      TDirectory::TContext ctxt(m_file);
      t = new TTree(section.c_str(),"Root data for Gaudi");
    }
    if ( t ) {
      int cacheSize = m_setup->cacheSize;
      if ( create ) {
	//t->SetAutoFlush(100);
      }
      if ( section == m_setup->loadSection && cacheSize>-2 )  {
	MsgStream& msg = msgSvc();
	int learnEntries = m_setup->learnEntries;
	t->SetCacheSize(cacheSize);
	t->SetCacheLearnEntries(learnEntries);
	const StringVec& vB = m_setup->vetoBranches;
	const StringVec& cB = m_setup->cacheBranches;
	msg << MSG::DEBUG;
	if ( create ) {
	  msg << "Tree:" << section << "Setting up tree cache:" << cacheSize << endmsg;
	}
	else {
	  msg << "Tree:" << section << " Setting up tree cache:" << cacheSize << " Add all branches." << endmsg;
	  msg << "Tree:" << section << " Learn for " << learnEntries << " entries." << endmsg;
	  if ( cB.size()==1 && cB[0]=="*" ) {
	    t->AddBranchToCache("*",kTRUE);
	  }
	  else if ( cB.size()==0 && !(vB.size()>0 && vB[0]=="*")) {
	    t->AddBranchToCache("*",kTRUE);
	  }
	  else {
	    bool add = false, veto = false;
	    StringVec::const_iterator i;
	    // Add all branches, which should be cached
	    for(TIter it(t->GetListOfBranches()); it.Next(); )  {
	      const char* n = ((TNamed*)(*it))->GetName();
	      for(i=vB.begin(); add && i!=vB.end();++i) {
		if ( !match_wild(n,(*i).c_str()) ) continue;
		veto = true;
		break;
	      }
	      for(i=cB.begin(); add && i!=cB.end();++i) {
		if ( !match_wild(n,(*i).c_str()) ) continue;
		add = true;
		break;
	      }
	      if ( add && !veto ) {
		msg << "Add " << n << " to branch cache." << endmsg;
		t->AddBranchToCache(n,kTRUE);
	      }
	    }
	  }
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
  if ( !b && cl && m_file->IsWritable() ) {
    void* ptr = 0;
    b = t->Branch(n.c_str(),cl->GetName(),&ptr);
  }
  if ( b )   {
    b->SetAutoDelete(kFALSE);
  }
  return b;
}

// Convert path string to path index
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

// Empty string reference
CSTR RootDataConnection::empty() const { 
  return s_empty;
}

// Save object of a given class to section and container
pair<int,unsigned long> 
RootDataConnection::saveObj(CSTR section, CSTR cnt, TClass* cl, DataObject* pObj,bool fill) {
  DataObjectPush push(pObj);
  return save(section,cnt,cl,pObj,fill);
}

// Save object of a given class to section and container
pair<int,unsigned long> 
RootDataConnection::save(CSTR section, CSTR cnt, TClass* cl, void* pObj, bool fill_missing) {
  TBranch* b = getBranch(section, cnt, cl);
  if ( b ) {
    Long64_t evt = b->GetEntries();
    if ( 0 == evt && fill_missing ) {
      Long64_t nevt = b->GetTree()->GetEntries();
      if ( nevt > evt ) {
	void* p = 0;
	b->SetAddress(&p);
	for(Long64_t i=0; i<nevt; ++i) {
	  b->Fill();
	}
	msgSvc() << MSG::INFO << "Added " << long(b->GetEntries()) 
		 << " NULL entries to:" << cnt << endmsg;
      }
    }
    b->SetAddress(&pObj);
    return make_pair(b->Fill(),evt);
  }
  else if ( 0 != pObj ) {
    msgSvc() << MSG::ERROR << "Failed to access branch " << m_name << "/" << cnt << endmsg;
  }
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
      if ( section == m_setup->loadSection ) {
	TTree* t = b->GetTree();
	if ( Long64_t(entry) != t->GetReadEntry() ) {
	  t->LoadTree(Long64_t(entry));
	}
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
  //size_t idx = cont.find('/',1);
  //string container = cont[0]=='/' ? cont.substr(1,idx==string::npos?idx:idx-1) : cont;
  MergeSections::const_iterator i=m_mergeSects.find(container);
  if ( i != m_mergeSects.end() ) {
    size_t cnt = 0;
    const ContainerSections& s = (*i).second;
    for(ContainerSections::const_iterator j=s.begin(); j != s.end(); ++j,++cnt) {
      const ContainerSection& c = *j;
      if ( entry >= c.start && entry < (c.start+c.length) ) {
	if ( m_linkSects.size() > cnt ) {
	  if ( msgSvc().isActive() ) {
	    msgSvc() << MSG::VERBOSE << "MergeSection for:" << container 
		     << "  [" << entry << "]" << endmsg
		     << "FID:" << m_fid << " -> PFN:" << m_pfn << endmsg;
	  }
	  return make_pair(&(m_linkSects[cnt]), &c);
	}
      }
    }
  }
  msgSvc() << MSG::ERROR << "Return INVALID MergeSection for:" << container 
	   << "  [" << entry << "]" << endmsg
	   << "FID:" << m_fid << " -> PFN:" << m_pfn << endmsg;
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

