/*
 * StorageSvc.h
 *
 *  Created on: Jun 21, 2012
 *      Author: Beat Jost
 */

#ifndef STORAGESVC_H_
#define STORAGESVC_H_
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"

class StorageSvc: public PubSvc
{
  public:
    StorageSvc(const std::string& name, ISvcLocator* sl);
    virtual ~StorageSvc();
    //IInterface pure virtual member functions
    StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    StatusCode start();
    StatusCode initialize();
    StatusCode finalize();
    void analyze(void *buff, int siz, MonMap* mmap);
    //  unsigned long long m_prevupdate;
    bool m_enableTrending;

    //    DimService *m_NoBytesSvc;
    //    DimService *m_NoFilesSvc;
    //    DimService *m_NoMEPsSvc;
    //
    //    float m_NoBytes;
    //    float m_NoFiles;
    //    float m_NoMEPs;
    //    double m_MEPRate;
    //  ISimpleTrendWriter* m_trender;

};

#endif /* STORAGESVC_H_ */
