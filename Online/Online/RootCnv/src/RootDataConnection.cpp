// $Id: RootDataConnection.cpp,v 1.3 2009-12-16 10:42:57 frankb Exp $
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "RootDataConnection.h"
#include "RootCnv/RootRefs.h"
#include "TFile.h"
#include "TTree.h"
#include "TClass.h"
#include "TBranch.h"


using namespace Gaudi;
using namespace std;
typedef const string& CSTR;

namespace {
  struct MyFile : public TFile {
    Long64_t offset() const { return this->TFile::GetRelOffset(); }
  };
}

/// Standard constructor
RootDataConnection::RootDataConnection(const IInterface* owner, const std::string& fname)
  : IDataConnection(owner,fname)
{ //               01234567890123456789012345678901234567890
  // Check if FID: A82A3BD8-7ECB-DC11-8DC0-000423D950B0
  if ( fname.length() == 36 && fname[8]=='-'&&fname[13]=='-'&&fname[18]=='-'&&fname[23]=='-' ) {
    m_name = "FID:"+fname;
  }
  m_age = 0;
  m_file = 0;
  m_dbBranch=m_cntBranch=m_lnkBranch=m_pathBranch=0;
}

/// Standard destructor      
RootDataConnection::~RootDataConnection()   {
}

StatusCode RootDataConnection::connectRead()  {
  m_file = TFile::Open(m_pfn.c_str());
  if ( m_file && !m_file->IsZombie() )   {
    return readRefs();
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::readRefs() {
  TTree* t = (TTree*)m_file->Get("Event");
  if ( !t ) {
    TDirectory::TContext ctxt(m_file);
    t = new TTree("Event","Root Event data");
    m_dbBranch = t->Branch("Databases",0,"C");
    m_cntBranch = t->Branch("Containers",0,"C");
    m_lnkBranch = t->Branch("Links",0,"C");
    m_pathBranch = t->Branch("Paths",0,"C");
  }
  else {
    size_t i;
    Long64_t n;
    char text[2048];
    m_dbBranch = t->GetBranch("Databases");
    m_cntBranch = t->GetBranch("Containers");
    m_lnkBranch = t->GetBranch("Links");
    m_pathBranch = t->GetBranch("Paths");    
    m_dbBranch->SetAddress(text);
    for(i=0, n=m_dbBranch->GetEntries(); i<n; ++i) {
      if ( m_dbBranch->GetEntry(i)>0 ) m_dbs.push_back(text);
    }
    m_cntBranch->SetAddress(text);
    for(i=0, n=m_cntBranch->GetEntries(); i<n; ++i) {
      if ( m_cntBranch->GetEntry(i)>0 ) m_conts.push_back(text);
    }
    m_lnkBranch->SetAddress(text);
    for(i=0, n=m_lnkBranch->GetEntries(); i<n; ++i) {
      if ( m_lnkBranch->GetEntry(i)>0 ) m_links.push_back(text);
    }
    m_pathBranch->SetAddress(text);
    for(i=0, n=m_pathBranch->GetEntries(); i<n; ++i) {
      if ( m_pathBranch->GetEntry(i)>0 ) m_paths.push_back(text);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RootDataConnection::saveRefs() {
  StatusCode sc = StatusCode::SUCCESS;
  if ( m_dbBranch && m_cntBranch && m_lnkBranch && m_pathBranch ) {
    if ( isWritable() ) {
      size_t i;
      Long64_t n;
      for(i=m_dbBranch->GetEntries(), n=m_dbs.size(); i<n; ++i) {
	m_dbBranch->SetAddress((char*)m_dbs[i].c_str());
	if ( m_dbBranch->Fill() <= 1) sc = StatusCode::FAILURE;
      }
      for(i=m_cntBranch->GetEntries(), n=m_conts.size(); i<n; ++i) {
	m_cntBranch->SetAddress((char*)m_conts[i].c_str());
	if ( m_cntBranch->Fill()<=1 ) sc = StatusCode::FAILURE;
      }
      for(i=m_lnkBranch->GetEntries(), n=m_links.size(); i<n; ++i) {
	m_lnkBranch->SetAddress((char*)m_links[i].c_str());
	if ( m_lnkBranch->Fill()<=1 ) sc = StatusCode::FAILURE;
      }
      for(i=m_pathBranch->GetEntries(), n=m_paths.size(); i<n; ++i) {
	m_pathBranch->SetAddress((char*)m_paths[i].c_str());
	if ( m_pathBranch->Fill()<=1 ) sc = StatusCode::FAILURE;
      }
    }
  }
  return sc;
}

StatusCode RootDataConnection::connectWrite(IoType typ)  {
  switch(typ)  {
  case CREATE:
    m_file = TFile::Open(m_pfn.c_str(),"CREATE","Root event data");
    break;
  case RECREATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"RECREATE","Root event data");
    break;
  case UPDATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"UPDATE","Root event data");
    if ( m_file && !m_file->IsZombie() )  {
      return readRefs();
    }
    break;
  default:
    m_file = 0;
    break;
  }
  return 0==m_file ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

/// Read raw byte buffer from input stream
StatusCode RootDataConnection::read(void* const data, size_t len)  {
  if ( m_file ) {
    resetAge();
    if ( size_t(m_file->GetBytesRead())+len > size_t(m_file->GetSize()) ) {
      return StatusCode::FAILURE;
    }
    if ( m_file->ReadBuffer((char*)data,len)==0 ) {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

/// Write raw byte buffer to output stream
StatusCode RootDataConnection::write(const void* data, int len)  {
  if ( m_file ) {
    resetAge();
    if ( m_file->WriteBuffer((const char*)data,len)==0 ) 
      return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Seek on the file described by ioDesc. Arguments as in ::seek()
long long int RootDataConnection::seek(long long int offset, int how)  {
  if ( m_file ) {
    resetAge();
    switch(how) {
    case SEEK_SET:
      m_file->Seek(offset,TFile::kBeg);
      return ((MyFile*)m_file)->offset();
    case SEEK_CUR:
      m_file->Seek(offset,TFile::kCur);
      return ((MyFile*)m_file)->offset();
    case SEEK_END:
      m_file->Seek(offset,TFile::kEnd);
      return ((MyFile*)m_file)->offset();
    default:
      break;
    }
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::disconnect()    {
  if ( m_file ) {
    if ( !m_file->IsZombie() )   {
      if ( m_file->IsWritable() ) {
	saveRefs().ignore();
      }
      m_file->Close();
    }
    delete m_file;
    m_file = 0;
  }
  return StatusCode::SUCCESS;
}

TBranch* RootDataConnection::getBranch(const std::string& n) {
  TTree* t = (TTree*)m_file->Get("Event");
  return t ? t->GetBranch(n.c_str()) : 0;
}

/// Access data branch by name
TBranch* RootDataConnection::getBranch(TClass* cl, CSTR n) {
  TTree* t = (TTree*)m_file->Get("Event");
  if ( !t ) {
    TDirectory::TContext ctxt(m_file);
    t = new TTree("Event","Root Event data");
  }
  TBranch* b = t->GetBranch(n.c_str());
  if ( !b && m_file->IsWritable() ) {
    void* ptr = 0;
    b = t->Branch(n.c_str(),cl->GetName(),ptr);
  }
  return b;
}

int RootDataConnection::makePath(const std::string& p) {
  int cnt = 0;
  StringMap::iterator ip;
  for(ip=m_paths.begin();ip!=m_paths.end();++ip,++cnt)
    if( (*ip) == p ) return cnt;

  m_paths.push_back(p);
  return m_paths.size()-1;
}

void RootDataConnection::makeRef(IRegistry* pR, RootRef& ref) {
  int cdb=0, ccnt=0, clnk=0;
  StringMap::iterator idb, icnt, ilnk;
  IOpaqueAddress* pA = pR->address();
  const string& name = pR->name();
  const string& db = pA->par()[0];
  const string& cnt = pA->par()[1];

  for(idb=m_dbs.begin(); idb!=m_dbs.end();++idb,++cdb)
    if( (*idb) == db ) break;

  for(icnt=m_conts.begin(); icnt!=m_conts.end();++icnt,++ccnt)
    if( (*icnt) == cnt ) break;

  for(ilnk=m_links.begin(); ilnk!=m_links.end();++ilnk,++clnk)
    if( (*ilnk) == name ) break;

  if ( idb == m_dbs.end() ) {
    cdb = m_dbs.size();
    m_dbs.push_back(pA->par()[0]);
  }
  if ( icnt == m_conts.end() ) {
    ccnt = m_conts.size();
    m_conts.push_back(pA->par()[1]);
  }
  if ( ilnk == m_links.end() ) {
    clnk = m_links.size();
    m_links.push_back(name);
  }
  ref.dbase     = cdb;
  ref.container = ccnt;
  ref.link      = clnk;
  ref.clid      = pA->clID();
  ref.svc       = pA->svcType();
}

static string s_empty;

const std::string& RootDataConnection::getPath(int which) {
  return (which>0)&&(size_t(which)<m_paths.size()) ? *(m_paths.begin()+which) : s_empty;
}

const std::string& RootDataConnection::getDb(int which) {
  return (which>0)&&(size_t(which)<m_dbs.size()) ? *(m_dbs.begin()+which) : s_empty;
}

const std::string& RootDataConnection::getCont(int which) {
  return (which>0)&&(size_t(which)<m_conts.size()) ? *(m_conts.begin()+which) : s_empty;
}

const std::string& RootDataConnection::getLink(int which) {
  return (which>0)&&(size_t(which)<m_links.size()) ? *(m_links.begin()+which) : s_empty;
}
