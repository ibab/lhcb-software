// $Id: RootEvtSelector.cpp,v 1.5 2010-03-03 14:30:47 frankb Exp $
//====================================================================
//	RootSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : RootCnv
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "RootEvtSelector.h"
#include <vector>
class TBranch;

namespace Gaudi {
  /** @class RootContext
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class RootContext : public IEvtSelector::Context {
  public:
    typedef std::vector<std::string> Files;
  private:
    const RootEvtSelector*        m_sel;
    Files                         m_files;
    Files::const_iterator         m_fiter;
    long                          m_entry;
    TBranch*                      m_branch;
  public:
    /// Standard constructor
    RootContext(const RootEvtSelector* s) : m_sel(s), m_entry(-1), m_branch(0){}
    /// Standard destructor
    virtual ~RootContext()                           {                        }
    const Files& files() const                       { return m_files;        }
    void setFiles(const Files& f)                    { 
      m_files = f; 
      m_fiter=m_files.begin();
    }
    virtual void* identifier() const                 { return (void*)m_sel;   }
    Files::const_iterator fileIterator() const       { return m_fiter;        }
    void setFileIterator(Files::const_iterator i)    { m_fiter = i;           }
    long entry() const                               { return m_entry;        }
    void setEntry(long e)                            { m_entry = e;           }
    TBranch* branch() const                          { return m_branch;       }
    void setBranch(TBranch* b)                       { m_branch = b;          }
  };
}

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "RootCnvSvc.h"
#include "RootDataConnection.h"
#include "TBranch.h"

using namespace Gaudi;
using namespace std;


RootEvtSelector::RootEvtSelector(const string& name,ISvcLocator* svcloc )
: base_class(name, svcloc), m_rootCLID(CLID_NULL)
{
  m_cnvSvcName = "RootCnvSvc";
  declareProperty("DbType",  m_dummy);
}

StatusCode RootEvtSelector::error(const string& msg) const   {
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

/// IService implementation: Db event selector override
StatusCode RootEvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() ) {
    return error("Error initializing base class Service!");
  }
  // Retrieve conversion service handling event iteration
  SmartIF<IConversionSvc> cnv(serviceLocator()->service(m_cnvSvcName));
  m_dbMgr = dynamic_cast<RootCnvSvc*>(cnv.get());
  if( !m_dbMgr ) {
    return error("Unable to localize service:"+m_cnvSvcName);
  }
  m_dbMgr->addRef();
  // Get DataSvc
  SmartIF<IDataManagerSvc> eds(serviceLocator()->service("EventDataSvc"));
  if( !eds.isValid() ) {
    return error("Unable to localize service EventDataSvc");
  }
  m_rootCLID = eds->rootCLID();
  m_rootName = eds->rootName();
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Selection root:" << m_rootName << " CLID:" << m_rootCLID << endmsg;
  return status;
}

/// IService implementation: Service finalization
StatusCode RootEvtSelector::finalize()    {
  // Initialize base class
  if ( m_dbMgr ) m_dbMgr->release();
  m_dbMgr = 0; // release
  return Service::finalize();
}

StatusCode RootEvtSelector::createContext(Context*& refpCtxt) const  {
  refpCtxt = new RootContext(this);
  return StatusCode::SUCCESS;
}

/// Access last item in the iteration
StatusCode RootEvtSelector::last(Context& /*refContext*/) const  {
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::next(Context& ctxt) const  {
  RootContext* pCtxt = dynamic_cast<RootContext*>(&ctxt);
  if ( pCtxt ) {
    TBranch* b = pCtxt->branch();
    if ( !b ) {
      RootContext::Files::const_iterator fileit = pCtxt->fileIterator();
      pCtxt->setBranch(0);
      pCtxt->setEntry(-1);
      if ( fileit != pCtxt->files().end() ) {
	RootDataConnection* con=0;
        string input = *fileit;
	StatusCode sc=m_dbMgr->connectDatabase(input,IDataConnection::READ,&con);
	if ( sc.isSuccess() ) {
	  b = con->getBranch(m_dbMgr->section(),m_rootName,m_rootCLID);
	  if ( b ) {
	    pCtxt->setBranch(b);
	    return next(ctxt);
	  }
	}
	m_dbMgr->disconnect(input).ignore();
	pCtxt->setFileIterator(++fileit);
	return next(ctxt);
      }
      return StatusCode::FAILURE;
    }
    long ent = pCtxt->entry();
    Long64_t nent = b->GetEntries();
    if ( nent > (ent+1) ) {
      pCtxt->setEntry(++ent);
      return StatusCode::SUCCESS;
    }
    RootContext::Files::const_iterator fit = pCtxt->fileIterator();
    pCtxt->setFileIterator(++fit);
    pCtxt->setEntry(-1);
    pCtxt->setBranch(0);
    return next(ctxt);
  }
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::next(Context& ctxt,int jump) const  {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::previous(Context& /* ctxt */) const   {
  return error("EventSelector Iterator, operator -- not supported ");
}

StatusCode RootEvtSelector::previous(Context& ctxt, int jump) const  {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::rewind(Context& ctxt) const   {
  RootContext* pCtxt = dynamic_cast<RootContext*>(&ctxt);
  if ( pCtxt ) {
    RootContext::Files::const_iterator fileit = pCtxt->fileIterator();
    if ( fileit != pCtxt->files().end() ) {
      string input = *fileit;
      m_dbMgr->disconnect(input).ignore();
    }
    pCtxt->setEntry(-1);
    pCtxt->setBranch(0);
    pCtxt->setFileIterator(pCtxt->files().begin());
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
RootEvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const  {
  const RootContext* pctxt = dynamic_cast<const RootContext*>(&ctxt);
  if ( pctxt ) {
    long ent = pctxt->entry();
    if ( ent >= 0 )  {
      RootContext::Files::const_iterator fileit = pctxt->fileIterator();
      if ( fileit != pctxt->files().end() ) {
	const std::string par[2] = {*fileit, m_rootName};
	const unsigned long ipar[2] = {0,ent};
	return m_dbMgr->createAddress(m_dbMgr->repSvcType(),m_rootCLID,&par[0],&ipar[0],pAddr);
      }
    }
  }
  pAddr = 0;
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::releaseContext(Context*& ctxt) const   {
  RootContext* pCtxt = dynamic_cast<RootContext*>(ctxt);
  if ( pCtxt ) {
    delete pCtxt;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
RootEvtSelector::resetCriteria(const string& criteria, Context& context)  const
{
  MsgStream log(msgSvc(), name());
  RootContext* ctxt = dynamic_cast<RootContext*>(&context);
  string db, typ, item, sel, stmt, aut, addr;
  if ( ctxt )  {
    if ( criteria.substr(0,5) == "FILE " )  {
      // The format for the criteria is:
      //        FILE  filename1, filename2 ...
      db = criteria.substr(5);
    }
    else  {
      Tokenizer tok(true);
      tok.analyse(criteria," ","","","=","'","'");
      for(Tokenizer::Items::iterator i=tok.items().begin(); i!=tok.items().end();i++) {
        string tmp = (*i).tag().substr(0,3);
        if(tmp=="DAT")  {
          db = (*i).value();
        }
        if(tmp=="OPT")   {
          if((*i).value() != "REA")   {
            log << MSG::ERROR << "Option:\"" << (*i).value() << "\" not valid" << endmsg;
            return StatusCode::FAILURE;
          }
        }
        if (tmp=="TYP") {
          typ = (*i).value();
        }
        if(tmp=="ADD")  {
          item = (*i).value();
        }
        if(tmp=="SEL")  {
          sel = (*i).value();
        }
        if(tmp=="FUN")  {
          stmt = (*i).value();
        }
        if(tmp=="AUT")  {
          aut = (*i).value();
        }
        if(tmp=="COL")  {
          addr = (*i).value();
        }
      }
    }
    // It's now time to parse the criteria for the event selection
    // The format for the criteria is:
    //        FILE  filename1, filename2 ...
    //        JOBID number1-number2, number3, ...
    RootContext::Files files;
    string rest = db;
    files.clear();
    while(true)  {
      int ipos = rest.find_first_not_of(" ,");
      if (ipos == -1 ) break;
      rest = rest.substr(ipos,string::npos);// remove blanks before
      int lpos  = rest.find_first_of(" ,");       // locate next blank
      files.push_back(rest.substr(0,lpos )); // insert in list
      if (lpos == -1 ) break;
      rest = rest.substr(lpos,string::npos);// get the rest
    }
    ctxt->setFiles(files);
    ctxt->setFileIterator(ctxt->files().begin());
    return StatusCode::SUCCESS;
  }
  return error("Invalid iteration context.");
}


#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootEvtSelector);
