// $Id: PropertyConfigSvc.cpp,v 1.5 2008-03-05 08:06:30 graven Exp $
// Include files 

#include <sstream>
#include <algorithm>

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
namespace lam = boost::lambda;

//-----------------------------------------------------------------------------
// Implementation file for class : PropertyConfigSvc
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_SERVICE_FACTORY( PropertyConfigSvc );

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
                               lam::bind(&Property::name,lam::_1)==name);
               return i==m_properties->end() ? 0 : *i;
            }
            IJobOptionsSvc*                     m_jos;
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
  , m_appMgr(0)
  , m_os(0)
{
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigFileAccessSvc");
  declareProperty("prefetchConfig", m_prefetch);
  declareProperty("skipAlgorithm", m_skip); // do not touch these algorithms configurations, NOR THEIR DEPENDENTS!
  declareProperty("optionsfile", m_ofname);
  declareProperty("createGraphVizFile", m_createGraphVizFile);
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
   status = service("ApplicationMgr",m_appMgr);
   if ( !status.isSuccess() )   return status;
   status = service("JobOptionsSvc",m_joboptionsSvc);
   if ( !status.isSuccess() )   return status;

   if (!m_ofname.empty()) m_os.reset( new boost::filesystem::ofstream( m_ofname.c_str() ) );

  // read table of pre-assigned, possible configurations for this job...
  // i.e. avoid reading _everything_ when we really need to be quick
  for (vector<string>::const_iterator i = m_prefetch.begin(); i!=m_prefetch.end(); ++i ) {
     ConfigTreeNode::digest_type digest = MD5::convertString2Digest(*i);
     assert( digest.str() == *i) ;
     loadConfig( digest );
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
  m_appMgr->release();        m_appMgr = 0;
  m_accessSvc->release();     m_accessSvc = 0;
  m_os.reset();               
  return status;
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
  string kind = "Unknown";
  if      (SmartIF<IAlgorithm>(ini).isValid()) kind = "IAlgorithm";
  else if (SmartIF<IService>(ini).isValid())   kind = "IService";
  else if (SmartIF<IAlgTool>(ini).isValid())   kind = "IAlgTool";
  else if (SmartIF<IAuditor>(ini).isValid())   kind = "IAuditor";

  SmartIF<IProperty> ip(ini);
  return PropertyConfig( obj.name(), *ip, kind);
}

PropertyConfig::digest_type 
PropertyConfigSvc::findInTree(const ConfigTreeNode::digest_type& configTree, const std::string& name) const {
   const vector<PropertyConfig::digest_type>& leafs = collectLeafRefs(configTree);
   for( vector<PropertyConfig::digest_type>::const_iterator i = leafs.begin(); i!=leafs.end();++i) {
        const PropertyConfig* pc = resolvePropertyConfig(*i);
        assert(pc!=0);
        if ( pc->name() == name ) return *i;
   }
   return MD5::createInvalidDigest();
}

//=============================================================================
// Walk configuration trees (& cache the result)
//=============================================================================
const list<ConfigTreeNode::digest_type>&
PropertyConfigSvc::collectNodeRefs(const ConfigTreeNode::digest_type& nodeRef) const 
{
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
            // make this operation quadratic...
            if (find(nrefs.begin(),nrefs.end(),*j)==nrefs.end()) {
               nrefs.push_back(*j);
            }
        }
        ++i;
    }
    pair<Tree2NodeMap_t::iterator,bool> rv = m_nodesInTree.insert( make_pair(nodeRef, nrefs) );
    return rv.first->second;
}

const vector<PropertyConfig::digest_type>&
PropertyConfigSvc::collectLeafRefs(const ConfigTreeNode::digest_type& nodeRef) const 
{
     Tree2LeafMap_t::const_iterator i = m_leavesInTree.find(nodeRef);
     if (i!=m_leavesInTree.end()) return i->second;
     list<ConfigTreeNode::digest_type> nodeRefs = collectNodeRefs(nodeRef);
     vector<PropertyConfig::digest_type> leafRefs;
     for (list<ConfigTreeNode::digest_type>::const_iterator i=nodeRefs.begin();i!=nodeRefs.end();++i) {
        const ConfigTreeNode *node = resolveConfigTreeNode(*i);
        PropertyConfig::digest_type leafRef = node->leaf();
        if (!leafRef.invalid()) {
            leafRefs.push_back(leafRef);
        }
     }
     pair<Tree2LeafMap_t::iterator,bool> rv = m_leavesInTree.insert( make_pair(nodeRef, leafRefs) );
     return rv.first->second;
}

