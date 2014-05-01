// $Id$
#ifndef HACKRUNNRANDGPSSVC_H
#define HACKRUNNRANDGPSSVC_H 1

// Include files
#include <string>
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"
#include "dim/dic.hxx"

class IGenericTool;
namespace Gaudi { class Time; };
namespace LHCb { class RawBank; };

/** @class HackRunNrAndGPSSvc HackRunNrAndGPSSvc.h
 *  Hacks the run nr. and GPS time in the ODIN raw bank
 *
 *  @author Gerhard Raven
 *  @date   2014-05-01
 */
class HackRunNrAndGPSSvc : public virtual extends1<Service, IIncidentListener>
{
  public:
    HackRunNrAndGPSSvc( const std::string& name,
                  ISvcLocator* pSvcLocator );  ///< Standard constructor
    ~HackRunNrAndGPSSvc() override = default;        ///< Destructor
    StatusCode initialize() override;          ///< Service initialization
    StatusCode finalize() override;            ///< Service finalization
    StatusCode start() override;            ///< Service start
    StatusCode stop() override;            ///< Service stop
    void handle( const Incident& ) override;

  private:
    class RunNumber: public DimInfo {
        public:
            // Subscribe to the relevant DIM service
            RunNumber() : DimInfo("RunInfo/LHCb1/RunNumber", -1) {}
            int operator()() const { return m_runnr; }
        private:
            int m_runnr = -1;
            void infoHandler() override {
                m_runnr = getInt();
                std::cerr << "got new runnr! " << m_runnr << std::endl; 
            }
     };


    void Assert(bool ok, std::string msg , std::string tag ,
                                 const StatusCode   sc  ) const;
    void poke( LHCb::RawEvent *event, unsigned runnr ) ;
    void pokeODINRawBank( LHCb::RawBank *bank, unsigned runnr, Gaudi::Time time );

    ToolHandle<IGenericTool> m_odinTool; ///< Pointer to odin encoding tool
    IDataProviderSvc* m_evtSvc; ///< get Evt Svc to get ODIN (which contains TCK)
    std::unique_ptr<RunNumber> m_run;
};
#endif
