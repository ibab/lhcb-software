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

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  typedef int MergeStatus;
  enum MergeStatusEnum { 
    MERGE_ERROR=0, 
    MERGE_SUCCESS=1 
  };

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
    bool               m_treeSections;

    union uuid_data {
      unsigned char  buf[16];
      unsigned short sbuf[8];
      unsigned int   ibuf[4];
    };

  public:
    /// Standard constructor
    RootDatabaseMerger();
    /// Default destructor
    virtual ~RootDatabaseMerger();
    /// Check if a database exists
    bool exists(const std::string& fid) const;
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

    /// Copy all data trees from the input file to the output file.
    MergeStatus copyAllTrees(TFile* source);
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
#include "TInterpreter.h"
#ifdef _WIN32
#else
#include <libgen.h>
#endif

using namespace Gaudi;
using namespace std;

namespace {
  static bool s_dbg = true;
}

/// Standard constructor
RootDatabaseMerger::RootDatabaseMerger() : m_output(0), m_treeSections(false) {
}

/// Default destructor
RootDatabaseMerger::~RootDatabaseMerger() {
  close();
}

/// Check if a database exists
bool RootDatabaseMerger::exists(const std::string& fid) const {
  Bool_t result = gSystem->AccessPathName(fid.c_str(), kFileExists);
  // if ( s_dbg ) ::printf("File %s %s!\n",fid.c_str(),result == kFALSE ? "EXISTS" : "DOES NOT EXIST");
  return result == kFALSE;
}

/// Attach to existing output file for further merging
MergeStatus RootDatabaseMerger::attach(const string& file_id) {
  const char* fid = file_id.c_str();
  if ( m_output ) {
    ::printf("+++ Another output file %s is already open. Request denied.\n",m_output->GetName());
    return MERGE_ERROR;
  }
  else if ( !exists(file_id) ) {
    ::printf("+++ Cannot attach output file %s --- file does not exist.\n",fid);
    return MERGE_ERROR;
  }
  m_output = TFile::Open(fid,"UPDATE");
  if ( m_output && !m_output->IsZombie() ) {
    if ( s_dbg ) ::printf("+++ Update output file %s.\n",fid);
    return MERGE_SUCCESS;
  }
  ::printf("+++ Failed to open new output file %s.\n",fid);
  return MERGE_ERROR;
}

/// Create new output file
MergeStatus RootDatabaseMerger::create(const string& fid) {
  if ( m_output ) {
    ::printf("+++ Another output file %s is already open. Request denied.\n",m_output->GetName());
    return MERGE_ERROR;
  }
  else if ( exists(fid) ) {
    ::printf("+++ Cannot create output file %s --- file already exists.\n",fid.c_str());
    return MERGE_ERROR;
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
      if ( s_dbg ) ::printf("+++ Opened new output file %s.\n",fid.c_str());
      return MERGE_SUCCESS;
    }
  }
  ::printf("+++ Failed to open new output file %s.\n",fid.c_str());
  return MERGE_ERROR;
}

/// Close output file
MergeStatus RootDatabaseMerger::createFID() {
  static const char* fmt = "FID=%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";
  if ( m_output ) {
    TTree* t = (TTree*)m_output->Get("Refs");
    if ( t ) {
      uuid_data d;
      char text[256];
      TUUID uuid;
      TBranch* b = t->GetBranch("Params");
      if ( b ) {
	uuid.GetUUID(d.buf);
	sprintf(text,fmt,d.ibuf[0],d.sbuf[2],d.sbuf[3],d.buf[8],d.buf[9],
		d.buf[10],d.buf[11],d.buf[12],d.buf[13],d.buf[14],d.buf[15]);
	b->SetAddress(text);
	b->Fill();
	t->Write();
	if ( s_dbg ) ::printf("+++ Added new GUID %s to merge file.\n",text);
	return MERGE_SUCCESS;
      }
    }
  }
  ::printf("+++ Failed to add new GUID to merge file.\n");
  return MERGE_ERROR;
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
  return MERGE_SUCCESS;
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
	      ::printf("+++ Failed to update Sections tree with new entries. [WRITE_ERROR]\n");
	  }
	}
	::sprintf(text,"[END-OF-SECTION]");
	nb = t->Fill();
	++total;
	if ( nb > 0 )
	  nbytes += nb;
	else
	  ::printf("+++ Failed to update Sections branch with new entries. [WRITE_ERROR]\n");
	t->Write();
	if ( s_dbg ) ::printf("+++ Added %d Sections entries with %d bytes in total.\n",total,nbytes);
	return MERGE_SUCCESS;
      }
      ::printf("+++ Failed to update Sections tree with new entries. [NO_OUTPUT_BRANCH]\n");
      return MERGE_ERROR;
    }
    ::printf("+++ Failed to update Sections tree with new entries. [NO_OUTPUT_TREE]\n");
    return MERGE_ERROR;
  }
  ::printf("+++ Failed to update Sections tree with new entries. [NO_OUTPUT_FILE]\n");
  return MERGE_ERROR;
}

/// Dump collected database sections
void RootDatabaseMerger::dumpSections() {
  for(DatabaseSections::const_iterator i=m_sections.begin(); i!= m_sections.end();++i) {
    int cnt = 0;
    string prefix = (*i).first;
    const ContainerSections& cntSects = (*i).second;
    for(ContainerSections::const_iterator j=cntSects.begin(); j != cntSects.end();++j, ++cnt) {
      char text[1024];
      ::sprintf(text,"['%s'][%d]",prefix.c_str(),cnt); 
      if ( s_dbg ) {
	::printf("+++ section %-55s Start:%8d ... %8d [%d entries]\n",
		 text,(*j).start,(*j).start+(*j).length,(*j).length);
      }
    }
  }
}

