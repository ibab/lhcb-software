// $Id: PropertyConfigSvc.cpp,v 1.2 2008-02-11 14:56:10 graven Exp $
// Include files 

#include <sstream>
#include "boost/filesystem/fstream.hpp"
#include <algorithm>

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"

// from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/SvcFactory.h"

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
            property2jos(IJobOptionsSvc* jos,const std::string& name, std::ostream* os=0) :
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
            const Property* find(const std::string& name) {
               std::vector<const Property*>::const_iterator i =
                  std::find_if(m_properties->begin(),
                               m_properties->end(),
                               lam::bind(&Property::name,lam::_1)==name);
               return i==m_properties->end() ? 0 : *i;
            }
            IJobOptionsSvc*                     m_jos;
            std::string                         m_name;
            const std::vector<const Property*> *m_properties;
            std::ostream                       *m_out;
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
// Initialization
//=============================================================================
StatusCode PropertyConfigSvc::initialize() {
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
  for (vector<std::string>::const_iterator i = m_prefetch.begin(); i!=m_prefetch.end(); ++i ) {
     PropertyConfig::digest_type digest = MD5::convertString2Digest(*i);
     assert( digest.str() == *i) ;
     loadConfig( digest );
     validateConfig( digest );
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
// Configure
//=============================================================================
StatusCode 
PropertyConfigSvc::configure(const PropertyConfig& config) const
{
    // FIXME: check whether 'config' has already been 
    //        done in order to prevent (circular!) loops...
    if (find(m_skip.begin(),m_skip.end(),config.name())!=m_skip.end()) {
        warning() << " skipping configuration of " << config.name() 
                  << " and its dependants because it is in the 'skip' list" 
                  << endl;
        return StatusCode::SUCCESS;
    }
    // do the dependants of specified algo first...
    const PropertyConfig::Dependencies& deps = config.dependants();
    PropertyConfig::Dependencies::const_iterator i = deps.begin(); 
    while (i!=deps.end()) {
        if (configure(i->second).isFailure()) return StatusCode::FAILURE;
        ++i;
    }
    debug() << " configuring " << config.name() << endl;
    // now do specified IProperty object itself
    const PropertyConfig::Properties& map = config.properties();
    if (!map.empty()) {
        for_each(map.begin(),
                 map.end(),
                 property2jos(m_joboptionsSvc,config.name(),m_os.get())) ;
    }
    return StatusCode::SUCCESS;
}

StatusCode 
PropertyConfigSvc::configure(const PropertyConfig::digest_type& configID) const {
    ConfigMap_t::const_iterator config = m_configs.find(configID);
    if ( config == m_configs.end() ) {
        error() << " could not find " << configID << endl;
        return StatusCode::FAILURE;
    }
    info() << " configuring " << config->second.name() 
           << " from config " << config->first << endl;
    return configure( config->second );
}


//=============================================================================
// Reconfigure & Reinitialize
//=============================================================================
StatusCode
PropertyConfigSvc::findServicesAndTopAlgorithms(const PropertyConfig::digest_type& configID,
                                             std::vector<IService*>& svcs,
                                             std::vector<IAlgorithm*>& algs) const {
    // very stupid: just try each name, and see if it happens to be a service...
    // better: when creating a config, we KNOW what things are. Add this info to the schema, and use that..
    //  (in general, push the time-consuming stuff into generating a config, not in using it!)
    ConfigMap_t::const_iterator config = m_configs.find(configID);
    if ( config == m_configs.end() ) {
        debug() << " could not find " << configID << endmsg;
        error() << " could not find a configuration ID" << endmsg;
        return StatusCode::FAILURE;
    }
    const std::string &name = config->second.name();
    IService *isvc(0); 
    if (serviceLocator()->getService( name, isvc ).isSuccess()) {
        assert(isvc!=0);
        svcs.push_back(isvc);
        return StatusCode::SUCCESS;
    }
    IAlgorithm* ialgo(0);
    if (m_appMgr->getAlgorithm( name, ialgo ).isSuccess()) {
        algs.push_back(ialgo);
        return StatusCode::SUCCESS;
    }
    // not a service, not an algorithm, shouldn't be a tool,
    // so it can only be something without properties, and with
    // dependencies, i.e. a 'meta' config... otherwise I don't know
    // what it could be...
    if (!config->second.properties().empty()) {
        debug() << " got unexpected type of config during reinitialize: " << config->second 
                << " you probably asked for an algorithm that didn't exist already..." 
                << endmsg;
        error() << " got unexpected type of config during reinitialize " << endmsg;
        return StatusCode::FAILURE;
    }
    StatusCode sc(StatusCode::SUCCESS);
    const PropertyConfig::Dependencies& deps = config->second.dependants();
    for (PropertyConfig::Dependencies::const_iterator i  = deps.begin();
                                                 i != deps.end() && sc.isSuccess(); ++i ) {
        sc = findServicesAndTopAlgorithms(i->second,svcs,algs);
    }
    return sc;
}

StatusCode 
PropertyConfigSvc::reconfigure(const PropertyConfig::digest_type& top) const
{
    // push the right properties to the JobOptionsSvc
    StatusCode sc = configure(top);
    if (!sc.isSuccess()) return sc;

    // and make sure everyone goes back to the JobOptionsSvc
    // to pick them up!
    std::vector<IService*> svcs;
    std::vector<IAlgorithm*> algs;
    sc = findServicesAndTopAlgorithms(top,svcs,algs);
    if (!sc.isSuccess()) return sc;
    for (std::vector<IService*>::iterator i = svcs.begin(); i!=svcs.end()&&sc.isSuccess(); ++i) {
       debug() << " calling sysReinitialize for service " << (*i)->name() << endmsg;
       sc = (*i)->sysReinitialize();
    }
    for (std::vector<IAlgorithm*>::iterator i = algs.begin(); i!=algs.end()&&sc.isSuccess(); ++i) {
       debug() << " calling sysReinitialize for algorithm " << (*i)->name() << endmsg;
       sc = (*i)->sysReinitialize();
    }
    return sc;
}

//=============================================================================
// Validate
//=============================================================================
bool 
PropertyConfigSvc::validateConfig(const PropertyConfig::digest_type& ref) const {
   // check that there are no overlaps, i.e. that a given named
   // object doesn't appear in this configuration tree
   // with more than one id...
   map<string,PropertyConfig::digest_type> inv;

   list<PropertyConfig::digest_type> ids; // note: we use list, as appending to a list
                      // does not invalidate iterators!!!!
   ids.push_back(ref);
   list<PropertyConfig::digest_type>::iterator i = ids.begin();
   while (i!=ids.end()) {
       ConfigMap_t::const_iterator config = m_configs.find(*i);
       if ( config == m_configs.end() ) {
            error() << " could not resolve " << *i << endl;
            return false;
       }
       map<string,PropertyConfig::digest_type>::iterator f = inv.find( config->second.name() );
       if (f==inv.end()) { // not yet present
           inv.insert( make_pair(config->second.name(), config->first) );
           transform( config->second.dependants().begin(),
                      config->second.dependants().end(),
                      back_inserter(ids), // shouldn't we check that *_1 isn't in here already? (beware of circular dependencies!!) (color vertex?)
                      lam::bind(&PropertyConfig::Dependencies::value_type::second,lam::_1));
       } else if ( f->second != config->first )  { // collision detected: present, and inconsistent
           error() << "mutually exclusive requests detected for " 
                << config->second.name() << " : " 
                << config->first << " and " << f->second << endl ;
           return false;
       } else {  // already present, and consistent
           // DO NOTHING
       }
       ++i;
   } 
   for (map<string,PropertyConfig::digest_type>::const_iterator j = inv.begin(); j!= inv.end(); ++j) {
        debug() << j->first << " -> " << j->second << endl;
   }
   return true;
}

void 
PropertyConfigSvc::createGraphVizFile(const PropertyConfig::digest_type& ref, const std::string& fname) const
{
   list<PropertyConfig::digest_type> ids; // note: we use list, as appending to a list
                      // does not invalidate iterators!!!!
   ids.push_back(ref);
   list<PropertyConfig::digest_type>::iterator i=ids.begin();
   while (i!=ids.end()) {
       ConfigMap_t::const_iterator config = m_configs.find(*i);
       if ( config == m_configs.end() ) {
            error() << " could not resolve " << *i << endl;
            continue;
       }
       const PropertyConfig::Dependencies& deps = config->second.dependants();
       for (PropertyConfig::Dependencies::const_iterator j = deps.begin(); j!=deps.end(); ++j) {
            if ( find(ids.begin(),ids.end(),j->second) == ids.end() ) ids.push_back(j->second);
       }
       ++i;
   } 
   boost::filesystem::ofstream df( fname.c_str() );
   df << "digraph pvn {\n ordering=out;\n rankdir=LR;\n";
   for ( i = ids.begin(); i!=ids.end(); ++i) {
       ConfigMap_t::const_iterator config = m_configs.find(*i);
       df <<  "\""<< *i << "\" [ label=\""
          // <<  "id: " << *i << "\\n" 
          <<  "name: " << config->second.name() << "\\n" 
          <<  "type: " << config->second.type() << "\\n" 
          << "\"];\n";
       const PropertyConfig::Dependencies& deps = config->second.dependants();
       for (PropertyConfig::Dependencies::const_iterator j = deps.begin(); j!=deps.end(); ++j) {
           df << "\"" << *i << "\" -> \"" << j->second << "\";\n";
       }
   } 
   df <<"}"<<endl;
}

//=============================================================================
// Load
//=============================================================================
bool 
PropertyConfigSvc::loadConfig(const PropertyConfig::digest_type& ref)
{
   // make sure we don't already have this config in our list...
   if (m_configs.find(ref)!=m_configs.end()) {
        debug() << "already have an entry for id " << ref << endl;
        return true;
   }
   boost::optional<PropertyConfig> config = m_accessSvc->read(ref);
   if (!config) { 
        error() << " could not obtain ref " << ref << endmsg;
        return false;
   }
   if (config->digest()!=ref) {
        error() << " got unexpected config: ref " << ref.str() 
                << " points at " << config->digest().str() << endmsg;
        return false;
   }
   m_configs.insert( make_pair( ref, *config) );
   // next resolve dependencies (by recursion)
   const PropertyConfig::Dependencies& cdeps = config->dependants();
   for (PropertyConfig::Dependencies::const_iterator i=cdeps.begin();i!=cdeps.end();++i) {
      if (!loadConfig(i->second)) return false;
      debug() << " adding dep " << i->second << " for " << ref << endmsg;
   }
   return true;
}
