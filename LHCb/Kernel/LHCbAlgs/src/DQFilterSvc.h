#ifndef DQFILTERSVC_H
#define DQFILTERSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Kernel/IAccept.h"

/** Simple service installing itself in the IncidentSvc to filter events on
 * DQ Flags via an IAccept tool (by default DQAcceptTool).
 *
 * @author Marco Clemencic
 * @date 2012-02-10
 */
class DQFilterSvc: public extends1<Service, IIncidentListener> {
public:
  /// Constructor
  DQFilterSvc(const std::string& name, ISvcLocator* svc);

  /// Initialize the service
  virtual StatusCode initialize();

  /// Finalize the service
  virtual StatusCode finalize();

  /// @see IIncidentListener::handle
  virtual void handle( const Incident& );

  /// Destructor
  virtual ~DQFilterSvc();

private:

  /// Type/Name of the (public) IAccept tool used to choose if the event has to
  /// be accepted or not (default: DQAcceptTool).
  std::string m_acceptToolName;

  /// Pointer to the IAccept tool.
  IAccept *m_acceptTool;

  /// Pointer to the ToolSvc.
  SmartIF<IToolSvc> m_toolSvc;

  /// Pointer to the IncidentSvc.
  SmartIF<IIncidentSvc> m_incSvc;

};

#endif
