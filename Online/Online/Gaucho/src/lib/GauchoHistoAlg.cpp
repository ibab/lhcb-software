#include "GaudiKernel/ServiceLocatorHelper.h"
#include "Gaucho/GauchoHistoAlg.h"

/** @file GauchoHistoAlg.cpp
 *
 *  Trivial implementation file for class : GauchoHistoAlg
 *
 *  @author Juan Martin Otalora Goicochea
 *  @date 2008-06-04
 */

GauchoHistoAlg::GauchoHistoAlg ( const std::string& name,
                               ISvcLocator*       pSvc )
  : GaudiHistoAlg( name , pSvc ) { }

GauchoHistoAlg::~GauchoHistoAlg() { }

StatusCode GauchoHistoAlg::initialize()
{
  return GaudiHistoAlg::initialize();
}

StatusCode GauchoHistoAlg::finalize()
{ 
  return GaudiHistoAlg::finalize();
}

// This is because the service_i method in Algorith is private..
StatusCode GauchoHistoAlg::service_i(const std::string& svcName,
                     bool createIf,
                     const InterfaceID& iid,
                     void** ppSvc) const {
  MsgStream log(msgSvc(), name());
  ServiceLocatorHelper helper(*serviceLocator(), log, name());
  return helper.getService(svcName, createIf, iid, ppSvc);
}


