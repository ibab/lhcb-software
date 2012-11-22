// $Id:
//====================================================================
//
//  Author     : M.Frank  CERN/LHCb
//
//====================================================================
// ROOT include files
#include "TUUID.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TClass.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"

// Standad C/C++ include files
#include <stdexcept>
#include <string>
#include <vector>

/*
 *   Gaudi namespace declaration
 */
namespace Gaudi {

  /** @struct RootRef RootRefs.h RootCnv/RootRefs.h
   *
   * Persistent reference object.
   * 
   * @author  M.Frank
   * @version 1.0
   */
  struct RootRef {
    /// Data members to define object location in the persistent world
    int      dbase,container,link,clid,svc,entry;
    /// Standard constructor
    RootRef()     {  this->reset(); }
    /// Copy constructor
    RootRef(const RootRef& c) 
      : dbase(c.dbase),container(c.container),link(c.link),clid(c.clid),svc(c.svc),entry(c.entry)
    {
    }
    /// Assignment operator
    RootRef& operator=(const RootRef& c)  {
      dbase     = c.dbase;
      container = c.container;
      link      = c.link;
      clid      = c.clid;
      svc       = c.svc;
      entry     = c.entry;
      return *this;
    }
    /// Reset data content
    void reset() {
      dbase     = -1;
      container = -1;
      link      = -1;
      entry     = -1;
      svc       = 1;
      clid      = 1;
    }
  };

  /** @struct RootObjectRefs RootRefs.h RootCnv/RootRefs.h
   *
   * Persistent reference object containing all leafs and links
   * corresponding to a Gaudi DataObject.
   * 
   * @author  M.Frank
   * @version 1.0
   */
  struct RootObjectRefs {
    /// The links of the link manager
    std::vector<int>       links;
    /// The references corresponding to the next layer of items in the data store
    std::vector<RootRef>   refs;
    
    /// Default constructor
    RootObjectRefs() {}
    /// Copy constructor
    RootObjectRefs(const RootObjectRefs& r) : links(r.links), refs(r.refs) {}
    /// Default destructor
    ~RootObjectRefs() {}
    /// Assignment operator
    RootObjectRefs& operator=(const RootObjectRefs& r) {
      links = r.links;
      refs = r.refs; 
      return *this;
    }
  };

