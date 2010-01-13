// $Id: RootDataConnection.cpp,v 1.6 2010-01-13 18:34:21 frankb Exp $
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "RootDataConnection.h"
#include "RootCnv/RootRefs.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TBranch.h"


using namespace Gaudi;
using namespace std;
typedef const string& CSTR;

static string s_empty;
static int _print = 0;

/// Standard constructor
RootDataConnection::RootDataConnection(const IInterface* owner, const string& fname)
  : IDataConnection(owner,fname)
{ //               01234567890123456789012345678901234567890
  // Check if FID: A82A3BD8-7ECB-DC11-8DC0-000423D950B0
  if ( fname.length() == 36 && fname[8]=='-'&&fname[13]=='-'&&fname[18]=='-'&&fname[23]=='-' ) {
    m_name = "FID:"+fname;
  }
  m_age = 0;
  m_file = 0;
  m_refs = 0;
  m_dbBranch=m_cntBranch=m_lnkBranch=m_pathBranch=0;
}

/// Standard destructor      
RootDataConnection::~RootDataConnection()   {
}

StatusCode RootDataConnection::connectRead()  {
  m_file = TFile::Open(m_pfn.c_str());
  if ( m_file && !m_file->IsZombie() )   {
    if ( _print > 0 ) cout << "Opened file " << m_pfn << " in mode READ." << endl;
    if ( _print > 1 ) m_file->ls();
    if ( _print > 2 ) m_file->Print();
    m_refs = (TTree*)m_file->Get("Refs");
    if ( m_refs ) {
      return readRefs();
    }
  }
  else if ( m_file ) {
    delete m_file;
    m_file = 0;
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::readRefs() {
  if ( m_refs ) {
    TLeaf* l;
    Long64_t i, n;
    char text[2048];
    m_dbBranch = m_refs->GetBranch("Databases");
    m_cntBranch = m_refs->GetBranch("Containers");
    m_lnkBranch = m_refs->GetBranch("Links");
    m_pathBranch = m_refs->GetBranch("Paths");    
    m_dbBranch->SetAddress(text);
    l = m_dbBranch->GetLeaf("Databases");
    for(i=0, n=m_dbBranch->GetEntries(); i<n; ++i) {
      if ( m_dbBranch->GetEntry(i)>0 ) m_dbs.push_back((char*)l->GetValuePointer());
      if ( _print > 2 ) cout << "Dbase:" << (char*)l->GetValuePointer() << endl;
    }
    m_cntBranch->SetAddress(text);
    l = m_cntBranch->GetLeaf("Containers");
    for(i=0, n=m_cntBranch->GetEntries(); i<n; ++i) {
      if ( m_cntBranch->GetEntry(i)>0 ) m_conts.push_back((char*)l->GetValuePointer());
      if ( _print > 2 ) cout << "Cont:" << (char*)l->GetValuePointer() << endl;
    }
    m_lnkBranch->SetAddress(text);
    l = m_lnkBranch->GetLeaf("Links");
    for(i=0, n=m_lnkBranch->GetEntries(); i<n; ++i) {
      if ( m_lnkBranch->GetEntry(i)>0 ) m_links.push_back((char*)l->GetValuePointer());
      if ( _print > 2 ) cout << "Link:" << (char*)l->GetValuePointer() << endl;
    }
    m_pathBranch->SetAddress(text);
    l = m_pathBranch->GetLeaf("Paths");
    for(i=0, n=m_pathBranch->GetEntries(); i<n; ++i) {
      if ( m_pathBranch->GetEntry(i)>0 ) m_paths.push_back((char*)l->GetValuePointer());
      if ( _print > 2 ) cout << "Path:" << (char*)l->GetValuePointer() << endl;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::saveRefs() {
  StatusCode sc = StatusCode::SUCCESS;
  if ( m_refs ) {
    TDirectory::TContext ctxt(m_file);
    if ( !m_dbBranch )   m_dbBranch = m_refs->Branch("Databases",0,"Databases/C");
    if ( !m_cntBranch )  m_cntBranch = m_refs->Branch("Containers",0,"Containers/C");
    if ( !m_lnkBranch )  m_lnkBranch = m_refs->Branch("Links",0,"Links/C");
    if ( !m_pathBranch ) m_pathBranch = m_refs->Branch("Paths",0,"Paths/C");
  }
  if ( m_dbBranch && m_cntBranch && m_lnkBranch && m_pathBranch ) {
    Long64_t i, n;
    if ( _print > 2 ) cout << "Saving reference tables...." << endl;
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
    if ( _print > 2 ) cout << "....Done" << endl;
    return sc;
  }
  return StatusCode::FAILURE;
}

StatusCode RootDataConnection::connectWrite(IoType typ)  {
  switch(typ)  {
  case CREATE:
    m_file = TFile::Open(m_pfn.c_str(),"CREATE","Root event data");
    m_refs = new TTree("Refs","Root reference data");
    if ( _print > 0 ) cout << "Opened file " << m_pfn << " in mode CREATE." << endl;
    break;
  case RECREATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"RECREATE","Root event data");
    if ( _print > 0 ) cout << "Opened file " << m_pfn << " in mode RECREATE." << endl;
    m_refs = new TTree("Refs","Root reference data");
    break;
  case UPDATE:
    resetAge();
    m_file = TFile::Open(m_pfn.c_str(),"UPDATE","Root event data");
    if ( _print > 0 ) cout << "Opened file " << m_pfn << " in mode UPDATE." << endl;
    if ( m_file && !m_file->IsZombie() )  {
      m_refs = (TTree*)m_file->Get("Refs");
      if ( m_refs ) {
	return readRefs();
      }
      TDirectory::TContext ctxt(m_file);
      m_refs = new TTree("Refs","Root reference data");
      return StatusCode::SUCCESS;
    }
    break;
  default:
    m_refs = 0;
    m_file = 0;
    break;
  }
  return 0==m_file ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

StatusCode RootDataConnection::disconnect()    {
  if ( m_file ) {
    if ( !m_file->IsZombie() )   {
      if ( m_file->IsWritable() ) {
	TDirectory::TContext ctxt(m_file);
	if ( m_refs ) {
	  saveRefs().ignore();
	  m_refs->Write();
	}
	for(Sections::iterator i=m_sections.begin(); i!= m_sections.end();++i) {
	  if ( (*i).second ) {
	    (*i).second->Write();
	    if ( _print > 0 ) cout << "Disconnect section " << (*i).first << " " << (*i).second->GetName() << endl;
	  }
	}
	m_sections.clear();
      }
      if ( _print > 1 ) m_file->ls();
      if ( _print > 2 ) m_file->Print();
      m_file->Close();
    }
    if ( _print > 0 ) cout << "Disconnected file " << m_pfn << " " << m_file->GetName() << endl;
    delete m_file;
    m_file = 0;
  }
  return StatusCode::SUCCESS;
}

TTree* RootDataConnection::getSection(const std::string& section, bool create) {
  TTree* t = m_sections[section];
  if ( !t ) {
    t = (TTree*)m_file->Get(section.c_str());
    if ( !t && create ) {
      TDirectory::TContext ctxt(m_file);
      t = new TTree(section.c_str(),"Root Event data");
    }
    if ( t ) {
      m_sections[section] = t;
    }
  }
  return t;
}


TBranch* RootDataConnection::getBranch(const string& section, const string& n) {
  string m = n;
  TTree* t = getSection(section);
  TBranch* b = t ? t->GetBranch(m.c_str()) : 0;
  if ( b ) b->SetAutoDelete(kFALSE);
  return b;
}

/// Access data branch by name
TBranch* RootDataConnection::getBranch(const string& section, CSTR n, TClass* cl) {
  string m = n;
  TTree* t = getSection(section,true);
  TBranch* b = t->GetBranch(m.c_str());
  if ( !b && m_file->IsWritable() ) {
    void* ptr = 0;
    b = t->Branch(m.c_str(),cl->GetName(),&ptr);
  }
  if ( b ) b->SetAutoDelete(kFALSE);
  return b;
}

int RootDataConnection::makePath(const string& p) {
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

  ref.entry = -1;
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
