/*
 * hr.h
 *
 *  Created on: Jul 21, 2015
 *      Author: beat
 */

#ifndef SOURCE_DIRECTORY__ALIGNMENTONLINE_ALIGNONLINE_SRC_HISTOGRAMRESETTER_H_
#define SOURCE_DIRECTORY__ALIGNMENTONLINE_ALIGNONLINE_SRC_HISTOGRAMRESETTER_H_


#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

class HistogramResetter: public GaudiHistoAlg, virtual public IIncidentListener
{
  public:
    HistogramResetter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~HistogramResetter( ); ///< Destructor
    virtual StatusCode initialize();
    virtual StatusCode finalize();    ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode stop();  ///<Transition executed upon the Stop command
    virtual StatusCode start(); ///<Transition executed upon the Start command
    /// virtuals incident
    void handle(const Incident& );
    StatusCode queryInterface(const InterfaceID& id, void** ppI);
};




#endif /* SOURCE_DIRECTORY__ALIGNMENTONLINE_ALIGNONLINE_SRC_HISTOGRAMRESETTER_H_ */
