#include "PoolClasses.h"
/*
*  Gaudi namespace declaration
*/
namespace Gaudi {


  /** @class RootTool RootTool.h src/RootTool.h
  *
  * Description:
  *
  * Concrete implementation to read objects from POOL files.
  *
  * @author  M.Frank
  * @version 1.0
  */
  class PoolTool : virtual public RootDataConnection::Tool {
    /// Image of the POOL ##Links table
    std::vector<Gaudi::RootRef> m_poolLinks;

  public:
    /// Standard constructor
    PoolTool(RootDataConnection* con) { c = con; }

    /// Convert TES object identifier to ROOT tree name
    string _treeName(string t) {
      for(string::iterator j = t.begin(); j != t.end(); ++j )
        if ( *j == '/' ) *j = '_';
      return t;
    }

    virtual RootRef poolRef(size_t i) const { return m_poolLinks[i]; }

    /// Load references object from file
    virtual int loadRefs(CSTR /* section */, CSTR cnt, unsigned long entry, RootObjectRefs& refs)   {
      TTree* t = sections()[cnt];
      if ( !t ) {
        t = (TTree*)c->file()->Get(_treeName(cnt).c_str());
      }
      if ( t ) {
        TBranch* b1 = t->GetBranch("Links");
        TBranch* b2 = t->GetBranch("Refs");
        MsgStream& msg = msgSvc();
        if ( b1 && b2 ) {
          LinkManager lm, *plm = &lm;
          PoolDbLinkManager mgr, *pmgr = &mgr;
          b1->SetAutoDelete(kFALSE);
          b2->SetAutoDelete(kFALSE);
          b1->SetAddress(&plm);
          b2->SetAddress(&pmgr);
          int nb1 = b1->GetEvent(entry);
          int nb2 = b2->GetEvent(entry);
          if ( nb1>1 && nb2>1 ) {
            size_t ref_size  = mgr.references().size();
            refs.refs.resize(ref_size);
            msg << MSG::VERBOSE;
            for(size_t j = 0; j < ref_size; ++j) {
              const pair<int, int>& oid = mgr.references()[j]->m_oid;
              string loc = mgr.links()[j].substr(1);
              RootRef& r  = refs.refs[j];
              if ( oid.first>=0 ) {
                r = m_poolLinks[oid.first];
                r.entry = oid.second;
                r.link  = c->makeLink(loc);
                msg << "Add leaf:" << oid.first << "->" << loc << " from " << c->getDb(r.dbase)
                  << "#" << c->getCont(r.container)
                  << " Link:" << c->getLink(r.link)
                  << " CLID:" << hex << r.clid
                  << " Typ:" << hex << int(r.svc)
                  << " Ent:" << dec << r.entry << endl;
              }
            }
            for(int i = 0, n=lm.size(); i < n; ++i)  {
              LinkManager::Link* lnk = lm.link(i);
              int link_id = c->makeLink(lnk->path());
              msg << "Add Link:" << lnk->path() << endl;
              refs.links.push_back(link_id);
            }
            return nb1 + nb2;
          }
        }
        msg << MSG::ERROR << "Failed to access POOL Ref/Link branches:" << cnt
          << " [" << _treeName(cnt) << "]" << endmsg;
        t->Print();
      }
      return -1;
    }

    /// Access data branch by name: Get existing branch in read only mode
    virtual TBranch* getBranch(CSTR /* section */, CSTR branch_name) {
      TTree* t = sections()[branch_name];
      if ( t ) {
        return (TBranch*)t->GetListOfBranches()->At(0);
      }
      string tname = _treeName(branch_name);
      t = (TTree*)c->file()->Get(tname.c_str()); // c->getSection(tname);
      if ( t )   {
        TBranch* b = (TBranch*)t->GetListOfBranches()->At(0);
        if ( b ) {
          sections()[branch_name] = t;
          return b;
        }
        msgSvc() << MSG::ERROR << "Failed to access POOL branch:"
          << branch_name << " [" << tname << "]" << endmsg;
        t->Print();
      }
      return 0;
    }

    /// Save references section when closing data file (NOT SUPPORTED)
    virtual StatusCode saveRefs()     {      return StatusCode::FAILURE;     }

    /// Internal helper to read reference tables ##Params and ##Links
    StatusCode readRefs()  {
      int i;
      char text[2048];
      msgSvc() << MSG::VERBOSE;

      // First read ##Params
      TTree* t = (TTree*)c->file()->Get("##Params");
      if ( 0 == t ) {
        return StatusCode::FAILURE;
      }
      TBranch* b = t->GetBranch("db_string");
      if ( 0 == b ) {
        return StatusCode::FAILURE;
      }
      for(i=0,b->SetAddress(text); i < b->GetEntries(); ++i) {
        b->GetEvent(i);
        char* id1 = strstr(text,"[NAME=");
        char* id2 = strstr(text,"[VALUE=");
        if ( id1 && id2 )  {
          id1 += 6;
          id2 += 7;
          char* id11 = strstr(id1, "]");
          char* id22 = strstr(id2, "]");
          if ( id11 && id22 )  {
            *id11 = 0;
            *id22 = 0;
            params().push_back(make_pair(id1,id2));
            msgSvc() << "Param:" << id1 << "=" << id2 << "." << endmsg;
          }
        }
      }

      // Read ##Links
      t = (TTree*)c->file()->Get("##Links");
      if ( 0 == t ) {
        return StatusCode::FAILURE;
      }
      b = t->GetBranch("db_string");
      if ( 0 == b ) {
        return StatusCode::FAILURE;
      }
      m_poolLinks.resize((size_t)b->GetEntries()+2); // Take into account the ##Links and ##Shapes entry of POOL!
      for(i=0,b->SetAddress(text); i < b->GetEntries(); ++i) {
        b->GetEvent(i);
        std::string db, container;
        int clid = 1, technology = 0, ipar[2] = {-1,-1};
        for(char* p1 = (char*)text; p1; p1 = ::strchr(++p1,'[')) {
          char* p2 = ::strchr(p1, '=');
          char* p3 = ::strchr(p1, ']');
          if ( p2 && p3 )   {
            if ( ::strncmp("[DB=", p1, 4) == 0 )  {
              *p3 = 0;
              db = p1+4;
            }
            else if ( ::strncmp("[CNT=", p1, 5) == 0 )  {
              *p3 = 0;
              container = p1+5;
            }
            else if ( ::strncmp("[OID=", p1, 5) == 0 )  {
              *p3 = 0;
              ::sscanf(p1+5,"%08X,%08X",(unsigned int*)&ipar[0],(unsigned int*)&ipar[1]);
            }
            else if ( ::strncmp("[CLID=", p1, 6) == 0 )  {
              *p3 = 0;
              ::sscanf(p1+6,"%08X",(unsigned int*)&clid);
            }
            else if ( ::strncmp("[TECH=", p1, 6) == 0 )  {
              *p3 = 0;
              ::sscanf(p1+6,"%08X",(unsigned int*)&technology);
            }
            else    {
              *p3 = *p2 = 0;
            }
            *p3 = ']';
            *p2 = '=';
          }
        }
        c->makeRef("",clid,technology,db,container,-1,m_poolLinks[i+2]);
        RootRef& r = m_poolLinks[i];
        msgSvc() << "Add link[" << i << "]:" << db << container << " [" << r.dbase << "," << r.container << "] "
          << " tech:" << hex << setw(8) << r.svc << " CLID:" << setw(8) << r.clid << dec << endmsg;
      }
      return StatusCode::SUCCESS;
    }
  };
}