  /** @class RootNTupleDescriptor RootRefs.h RootCnv/RootRefs.h
   *
   * Description:
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct RootNTupleDescriptor {
    /// Description string
    std::string   description;
    /// Optional description 
    std::string   optional;
    /// Identifier of description
    std::string   container;
    /// Class ID of the described object
    unsigned long clid;
    /// Standard constructor
    RootNTupleDescriptor() {}
    /// Standard destructor
    virtual ~RootNTupleDescriptor() {}
  };

  typedef int ExtractStatus;
  enum ExtractStatusEnum { 
    EXTRACT_ERROR=0, 
    EXTRACT_SUCCESS=1 
  };

  struct RootEventExtractor {
  protected:
    TFile* m_in;
    TTree* m_evt_in;
    TTree* m_ref_in;

    TFile* m_out;
    TTree* m_evt_out;
    TTree* m_ref_out;

    int m_localDB_id;
    std::vector<int> m_eventList;

  public:
    /// Default constructor
    RootEventExtractor();

    /// Initializing constructor directly opening the input file and the output file
    RootEventExtractor(const char* input, const char* output, const char* output_option);

    ///  Default destructor
    virtual ~RootEventExtractor();

    /// Open input file
    ExtractStatus openInput(const char* name);

    /// Close input file
    ExtractStatus closeInput();

    /// Open output file
    ExtractStatus openOutput(const char* name, const char* option);

    /// Close output file
    ExtractStatus closeOutput();

    /// Clear the list of event numbers to be extracted from the file.
    ExtractStatus cancel();

    /// Add a given entry number to the list of events to be selected to the output file
    ExtractStatus select(int evt_num);

    /// Extract all previously selected events from the input file and write it to the output 
    ExtractStatus extract();
  };


  /// Function to extract single event and write it to an output file
  int extractEvt(const char* input, const char* output, long num_evt);
}

using namespace Gaudi;
using namespace std;

/// Default constructor
RootEventExtractor::RootEventExtractor()     
  : m_in(0), m_evt_in(0), m_ref_in(0), m_out(0), m_evt_out(0), m_ref_out(0)
{
}

/// Initializing constructor directly opening the input file and the output file
RootEventExtractor::RootEventExtractor(const char* input, const char* output, const char* output_option)
  : m_in(0), m_evt_in(0), m_ref_in(0), m_out(0), m_evt_out(0), m_ref_out(0)
{
  if ( EXTRACT_SUCCESS != openInput(input) ) {
    throw std::runtime_error("Failed to open input file:"+std::string(input));
  }
  if ( EXTRACT_SUCCESS != openOutput(output,output_option) ) {
    throw std::runtime_error("Failed to open output file:"+std::string(output));
  }
}

///  Default destructor
RootEventExtractor::~RootEventExtractor()    {
  closeInput();
  closeOutput();
}

/// Clear the list of event numbers to be extracted from the file.
ExtractStatus RootEventExtractor::cancel() {
  m_eventList.clear(); 
  ::printf("+++ Event list cleared.\n");
  return EXTRACT_SUCCESS;
}

/// Add a given entry number to the list of events to be selected to the output file
ExtractStatus RootEventExtractor::select(int evt_num) {
  m_eventList.push_back(evt_num);
  return EXTRACT_SUCCESS;
}

/// Close input file
ExtractStatus RootEventExtractor::openInput(const char* name) {
  if ( m_in ) closeInput();
  m_in = TFile::Open(name);
  if ( m_in && !m_in->IsZombie() ) {
    m_evt_in  = (TTree*)m_in->Get("Event");
    m_ref_in  = (TTree*)m_in->Get("Refs");
    return EXTRACT_SUCCESS;
  }
  return EXTRACT_ERROR;
}

/// Close input file
ExtractStatus RootEventExtractor::openOutput(const char* name, const char* option) {
  if ( m_out ) closeOutput();
  m_out = TFile::Open(name,option);
  m_evt_out = m_ref_out = 0;
  return EXTRACT_SUCCESS;
}

/// Close input file
ExtractStatus RootEventExtractor::closeInput() {
  if ( m_in )   {
    ::printf("+++ Closing input  file:%s\n",m_in->GetName());
    m_in->Close();
    delete m_in;
  }
  m_in = 0;
  m_evt_in = 0;
  m_ref_in = 0;
  return EXTRACT_SUCCESS;
}

/// Close output file
ExtractStatus RootEventExtractor::closeOutput() {
  if ( m_out )   {
    ::printf("+++ Closing output file:%s\n",m_out->GetName());
    if ( m_evt_out ) m_evt_out->Write();
    if ( m_ref_out ) m_ref_out->Write();
    m_out->Close();
    delete m_out;
  }
  m_out     = 0;
  m_evt_out = 0;
  m_ref_out = 0;
  return EXTRACT_SUCCESS;
}

/// Extract all previously selected events from the input file and write it to the output 
ExtractStatus RootEventExtractor::extract()   {
  char text[1024];
  bool new_output = false;
  TBranch  *br_in, *br_out;
  if ( !m_in || m_in->IsZombie() ) {
    throw std::runtime_error("Input file missing - cannot extract events.");
  }
  else if ( !m_out || m_out->IsZombie() ) {
    throw std::runtime_error("Output file missing - cannot extract events.");
  }
  else {
    m_evt_out = (TTree*)m_out->Get("Event");
    if ( 0 == m_evt_out )  {
      m_evt_out  = m_evt_in->CloneTree(0);
    }
    m_ref_out = (TTree*)m_out->Get("Refs");
    if ( 0 == m_ref_out )  {
      m_ref_out = m_ref_in->CloneTree(0);
      new_output = true;
    }
  }
  m_localDB_id = -1;
  if ( new_output )    {
    ::printf("+++ Copy refs table to new output file:%s\n",m_out->GetName());
    br_in = m_ref_in->GetBranch("Databases");
    br_in->SetAddress(text);
    br_out = m_ref_out->GetBranch("Databases");
    br_out->SetAddress(text);
    for( int i=0; i<br_in->GetEntries(); ++i ) { 
      br_in->GetEntry(i);
      br_out->Fill();
      if ( m_localDB_id<0 && strcmp(text,"<localDB>") == 0 ) {
	m_localDB_id = i;
      }
    }

    br_in = m_ref_in->GetBranch("Containers");
    br_in->SetAddress(text);
    br_out = m_ref_out->GetBranch("Containers");
    br_out->SetAddress(text);
    for( int i=0; i<br_in->GetEntries(); ++i ) { 
      br_in->GetEntry(i);
      br_out->Fill();
    }

    br_in = m_ref_in->GetBranch("Links");
    br_in->SetAddress(text);
    br_out = m_ref_out->GetBranch("Links");
    br_out->SetAddress(text);
    for( int i=0; i<br_in->GetEntries(); ++i ) { 
      br_in->GetEntry(i);
      br_out->Fill();
    }

    br_in = m_ref_in->GetBranch("Params");
    br_in->SetAddress(text);
    br_out = m_ref_out->GetBranch("Params");
    br_out->SetAddress(text);
    for( int i=0; i<br_in->GetEntries(); ++i ) { 
      br_in->GetEntry(i);
      if ( strncmp(text,"PFN=",4) == 0 ) {  // Update PFN entry
	sprintf(text,"PFN=%s",br_out->GetFile()->GetName());    
	::printf("+++ PFN of the created output file is:%s\n",text);
      }
      else if ( strncmp(text,"FID=",4) == 0 ) {    // Create new FID for new file
	static const char* fmt = "FID=%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";
	union uuid_data {
	  unsigned char  buf[16];
	  unsigned short sbuf[8];
	  unsigned int   ibuf[4];
	} d;
	TUUID uuid;
	uuid.GetUUID(d.buf);
	sprintf(text,fmt,d.ibuf[0],d.sbuf[2],d.sbuf[3],d.buf[8],d.buf[9],
		d.buf[10],d.buf[11],d.buf[12],d.buf[13],d.buf[14],d.buf[15]);
	::printf("+++ FID of the created output file is:%s\n",text);
      }
      br_out->Fill();
    }
    m_ref_out->Write();
  }

  TIter next(m_evt_in->GetListOfBranches());
  while( (br_in=(TBranch*)next()) ) {
    TString     name = br_in->GetName();
    TClass*     br_class = gROOT->GetClass(br_in->GetClassName(),kTRUE);
    br_out =    m_evt_out->GetBranch(name);
    if ( 0 == br_out ) {
      ::printf("+++ ERROR: Input and output event trees are incompatible. Selection not possible.\n");
      return EXTRACT_ERROR;
    }
    int         out_num_entries = br_out->GetEntries();
    for(std::vector<int>::const_iterator i=m_eventList.begin(); i != m_eventList.end(); ++i) {
      int   num_evt = *i;
      const char* br_type = "DataObject";
      void* pObject = br_class->New();
      br_in->SetAddress(&pObject);
      br_out->SetAddress(&pObject);
      int num_rd = br_in->GetEntry(num_evt);
      if ( num_rd < 0 ) {	// Definitive error
	::printf("+++ ERROR: Failed to read data from branch:%s\n",name.Data());
	return EXTRACT_ERROR;
      }
      if ( name.EndsWith("_R.") )   {
	RootObjectRefs *refs=(RootObjectRefs*)pObject;
	for(std::vector<RootRef>::iterator ir=refs->refs.begin(); ir!= refs->refs.end(); ++ir)     {
	  RootRef& r = *ir;
	  if ( r.dbase == m_localDB_id ) r.entry = out_num_entries;
	}
	br_type = "*Index*";
      }
      int num_wr = br_out->Fill();
      if ( num_wr < 0 ) {
	::printf("+++ ERROR: Failed to write data to extraction branch:%s "
		 "read:%d wrote:%d bytes [Length-mismatch]\n",name.Data(),num_rd,num_wr);
	return EXTRACT_ERROR;	
      }
      ::printf("+++ Copied %8d bytes to %-10s branch %s(%d)\n",num_rd,br_type,name.Data(),out_num_entries);
      ++out_num_entries;
    }
    m_evt_out->SetEntries(br_out->GetEntries()+1);
  }
  m_evt_out->Write();
  return cancel();
}


/// Function to extract single event and write it to an output file
int Gaudi::extractEvt(const char* input, const char* output, long num_evt) {
  RootEventExtractor e(input,output,"RECREATE");
  if ( e.select(num_evt) == EXTRACT_SUCCESS ) {
    if ( e.extract() == EXTRACT_SUCCESS ) {
      return EXTRACT_SUCCESS;
    }
  }
  return EXTRACT_ERROR;
}

int extract_event(const char* input, const char* output, long num_evt)  {
  return extractEvt(input,output,num_evt);
}

#if 0
int main(int, char**) {
  gROOT->SetBatch(kTRUE);
  gSystem->Load("libCintex.so");
  gInterpreter->ProcessLine("ROOT::Cintex::Enable()");
  return extractEvtFunc("castor:/castor/cern.ch/grid/lhcb/LHCb/Collision12/FULL.DST/00020565/0000/00020565_00005381_1.full.dst","bla.root",2);
}
#endif
