/*
 * TestHistogramAlg.h
 *
 *  Created on: Jul 14, 2015
 *      Author: beat
 */

#ifndef SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_TESTHISTOGRAMALG_H_
#define SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_TESTHISTOGRAMALG_H_
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

//class IHistogram1D;
class TestHistogramAlg: public GaudiHistoAlg, virtual public IIncidentListener
{
  public:
    IHistogram1D *m_hist;
    TestHistogramAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TestHistogramAlg( ); ///< Destructor
    virtual StatusCode initialize();
    virtual StatusCode finalize();    ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode stop();  ///<Transition executed upon the Stop command
    virtual StatusCode start(); ///<Transition executed upon the Start command
    /// virtuals incident
    void handle(const Incident& );
    StatusCode queryInterface(const InterfaceID& id, void** ppI);
};



#endif /* SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_TESTHISTOGRAMALG_H_ */
