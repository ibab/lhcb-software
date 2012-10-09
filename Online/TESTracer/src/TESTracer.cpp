#include "TESTracer.h"
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include <iostream>
#include <iomanip>
#include <sys/time.h>

using namespace std;
using namespace TES;

DECLARE_AUDITOR_FACTORY(TraceAuditor)

#include <stdexcept>

Tracer& TES::_tracer() {
  static Tracer _tracer;
  return _tracer;
}

TESTracer& TESTracer::instance() {
  return _tracer();
}

Tracer::Tracer() 
: m_level(0), m_nested(0) 
{
}

const char* Tracer::indent(int n) {
  for(size_t i=0; i<size_t(n); ++i)
    cout << "  ";
  return "";
}

void Tracer::fillAlgProperties(INamedInterface*  a)    {
  string typ = a ? System::typeinfoName(typeid(*a)) : string("Framework");
  string nam = a ? a->name() : string("Framework");
  AlgEntries::iterator i=m_algos.find(nam);

  if ( i != m_algos.end() ) {
    AlgInfo* e = (*i).second;
    if ( e->typ == "GaudiSequencer" ) {
      SmartIF<IProperty> prop(a);
      if ( prop ) {
	string val;
	prop->getProperty("ModeOR",val);
	if ( val == "true" ) e->flags |= GAUDISEQ_OR;
	prop->getProperty("ShortCircuit",val);
	if ( val == "true" ) e->flags |= GAUDISEQ_SHORT;
	prop->getProperty("IgnoreFilterPassed",val);
	if ( val == "true" ) e->flags |= GAUDISEQ_IGNORE;
	prop->getProperty("ReturnOK",val);
	if ( val == "true" ) e->flags |= GAUDISEQ_RETOK;
      }
    }
    else if ( e->typ == "Sequencer" ) {
      /*
	declareProperty( "Members", m_names );
	declareProperty( "BranchMembers", m_branchNames );
	declareProperty( "StopOverride", m_stopOverride=false );
      */
      SmartIF<IProperty> prop(a);
      if ( prop ) {
	string val;
	prop->getProperty("StopOverride",val);
	if ( val == "true" ) e->flags |= SEQ_STOPOVERRIDE;
	cout << " Mode-OR:" << val;
	prop->getProperty("ShortCircuit",val);
      }
    }
    else if ( e->typ == "Prescaler" ) {
      // barrier
    }
    else if ( e->typ == "Algorithm" ) {
    }
    else {
    }
  }
  else {    
    cout << "StoreTracer (Initialized Algorithm) :" << nam
	 << " of type " << typ << endl;
  }
}

void Tracer::addAlgorithm(INamedInterface* a, AlgInfo* pi)    {
  string framework("Framework");
  string nam = a ? a->name() : framework;
  AlgEntries::iterator i=m_algos.find(nam);
  if ( i == m_algos.end() ) {
    AlgInfo* e = new AlgInfo();
    string typ = a ? System::typeinfoName(typeid(*a)) : nam;
    e->level   = m_level;
    e->order   = m_algos.size()+1;
    e->ptr     = a;
    e->name    = nam;
    e->parent  = pi;
    e->flags   = 0;
    
    e->time    = 0;
    e->time2   = 0;
    e->calls   = 0;
    e->abiTyp  = typ;

    if ( typ == "GaudiSequencer" ) {
      e->typ = typ;
      e->flags |= IS_GAUDISEQUENCER;
    }
    else if ( typ == "ProcessPhase" ) {
      e->typ = "GaudiSequencer";
      e->flags |= IS_GAUDISEQUENCER;
    }
    else if ( typ == "Sequencer" ) {
      e->typ = typ;
      e->flags |= IS_SEQUENCER;
    }
    else if ( typ == "Prescaler" ) {
      e->typ = typ;
      e->flags |= IS_PRESCALER;
    }
    else if ( typ == framework ) {
      e->typ = typ;
      e->flags |= IS_FRAMEWORK;
    }
    else if ( dynamic_cast<Algorithm*>(a) ) {
      e->typ = "Algorithm";
    }
    else {
      e->typ = "Unknown";
    }
    if ( e->parent ) {
      i = m_algos.find(e->parent->name);
      (*i).second->daughters.push_back(e);
    }
    m_algos.insert(make_pair(e->name,e));
    m_initSeq.push_back(e);
  }
}

