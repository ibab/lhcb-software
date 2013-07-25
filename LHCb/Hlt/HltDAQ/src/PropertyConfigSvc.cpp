// Include files

#include <map>
#include <string>

class StatusCode;
namespace Gaudi { namespace Parsers {
StatusCode parse(std::map<std::string, std::map<std::string,
                 std::map< std::string, std::string> > >& result,
                 const std::string& input ) ;
} }
#include <sstream>
#include <algorithm>
#include <list>
#include <iterator>

#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/IAlgTool.h"


// local
#include "PropertyConfigSvc.h"

using namespace std;
using boost::ptr_vector;
namespace bl = boost::lambda;



//-----------------------------------------------------------------------------
// Implementation file for class : PropertyConfigSvc
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_SERVICE_FACTORY( PropertyConfigSvc )


namespace {

    // model this as output iterator
    class property2jos  : public std::iterator<std::output_iterator_tag,const PropertyConfig::Prop> {
         public:
            property2jos& operator++()   { return *this; }
//            property2jos operator++(int) { return *this; } // Not yet implemented
            property2jos& operator*()    { return *this; }
            property2jos(IJobOptionsSvc* jos,const string& name, ostream* os=0) :
                m_jos(jos),m_name(name),m_properties(jos->getProperties(name)),m_out(os) { assert(m_jos!=0); }
            property2jos& operator=(const PropertyConfig::Prop& prop) {
                if (m_out!=0) {
                   *m_out << m_name << '.' <<prop.first<< '=' << prop.second << ";\n";
                }
                const Property *p = (m_properties !=0 ? find(prop.first): 0);
                if ( p==0 || p->toString() != prop.second ) {  // only update if non-existant or not up-to-date
                    // resolve references
                    // this is done in order to support the online use case, where we have options
                    // of the form foo.something = @OnlineEnv.somethingElse
                    // WARNING: values are substituted when seen, so there is a possibility that they are
                    //          substituted just prior to the reference changing -- this is not the case
                    //          in the online scenario, as the reference come from 'static' items, _not_
                    //          configured through this service!!!
                    // NOTE: if the reference is not found, optionally, a default, can be specified.
                    //       eg.  foo.something = @OnlineEnv.somethingelse@somedefault
                   if (!prop.second.empty() && prop.second[0]=='@') {
                       static boost::regex pattern("^@([^\\.]+)\\.([^@]+)(@.+)?$");
                       boost::smatch what;
                       if (!boost::regex_match(prop.second,what,pattern) ) {
                            throw GaudiException(prop.second,"badly formatted reference property ", StatusCode::FAILURE);
                       }
                       std::string value;
                       const Property * refProp = Gaudi::Utils::getProperty( m_jos->getProperties(string(what[1])),string(what[2]));
                       if (refProp!=0) {
                            value = refProp->toString();
                       } else if (what[3].first!=what[3].second) {
                           value = string(what[3]).substr(1);
                       } else {
                           throw GaudiException(prop.second,"failed to find reference property ", StatusCode::FAILURE);
                       }
                       m_jos->addPropertyToCatalogue(m_name, StringProperty(prop.first,value));
                       // Q: we do not chase references (to references, to ... ) -- should we?
                       // A: no, as the JobOptionsSvc parser substitutes values, so what we get back from
                       //    the jos NEVER has references in it...
                   } else {
                       m_jos->addPropertyToCatalogue(m_name, StringProperty(prop.first,prop.second));
                   }
                }
                return *this;
            }
         private:
            const Property* find(const string& name) {
               vector<const Property*>::const_iterator i =
                  find_if(m_properties->begin(),
                          m_properties->end(),
                          bl::bind(&Property::name,bl::_1)==name);
               return i==m_properties->end() ? 0 : *i;
            }
            IJobOptionsSvc*                m_jos;
            string                         m_name;
            const vector<const Property*> *m_properties;
            ostream                       *m_out;
    };
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PropertyConfigSvc::PropertyConfigSvc( const string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  , m_joboptionsSvc(0)
  , m_toolSvc(0)
  , m_algMgr(0)
  , m_appMgrUI(0)
  , m_accessSvc(0)
  , m_os(0)
{
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigTarFileAccessSvc");
  declareProperty("prefetchConfig", m_prefetch);
  declareProperty("SkipComponent", m_skip); // do not touch these algorithms configurations, NOR THEIR DEPENDENTS!
  declareProperty("optionsfile", m_ofname);
  declareProperty("createGraphVizFile", m_createGraphVizFile=false);
  declareProperty("AllowFlowChanges",m_allowFlowChanges = false);
  declareProperty("ApplyTransformation",m_transform);
}

//=============================================================================
// Destructor
//=============================================================================
PropertyConfigSvc::~PropertyConfigSvc() {
}

MsgStream& PropertyConfigSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     *m_msg << level;
     return *m_msg;
}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode PropertyConfigSvc::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown) {
  if ( IPropertyConfigSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IPropertyConfigSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode PropertyConfigSvc::initialize() {
   if(msgLevel(MSG::DEBUG)) debug() << "Initialize" << endmsg;
   StatusCode status = Service::initialize();
   if ( !status.isSuccess() )   return status;
   status = service(s_accessSvc,m_accessSvc);
   if ( !status.isSuccess() )   return status;
   status = service("ApplicationMgr",m_algMgr);
   if ( !status.isSuccess() )   return status;
   status = service("ApplicationMgr",m_appMgrUI);
   if ( !status.isSuccess() )   return status;
   status = service("JobOptionsSvc",m_joboptionsSvc);
   if ( !status.isSuccess() )   return status;
   status = service("ToolSvc",m_toolSvc);
   if ( !status.isSuccess() )   return status;
   m_toolSvc->registerObserver(this);

   if (!m_ofname.empty()) m_os.reset( new boost::filesystem::ofstream( m_ofname.c_str() ) );

  // read table of pre-assigned, possible configurations for this job...
  // i.e. avoid reading _everything_ when we really need to be quick
  for (vector<string>::const_iterator i = m_prefetch.begin(); i!=m_prefetch.end(); ++i ) {
     ConfigTreeNode::digest_type digest = ConfigTreeNode::digest_type::createFromStringRep(*i);
     assert( digest.str() == *i) ;
     if (loadConfig( digest ).isFailure()) {
        error() << " failed to load " << digest << endmsg;
        return StatusCode::FAILURE;
     }
     if (m_createGraphVizFile) createGraphVizFile(digest, digest.str());
  }
  return status;
}

void PropertyConfigSvc::onCreate(const IAlgTool* tool) {
    assert(tool!=0);
    string name = tool->name();
    //string key = name.substr(0,name.rfind('.'));
    if(msgLevel(MSG::DEBUG)) debug() << "adding " << name << " to toolmap " << endmsg;
    m_toolmap.insert( make_pair(name,tool) );
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode PropertyConfigSvc::finalize() {
  StatusCode status = Service::finalize();
  m_joboptionsSvc->release(); m_joboptionsSvc = 0;
  m_algMgr->release();        m_algMgr = 0;
  m_accessSvc->release();     m_accessSvc = 0;
  m_os.reset();
  return status;
}

template <typename T>
T*
PropertyConfigSvc::resolve(const string&) const
{
    error() << "Generic resolve called -- this is bad news!" << endmsg;
    return 0;
}
template <>
IService*
PropertyConfigSvc::resolve(const string& name) const
{
    IService *i(0);
    if (serviceLocator()->getService(name,i).isFailure()) return 0;
    return i;
}
template <>
IAlgorithm*
PropertyConfigSvc::resolve(const string& name) const
{
    IAlgorithm *i(0);
    if (m_algMgr->getAlgorithm(name,i).isFailure()) return 0;
    return i;
}

template <>
IAlgTool*
PropertyConfigSvc::resolve(const string& name) const
{
    std::map<std::string, const IAlgTool*>::const_iterator i = m_toolmap.find(name);
    if (i==m_toolmap.end()) return 0;
    return const_cast<IAlgTool*>(i->second);
}

template <>
Algorithm*
PropertyConfigSvc::resolve(const string& name) const
{
    return dynamic_cast<Algorithm*>(resolve<IAlgorithm>(name));
}
template <>
Service*
PropertyConfigSvc::resolve(const string& name) const
{
    return dynamic_cast<Service*>(resolve<IService>(name));
}
template <>
AlgTool*
PropertyConfigSvc::resolve(const string& name) const
{
    return dynamic_cast<AlgTool*>(resolve<IAlgTool>(name));
}

StatusCode
PropertyConfigSvc::invokeSetProperties(const PropertyConfig& conf) const {
    info() << " calling SetProperties for " << conf.name() << endmsg;
    if (conf.kind() == "IAlgorithm") {
        Algorithm *alg = resolve<Algorithm>(conf.name());
        return alg!=0 ? alg->setProperties() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IService")  {
        Service *svc = resolve<Service>(conf.name());
        return svc!=0 ? svc->setProperties() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IAlgTool")  {
        AlgTool *tool = resolve<AlgTool>(conf.name());
        return tool!=0 ? tool->setProperties() : StatusCode::FAILURE ;
    }
    return StatusCode::FAILURE;
}



//=============================================================================
// implemenation of the IPropertyConfigSvc interface:
//=============================================================================
PropertyConfig
PropertyConfigSvc::currentConfiguration(const INamedInterface& obj) const {
  //TODO: move the next blob into PropertyConfig c'tor itself???
  INamedInterface *ini = const_cast<INamedInterface*>(&obj); // we do treat obj logically const,
                                                             // even if we call code which seems
                                                             // wants non-const version of obj

  // figure out whether we have a Service, Tool, Algorithm or Auditor...
  string kind = ( SmartIF<IAlgorithm>(ini).isValid() ? "IAlgorithm"
                : SmartIF<IService  >(ini).isValid() ? "IService"
                : SmartIF<IAlgTool  >(ini).isValid() ? "IAlgTool"
                : SmartIF<IAuditor  >(ini).isValid() ? "IAuditor"
                : "Unknown" );

  SmartIF<IProperty> ip(ini);
  return PropertyConfig( obj.name(), *ip, kind);
}

PropertyConfig::digest_type
PropertyConfigSvc::findInTree(const ConfigTreeNode::digest_type& configTree, const string& name) const {
   const vector<PropertyConfig::digest_type>& leafs = collectLeafRefs(configTree);
   for( vector<PropertyConfig::digest_type>::const_iterator i = leafs.begin(); i!=leafs.end();++i) {
        const PropertyConfig* pc = resolvePropertyConfig(*i);
        assert(pc!=0);
        if ( pc->name() == name ) return *i;
   }
   return PropertyConfig::digest_type::createInvalid();
}

//=============================================================================
// Walk configuration trees (& cache the result)
//=============================================================================

ConfigTreeNode::digest_type
PropertyConfigSvc::resolveAlias(const ConfigTreeNodeAlias::alias_type& alias) const
{
    ConfigTreeNodeAliasMap_t::const_iterator i = m_aliases.find(alias);
    if (i!=m_aliases.end()) return i->second;
    boost::optional<ConfigTreeNode> node = m_accessSvc->readConfigTreeNodeAlias(alias);
    if (!node) {
        error() << " could not obtain alias " << alias << endmsg;
        return ConfigTreeNode::digest_type::createInvalid();
    }
    // add it into alias cache
    ConfigTreeNode::digest_type ref = node->digest();
    m_aliases.insert(make_pair( alias,ref ));
    // add to ConfigTreeNode cache now that we got it anyway...
    if (m_nodes.find(ref)==m_nodes.end()) {
        m_nodes.insert( make_pair( ref, *node) );
    }
    info() << " resolved " << alias << " to " << ref << endmsg;
    return ref;
}

const list<ConfigTreeNode::digest_type>&
PropertyConfigSvc::collectNodeRefs(const ConfigTreeNodeAlias::alias_type& alias) const
{
    return collectNodeRefs(resolveAlias(alias));
}

const list<ConfigTreeNode::digest_type>&
PropertyConfigSvc::collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const
{
    // first check cache...
    Tree2NodeMap_t::const_iterator j = m_nodesInTree.find(nodeRef);
    if (j!=m_nodesInTree.end()) return j->second;

    list<ConfigTreeNode::digest_type> nrefs; // note: we use list, as appending to a list
                                             // does not invalidate iterators!!!!
    nrefs.push_back(nodeRef);
    list<ConfigTreeNode::digest_type>::iterator i = nrefs.begin();
    while(i!=nrefs.end()) {
        const ConfigTreeNode *node = resolveConfigTreeNode(*i);
        if (node==0) {
            throw GaudiException("failed to resolve node ", this->name() + "::collectNodeRefs",StatusCode::FAILURE);
        }
        const ConfigTreeNode::NodeRefs& nodeRefs = node->nodes();
        for (ConfigTreeNode::NodeRefs::const_iterator j=nodeRefs.begin();j!=nodeRefs.end();++j) {
            //FIXME: this is not going to be very fast, as it going to
            // make this operation quadratic...  should keep list sorted..
            if (find(nrefs.begin(),nrefs.end(),*j)==nrefs.end()) nrefs.push_back(*j);
        }
        ++i;
    }
    // insert in cache
    pair<Tree2NodeMap_t::iterator,bool> rv = m_nodesInTree.insert( make_pair(nodeRef, nrefs) );
    return rv.first->second;
}

const vector<PropertyConfig::digest_type>&
PropertyConfigSvc::collectLeafRefs(const ConfigTreeNodeAlias::alias_type& alias) const
{
    return collectLeafRefs(resolveAlias(alias));
}

const vector<PropertyConfig::digest_type>&
PropertyConfigSvc::collectLeafRefs(const ConfigTreeNode::digest_type& nodeRef) const
{
     Tree2LeafMap_t::const_iterator i = m_leavesInTree.find(nodeRef);
     if (i!=m_leavesInTree.end()) return i->second;
     list<ConfigTreeNode::digest_type>   nodeRefs = collectNodeRefs(nodeRef);
     vector<PropertyConfig::digest_type> leafRefs;
     for (list<ConfigTreeNode::digest_type>::const_iterator i=nodeRefs.begin();i!=nodeRefs.end();++i) {
        const ConfigTreeNode *node = resolveConfigTreeNode(*i);
        PropertyConfig::digest_type leafRef = node->leaf();
        if (leafRef.valid()) leafRefs.push_back(leafRef);
     }
     pair<Tree2LeafMap_t::iterator,bool> rv = m_leavesInTree.insert( make_pair(nodeRef, leafRefs) );
     return rv.first->second;
}

StatusCode
PropertyConfigSvc::loadConfig(const ConfigTreeNode::digest_type& nodeRef)
{
     try {
         if (!nodeRef.valid()) return StatusCode::FAILURE;
         info() << "loading config " << nodeRef.str() << endmsg;
         const vector<PropertyConfig::digest_type>& list = collectLeafRefs(nodeRef);
         for (vector<PropertyConfig::digest_type>::const_iterator i = list.begin();i!=list.end();++i) {
                resolvePropertyConfig(*i);
         }
         return validateConfig( nodeRef );
     } catch ( const boost::filesystem::filesystem_error& x )  {
         error() << x.what() << endmsg;
         throw x;
     }
     return StatusCode::FAILURE;

}

//=============================================================================
// Configure
//=============================================================================
StatusCode
PropertyConfigSvc::configure(const ConfigTreeNodeAlias::alias_type& alias,
                             bool callSetProperties) const
{
    info() << " configuring using " << alias << endmsg;
    return configure( resolveAlias(alias), callSetProperties );
}

StatusCode
PropertyConfigSvc::outOfSyncConfigs(const ConfigTreeNode::digest_type& configID,
                                    back_insert_iterator<vector<const PropertyConfig*> > newConfigs) const
{
    if (!configID.valid())  return StatusCode::FAILURE;
    const vector<PropertyConfig::digest_type>& configs = collectLeafRefs(configID);
    for (vector<PropertyConfig::digest_type>::const_iterator i = configs.begin(); i!=configs.end();++i) {
        const PropertyConfig *config = resolvePropertyConfig(*i);
        if ( config==0 ) return StatusCode::FAILURE;
        if (find(m_skip.begin(),m_skip.end(),config->name())!=m_skip.end()) {
            warning() << " skipping configuration of " << config->name()
                      << " because it is in the 'skip' list"
                      << endmsg;
            continue;
        }
        ConfigPushed_t::const_iterator current = m_configPushed.find( config->name() );

        // WARNING: the following doesn't catch all flow changes
        //          as it fails to recognize that we've dropped some
        //          part of the configuration tree... it only recognizes
        //          additional new branches...
        if ( current == m_configPushed.end() && !m_configPushed.empty() ) { // new, after already having pushed
            warning() << " WARNING WARNING WARNING WARNING WARNING WARNING" << endmsg;
            warning() << " WARNING  new configuration has entry not in old configuration" << endmsg;
            warning() << " WARNING  " << config->name() << endmsg;
            warning() << " WARNING  this transition will most likely fail to do what you want it to do... " << endmsg;
            warning() << " WARNING WARNING WARNING WARNING WARNING WARNING\n" << endmsg;
            if (!m_allowFlowChanges) return StatusCode::FAILURE;
        }

        if ( current == m_configPushed.end() ||  current->second != *i ) {  // not present, or different; needs to be pushed
          if(msgLevel(MSG::DEBUG)) debug() << " " << config->name()
                                           << " current: " <<  current->second
                                           << " requested: " << *i << endmsg;
          *newConfigs = config;
        }
    }
    return StatusCode::SUCCESS;
}


//TODO: add a locking mechanism that, after the first configure, forbids new entries,
//      i.e. only allow pushing changes to existing entries, i.e. m_configPushed must
//      be a valid digest prior to pushing...
//      This would basically forbid changes in flow, as any added 'member' in an existing
//      sequencer would not have its properties pushed yet, but will in the config, so it
//      would be caught when trying to push the properties of the new algorithm.
StatusCode
PropertyConfigSvc::configure(const ConfigTreeNode::digest_type& configID, bool callSetProperties) const
{
    if(msgLevel(MSG::DEBUG)) debug() << " configuring using " << configID << endmsg;
    if (!configID.valid()) return StatusCode::FAILURE;
    setTopAlgs(configID); // do this last instead of first?
    vector<const PropertyConfig*> configs;
    StatusCode sc = outOfSyncConfigs(configID,back_inserter(configs));
    if (sc.isFailure()) return sc;
    for (vector<const PropertyConfig*>::const_iterator i = configs.begin(); i!=configs.end();++i) {
        string name = (*i)->name();
        if(msgLevel(MSG::DEBUG)) debug() << " configuring " << name << " using " << (*i)->digest() << endmsg;
        const PropertyConfig::Properties& map = (*i)->properties();

        //TODO: make sure that online this cannot be done...
        std::string fqname = (*i)->type() + "/" + (*i)->name();
        Transformer transformer(fqname,warning());
        for (TransformMap::const_iterator itrans = m_transform.begin(); itrans!= m_transform.end(); ++itrans) {
            boost::regex re( itrans->first );
            if ( boost::regex_match( fqname, re ) ) transformer.push_back( &itrans->second );
        }
        if (!transformer.empty()) {
            std::transform(map.begin(),map.end(),property2jos(m_joboptionsSvc,name,m_os.get()), transformer);
        } else {
            std::copy(map.begin(),
                      map.end(),
                      property2jos(m_joboptionsSvc,name,m_os.get())) ;
        }

        m_configPushed[name] = (*i)->digest();
    }
    //  _after_ pushing all configurations, invoke 'setProperties'...
    //@TODO: should we do this in reverse order??
    if (callSetProperties) {
        for (vector<const PropertyConfig*>::const_iterator i=configs.begin();i!=configs.end();++i) {
             sc = invokeSetProperties(**i);
             if (sc.isFailure()) {
                  error() << "failed whilst invoking setProperties for " << (*i)->name() << endmsg;
                  return sc;
             }
        }
    }
    return StatusCode::SUCCESS;
}


//=============================================================================
// Reconfigure & Reinitialize
//=============================================================================
StatusCode
PropertyConfigSvc::findTopKind(const ConfigTreeNode::digest_type& configID,
                               const string& kind,
                               back_insert_iterator<vector<const PropertyConfig*> > configs) const
{
    const ConfigTreeNode *node = resolveConfigTreeNode(configID);
    PropertyConfig::digest_type id = node->leaf();
    if (id.invalid()) {
        // only follow dependencies if no data at current node....
        StatusCode sc(StatusCode::SUCCESS);
        const ConfigTreeNode::NodeRefs& deps = node->nodes();
        for (ConfigTreeNode::NodeRefs::const_iterator i  = deps.begin();
                                                      i != deps.end() && sc.isSuccess(); ++i ) {
            sc = findTopKind(*i,kind,configs);
        }
        return sc;
    }
    // we (should) have a leaf ! get it and use it!!!
    const PropertyConfig *config = resolvePropertyConfig(id);
    if ( config == 0 ) {
        if(msgLevel(MSG::DEBUG)) debug() << " could not find " << id << endmsg;
        error() << " could not find a configuration ID" << endmsg;
        return StatusCode::FAILURE;
    }
    if ( config->kind()==kind ) *configs = config;
    return StatusCode::SUCCESS;
}

StatusCode
PropertyConfigSvc::setTopAlgs(const ConfigTreeNode::digest_type& id) const {

    // Obtain the IProperty of the ApplicationMgr
    SmartIF<IProperty> appProps(serviceLocator());
    StringArrayProperty topAlgs("TopAlg",vector<string>());
    if ( appProps->getProperty(&topAlgs).isFailure() ) {
        error() << " problem getting StringArrayProperty \"TopAlg\"" << endmsg;
    }
    if(msgLevel(MSG::DEBUG)) debug() << " current TopAlgs: " << topAlgs.toString() << endmsg;

    if (m_initialTopAlgs.get()==0) {
        m_initialTopAlgs.reset( new vector<string>( topAlgs.value() ) );
    }

    vector<const PropertyConfig*> ids;
    StatusCode sc = findTopKind(id, "IAlgorithm", back_inserter(ids));
    if (sc.isFailure()) return sc;
    list<string> request;
    for (vector<const PropertyConfig*>::const_iterator i = ids.begin();i!=ids.end();++i) {
        request.push_back( (*i)->fullyQualifiedName() );
    }
//    transform( ids.begin(), ids.end(),
//               request.end(),
//               bl::bind(&PropertyConfig::fullyQualifiedName,bl::_1));

    // merge the current TopAlg, and requested TopAlg list, conserving
    // the order of BOTH lists.
    // We first loop over the to-be-inserted algos. As soon as one of them
    // is already present, we push everyone 'up to' this algo just in front
    // of it. Next we repeat until done, checking that the next requested algo is not
    // already in the current list prior to the point we're at...
    //@TODO: this doesn't work if topAlgs change on a TCK change: the old ones not
    //       present in the new one will stay..
    //@TODO: find topAlgs before we touched them -- and use those to merge with...


    list<string> merge(m_initialTopAlgs->begin(),m_initialTopAlgs->end());

    list<string>::iterator ireq = request.begin();
    while (ireq != request.end()) {
        list<string>::iterator i = find(merge.begin(),merge.end(),*ireq);
        //TODO: make sure we don't go backward in i...
        // example: topalgs = [ A, B, C ]
        //          merge = [ X,Y,B,Z,A ] should give error on meeting A
        if (i != merge.end()) {
            merge.splice(i,request,request.begin(),ireq);
            request.erase( ireq++ );
        } else {
            ++ireq;
        }
    }
    merge.splice(merge.end(),request);
    assert(request.empty());

    bool hasChanged = (merge.size()!=topAlgs.value().size());
    if (!hasChanged) {
        pair<std::vector<std::string>::const_iterator, std::list<std::string>::const_iterator>
        diff = std::mismatch(topAlgs.value().begin(), topAlgs.value().end(), merge.begin());
        hasChanged = ( diff.first != topAlgs.value().end() );
    }

    if (hasChanged) {
        topAlgs.setValue( vector<string>(merge.begin(),merge.end()) );
        if ( appProps->setProperty(topAlgs).isFailure() ) {
            error() << " failed to set property" << endmsg;
            return StatusCode::FAILURE;
        }
        info() << " updated TopAlgs: " << topAlgs.toString() << endmsg;
    } else {
        if(msgLevel(MSG::DEBUG)) debug() << " TopAlgs remain unchanged: " << topAlgs.toString() << endmsg;
    }
    return StatusCode::SUCCESS;
}

StatusCode
PropertyConfigSvc::reconfigure(const ConfigTreeNode::digest_type& top) const
{
    // push the right properties to the JobOptionsSvc, and call 'setProperties'
    // on the ones we pushed...
    StatusCode sc = configure(top,true);
    if (!sc.isSuccess()) return sc;
    assert(m_appMgrUI!=0);
    return m_appMgrUI->restart();
}

//=============================================================================
// Validate & create pictures...
//=============================================================================
StatusCode
PropertyConfigSvc::validateConfig(const ConfigTreeNode::digest_type& ref) const {
   // check that there are no overlaps, i.e. that a given named
   // object doesn't appear in this configuration tree
   // with more than one id...
   map<string,PropertyConfig::digest_type> inv;

   const vector<PropertyConfig::digest_type>& ids = collectLeafRefs(ref);
   for (vector<PropertyConfig::digest_type>::const_iterator i = ids.begin(); i!=ids.end();++i) {
       const PropertyConfig *config = resolvePropertyConfig(*i);
       if ( config == 0 ) {
            error() << "validateConfig:: could not resolve " << *i << endl;
            return StatusCode::FAILURE;
       }
       map<string,PropertyConfig::digest_type>::iterator f = inv.find( config->name() );
       if (f==inv.end()) { // not yet present
           inv.insert( make_pair(config->name(), *i ) );
       } else if ( f->first != config->name() )  { // collision detected: present, and inconsistent
           error() << "mutually exclusive configs detected for "
                << config->name() << " : "
                << *i << " and " << f->second << endl ;
           return StatusCode::FAILURE;
       } else {  // already present, and consistent
           // DO NOTHING
       }
   }

   if(msgLevel(MSG::DEBUG)) {
     for (map<string,PropertyConfig::digest_type>::const_iterator j = inv.begin(); j!= inv.end(); ++j) {
       debug() << j->first << " -> " << j->second << endl;
     }
   }
   
   return StatusCode::SUCCESS;
}

void
PropertyConfigSvc::createGraphVizFile(const ConfigTreeNode::digest_type& ref, const string& fname) const
{
   boost::filesystem::ofstream df( fname.c_str() );

   list<ConfigTreeNode::digest_type> ids = collectNodeRefs(ref);
   df << "digraph pvn {\n ordering=out;\n rankdir=LR;\n";
   for (list<ConfigTreeNode::digest_type>::const_iterator i = ids.begin(); i!=ids.end(); ++i) {
       const ConfigTreeNode *node   = resolveConfigTreeNode(*i);
       const PropertyConfig *config = resolvePropertyConfig(node->leaf());
       df <<  "\""<< *i << "\" [ label=\""
          // <<  "id: " << *i << "\\n"
          <<  "name: " << config->name() << "\\n"
          <<  "type: " << config->type() << "\\n"
          << "\"];\n";
       const ConfigTreeNode::NodeRefs& nodes = node->nodes();
       for (ConfigTreeNode::NodeRefs::const_iterator j = nodes.begin(); j!=nodes.end(); ++j) {
           df << "\"" << *i << "\" -> \"" << *j << "\";\n";
       }
   }
   df <<"}"<<endl;
}

//=============================================================================
// Resolve
//=============================================================================
const PropertyConfig*
PropertyConfigSvc::resolvePropertyConfig(const PropertyConfig::digest_type& ref) const
{
   PropertyConfigMap_t::const_iterator i = m_configs.find(ref);
   if (i!=m_configs.end()) {
        if(msgLevel(MSG::DEBUG)) debug() << "already have an entry for id " << ref << endl;
        return &(i->second);
   }
   boost::optional<PropertyConfig> config = m_accessSvc->readPropertyConfig(ref);
   if (!config) {
        error() << " could not obtain ref " << ref << endmsg;
        return 0;
   }
   if (config->digest()!=ref) {
        error() << " got unexpected config: ref " << ref.str()
                << " points at " << config->digest().str() << endmsg;
        return 0;
   }
   pair<PropertyConfigMap_t::iterator,bool> rv = m_configs.insert( make_pair( ref, *config) );
   return &(rv.first->second);
}

const ConfigTreeNode*
PropertyConfigSvc::resolveConfigTreeNode(const ConfigTreeNodeAlias::alias_type& ref) const
{
  info() << " resolving alias " << ref << endmsg;
  return resolveConfigTreeNode( resolveAlias(ref) );
}

const ConfigTreeNode*
PropertyConfigSvc::resolveConfigTreeNode(const ConfigTreeNode::digest_type& ref) const
{
   if(msgLevel(MSG::DEBUG)) debug() << " resolving nodeRef " << ref << endmsg;
   ConfigTreeNodeMap_t::const_iterator i = m_nodes.find(ref);
   if (i!=m_nodes.end()) {
        if(msgLevel(MSG::DEBUG)) debug() << "already have an entry for id " << ref << endl;
        return &(i->second);
   }
   assert(m_accessSvc!=0);
   boost::optional<ConfigTreeNode> node = m_accessSvc->readConfigTreeNode(ref);
   if (!node) {
        error() << " could not obtain ref " << ref << endmsg;
        return 0;
   }
   if (node->digest()!=ref) {
        error() << " got unexpected node: ref " << ref.str()
                << " points at " << node->digest().str() << endmsg;
        return 0;
   }
   pair<ConfigTreeNodeMap_t::iterator,bool> rv = m_nodes.insert( make_pair( ref, *node) );
   return &(rv.first->second);
}


PropertyConfig::Prop
PropertyConfigSvc::Transformer::operator()(const PropertyConfig::Prop& in) {
   std::string out = in.second;
   for (List_t::const_iterator i = m_list.begin(); i!=m_list.end(); ++i) { // vector of all component maps to apply
       for (PropMap_t::const_iterator j = (*i)->begin(); j!=(*i)->end();++j) {  // map to apply
           if (in.first != j->first) continue;
           for (ReplMap_t::const_iterator k = j->second.begin(); k!=j->second.end(); ++k) {
               boost::regex pattern(k->first);
               out = boost::regex_replace(out, pattern, k->second);
           }
       }
   }
   if (out == in.second) return in;
   m_os <<  "Applying substitution " << m_c << "." << in.first << " : " << in.second << " => " << out << endmsg;
   return PropertyConfig::Prop(in.first,out);
}


// ============================================================================
// BEGIN new parser:
// ============================================================================
// STD && STL:
// ============================================================================
#include <map>
#include <string>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ParsersFactory.h"
// ============================================================================
namespace Gaudi { namespace Parsers {
// 2011-08-26 : alexander.mazurov@gmail.com
StatusCode parse(std::map<std::string, std::map<std::string,
                 std::map< std::string, std::string> > >& result,
                 const std::string& input )
{
#if (defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L) \
    && ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40702)
#warning parser for map<string, map<string, map<string, string> > > not produced for a problem with Spirit
  return StatusCode::FAILURE;
#else
  return parse_(result, input);
#endif
}
// ============================================================================
// END of parser
// ============================================================================
} }
