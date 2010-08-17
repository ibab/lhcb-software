
namespace Gaudi {

  class RootTool : virtual public RootDataConnection::Tool {
  public:
    RootTool(RootDataConnection* con) { c = con; }

    /// Access data branch by name: Get existing branch in read only mode
    virtual TBranch* getBranch(CSTR section, CSTR n) {
      TTree* t = c->getSection(section);
      TBranch* b = t ? t->GetBranch(n.c_str()) : 0;
      if ( b ) b->SetAutoDelete(kFALSE);
      return b;
    }

    /// Load references object
    virtual int loadRefs(CSTR section, CSTR cnt, unsigned long entry, RootObjectRefs& refs)   {
      TBranch* b = getBranch(section,cnt+"#R");
      if ( b ) {
	RootObjectRefs* prefs = &refs;
	b->SetAddress(&prefs);
	return b->GetEntry(entry);
      }
      return -1;
    }

    void addParam(RootDataConnection::ParamMap& c, char* p) {
      char* q = strchr(p,'=');
      if ( q ) {
	*q = 0;
	c.push_back(make_pair(p,++q));
      }
    }
    void addEntry(RootDataConnection::StringVec& c, char* val) {
      c.push_back(val);
    }
    template <class C, class F> StatusCode readBranch(const char* nam, C& v, F pmf) {
      char text[2048];
      TBranch* b = refs()->GetBranch(nam);
      if ( b ) {
	TLeaf* l = b->GetLeaf(nam);
	if ( l ) {
	  b->SetAddress(text);
	  for(Long64_t i=0, n=b->GetEntries(); i<n; ++i)
	    if ( b->GetEntry(i)>0 )
	      (this->*pmf)(v,(char*)l->GetValuePointer());
	  return StatusCode::SUCCESS;
	}
      }
      MsgStream msg(msgSvc(),name());
      msg << MSG::ERROR << "Failed to read '" << nam << "' table." << endmsg;
      return StatusCode::FAILURE;
    }
    /// Internal helper to read reference tables
    StatusCode readRefs()  {
      if ( refs() ) {
	if ( !readBranch("Databases", dbs(),    &RootTool::addEntry).isSuccess() )
	  return StatusCode::FAILURE;
	if ( !readBranch("Containers",conts(),  &RootTool::addEntry).isSuccess() )
	  return StatusCode::FAILURE;
	if ( !readBranch("Links",     links(),  &RootTool::addEntry).isSuccess() )
	  return StatusCode::FAILURE;
	if ( !readBranch("Params",    params(), &RootTool::addParam).isSuccess() )
	  return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
    }

    string getEntry(const string& c)              {  return c;                    }
    string getParam(const pair<string,string>& p) {  return p.first+"="+p.second; }

    template <class C, class F> StatusCode saveBranch(const char* nam, C& v, F pmf) {
      Long64_t i, n;
      string val, typ = nam;
      StatusCode sc = StatusCode::SUCCESS;
      TDirectory::TContext ctxt(c->file());
      TBranch* b = refs()->GetBranch(nam);
      if ( !b ) b = refs()->Branch(nam,0,(typ+"/C").c_str());
      if ( b ) {
	for(i=b->GetEntries(), n=v.size(); i<n; ++i) {
	  val = (this->*pmf)(v[i]);
	  b->SetAddress((char*)val.c_str());
	  if ( b->Fill() <= 1) sc = StatusCode::FAILURE;
	}
	return sc;
      }
      return StatusCode::FAILURE;
    }
    /// Internal helper to save/update reference tables
    StatusCode saveRefs() {
      RootDataConnection* c = c;
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