// IAuditor implementation
void Tracer::beforeInitialize(INamedInterface* a) {
  AlgEntry alg(a->name(),m_level++);
  AlgInfo* pi = 0;
  if ( !m_algorithms.empty() ) {
    const AlgEntry& parent = m_algorithms.back();
    AlgEntries::iterator pit = m_algos.find(parent.first);
    if ( pit != m_algos.end() ) {
      pi = (*pit).second;
    }
  }
  m_algorithms.push_back(alg);
  m_current = a;
  addAlgorithm(a,pi);
}

void Tracer::afterInitialize (INamedInterface* a) {
  fillAlgProperties(a);
  m_algorithms.pop_back();
  --m_level;
  m_current = 0;
}

void Tracer::beforeExecute   (INamedInterface* a) {
  AlgEntry alg(a->name(),++m_level);
  m_algorithms.push_back(alg);
  m_current = a;
  AlgEntries::iterator ialg=m_algos.find(alg.first);
  if ( ialg == m_algos.end() ) {
    addAlgorithm(a,0);
    fillAlgProperties(a);
    ialg=m_algos.find(alg.first);
  }
  struct timezone tz;
  (*ialg).second->calls++;
  ::gettimeofday(&((*ialg).second->start), &tz);
}

void Tracer::afterExecute    (INamedInterface* a, const StatusCode& ) {
  struct timeval end;
  struct timezone tz;
  AlgEntry alg = m_algorithms.back();
  AlgEntries::iterator ialg=m_algos.find(alg.first);

  m_algorithms.pop_back();
  --m_level;
  m_current = 0;
  if ( alg.first != a->name() ) {
    cout << "StoreTracer (Done with Algorithm[" << alg.second << "]) :";
    cout << indent(alg.second);
    cout << a->name();
    cout << " : UNEXPECTED!  should be :" << alg.first;
    cout << endl;
  }
  ::gettimeofday(&end, &tz);
  long delta = (1000000*(end.tv_sec-(*ialg).second->start.tv_sec));
  delta += ((end.tv_usec-(*ialg).second->start.tv_usec));
  (*ialg).second->time  += delta;
  (*ialg).second->time2 += (delta*delta);
}

static void printIO(int level, set<string>& input, set<string>& output, set<string>& aux) {
  char text[1024];
  set<string>::const_iterator s,j;

  if ( level > 0 ) ::memset(text,' ',2*level);
#if 0
  for(j=input.begin(); j!=input.end();++j) {
    s = output.find(*j);
    if ( s != output.end() )  {
      input.erase(j);
      j = input.begin();
    }
  }
#endif
  for(s=input.begin(), j=s; j!=input.end();++j) {
    ::sprintf(text+2*level,"    %-8s %s\n",j!=s ? "" : "Inputs:",(*j).c_str());	
    ::printf(text);
  }
  for(s=output.begin(), j=s; j!=output.end();++j) {
    ::sprintf(text+2*level,"    %-8s %s\n",j!=s ? "" : "Outputs:",(*j).c_str());
    ::printf(text);
  }
  for(s=aux.begin(), j=s; j!=aux.end();++j) {
    ::sprintf(text+2*level,"    %-8s %s\n",j!=s ? "" : "Aux:",(*j).c_str());
    ::printf(text);
  }
}

