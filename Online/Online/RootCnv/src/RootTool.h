/*
* Gaudi namespace declaration
*/
namespace Gaudi {

  /** @class RootTool RootTool.h src/RootTool.h
  *
  * Description:
  *
  * Concrete implementation to read objects from ROOT files.
  *
  * @author  M.Frank
  * @version 1.0
  */
  class RootTool : virtual public RootDataConnection::Tool {
  public:
    /// Standard constructor
    RootTool(RootDataConnection* con) { c = con; }
    /// Access data branch by name: Get existing branch in read only mode
    virtual TBranch* getBranch(CSTR section, CSTR branch_name) {
      std::string n = branch_name+".";
      for(int i=0, m=n.length()-1; i<m; ++i) if ( !isalnum(n[i]) ) n[i]='_';
      TTree* t = c->getSection(section);
      TBranch* b = t ? t->GetBranch(n.c_str()) : 0;
      if ( !b ) b = t ? t->GetBranch(branch_name.c_str()) : 0;
      if (  b ) b->SetAutoDelete(kFALSE);
      return b;
    }
    /// Load references object from file
    virtual int loadRefs(CSTR section, CSTR cnt, unsigned long entry, RootObjectRefs& refs)   {
      TBranch* b = getBranch(section,cnt+"#R");
      RootObjectRefs* prefs = &refs;
      if ( b ) {
        b->SetAddress(&prefs);
        int nb = b->GetEntry(entry);
        if ( nb >= 1 )   {
          const MergeSections& ms = c->mergeSections();
          if ( !ms.empty() ) {
            MsgStream& msg = msgSvc();
            msgSvc() << MSG::VERBOSE;
            pair<const RootRef*,const ContainerSection*> ls = c->getMergeSection(cnt,entry);
            if ( ls.first )  {
              if ( ls.first->dbase >= 0 ) {
                // Now patch the references and links 'en block' to be efficient
                // First the leafs from the TES
                if ( msg.isActive() ) {
                  msg << "Refs: LS [" << entry << "] -> " 
                    << ls.first->dbase << "," << ls.first->container 
                    << "," << ls.first->link 
                    << "," << ls.first->entry
                    << endmsg;
                }
                for(size_t j=0, n=refs.refs.size(); j<n; ++j)  {
                  RootRef& r = refs.refs[j];
                  if ( r.entry>= 0 && r.dbase >= 0 ) {
                    int db = r.dbase + ls.first->dbase;
                    if ( c->getDb(db) == c->fid() ) {
                      if ( r.dbase      ) r.dbase     += ls.first->dbase;
                      if ( r.container  ) r.container += ls.first->container;
                      if ( r.link       ) r.link      += ls.first->link;
                      const string& rc = c->getCont(r.container);
                      MergeSections::const_iterator k = ms.find(rc);
                      if ( k != ms.end() )   {
                        const ContainerSections& cs = (*k).second;
                        r.entry = ( ls.first->entry >= 0 && ls.first->entry < (int)cs.size() )
                          ? cs[ls.first->entry].start + r.entry : -1;
                        if ( msg.isActive() ) {
                          msg << "Add link [" << r.entry 
                            << "," << ls.first->entry 
                            << "," << ls.first->container
                            << "," << r.container
                            << "," << r.entry
                            << "] to -> " << rc << endmsg;
                        }
                      }
                      else {
                        msg << MSG::WARNING << c->fid() << "  [" << c->getDb(db) 
                          << "] Evt:" << entry << " Invalid link to " << rc << endmsg;
                        msg << MSG::VERBOSE;
                      }
                    }
                  }
                }
                /// Link manager:
                for(vector<int>::iterator i=refs.links.begin(); i!=refs.links.end();++i) {
                  (*i) += ls.first->link;
                }
              }
              return nb;
            }
            return -1;
          }
          return nb;
        }
      }
      return -1;
    }
    /// Helper function to read params table
    void addParam(ParamMap& c, char* p) {
      char* q = strchr(p,'=');
      if ( q ) {
        *q = 0;
        c.push_back(make_pair(p,++q));
      }
    }
    /// Helper function to read string tables
    void addEntry(StringVec& c, char* val) {      c.push_back(val);    }
    /// Helper function  to read internal file tables
    template <class C, class F> StatusCode readBranch(TTree* t, const char* nam, C& v, F pmf) {
      char text[2048];
      TBranch* b = t->GetBranch(nam);
      if ( b ) {
        TLeaf* l = b->GetLeaf(nam);
        if ( l ) {
	  StatusCode sc = StatusCode::SUCCESS;
          b->SetAddress(text);
          msgSvc() << MSG::VERBOSE;
          for(Long64_t i=0, n=b->GetEntries(); i<n; ++i) {
            if ( b->GetEntry(i)>0 ) {
              char* p = (char*)l->GetValuePointer();
              msgSvc() << "Add Value[" << b->GetName() << "]:" << p << endmsg;
              (this->*pmf)(v,p);
            }
	    else {
	      sc = RootDataConnection::ROOT_READ_ERROR;
	    }
          }
          return sc;
        }
      }
      msgSvc() << MSG::ERROR << "Failed to read '" << nam << "' table." << endmsg;
      return RootDataConnection::ROOT_READ_ERROR;
    }
    /// Analyze the Sections table entries
    bool get(const string& dsc, pair<string,ContainerSection>& e) {
      if ( dsc != "[END-OF-SECTION]" ) { 
        size_t id1 = dsc.find("[CNT=");
        size_t id2 = dsc.find("[START=");
        size_t id3 = dsc.find("[LEN=");
        if ( id1 != string::npos && id2 != string::npos && id3 != string::npos ) {
          string tmp;
          string cnt = dsc.substr(id1+5, id2-1-5);
          int section_start  = ::atoi((tmp=dsc.substr(id2+7,id3-id2-8)).c_str());
          int section_length = ::atoi((tmp=dsc.substr(id3+5,dsc.find("]",id3+5)-id3-5)).c_str());
          e.first = cnt;
          e.second = ContainerSection(section_start,section_length);
          return true;
        }
      }
      e.first = "";
      e.second = ContainerSection(-1,-1);
      return false;
    }
    /// Build merge sections from the Sections table entries
    void analyzeMergeMap(StringVec& tmp) {
      StringVec::const_iterator i;
      LinkSections&  ls = linkSections();
      MergeSections& ms = mergeSections();
      pair<string,ContainerSection> e;
      MsgStream& msg = msgSvc();
      RootRef r;
      int cnt = 0;
      ls.clear();
      ms.clear();
      msg << MSG::VERBOSE;
      r.dbase = r.container = r.link = r.clid = r.svc = r.entry = 0;
      for(i=tmp.begin(); i!=tmp.end();++i) {
        if ( get(*i,e) ) {
          msg << "Added Merge Section:" << e.first << endmsg;
          ms[e.first].push_back(e.second);
          if (      e.first == "Links" )
            r.link      = e.second.start;
          else if ( e.first == "Containers" )
            r.container = e.second.start;
          else if ( e.first == "Databases" )
            r.dbase     = e.second.start;
          else if ( e.first == "Params" )
            r.svc       = e.second.start;
        }
        else if ( (*i) == "[END-OF-SECTION]" ) {
          r.entry = cnt;
          if ( msg.isActive() ) {
            msg << "Link Section [" << r.entry << "," << ls.size()
              << "] -> D:" << r.dbase
              << " C:" << r.container
              << " L:" << r.link 
              << " P:" << r.svc
              << endmsg;
          }
          ls.push_back(r);
          cnt++;
        }
      }
    }
    /// Read reference tables
    StatusCode readRefs()  {
      TTree* t = (TTree*)c->file()->Get("Sections");
      StatusCode sc(StatusCode::SUCCESS,true);
      StringVec tmp;
      if ( t && !(sc=readBranch(t,  "Sections",  tmp,       &RootTool::addEntry)).isSuccess() )
        return sc;
      else if ( refs() ) {
        analyzeMergeMap(tmp);
        if ( !(sc=readBranch(refs(),"Databases", dbs(),     &RootTool::addEntry)).isSuccess() )
          return sc;
        if ( !(sc=readBranch(refs(),"Containers",conts(),   &RootTool::addEntry)).isSuccess() )
          return sc;
	if ( !(sc=readBranch(refs(),"Links",     links(),   &RootTool::addEntry)).isSuccess() )
          return sc;
	if ( !(sc=readBranch(refs(),"Params",    params(),  &RootTool::addParam)).isSuccess() )
          return sc;
        return sc;
      }
      return StatusCode::FAILURE;
    }
    /// Helper function to convert string vectors to branch entries
    string getEntry(const string& c)              {  return c;                    }
    /// Helper function to convert parameter vectors to branch entries
    string getParam(const pair<string,string>& p) {  return p.first+"="+p.second; }
    /// Helper function to save internal tables
    template <class C, class F> StatusCode saveBranch(const char* nam, C& v, F pmf) {
      Long64_t i, n;
      string val, typ = nam;
      StatusCode sc = StatusCode::SUCCESS;
      TDirectory::TContext ctxt(c->file());
      TBranch* b = refs()->GetBranch(nam);
      if ( !b ) b = refs()->Branch(nam,0,(typ+"/C").c_str());
      if ( b ) {
        for(i=b->GetEntries(), n=Long64_t(v.size()); i<n; ++i) {
          val = (this->*pmf)(v[size_t(i)]);
          b->SetAddress((char*)val.c_str());
          msgSvc() << MSG::VERBOSE << "Save Value[" << b->GetName() << "]:" << val << endmsg;
          if ( b->Fill() < 0 ) sc = StatusCode::FAILURE;
        }
        return sc;
      }
      return StatusCode::FAILURE;
    }
    /// Save/update reference tables
    StatusCode saveRefs() {
      if ( refs() ) {
        if ( !saveBranch("Databases", dbs(),   &RootTool::getEntry).isSuccess() )
          return StatusCode::FAILURE;
        if ( !saveBranch("Containers",conts(), &RootTool::getEntry).isSuccess() )
          return StatusCode::FAILURE;
        if ( !saveBranch("Links",     links(), &RootTool::getEntry).isSuccess() )
          return StatusCode::FAILURE;
        if ( !saveBranch("Params",    params(),&RootTool::getParam).isSuccess() )
          return StatusCode::FAILURE;
        return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
    }
  };
}