bool 
PropertyConfigSvc::loadConfig(const ConfigTreeNode::digest_type& nodeRef) 
{
     const vector<PropertyConfig::digest_type>& list = collectLeafRefs(nodeRef);
     for (vector<PropertyConfig::digest_type>::const_iterator i = list.begin();i!=list.end();++i) {
            resolvePropertyConfig(*i);
     }
//     for_each(list.begin(),
//                   list.end(), 
//                   lam::bind(&PropertyConfigSvc::resolvePropertyConfig,this,lam::_1) );
     return validateConfig( nodeRef );
}

//=============================================================================
// Configure
//=============================================================================
StatusCode 
PropertyConfigSvc::configure(const PropertyConfig& config) const
{
    if (find(m_skip.begin(),m_skip.end(),config.name())!=m_skip.end()) {
        warning() << " skipping configuration of " << config.name() 
                  << " because it is in the 'skip' list" 
                  << endl;
        return StatusCode::SUCCESS;
    }
    debug() << " configuring " << config.name() << endl;
    const PropertyConfig::Properties& map = config.properties();
    if (!map.empty()) {
        for_each(map.begin(),
                 map.end(),
                 property2jos(m_joboptionsSvc,config.name(),m_os.get())) ;
    }
    return StatusCode::SUCCESS;
}

StatusCode 
PropertyConfigSvc::configure(const ConfigTreeNode::digest_type& configID) const {
  // FIXME: make sure appmgr has the right topalgorithms...
    vector<IService*> svcs;
    vector<IAlgorithm*> algs;
    // StatusCode sc = findServicesAndTopAlgorithms(configID,svcs,algs);
    // if (sc.isFailure()) return sc;

    const vector<PropertyConfig::digest_type>& configs = collectLeafRefs(configID);
    for (vector<PropertyConfig::digest_type>::const_iterator i = configs.begin(); i!=configs.end();++i) {
        const PropertyConfig *config = resolvePropertyConfig(*i);
        if (config==0) return StatusCode::FAILURE;
        StatusCode sc = configure(*config);
        if (sc.isFailure()) return sc;
    }
    return StatusCode::SUCCESS;
}