void Tracer::printExecAlgs(int max_level) {
  char text[1024];
  map<int,AlgInfo*> ordered;

  ::printf("\n=================  Algorithm execution list (max. level:%d):\n",max_level);
  for(AlgEntries::iterator i=m_algos.begin(); i!=m_algos.end(); ++i) 
    ordered.insert(make_pair((*i).second->order,(*i).second));
  for(map<int,AlgInfo*>::iterator i=ordered.begin(); i!=ordered.end(); ++i) {
    AlgInfo* e = (*i).second;
    if ( e->level <= max_level) {
      if ( e->level > 0 ) ::memset(text,' ',2*e->level);
      if ( e->flags&IS_GAUDISEQUENCER ) {
	::sprintf(text+2*e->level,"--> %s / %s(%s) [Seq:%d / Level:%d] %s%s",
		  e->name.c_str(),e->typ.c_str(),
		  e->flags&GAUDISEQ_OR ? "OR" : "AND",
		  e->order,
		  e->level,
		  e->parent ? "Parent:"               : "",
		  e->parent ? e->parent->name.c_str() : "");
	::printf("%s\n",text);
	::sprintf(text+2*e->level,"    Mode:%s Filter:%s Return:%s Calls:%d Time:%7.3e",
		  e->flags&GAUDISEQ_SHORT  ? "Shortcut" : "ExecAll",
		  e->flags&GAUDISEQ_IGNORE ? "Ignore"   : "Validate",
		  e->flags&GAUDISEQ_RETOK  ? "Force-OK" : "True-Status",
		  e->calls,
		  e->calls ? e->time/e->calls : 0e0
		  );
      }
      else {
	::sprintf(text+2*e->level,"--> %s / %s [Seq:%d / Level:%d] %s%s",
		  e->name.c_str(),e->typ.c_str(),
		  e->order,
		  e->level,
		  e->parent ? "Parent:" : "",
		  e->parent ? e->parent->name.c_str() : ""
		  );
	::printf("%s\n",text);
	::sprintf(text+2*e->level,"    Abi:%s Calls:%d Time:%7.3e",
		  e->abiTyp.c_str(),
		  e->calls,
		  e->calls ? e->time/e->calls : 0e0
		  );
      }
      ::printf("%s\n",text);
      printIO(e->level,e->get,e->put,e->aux);
    }
  }
}

void Tracer::printInitAlgs(int max_level) {
  char text[1024];
  ::printf("\n=================  Algorithm initialization list (max. level:%d):\n",max_level);
  for(vector<AlgInfo*>::const_iterator i=m_initSeq.begin(); i!=m_initSeq.end();++i) {
    AlgInfo* e = (*i);
    int level = e->level;
    if ( level <= max_level) {
      if ( level > 0 ) ::memset(text,' ',2*level);
      if ( e->flags&IS_GAUDISEQUENCER ) {
	::sprintf(text+2*level,"--> %s / %s(%s) [Seq:%d / Level:%d] %s%s %s Filter:%s Return:%s",
		  e->name.c_str(),e->typ.c_str(),
		  e->flags&GAUDISEQ_OR ? "OR" : "AND",
		  e->order,
		  level,
		  e->parent ? "Parent:"               : "",
		  e->parent ? e->parent->name.c_str() : "",
		  e->flags&GAUDISEQ_SHORT  ? "Shortcuting" : "Exec-all",
		  e->flags&GAUDISEQ_IGNORE ? "Ignore"      : "Validate",
		  e->flags&GAUDISEQ_RETOK  ? "Force-OK"    : "True-Status"
		  );
      }
      else {
	::sprintf(text+2*level,"--> %s / %s [Seq:%d / Level:%d] %s%s",
		  e->name.c_str(),e->typ.c_str(),
		  e->order,level,
		  e->parent ? "Parent:" : "",
		  e->parent ? e->parent->name.c_str() : ""
		  );
      }
      printf("%s\n",text);
    }
  }
}

