// $Id: MooreInitSvc.h,v 1.1 2010-05-20 08:03:43 graven Exp $
#ifndef MOOREINIT_H
#define MOOREINIT_H 1

// Include files
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Service.h"

class IGenericTool;

/** @class MooreInitSvc MooreInitSvc.h
 *  Initializes random number service. Based on LbAppInit by Marco Cattaneo
 *
 *  @author Gerhard Raven
 *  @date   2010-05-12
 */
class MooreInitSvc : public virtual extends1<Service, IIncidentListener>
{

  public:
    MooreInitSvc( const std::string& name,
                  ISvcLocator* pSvcLocator ); ///< Standard constructor
    virtual ~MooreInitSvc();                  ///< Destructor
    virtual StatusCode initialize();          ///< Service initialization
    virtual StatusCode finalize();            ///< Service finalization
    virtual void handle( const Incident& );

  private:
    std::vector<long int> getSeeds( unsigned int seed1, ulonglong seed2 );
    StatusCode initRndm( std::vector<long int>& seeds );
    ToolHandle<IGenericTool> m_odinTool; ///< Pointer to odin encoding tool
    IDataProviderSvc* m_evtSvc; ///< get Evt Svc to get ODIN (which contains TCK)
};
#endif // MooreINIT_H