//=============================================================================
// Reconfigure & Reinitialize
//=============================================================================
//FIXME: return the leaf node ID's of the svcs and top algos, not more than that
//       because the remainder of the action depends on initialize vs reinitialize
StatusCode
PropertyConfigSvc::findServicesAndTopAlgorithms(const ConfigTreeNode::digest_type& configID,
                                             vector<IService*>& svcs,
                                             vector<IAlgorithm*>& algs) const {

    const ConfigTreeNode *node = resolveConfigTreeNode(configID);
    PropertyConfig::digest_type id = node->leaf();
    if (id.invalid()) {
        // only follow dependencies if no data at current node....
        StatusCode sc(StatusCode::SUCCESS);
        const ConfigTreeNode::NodeRefs& deps = node->nodes();
        for (ConfigTreeNode::NodeRefs::const_iterator i  = deps.begin();
                                                      i != deps.end() && sc.isSuccess(); ++i ) {
            sc = findServicesAndTopAlgorithms(*i,svcs,algs);
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
    if (config->kind()=="IService") {
        IService *isvc(0); 
        if (serviceLocator()->getService( config->type() + "/" + config->name(), isvc ).isSuccess()) {
            assert(isvc!=0);
//     FIXME: this check doesn't work: typeid(isvc) returns 'IService*'..... stupidly...
//            if (System::typeinfoName(typeid(isvc))!=config->type()) {
//               error() << "Got a service " << config->name() 
//                       << " from the serviceLocator whose typename does not match the configured typename: " 
//                       << System::typeinfoName(typeid(isvc)) << " vs " << config->type() << endmsg;
//               return StatusCode::FAILURE;
//            }
            svcs.push_back(isvc);
            return StatusCode::SUCCESS;
        } else {
            error() << "Requested an unknown service with name " << config->name() << endmsg;
            return StatusCode::FAILURE;
        }
    }
    if (config->kind()=="IAlgorithm") {
        IAlgorithm* ialgo(0);
        //TODO: make sure that  the MinimalEventLoopMgr  has our 'topalgs' in its 'TopAlg' property...
        if (m_appMgr->getAlgorithm( config->name(), ialgo ).isSuccess()) {
//     FIXME: this check doesn't work: typeid(ialgo) returns 'IAlgorithm*'..... stupidly...
//            if (System::typeinfoName(typeid(ialgo))!=config->type()) {
//               error() << "Got a top level algorithm " << config->name() 
//                       << " from the application manager  whose typename does not match the configured typename " 
//                       << System::typeinfoName(typeid(ialgo)) << " vs " << config->type() << endmsg;
//               return StatusCode::FAILURE;
//            }
            algs.push_back(ialgo);
            return StatusCode::SUCCESS;
        } else {
            error() << "Got a top level algorithm you didn't inform the AppMgr/EventLoopMgr about (yet)\n"
                    << " -- for now this is an error, update your options by adding\n "
                    << "ApplicationMgr.TopAlg += {\"" << config->type() << "/" << config->name() << "\"};\n"
                    << " -- in the future, we might do the equivalent automagically... "
                    << endmsg;
            // FIXME: it is insufficient to just create this algorithm -- it needs to be registered
            //        as a top level algorithm. Should be sufficient to 'push' the right value of
            //        the TopAlg property of the MinimalEventLoopMgr into the JOS (as this code executes
            //        before MinimalEventLoopMgr::initialize does....)
            // StatusCode sc = m_appMgr->createAlgorithm( config->type(), config->name(), ialgo );
            // if (sc.isFailure()) return sc;
            // algs.push_back(ialgo);
            return StatusCode::FAILURE;
        }
    }
    // not a service, not an algorithm, shouldn't be a tool
    // so instead of second guessing, give up...
    debug() << " got unexpected type of config during (re)initialize: " << config
            << " you probably asked for an algorithm that didn't exist already..." 
            << endmsg;
    error() << " got unexpected type of config during (re)initialize " << endmsg;
    return StatusCode::FAILURE;
}

StatusCode 
PropertyConfigSvc::reconfigure(const ConfigTreeNode::digest_type& top) const
{
    // push the right properties to the JobOptionsSvc
    StatusCode sc = configure(top);
    if (!sc.isSuccess()) return sc;

    vector<IService*> svcs;
    vector<IAlgorithm*> algs;
    sc = findServicesAndTopAlgorithms(top,svcs,algs);
    if (!sc.isSuccess()) return sc;
    // and make sure everyone goes back to the JobOptionsSvc
    // to pick them up! i.e. call reinitialize for everyone...
    for (vector<IService*>::iterator i = svcs.begin(); i!=svcs.end()&&sc.isSuccess(); ++i) {
       debug() << " calling sysReinitialize for service " << (*i)->name() << endmsg;
       sc = (*i)->sysReinitialize();
    }
    for (vector<IAlgorithm*>::iterator i = algs.begin(); i!=algs.end()&&sc.isSuccess(); ++i) {
       debug() << " calling sysReinitialize for algorithm " << (*i)->name() << endmsg;
       sc = (*i)->sysReinitialize();
    }
    return sc;
}

//=============================================================================
// Validate & create pictures...
//=============================================================================
bool 
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
            return false;
       }
       map<string,PropertyConfig::digest_type>::iterator f = inv.find( config->name() );
       if (f==inv.end()) { // not yet present
           inv.insert( make_pair(config->name(), *i ) );
       } else if ( f->first != config->name() )  { // collision detected: present, and inconsistent
           error() << "mutually exclusive configs detected for " 
                << config->name() << " : " 
                << *i << " and " << f->second << endl ;
           return false;
       } else {  // already present, and consistent
           // DO NOTHING
       }
   } 
   for (map<string,PropertyConfig::digest_type>::const_iterator j = inv.begin(); j!= inv.end(); ++j) {
        debug() << j->first << " -> " << j->second << endl;
   }
   return true;
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
        return 0;;
   }
   if (node->digest()!=ref) {
        error() << " got unexpected node: ref " << ref.str() 
                << " points at " << node->digest().str() << endmsg;
        return 0;
   }
   pair<ConfigTreeNodeMap_t::iterator,bool> rv = m_nodes.insert( make_pair( ref, *node) );
   return &(rv.first->second);
}
