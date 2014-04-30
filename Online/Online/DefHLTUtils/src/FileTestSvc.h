#ifndef ONLINE_GAUCHO_FILETESTSVC_H
#define ONLINE_GAUCHO_FILETESTSVC_H

#include "GaudiKernel/Service.h"
#include <string>
class FileTestSvc : public Service
{
public:
  FileTestSvc(const std::string& name, ISvcLocator* sl);
  virtual ~FileTestSvc();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode stop();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  std::string m_InputFile;
};
#endif // ONLINE_GAUCHO_FILETESTSVC_H
