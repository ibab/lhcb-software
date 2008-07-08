// $Id: PropertyConfigSvc.cpp,v 1.11 2008-07-08 14:22:49 graven Exp $
// Include files 

#include <sstream>
#include <algorithm>
#include <list>

#include "boost/filesystem/fstream.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/Algorithm.h" 
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
DECLARE_SERVICE_FACTORY( PropertyConfigSvc );


//@TODO: keep track of what we pushed for whom, (and when...)
//       we know which kind, so when 'reconfiguring', could issue 'setProperties'
//       of the relevant component... (need to pick up component, which depends on
//       'kind', and issue call to setProperties, which also depends on 'kind', as
//       components don't share an interface for that... (even though all kinds 
//       derive from the IProperty interface! -- but still there is a difference
//       in how to pick up the components depending on their kind...)
namespace {

    class property2jos {
         public:
            property2jos(IJobOptionsSvc* jos,const string& name, ostream* os=0) :
                m_jos(jos),m_name(name),m_properties(jos->getProperties(name)),m_out(os) { assert(m_jos!=0); }
            void operator()(const PropertyConfig::Prop& prop) {
                if (m_out!=0) {
                   *m_out << m_name << '.' <<prop.first<< '=' << prop.second << ";\n";
                }
                const Property *p = (m_properties !=0 ? find(prop.first): 0);
                if ( p==0 || p->toString() != prop.second ) {  // only update if non-existant or not up-to-date
                   m_jos->addPropertyToCatalogue(m_name, StringProperty(prop.first,prop.second));
                }

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
  , m_algMgr(0)
  , m_os(0)
{
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigFileAccessSvc");
  declareProperty("prefetchConfig", m_prefetch);
  declareProperty("skipAlgorithm", m_skip); // do not touch these algorithms configurations, NOR THEIR DEPENDENTS!
  declareProperty("optionsfile", m_ofname);
  declareProperty("createGraphVizFile", m_createGraphVizFile=false);
}

//=============================================================================
// Destructor
//=============================================================================
PropertyConfigSvc::~PropertyConfigSvc() {
} 

MsgStream& PropertyConfigSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
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
   info() << "Initialize" << endmsg;
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

StatusCode 
PropertyConfigSvc::invokeSetProperties(const PropertyConfig& conf) const {
    debug() << " calling SetProperties for " << conf.name() << endmsg;
    if (conf.kind() == "IAlgorithm") {
        Algorithm *alg = resolve<Algorithm>(conf.name()); 
        return alg!=0 ? alg->setProperties() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IService")  {
        Service *svc = resolve<Service>(conf.name()); 
        return svc!=0 ? svc->setProperties() : StatusCode::FAILURE ;
    } 
    return StatusCode::FAILURE;
}

StatusCode 
PropertyConfigSvc::invokeSysReinitialize(const PropertyConfig& conf) const {
    debug() << " calling SysReinitialize for " << conf.name() << endmsg;
    if (conf.kind() == "IAlgorithm") {
        Algorithm *alg = resolve<Algorithm>(conf.name()); 
        return alg!=0 ? alg->sysReinitialize() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IService")  {
        Service *svc = resolve<Service>(conf.name()); 
        return svc!=0 ? svc->sysReinitialize() : StatusCode::FAILURE ;
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
        pair<ConfigTreeNodeMap_t::iterator,bool> rv = m_nodes.insert( make_pair( ref, *node) );
    }
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
     if (!nodeRef.valid()) return StatusCode::FAILURE;
     info() << "loading config " << nodeRef.str() << endmsg;
     const vector<PropertyConfig::digest_type>& list = collectLeafRefs(nodeRef);
     for (vector<PropertyConfig::digest_type>::const_iterator i = list.begin();i!=list.end();++i) {
            resolvePropertyConfig(*i);
     }
//     for_each(list.begin(),
//                   list.end(), 
//                   bl::bind(&PropertyConfigSvc::resolvePropertyConfig,this,bl::_1) );
     return validateConfig( nodeRef );
}

//=============================================================================
// Configure
//=============================================================================
StatusCode 
PropertyConfigSvc::configure(const ConfigTreeNodeAlias::alias_type& alias, 
                             bool callSetProperties) const 
{
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
                      << endl;
            continue;
        }
        if ( m_configPushed[config->name()] != *i ) *newConfigs = config;
    }
    return StatusCode::SUCCESS;
}


StatusCode 
PropertyConfigSvc::configure(const ConfigTreeNode::digest_type& configID, bool callSetProperties) const 
{
    if (!configID.valid()) return StatusCode::FAILURE;
    setTopAlgs(configID); // do this last instead of first?
    vector<const PropertyConfig*> configs;
    StatusCode sc = outOfSyncConfigs(configID,back_inserter(configs));
    if (sc.isFailure()) return sc;
    for (vector<const PropertyConfig*>::const_iterator i = configs.begin(); i!=configs.end();++i) {
        string name = (*i)->name();
        debug() << " configuring " << name << endl;
        const PropertyConfig::Properties& map = (*i)->properties();
        for_each(map.begin(),
                 map.end(),
                 property2jos(m_joboptionsSvc,name,m_os.get())) ;
        m_configPushed[name] = (*i)->digest();
    }
    //  _after_ pushing all changes, invoke 'setProperties' on the changed ones...
    //@TODO: should we do this in reverse order??
    if (callSetProperties) {
        for (vector<const PropertyConfig*>::const_iterator i=configs.begin();i!=configs.end();++i)
            invokeSetProperties(**i);
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
        debug() << " could not find " << id << endmsg;
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
    debug() << " current TopAlgs: " << topAlgs.toString() << endmsg;

    vector<const PropertyConfig*> ids;
    StatusCode sc = findTopKind(id, "IAlgorithm", back_inserter(ids));
    list<string> request;
    transform( ids.begin(), ids.end(), 
               request.end(), 
               bl::bind(&PropertyConfig::fullyQualifiedName,bl::_1));

    // merge the current TopAlg, and requested TopAlg list, conserving
    // the order of BOTH lists.
    // We first loop over the to-be-inserted algos. As soon as one of them 
    // is already present, we push everyone 'up to' this algo just in front
    // of it. Next we repeat until done, checking that the next requested algo is not 
    // already in the current list prior to the point we're at...
    //@TODO: this doesn't work if topAlgs change on a TCK change: the old ones not
    //       present in the new one will stay..
    //@TODO: find topAlgs before we touched them -- and use those to merge with...


    list<string> merge(topAlgs.value().begin(),topAlgs.value().end());

    list<string>::iterator ireq = request.begin();
    while (ireq != request.end()) {
        list<string>::iterator i = find(merge.begin(),merge.end(),*ireq);
        //TODO: make sure we don't go backward in i...
        // example: topalgs = [ A, B, C ]
        //          merge = [ X,Y,B,Z,A ] should give error on meeting A
        if (i != merge.end()) merge.splice(i,request,request.begin(),ireq);
        ++ireq;
    }
    merge.splice(merge.end(),request);
    assert(request.empty());

    //@TODO first check if something actually changed... if not, don't do anything!!
    topAlgs.setValue( vector<string>(merge.begin(),merge.end()) );
    if ( appProps->setProperty(topAlgs).isFailure() ) { 
        error() << " failed to set property" << endmsg;
        return StatusCode::FAILURE;
    }
    debug() << " updated TopAlgs: " << topAlgs.toString() << endmsg;
    return StatusCode::SUCCESS;
} 

StatusCode 
PropertyConfigSvc::reconfigure(const ConfigTreeNode::digest_type& top) const
{
    // push the right properties to the JobOptionsSvc, and call 'setProperties'
    // on the ones we pushed...
    StatusCode sc = configure(top,true);
    if (!sc.isSuccess()) return sc;

        //@TODO: restart doesn't call setProperties -- need to make sure
        //       we call setProperties of those algorithms/services/tools which
        //       for which we have pushed things into the jos...

//    GAUDI v20rx...
//    m_appMgrUI->restart();

    // call reinitialize for services and top algorithms...
    vector<const PropertyConfig*> cfgs;
    sc = findTopKind(top, "IService", back_inserter(cfgs));
    if (!sc.isSuccess()) return sc;
    sc = findTopKind(top,"IAlgorithm",back_inserter(cfgs));
    if (!sc.isSuccess()) return sc;
    for (vector<const PropertyConfig*>::iterator i = cfgs.begin(); i!=cfgs.end()&&sc.isSuccess(); ++i) {
       sc = invokeSysReinitialize( **i );
    }
    return sc;
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
   for (map<string,PropertyConfig::digest_type>::const_iterator j = inv.begin(); j!= inv.end(); ++j) {
        debug() << j->first << " -> " << j->second << endl;
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
        debug() << "already have an entry for id " << ref << endl;
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
  return resolveConfigTreeNode( resolveAlias(ref) );
}

const ConfigTreeNode* 
PropertyConfigSvc::resolveConfigTreeNode(const ConfigTreeNode::digest_type& ref) const
{
   ConfigTreeNodeMap_t::const_iterator i = m_nodes.find(ref);
   if (i!=m_nodes.end()) {
        debug() << "already have an entry for id " << ref << endl;
        return &(i->second);
   }
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
