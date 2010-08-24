#ifndef GAUDIROOTCNV_ROOTDATABASEMERGER_H
#define GAUDIROOTCNV_ROOTDATABASEMERGER_H

// C++ includes 
#include <string>
#include <vector>
#include <map>

// Forward declarations
class TTree;
class TFile;
class TBranch;


static bool s_dbg = true;

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  typedef int MergeStatus;
  enum { ERROR=0, SUCCESS=1 };

  struct ContainerSection {
    int start;
    int length;
  };

  /**@class RootDatabaseMerger RootDatabaseMerger.h StorageSvc/RootDatabaseMerger.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class RootDatabaseMerger {

    typedef std::vector<ContainerSection>           ContainerSections;
    typedef std::map<std::string,ContainerSections> DatabaseSections;

    DatabaseSections   m_sections;
    TFile*             m_output;

  public:
    /// Standard constructor
    RootDatabaseMerger();
    /// Default destructor
    virtual ~RootDatabaseMerger();
    /// Check if a database exists
    bool exists(const std::string& fid, bool dbg=true) const;
    /// Create new output file
    MergeStatus create(const std::string& fid);
    /// Attach to existing output file for further merging
    MergeStatus attach(const std::string& fid);
    /// Close output file
    MergeStatus close();
    /// Merge new input to existing output
    MergeStatus merge(const std::string& fid);
    /// Dump collected database sections
    void dumpSections();
    /// Save new sections to the output file
    MergeStatus saveSections();
    /// Create and add new FID to the newly merged file
    MergeStatus createFID();
    /// Copy one single tree from the input file to the output file.
    MergeStatus copyTree(TFile* source, const std::string& name);
    /// Copy single reference branch
    MergeStatus copyBranch(TTree* src_tree,TTree* out_tree,const std::string& name);
    /// Copy one single tree from the input file to the output file.
    MergeStatus copyRefs(TFile* source, const std::string& name);
    /// Add section information for the next merge step
    MergeStatus addSections(TTree* in, TTree* out);
  };
}    // End namespace Gaudi

#endif // GAUDIROOTCNV_ROOTDATABASEMERGER_H


#ifndef GAUDIROOTCNV_ROOTDATABASEMERGER_H
#include "RootDatabaseMerger.h"
#endif // GAUDIROOTCNV_ROOTDATABASEMERGER_H
#include "TUUID.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TKey.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTreeCloner.h"
#include <iostream>
#include <iomanip>


using namespace Gaudi;
using namespace std;

namespace {
  const char* getLinkContainer(char* txt) {
    char* p = ::strstr(txt,"[CNT=");
    if ( p ) {
      char* q = strchr(p+4,']');
      *q = 0;
      return p+5;
    }
    return 0;
  }
  string getRootContainer(const char* t) {
    string r;
    for(const char* p=t; *p; ++p) {
      r += (*p == '/') ? '_' : *p;
    }
    return r;
  }
}

/// Standard constructor
RootDatabaseMerger::RootDatabaseMerger() : m_output(0) {
}

/// Default destructor
RootDatabaseMerger::~RootDatabaseMerger() {
  close();
}

/// Check if a database exists
bool RootDatabaseMerger::exists(const std::string& fid, bool dbg) const {
  Bool_t result = gSystem->AccessPathName(fid.c_str(), kFileExists);
  if ( result == kFALSE ) {
    if ( s_dbg ) cout << "file " << fid << " EXISTS!" << endl;
  }
  else if ( dbg ) {
    cout << "file " << fid << " DOES NOT EXIST!" << endl;
  }
  return result == kFALSE;
}

/// Attach to existing output file for further merging
MergeStatus RootDatabaseMerger::attach(const string& fid) {
  if ( m_output ) {
    cout << "+++ Another output file " << m_output->GetName() << " is already open. Request denied." << endl;
    return ERROR;
  }
  else if ( !exists(fid) ) {
    cout << "+++ Cannot attach output file " << fid << " --- file does not exist." << endl;
    return ERROR;
  }
  m_output = TFile::Open(fid.c_str(),"UPDATE");
  if ( m_output && !m_output->IsZombie() ) {
    if ( s_dbg ) cout << "+++ Opened new output file " << fid << "." << endl;
    return SUCCESS;
  }
  cout << "+++ Failed to open new output file " << fid << "." << endl;
  return ERROR;
}

/// Create new output file
MergeStatus RootDatabaseMerger::create(const string& fid) {
  if ( m_output ) {
    cout << "+++ Another output file " << m_output->GetName() << " is already open. Request denied." << endl;
    return ERROR;
  }
  else if ( exists(fid) ) {
    cout << "+++ Cannot create output file " << fid << " --- file already exists." << endl;
    return ERROR;
  }
  m_output = TFile::Open(fid.c_str(),"RECREATE");
  if ( m_output && !m_output->IsZombie() )     {
    TTree* t1 = new TTree("Sections","Root Section data");
    TTree* t2 = new TTree("Refs","Root Section data");
    if ( t1 && t2 ) {
      t1->Branch("Sections",0,"Sections/C");
      t2->Branch("Links",0,"Links/C");
      t2->Branch("Params",0,"Params/C");
      t2->Branch("Databases",0,"Databases/C");
      t2->Branch("Containers",0,"Containers/C");
      if ( s_dbg ) cout << "+++ Opened new output file " << fid << "." << endl;
      return SUCCESS;
    }
  }
  cout << "+++ Failed to open new output file " << fid << "." << endl;
  return ERROR;
}

/// Close output file
MergeStatus RootDatabaseMerger::createFID() {
  static const char* fmt = "FID=%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";
  if ( m_output ) {
    TTree* t = (TTree*)m_output->Get("Refs");
    if ( t ) {
      char text[256];
      unsigned char buf[16];
      TUUID uuid;
      TBranch* b = t->GetBranch("Params");
      if ( b ) {
	uuid.GetUUID(buf);
	sprintf(text,fmt,
		(*(int*)buf),*(short*)(buf+4),*(short*)(buf+6),
		*(buf+8),*(buf+8),
		*(buf+10),*(buf+11),
		*(buf+12),*(buf+13),
		*(buf+14),*(buf+15));
	b->SetAddress(text);
	b->Fill();
	t->Write();
	if ( s_dbg ) cout << "+++ Added new GUID " << text << " to merge file." << endl;
	return SUCCESS;
      }
    }
  }
  cout << "+++ Failed to add new GUID to merge file." << endl;
  return ERROR;
}

/// Close output file
MergeStatus RootDatabaseMerger::close() {
  if ( m_output ) {
    m_output->Write();
    m_output->Purge();
    if ( s_dbg ) m_output->ls();
    m_output->Close();
    delete m_output;
    m_output = 0;
  }
  return SUCCESS;
}

/// Save new sections to the output file
MergeStatus RootDatabaseMerger::saveSections()     {
  if ( m_output ) {
    int nb, total = 0, nbytes = 0;
    char text[1024];
    TTree* t = (TTree*)m_output->Get("Sections");
    if ( t ) {
      TBranch* b = t->GetBranch("Sections");
      if ( b ) {
	b->SetAddress(text);
	for(DatabaseSections::const_iterator i=m_sections.begin(); i!= m_sections.end();++i) {
	  int cnt = 0;
	  string cont = (*i).first;
	  const ContainerSections& cntSects = (*i).second;
	  for(ContainerSections::const_iterator j=cntSects.begin(); j != cntSects.end();++j, ++cnt) {
	    ::sprintf(text,"[CNT=%s][START=%d][LEN=%d]",
		      cont.c_str(),(*j).start,(*j).length); 
	    nb = t->Fill();
	    ++total;
	    if ( nb > 0 )
	      nbytes += nb;
	    else
	      cout << "+++ Failed to update Sections tree with new entries. [WRITE_ERROR]" << endl;
	  }
	}
	::sprintf(text,"[END-OF-SECTION]");
	nb = t->Fill();
	++total;
	if ( nb > 0 )
	  nbytes += nb;
	else
	  cout << "+++ Failed to update Sections branch with new entries. [WRITE_ERROR]" << endl;
	t->Write();
	if ( s_dbg ) cout << "+++ Added " << total << " Sections entries with " << nbytes << " bytes in total." << endl;    return SUCCESS;
      }
      cout << "+++ Failed to update Sections tree with new entries. [NO_OUTPUT_BRANCH]" << endl;
      return ERROR;
    }
    cout << "+++ Failed to update Sections tree with new entries. [NO_OUTPUT_TREE]" << endl;
    return ERROR;
  }
  cout << "+++ Failed to update Sections tree with new entries. [NO_OUTPUT_FILE]" << endl;
  return ERROR;
}

/// Dump collected database sections
void RootDatabaseMerger::dumpSections() {
  for(DatabaseSections::const_iterator i=m_sections.begin(); i!= m_sections.end();++i) {
    int cnt = 0;
    string prefix = (*i).first;
    const ContainerSections& cntSects = (*i).second;
    for(ContainerSections::const_iterator j=cntSects.begin(); j != cntSects.end();++j, ++cnt) {
      char text[32];
      ::sprintf(text,"'][%d]",cnt); 
      if ( s_dbg ) {
	cout << "+++ " << setw(60) << left << "section['"+prefix+text
	     << "  Start:" << setw(8) << right << (*j).start 
	     << " ... "    << setw(8) << right << (*j).start+(*j).length 
	     << " ["       << (*j).length << "  entries]." << endl;
      }
    }
  }
}

/// Merge new input to existing output
MergeStatus RootDatabaseMerger::merge(const string& fid) {
  if ( m_output ) {
    TFile* source = TFile::Open(fid.c_str());
    if ( source && !source->IsZombie() )  {
      if ( copyTree(source,"E") == SUCCESS )  {
	if ( copyRefs(source,"Refs") == SUCCESS )  {
	  source->Close();
	  delete source;
	  return SUCCESS;
	}
      }
    }
    cout << "+++ Cannot open input file:" << source << endl;
    m_output->cd();
    return ERROR;
  }
  cout << "+++ No valid output file present. Merge request refused for fid:" << fid << endl;
  return ERROR;
}

/// Add section information for the next merge step
MergeStatus RootDatabaseMerger::addSections(TTree* in, TTree* out) {
  TObjArray* a_in  = in->GetListOfBranches();
  for(int i=0, n=a_in->GetLast(); i<n; ++i) {
    TBranch* b_in = (TBranch*)a_in->At(i);
    TBranch* b_out = out ? out->GetBranch(b_in->GetName()) : 0;
    if ( !out || b_out ) {
      ContainerSection s;
      s.start  = (int)(b_out ? b_out->GetEntries() : 0);
      s.length = (int)b_in->GetEntries();
      m_sections[b_in->GetName()].push_back(s);
      continue;
    }
    cout << "+++ Cannot merge incompatible branches:" << b_in->GetName() << endl;
    return ERROR;
  }
  return SUCCESS;
}

/// Copy single reference branch
MergeStatus RootDatabaseMerger::copyBranch(TTree* src_tree,TTree* out_tree,const string& name) {
  char text[2048];
  TBranch* s = src_tree->GetBranch(name.c_str());
  TBranch* o = out_tree->GetBranch(name.c_str());
  if ( s && o ) {
    s->SetAddress(text);
    o->SetAddress(text);
    for(int i=0, n=s->GetEntries(); i<n; ++i) {
      s->GetEntry(i);
      o->Fill();
    }
    return SUCCESS;
  }
  return ERROR;
}

/// Copy one single tree from the input file to the output file.
MergeStatus RootDatabaseMerger::copyTree(TFile* source, const string& name) {
  TTree* src_tree = (TTree*)source->Get(name.c_str());
  if ( src_tree ) {
    Long64_t src_entries = src_tree->GetEntries();
    TTree *out_tree = (TTree*)m_output->Get(name.c_str());
    m_output->cd();
    if ( 0 == src_tree->GetEntries() )  {
      src_tree->SetEntries(1);
    }
    addSections(src_tree,out_tree);
    if ( out_tree == 0 ) {
      out_tree = src_tree->CloneTree(-1,"fast");
      out_tree->Write();
      if ( s_dbg ) cout << "+++ Created new Tree " << out_tree->GetName() << endl;
      return SUCCESS;
    }
    m_output->GetObject(name.c_str(),out_tree);
    TTreeCloner cloner(src_tree,out_tree,"fast");
    if (cloner.IsValid()) {
      Long64_t out_entries = out_tree->GetEntries();
      out_tree->SetEntries(out_entries+src_entries);
      Bool_t res = cloner.Exec();
      out_tree->Write();
      if ( s_dbg ) cout << "+++ Merged tree: " << out_tree->GetName() << " res=" << res << endl;
      return SUCCESS;
    }
    else {
      // Fast cloning is not possible for this input TTree.
      // ... see TTree::CloneTree for example of recovery code ...
      cout << "+++ Got a tree where fast cloning is not possible -- operation failed." << endl;
      return ERROR;
    }
  }
  return ERROR;
}

/// Copy refs of one single tree from the input file to the output file.
MergeStatus RootDatabaseMerger::copyRefs(TFile* source, const string& name) {
  TTree* src_tree = (TTree*)source->Get(name.c_str());
  if ( src_tree ) {
    TTree *out_tree = (TTree*)m_output->Get(name.c_str());    
    if ( out_tree ) {
      addSections(src_tree,out_tree);
      copyBranch(src_tree,out_tree,"Links");
      copyBranch(src_tree,out_tree,"Params");
      copyBranch(src_tree,out_tree,"Containers");
      copyBranch(src_tree,out_tree,"Databases");
      out_tree->Write();
      return SUCCESS;
    }
  }
  return ERROR;
}

int merge(const char* target, const char* source, bool fixup=false, bool dbg=true) {
  s_dbg = dbg;
  s_dbg = true;
  gSystem->Load("libCintex");
  RootDatabaseMerger m;
  MergeStatus ret = m.exists(target,s_dbg) ? m.attach(target) : m.create(target);
  if ( ret == SUCCESS ) {
    ret = m.merge(source);
    if ( ret == SUCCESS ) {
      m.dumpSections();
      if ( fixup ) m.createFID();
      m.saveSections();
      return m.close();
    }
  }
  cout << "+++ Cannot open output file:" << target << endl;
  return 0;
}