void Tracer::printDataRegistry() {
  printf("\n=================  Data registry:\n");
  typedef map<int,AccessEntries::const_iterator> AccessStat;
  AccessStat tes_access;
  for(AccessEntries::const_iterator r=m_data.begin(); r!=m_data.end();++r)
    tes_access[(*r).second.first] = r;
#define SEL_CMD(x)  ((e.second.first & TRACE_##x )==TRACE_##x) ? (string(#x)+" ").c_str() : ""
  for(AccessStat::const_iterator r=tes_access.begin(); r!=tes_access.end();++r) {
    AccessEntries::const_iterator ri=(*r).second;
    const AccessEntry& e = (*ri).second;
    ::printf("\t%5d  -> %s [%d calls]  (%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s)\n",
	     e.first,
	     (*ri).first.c_str(),
	     e.second.second,
	     SEL_CMD(REGOBJ),
	     SEL_CMD(FIND),
	     SEL_CMD(DATAFAULT),
	     SEL_CMD(DATAFAULT_OK),
	     SEL_CMD(DATAFAULT_FAIL),
	     SEL_CMD(LOAD),
	     SEL_CMD(RETRIEVE),
	     SEL_CMD(CLEAR),
	     SEL_CMD(CLEAR_ROOT),
	     SEL_CMD(TRAVERSE),
	     SEL_CMD(SETROOT),
	     SEL_CMD(REGADDR),
	     SEL_CMD(UNREGADDR),
	     SEL_CMD(UNREGOBJ),
	     SEL_CMD(LINK),
	     SEL_CMD(UNLINK)
	     );
  }
}

inline int PRINT_LEVEL(int x) { 
  return 1<<x;
}
enum {
  PRINT_ALL_LEVELS = 1<<30,
  PRINT_TRACES     = 1<<31,
  _last
};
//static int s_print_levels = PRINT_ALL_LEVELS|PRINT_TRACES;
static int s_print_levels = PRINT_TRACES;
//static int s_print_levels = PRINT_ALL_LEVELS;


void Tracer::beforeFinalize  (INamedInterface*)    {
  if ( !m_initSeq.empty() ) {
    makeCode();
  }

  if ( !m_algos.empty() ) {
    if ( s_print_levels&PRINT_LEVEL(2) ) printExecAlgs(2);
    if ( s_print_levels&PRINT_LEVEL(3) ) printExecAlgs(3);
    if ( s_print_levels&PRINT_LEVEL(4) ) printExecAlgs(4);
    if ( s_print_levels&PRINT_ALL_LEVELS ) printExecAlgs(9999);
  }

  if ( !m_data.empty() ) {
    printDataRegistry();
    m_data.clear();
  }

  if ( !m_initSeq.empty() ) {
    if ( s_print_levels&PRINT_LEVEL(2) ) printInitAlgs(2);
    if ( s_print_levels&PRINT_LEVEL(3) ) printInitAlgs(3);
    if ( s_print_levels&PRINT_LEVEL(4) ) printInitAlgs(4);
    if ( s_print_levels&PRINT_ALL_LEVELS ) printInitAlgs(9999);
  }

  if ( !m_algos.empty() ) {
    for(AlgEntries::iterator i=m_algos.begin(); i!=m_algos.end(); ++i) 
      delete (*i).second;
    m_algos.clear();
  }
  m_initSeq.clear();
}

void Tracer::afterFinalize   (INamedInterface*) {
}

void Tracer::start() {
  ++m_nested;
}

void Tracer::stop() {
  --m_nested;
}

void Tracer::i_trace(IService* svc, TracerCommand cmd, IRegistry* pReg, const char* path) {
  if ( svc ) {
    bool trace_evt_datasvc = true;
    bool trace_det_datasvc = false;
    bool trace_his_datasvc = false;
    const string& svc_name = svc->name();
    string curr = m_current ? m_current->name() : "Framework";

    if      ( trace_evt_datasvc && svc_name == "EventDataSvc" )
      i_trace(svc, cmd, curr, pReg, path);
    else if ( trace_det_datasvc && svc_name == "DetectorDataSvc" )
      i_trace(svc, cmd, curr, pReg, path);
    else if ( trace_his_datasvc && svc_name == "HistogramDataSvc" )
      i_trace(svc, cmd, curr, pReg, path);
  }
  else {
    i_trace(0, cmd, "Anonymous", pReg, path);
  }
}

