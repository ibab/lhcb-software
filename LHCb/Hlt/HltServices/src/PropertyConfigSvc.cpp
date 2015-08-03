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
#include <functional>
#include <chrono>

#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/IAlgTool.h"

// local
#include "PropertyConfigSvc.h"

using namespace std;

template <typename T>
auto equals( const T& t) 
-> decltype( std::bind( std::equal_to<T>() , std::placeholders::_1, std::cref(t) ) )
{     return std::bind( std::equal_to<T>() , std::placeholders::_1, std::cref(t) ); }

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
            property2jos(IJobOptionsSvc* jos,const string& name, ostream* os=nullptr) :
                m_jos(jos),m_name(name),m_properties(jos->getProperties(name)),m_out(os) { assert(m_jos); }
            property2jos& operator=(const PropertyConfig::Prop& prop) {
                if (m_out) {
                   *m_out << m_name << '.' <<prop.first<< '=' << prop.second << ";\n";
                }
                const Property *p = (m_properties !=0 ? find(prop.first): nullptr);
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
               auto i = find_if(std::begin(*m_properties),
                                std::end(*m_properties),
                                [&](const Property* p) { return p->name()==name; } );
               return i!=std::end(*m_properties) ? *i : nullptr;
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
  , m_joboptionsSvc{nullptr}
  , m_toolSvc{nullptr}
  , m_algMgr{nullptr}
  , m_appMgrUI{nullptr}
  , m_accessSvc{nullptr}
  , m_os{nullptr}
{
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigCDBAccessSvc");
  declareProperty("prefetchConfig", m_prefetch);
  declareProperty("SkipComponent", m_skip); // do not touch these algorithms configurations, NOR THEIR DEPENDENTS!
  declareProperty("optionsfile", m_ofname);
  declareProperty("createGraphVizFile", m_createGraphVizFile=false);
  declareProperty("AllowFlowChanges",m_allowFlowChanges = false);
  declareProperty("ApplyTransformation",m_transform);
}


MsgStream& PropertyConfigSvc::msg(MSG::Level level) const {
     if (!m_msg) m_msg.reset( new MsgStream( msgSvc(), name() ));
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
  for (const auto &i : m_prefetch ) {
     ConfigTreeNode::digest_type digest = ConfigTreeNode::digest_type::createFromStringRep(i);
     assert( digest.str() == i) ;
     if (loadConfig( digest ).isFailure()) {
        error() << " failed to load " << digest << endmsg;
        return StatusCode::FAILURE;
     }
     if (m_createGraphVizFile) createGraphVizFile(digest, digest.str());
  }
  return status;
}

void PropertyConfigSvc::onCreate(const IAlgTool* tool) {
    assert(tool!=nullptr);
    string name = tool->name();
    if(msgLevel(MSG::DEBUG)) debug() << "adding " << name << " to toolmap " << endmsg;
    m_toolmap.emplace( name,tool );
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode PropertyConfigSvc::finalize() {
  StatusCode status = Service::finalize();
  m_joboptionsSvc->release(); m_joboptionsSvc = nullptr;
  m_algMgr->release();        m_algMgr = nullptr;
  m_accessSvc->release();     m_accessSvc = nullptr;
  m_os.reset();
  return status;
}

template <>
IService*
PropertyConfigSvc::resolve(const string& name) const
{
    IService *i{nullptr};
    if (serviceLocator()->getService(name,i).isFailure()) return nullptr;
    return i;
}
template <>
IAlgorithm*
PropertyConfigSvc::resolve(const string& name) const
{
    IAlgorithm *i{nullptr};
    if (m_algMgr->getAlgorithm(name,i).isFailure()) return nullptr;
    return i;
}

template <>
IAlgTool*
PropertyConfigSvc::resolve(const string& name) const
{
    auto i = m_toolmap.find(name);
    if (i==m_toolmap.end()) return nullptr;
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
        return alg ? alg->setProperties() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IService")  {
        Service *svc = resolve<Service>(conf.name());
        return svc ? svc->setProperties() : StatusCode::FAILURE ;
    } else if (conf.kind() == "IAlgTool")  {
        AlgTool *tool = resolve<AlgTool>(conf.name());
        return tool ? tool->setProperties() : StatusCode::FAILURE ;
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
   for( const auto& i : collectLeafRefs(configTree) ) {
        const PropertyConfig* pc = resolvePropertyConfig(i);
        assert(pc!=nullptr);
        if ( pc->name() == name ) return i;
   }
   return PropertyConfig::digest_type::createInvalid();
}

//=============================================================================
// Walk configuration trees (& cache the result)
//=============================================================================

ConfigTreeNode::digest_type
PropertyConfigSvc::resolveAlias(const ConfigTreeNodeAlias::alias_type& alias) const
{
    auto i = m_aliases.find(alias);
    if (i!=m_aliases.end()) return i->second;
    boost::optional<ConfigTreeNode> node = m_accessSvc->readConfigTreeNodeAlias(alias);
    if (!node) {
        error() << " could not obtain alias " << alias << endmsg;
        return ConfigTreeNode::digest_type::createInvalid();
    }
    // add it into alias cache
    ConfigTreeNode::digest_type ref = node->digest();
    m_aliases.emplace( alias,ref );
    // add to ConfigTreeNode cache now that we got it anyway...
    if (m_nodes.find(ref)==m_nodes.end()) m_nodes.emplace( ref, *node );
    info() << " resolved " << alias << " to " << ref << endmsg;
    return ref;
}

const list<ConfigTreeNode::digest_type>&
PropertyConfigSvc::collectNodeRefs(const ConfigTreeNodeAlias::alias_type& alias) const
{
    return collectNodeRefs(resolveAlias(alias));
}


const PropertyConfigSvc::Tree2NodeMap_t::mapped_type&
PropertyConfigSvc::collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const
{
    // first check cache...
    auto j = m_nodesInTree.find(nodeRef);
    if (j!=m_nodesInTree.end()) return j->second;

    //std::chrono::time_point<std::chrono::system_clock> start ;
    //start = std::chrono::system_clock::now();

    Tree2NodeMap_t::mapped_type nrefs; // note: we use list, as appending to a list
                                       // does not invalidate iterators!!!! (switch to std::deque???)
    nrefs.push_back(nodeRef);
    for (auto i = std::begin(nrefs); i!= std::end(nrefs); ++i) { // WARNING: std::end(nrefs) updates in the body of the loop!!!
        const ConfigTreeNode *node = resolveConfigTreeNode(*i);
        if (!node) {
            throw GaudiException("failed to resolve node ", this->name() + "::collectNodeRefs",StatusCode::FAILURE);
        }
        //FIXME: this is not going to be very fast, as it going to
        // make this operation quadratic...  should keep list sorted..
        // but if we do, we need to 'restart' at the insertion point
        // to insure the dependencies of the inserted elements are 
        // taken into account... On top of that, the current linear
        // structure is sorted by dependency, which is kind of nice ;-)
        std::copy_if( std::begin(node->nodes()), std::end(node->nodes()), 
                      std::back_inserter(nrefs) , 
                      [&nrefs](Tree2NodeMap_t::mapped_type::const_reference k ) { 
                          return std::none_of(std::begin(nrefs),std::end(nrefs),equals(k)) ; 
                      });

    }
    //std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-start;
    // std::cerr << "PropertyConfigSvc::collectNodeRefs: elapsed time to resolve " << nodeRef << " : " << elapsed_seconds.count() << std::endl;

    // insert in cache
    auto rv = m_nodesInTree.emplace( nodeRef, nrefs );
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
     auto i  = m_leavesInTree.find(nodeRef);
     if ( i != m_leavesInTree.end() ) return i->second;

     vector<PropertyConfig::digest_type> leafRefs;
     for (auto& i : collectNodeRefs(nodeRef) ) {
        const ConfigTreeNode *node = resolveConfigTreeNode(i);
        PropertyConfig::digest_type leafRef = node->leaf();
        if (leafRef.valid()) leafRefs.push_back(leafRef);
     }
     auto rv = m_leavesInTree.emplace( nodeRef, std::move(leafRefs) );
     return rv.first->second;
}

StatusCode
PropertyConfigSvc::loadConfig(const ConfigTreeNode::digest_type& nodeRef)
{
     try {
         if (!nodeRef.valid()) return StatusCode::FAILURE;
         info() << "loading config " << nodeRef.str() << endmsg;
         for (auto& i : collectLeafRefs(nodeRef) ) resolvePropertyConfig(i);
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
    for (const auto& i : collectLeafRefs(configID) ) {
        const PropertyConfig *config = resolvePropertyConfig(i);
        if ( !config ) {
            error() << "Failed to resolve " <<  i << endmsg;
            return StatusCode::FAILURE;
        }
        if (std::any_of( std::begin(m_skip),std::end(m_skip),equals(config->name()))) {
            warning() << " skipping configuration of " << config->name()
                      << " because it is in the 'skip' list"
                      << endmsg;
            continue;
        }
        auto current = m_configPushed.find( config->name() );

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

        if ( current == m_configPushed.end() ||  current->second != i ) {  // not present, or different; needs to be pushed
          if(msgLevel(MSG::DEBUG)) debug() << " " << config->name()
                                           << " current: " <<  current->second
                                           << " requested: " << i << endmsg;
          *newConfigs = config;
        }
    }
    // TODO: add check for 'dropped' parts of the configuration tree...
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
    for (const auto& i : configs ) {
        if(msgLevel(MSG::DEBUG)) debug() << " configuring " << i->name() << " using " << i->digest() << endmsg;
        const PropertyConfig::Properties& map = i->properties();

        //TODO: make sure that online this cannot be done...
        std::string fqname = i->type() + "/" + i->name();
        Transformer transformer(fqname,warning());
        for (const auto& trans : m_transform ) {
            boost::regex re( trans.first );
            if ( boost::regex_match( fqname, re ) ) transformer.push_back( &trans.second );
        }
        if (!transformer.empty()) {
          std::transform(std::begin(map), std::end(map),
                         property2jos(m_joboptionsSvc, i->name(), m_os.get()),
                         transformer);
        } else {
          std::copy(std::begin(map), std::end(map),
                    property2jos(m_joboptionsSvc, i->name(), m_os.get()));
        }
        m_configPushed[i->name()] = i->digest();
    }
    //  _after_ pushing all configurations, invoke 'setProperties'...
    //@TODO: should we do this in reverse order??
    if (callSetProperties) {
        for (const auto&  i : configs ) {
             sc = invokeSetProperties(*i);
             if (sc.isFailure()) {
                  error() << "failed whilst invoking setProperties for " << i->name() << endmsg;
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
        for (const auto& i : node->nodes() ) {
            sc = findTopKind(i,kind,configs);
            if (!sc.isSuccess()) break;
        }
        return sc;
    }
    // we (should) have a leaf ! get it and use it!!!
    const PropertyConfig *config = resolvePropertyConfig(id);
    if ( !config ) {
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

    if (!m_initialTopAlgs) {
        m_initialTopAlgs.reset( new vector<string>( topAlgs.value() ) );
    }

    vector<const PropertyConfig*> ids;
    StatusCode sc = findTopKind(id, "IAlgorithm", back_inserter(ids));
    if (sc.isFailure()) return sc;
    list<string> request;
    std::transform(
        std::begin(ids), std::end(ids), std::back_inserter(request),
        [](const PropertyConfig *i) { return i->fullyQualifiedName(); });

    // merge the current TopAlg, and requested TopAlg list, conserving
    // the order of BOTH lists.
    // We first loop over the to-be-inserted algos. As soon as one of them
    // is already present, we push everyone 'up to' this algo just in front
    // of it. Next we repeat until done, checking that the next requested algo is not
    // already in the current list prior to the point we're at...
    //@TODO: this doesn't work if topAlgs change on a TCK change: the old ones not
    //       present in the new one will stay..
    //@TODO: find topAlgs before we touched them -- and use those to merge with...


    list<string> merge( std::begin(*m_initialTopAlgs),std::end(*m_initialTopAlgs));

    auto ireq = request.begin();
    while (ireq != request.end()) {
        auto i = find(merge.begin(),merge.end(),*ireq);
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
        auto diff = std::mismatch(topAlgs.value().begin(), topAlgs.value().end(), merge.begin());
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
    assert(m_appMgrUI);
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

   for (const auto& i : collectLeafRefs(ref)) {
       const PropertyConfig *config = resolvePropertyConfig(i);
       if ( !config ) {
            error() << "validateConfig:: could not resolve " << i << endl;
            return StatusCode::FAILURE;
       }
       auto f = inv.find( config->name() );
       if (f==inv.end()) { // not yet present
           inv.emplace( config->name(), i );
       } else if ( f->first != config->name() )  { // collision detected: present, and inconsistent
           error() << "mutually exclusive configs detected for "
                << config->name() << " : "
                << i << " and " << f->second << endl ;
           return StatusCode::FAILURE;
       } else {  // already present, and consistent
           // DO NOTHING
       }
   }

   if(msgLevel(MSG::DEBUG)) {
     for (const auto& j : inv ) {
       debug() << j.first << " -> " << j.second << endl;
     }
   }
   return StatusCode::SUCCESS;
}

void
PropertyConfigSvc::createGraphVizFile(const ConfigTreeNode::digest_type& ref, const string& fname) const
{
   boost::filesystem::ofstream df( fname );
   df << "digraph pvn {\n ordering=out;\n rankdir=LR;\n";
   for (const auto& i : collectNodeRefs(ref) ) {
       const ConfigTreeNode *node   = resolveConfigTreeNode(i);
       const PropertyConfig *config = resolvePropertyConfig(node->leaf());
       df <<  "\""<< i << "\" [ label=\""
          // <<  "id: " << *i << "\\n"
          <<  "name: " << config->name() << "\\n"
          <<  "type: " << config->type() << "\\n"
          << "\"];\n";
       for (const auto& j : node->nodes()) {
           df << "\"" << i << "\" -> \"" << j << "\";\n";
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
        return nullptr;
   }
   if (config->digest()!=ref) {
        error() << " got unexpected config: ref " << ref.str()
                << " points at " << config->digest().str() << endmsg;
        return nullptr;
   }
   pair<PropertyConfigMap_t::iterator,bool> rv = m_configs.emplace(  ref, *config);
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
   auto i = m_nodes.find(ref);
   if (i!=m_nodes.end()) {
        if(msgLevel(MSG::DEBUG)) debug() << "already have an entry for id " << ref << endl;
        return &(i->second);
   }
   assert(m_accessSvc);
   boost::optional<ConfigTreeNode> node = m_accessSvc->readConfigTreeNode(ref);
   if (!node) {
        error() << " could not obtain ref " << ref << endmsg;
        return nullptr;
   }
   if (node->digest()!=ref) {
        error() << " got unexpected node: ref " << ref.str()
                << " points at " << node->digest().str() << endmsg;
        return nullptr;
   }
   pair<ConfigTreeNodeMap_t::iterator,bool> rv = m_nodes.emplace( ref, *node );
   return &(rv.first->second);
}


PropertyConfig::Prop
PropertyConfigSvc::Transformer::operator()(const PropertyConfig::Prop& in) {
   std::string out = in.second;
   for (const auto& i : m_list) { // vector of all component maps to apply
       for (const auto& j : *i ) {  // map to apply
           if (in.first != j.first) continue;
           for (const auto& k : j.second ) {
               boost::regex pattern(k.first);
               out = boost::regex_replace(out, pattern, k.second);
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
  return parse_(result, input);
}
// ============================================================================
// END of parser
// ============================================================================
} }
