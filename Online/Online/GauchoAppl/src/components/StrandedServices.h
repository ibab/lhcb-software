/*
 * StrandedServices.h
 *
 *  Created on: Sep 9, 2011
 *      Author: beat
 */

#ifndef STRANDEDSERVICES_H_
#define STRANDEDSERVICES_H_

#include <string>
#include <vector>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "dic.hxx"


#include "CPP/Event.h"
#include "CPP/Interactor.h"

class StrandedServices;

class StrandBrowser : public Interactor
{
  public:
  StrandBrowser(StrandedServices *s);
  StrandedServices *gService;
    /// Interactor overload: handle Sensor stimuli
    virtual void handle(const Event& ev);
};







class StrandInfo : public DimInfo
{
public:
  StrandInfo(std::string name, StrandBrowser *s);
  virtual ~StrandInfo()
  {

  }
  void infoHandler();
  std::string m_name;
  StrandBrowser *m_service;

};

class StrandedServices : public Service,virtual public IIncidentListener
{
public:
  StrandedServices(const std::string& name, ISvcLocator* sl);
  virtual ~StrandedServices();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode initialize();
  StatusCode stop();
  StatusCode start();
  int analyze();
  void handle(const Incident&);
  StrandInfo *m_hltstate;
  StrandBrowser *m_Browser;
  std::string m_name;
  std::string m_partname;
  std::vector<std::string> m_subfarms;
};
#endif /* STRANDEDSERVICES_H_ */
