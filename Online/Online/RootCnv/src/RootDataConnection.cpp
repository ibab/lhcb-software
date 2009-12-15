// $Id: RootDataConnection.cpp,v 1.1 2009-12-15 15:37:25 frankb Exp $
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
}

/// Standard destructor      
RootDataConnection::~RootDataConnection()   {
}

StatusCode RootDataConnection::connectRead()  {
  m_file = TFile::Open(m_pfn.c_str());
  return 0==m_file ? StatusCode::FAILURE : StatusCode::SUCCESS;
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
    if ( m_file->ReadBuffer((char*)data,len)==0 )
      return StatusCode::SUCCESS;
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
    if ( !m_file->IsZombie() ) m_file->Close();
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
  StringMap::iterator ip;
  for(ip=m_paths.begin();ip!=m_paths.end();++ip)
    if( (*ip).second == p ) break;

  if ( ip == m_paths.end() ) {
    ip = m_paths.insert(make_pair(m_paths.size(),p)).first;
  }
  return (*ip).first;
}

void RootDataConnection::makeRef(IRegistry* pR, RootRef& ref) {
  StringMap::iterator idb, icnt, ilnk;
  IOpaqueAddress* pA = pR->address();
  const string& name = pR->name();
  const string& db = pA->par()[0];
  const string& cnt = pA->par()[1];

  for(idb=m_dbs.begin();idb!=m_dbs.end();++idb)
    if( (*idb).second == db ) break;

  for(icnt=m_conts.begin();icnt!=m_conts.end();++icnt)
    if( (*icnt).second == cnt ) break;

  for(ilnk=m_links.begin();ilnk!=m_links.end();++ilnk)
    if( (*ilnk).second == name ) break;

  if ( idb == m_dbs.end() ) {
    idb = m_dbs.insert(make_pair(m_dbs.size(),pA->par()[0])).first;
  }
  if ( icnt == m_conts.end() ) {
    icnt = m_conts.insert(make_pair(m_conts.size(),pA->par()[1])).first;
  }
  if ( ilnk == m_links.end() ) {
    ilnk = m_links.insert(make_pair(m_links.size(),name)).first;
  }
  ref.dbase     = (*idb).first;
  ref.container = (*icnt).first;
  ref.link      = (*ilnk).first;
  ref.clid      = pA->clID();
  ref.svc       = pA->svcType();
}

static string s_empty;

const std::string& RootDataConnection::getPath(int which) {
  StringMap::iterator ipaths = m_paths.find(which);
  return ipaths==m_paths.end() ? s_empty : (*ipaths).second;
}

const std::string& RootDataConnection::getDb(int which) {
  StringMap::iterator idb  = m_dbs.find(which);
  return idb==m_dbs.end() ? s_empty : (*idb).second;
}

const std::string& RootDataConnection::getCont(int which) {
  StringMap::iterator icnt = m_conts.find(which);
  return icnt==m_conts.end() ? s_empty : (*icnt).second;
}

const std::string& RootDataConnection::getLink(int which) {
  StringMap::iterator ilnk = m_links.find(which);
  return ilnk==m_links.end() ? s_empty : (*ilnk).second;
}