void Tracer::i_trace(IService* svc, TracerCommand cmd, const std::string& caller, IRegistry* pReg, const char* path) {
  string trace_cmd;
#define TRACE_CMD(x)  case TRACE_##x: trace_cmd=#x; break
  switch(cmd) {
  TRACE_CMD(REGOBJ);
  TRACE_CMD(FIND);
  TRACE_CMD(DATAFAULT);
  TRACE_CMD(DATAFAULT_OK);
  TRACE_CMD(DATAFAULT_FAIL);
  TRACE_CMD(LOAD);
  TRACE_CMD(RETRIEVE);

  TRACE_CMD(CLEAR);
  TRACE_CMD(CLEAR_ROOT);
  TRACE_CMD(TRAVERSE);
  TRACE_CMD(SETROOT);
  TRACE_CMD(REGADDR);
  TRACE_CMD(UNREGADDR);
  TRACE_CMD(UNREGOBJ);
  TRACE_CMD(LINK);
  TRACE_CMD(UNLINK);
  default: trace_cmd="UNKNOWN";
    break;
  }
  if ( pReg != 0 ) {
    DataObject* pObj = pReg->object();
    if ( pObj ) {
      if ( typeid(*pObj) == typeid(DataObject) ) {
	return;
      }
    }
  }
  AlgEntry e("UnknownAlg",0);
  if ( !m_algorithms.empty() )   {
    e = m_algorithms.back();
  }
  string object_path;
  if ( pReg )  {
    object_path = pReg->identifier();
  }
  if ( path ) object_path += path;

  if ( s_print_levels&PRINT_TRACES ) {
    cout << m_nested;
    if ( svc )
      cout << " StoreTracer [" << svc->name() <<"] ";
    else
      cout << " StoreTracer [----] ";
    cout << indent(e.second);
    cout << "(" << caller;
    if ( !m_algorithms.empty() )   {
      if ( e.first != caller ) cout << " -- " << e.first;
      cout << " [" << e.second << "]";
    }
    cout << ") : " << setw(16) << left << trace_cmd << " : " << object_path << endl;
  }
  AccessEntries::iterator idata=m_data.find(object_path);
  if ( idata == m_data.end() ) {
    m_data.insert(make_pair(object_path,make_pair(0,make_pair(0,0))));
    idata = m_data.find(object_path);
    (*idata).second.first = m_data.size();
  }
  (*idata).second.second.first |= cmd;
  (*idata).second.second.second++;

  if ( object_path == "/Event" ) return;
  AlgEntries::iterator i = m_algos.find(caller);
  if ( i == m_algos.end() )   {
    addAlgorithm(m_current,0);
    fillAlgProperties(m_current);
    i = m_algos.find(m_current ? m_current->name() : "Framework");
    (*i).second->level = e.second;
  }
  if ( trace_cmd == "RETRIEVE" )
    (*i).second->get.insert(object_path);
  else if ( trace_cmd == "REGOBJ" )
    (*i).second->put.insert(object_path);
  else if ( trace_cmd == "DATAFAULT_FAIL" ) {    
  }
}