/// Merge new input to existing output
MergeStatus RootDatabaseMerger::merge(const string& fid) {
  if ( m_output )    {
    TFile* source = TFile::Open(fid.c_str());
    if ( source && !source->IsZombie() )  {
      size_t idx = fid.rfind('/');
      ::printf("+++ Start merging input file:%s\n",
	       idx != string::npos ? fid.substr(idx+1).c_str() : fid.c_str());
      if ( copyAllTrees(source) == MERGE_SUCCESS )  {
	if ( copyRefs(source,"Refs") == MERGE_SUCCESS )  {
	  source->Close();
	  delete source;
	  return MERGE_SUCCESS;
	}
      }
    }
    ::printf("+++ Cannot open input file:%s\n", source->GetName());
    m_output->cd();
    return MERGE_ERROR;
  }
  ::printf("+++ No valid output file present. Merge request refused for fid:%s.\n",fid.c_str());
  return MERGE_ERROR;
}

/// Add section information for the next merge step
MergeStatus RootDatabaseMerger::addSections(TTree* in, TTree* out) {
  if ( m_treeSections ) {
    ContainerSection s;
    s.start  = (int)(out ? out->GetEntries() : 0);
    s.length = (int)in->GetEntries();
    m_sections[in->GetName()].push_back(s);
    return MERGE_SUCCESS;
  }

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
    ::printf("+++ Cannot merge incompatible branches:%s.\n",b_in->GetName());
    return MERGE_ERROR;
  }
  return MERGE_SUCCESS;
}

/// Copy single reference branch
MergeStatus RootDatabaseMerger::copyBranch(TTree* src_tree,TTree* out_tree,const string& name) {
  char text[4096];
  TBranch* s = src_tree->GetBranch(name.c_str());
  TBranch* o = out_tree->GetBranch(name.c_str());
  if ( s && o ) {
    s->SetAddress(text);
    o->SetAddress(text);
    for(Long64_t i=0, n=s->GetEntries(); i<n; ++i) {
      s->GetEntry(i);
      o->Fill();
    }
    return MERGE_SUCCESS;
  }
  return MERGE_ERROR;
}

/// Copy all data trees from the input file to the output file.
MergeStatus RootDatabaseMerger::copyAllTrees(TFile* source) {
  TIter nextkey(source->GetListOfKeys());
  //m_treeSections = true;
  for(TKey* key = (TKey*)nextkey(); key; key = (TKey*)nextkey() ) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom("TTree")) {
      string name = key->GetName();
      if ( name == "Refs" ) continue;
      m_treeSections = 0 == ::strncmp(key->GetName(),"<local>_",7);
      printf("Tree:%s %d\n",name.c_str(),int(m_treeSections));
      if ( copyTree(source,name) != MERGE_SUCCESS ) {
	m_treeSections = false;
	return MERGE_ERROR;
      }
    }
  }
  m_treeSections = false;
  return MERGE_SUCCESS;
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
      if ( s_dbg ) ::printf("+++ Created new Tree %s.\n",out_tree->GetName());
      return MERGE_SUCCESS;
    }
    m_output->GetObject(name.c_str(),out_tree);
    TTreeCloner cloner(src_tree,out_tree,"fast");
    if (cloner.IsValid()) {
      Long64_t out_entries = out_tree->GetEntries();
      out_tree->SetEntries(out_entries+src_entries);
      Bool_t res = cloner.Exec();
      //out_tree->Write();
      if ( s_dbg ) ::printf("+++ Merged tree: %s res=%d\n",out_tree->GetName(),res);
      return MERGE_SUCCESS;
    }
    else {
      // Fast cloning is not possible for this input TTree.
      // ... see TTree::CloneTree for example of recovery code ...
      ::printf("+++ Got a tree where fast cloning is not possible -- operation failed.\n");
      return MERGE_ERROR;
    }
#if 0
    Long64_t nb = out_tree->CopyEntries(src_tree,-1,"fast");
    Long64_t out_entries = out_tree->GetEntries();
    out_tree->SetEntries(out_entries+src_entries);
    out_tree->Write();
    if ( s_dbg ) ::printf("+++ Merged tree: %s res=%lld\n",out_tree->GetName(),nb);
    return MERGE_SUCCESS;
#endif
  }
  return MERGE_ERROR;
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
      return MERGE_SUCCESS;
    }
  }
  return MERGE_ERROR;
}

int merge(const char* target, const char* source, bool fixup=false, bool dbg=true) {
  s_dbg = dbg;
  //s_dbg = true;
#if 0
  static bool first = true;
  if ( first ) {
    first = false;
    gSystem->Load("libCintex");
    gInterpreter->ProcessLine("Cintex::Enable()");
    //gSystem->Load("libGaudiKernelDict");
    //gSystem->Load("libGaudiExamplesDict");
  }
#endif
  //printf("+++ Target:%s\n+++ Source file:%s Fixup:%s Dbg:%s\n",
  //	 target,source, fixup ? "YES" : "NO",s_dbg ? "YES" : "NO");
  RootDatabaseMerger m;
  MergeStatus ret = m.exists(target) ? m.attach(target) : m.create(target);
  if ( ret == MERGE_SUCCESS ) {
    ret = m.merge(source);
    if ( ret == MERGE_SUCCESS ) {
      m.dumpSections();
      if ( fixup ) m.createFID();
      m.saveSections();
      return m.close();
    }
  }
  ::printf("+++ Cannot open output file:%s\n",target);
  return 0;
}
