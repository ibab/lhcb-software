// $Id: HltConfigSvc.h,v 1.11 2009-02-22 19:52:40 graven Exp $
#ifndef HLTCONFIGSVC_H 
#define HLTCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/format.hpp"
#include "boost/operators.hpp"


// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "PropertyConfigSvc.h"


/** @class HltConfig HltConfig.h
 *  
 *  functionality:
 *         configure the HLT algorithms...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class HltConfigSvc : public PropertyConfigSvc, virtual public IIncidentListener {
public:

  HltConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltConfigSvc( );           ///< Destructor
  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();      ///< Service finalization
  virtual void handle(const Incident&);

   //TODO: move in dedicated class in HltBase...
  class TCKrep : public boost::equality_comparable<TCKrep> {
      public:
        TCKrep() : m_unsigned(0) {}
        explicit TCKrep(unsigned int i) : m_unsigned(i) { set(i); }
        explicit TCKrep(const std::string& s) : m_unsigned(0) { set(s); }
        bool operator<(const TCKrep& rhs) const { return m_unsigned  < rhs.m_unsigned; } 
        bool operator==(const TCKrep& rhs) const { return m_unsigned == rhs.m_unsigned; } 
        bool operator==(unsigned int rhs) const { return m_unsigned == rhs; } 
        TCKrep& operator&=(unsigned int rhs) { return set( uint() & rhs ); }
        TCKrep& operator++() { return set( ++m_unsigned ); }
        const std::string&  str() const { return m_stringRep; }
        unsigned int uint() const { return m_unsigned;  }
        TCKrep& set(const std::string& s);
        TCKrep& set(unsigned i)           { 
                m_unsigned = i;
                m_stringRep = boost::str( boost::format("0x%08x")%i ) ;
                return *this;
        }
        bool valid() const { return m_unsigned != 0 ; }
      private:
        std::string m_stringRep;
        unsigned int m_unsigned;
  };
private:
  void dummyCheckOdin();
  void checkOdin();

   void updateMap(Property&);
   void updateInitial(Property&);

  typedef std::map<TCKrep,std::string> TCKMap_t;

  std::string                  m_prefetchDir;     ///< which set of configurations 
                                                  ///< to search for same types as initial TCK
                                                  ///< and to prefetch...

  TCKMap_t                     m_tck2config;      ///< from TCK to configuration ID
  std::map<std::string,std::string> m_tck2config_;      ///< from TCK to configuration ID

  TCKrep                       m_initialTCK;      ///< which TCK to start with...
  std::string                  m_initialTCK_;      ///< which TCK to start with...

  mutable TCKMap_t             m_tck2configCache; ///< from TCK to configuration ID
  TCKrep                       m_configuredTCK;   ///< which TCK is currently in use?
  IDataProviderSvc            *m_evtSvc;          ///< get Evt Svc to get ODIN (which contains TCK)
  IIncidentSvc                *m_incidentSvc;     ///< 
  bool                         m_checkOdin;
  bool                         m_maskL0TCK;

  // resolve TCK -> toplevel config ID, then call method with ID
  ConfigTreeNode::digest_type tck2id(TCKrep tck) const;

};
#endif // HLTCONFIGSVC_H