void Tracer::makeCode() {
  int max_level = 0;
  char text[4096*4];
  map<int,AlgInfo*> ordered;
  FILE* out = ::fopen("code.cpp","w");

  for(AlgEntries::iterator i=m_algos.begin(); i!=m_algos.end(); ++i) {
    AlgInfo* e = (*i).second;
    ordered.insert(make_pair(e->order,e));
    if ( e->level>max_level ) max_level = e->level;
  }

  // Patch for tools: 
  // If this damn alg does not need any input, we asume it needs all inputs created 
  // so far by the upper algorithms
  set<string> data_produced;
  for(map<int,AlgInfo*>::iterator i=ordered.begin(); i!=ordered.end(); ++i) {
    AlgInfo* e = (*i).second;
    e->aux.clear();
    for(set<string>::iterator j=data_produced.begin(); j!=data_produced.end();++j)
      e->aux.insert(*j);
    for(set<string>::iterator j=e->put.begin(); j!=e->put.end();++j)
      data_produced.insert(*j);
  }

  for(int k=0; k<max_level; ++k) {
    for(map<int,AlgInfo*>::iterator i=ordered.begin(); i!=ordered.end(); ++i) {
      AlgInfo* e = (*i).second;
      AlgInfo* p = e->parent;
      if ( p ) {
	if ( p->flags&IS_GAUDISEQUENCER ) {
	  for(set<string>::iterator j=e->get.begin(); j!=e->get.end();++j)
	    p->get.insert(*j);
	  for(set<string>::iterator j=e->put.begin(); j!=e->put.end();++j)
	    p->put.insert(*j);
	}
      }
    }
  }

  for(map<int,AlgInfo*>::iterator i=ordered.begin(); i!=ordered.end(); ++i) {
    AlgInfo* e = (*i).second;
    for(set<string>::iterator j=e->get.begin(); j!=e->get.end();++j) {
      set<string>::iterator s = e->put.find(*j);
      if ( s != e->put.end() )  {
	e->get.erase(j);
	j = e->get.begin();
	if ( j == e->get.end() ) break;
      }
    }
  }

  ::fprintf(out,"/*\n=================  Algorithm execution list:\n");
  for(map<int,AlgInfo*>::iterator i=ordered.begin(); i!=ordered.end(); ++i) {
    AlgInfo* e = (*i).second;
    if ( e->flags&IS_GAUDISEQUENCER ) continue;
    if ( e->get.empty() && e->aux.empty() ) {
      ::fprintf(out,"No Inputs for algorithm:%s / %s\n",e->name.c_str(),e->typ.c_str());
    }
  }
  ::fprintf(out,"*/\n");
  for(map<int,AlgInfo*>::const_iterator i=ordered.begin(); i!=ordered.end(); ++i) {
    AlgInfo* e = (*i).second;
    float t = e->calls!= 0 ? float(e->time)/float(e->calls) : 0e0;
    float t2 = e->calls!= 0 ? float(e->time2)/float(e->calls) : 0e0;
    float delta = e->calls!= 0 ? sqrt(abs(t*t-t2)) : 0e0;
    const set<string>& in = e->get.empty() ? e->aux : e->get;

    if ( e->flags&IS_GAUDISEQUENCER ) continue;
    ::fprintf(out,"  {\n");
    ::sprintf(text,"const char* daughters[] = {");
    for(std::vector<AlgInfo*>::const_iterator dit=e->daughters.begin(); dit!=e->daughters.end();++dit)
      ::sprintf(text+::strlen(text),"\"%s\",",(*dit)->name.c_str());
    ::fprintf(out,"    %s0};\n",text);
    if ( e->get.empty() ) ::fprintf(out,"    // Input data estimated \n");
    ::sprintf(text,"const char* inputs[] = {");
    
    for(set<string>::const_iterator j=in.begin(); j!=in.end();++j) 
      ::sprintf(text+::strlen(text),"\"%s\",",(*j).c_str());
    ::fprintf(out,"    %s0};\n",text);
    ::sprintf(text,"const char* outputs[] = {");
    for(set<string>::const_iterator j=e->put.begin(); j!=e->put.end();++j) 
      ::sprintf(text+::strlen(text),"\"%s\",",(*j).c_str());
    ::fprintf(out,"    %s0};\n",text);    
    ::fprintf(out,"    AlgConfig cfg(\"%s\",manager,inputs,outputs,%8.3e,%8.3e);\n",e->name.c_str(),t,delta);
    ::fprintf(out,"    cfg.setDaughters(daughters);\n");
    ::fprintf(out,"    cfg.parent  = \"%s\";\n",e->parent ? e->parent->name.c_str() : "Framework");
    ::fprintf(out,"    cfg.order   = %d;\n",e->order);
    ::fprintf(out,"    cfg.level   = %d;\n",e->level);
    ::fprintf(out,"    cfg.flags   = 0x%X;\n",e->flags);
    ::fprintf(out,"    cfg.type    = \"%s\";\n",e->typ.c_str());
    ::fprintf(out,"    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);\n");
    ::fprintf(out,"    a->initialize();\n");
    ::fprintf(out,"  }\n");
  }
  ::fclose(out);
}
