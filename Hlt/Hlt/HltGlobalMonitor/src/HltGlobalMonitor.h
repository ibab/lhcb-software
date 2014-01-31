#ifndef HLTGLOBALMONITOR_H
#define HLTGLOBALMONITOR_H 1

#include "Event/ODIN.h"
#include "Event/HltDecReports.h"

#include "boost/function.hpp"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/HistoDef.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "HltBase/HltBaseAlg.h"

#include "TProfile.h"
#include "GaudiUtils/Aida2ROOT.h"

class Condition;

/** @class HltGlobalMonitor HltGlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */
class HltGlobalMonitor : public HltBaseAlg, virtual public IIncidentListener
{
  public:
    /// Standard constructor
    HltGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltGlobalMonitor(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    void handle( const Incident& );

    /// update the condition
    StatusCode updateCondition();

  private:
    size_t rawEvtLength( const LHCb::RawEvent* evt );
    size_t rawEvtLength( const std::vector<LHCb::RawBank*>& banks );
    void monitorODIN( const LHCb::ODIN*, const LHCb::HltDecReports* );
    void monitorHLT( const LHCb::ODIN*, const LHCb::HltDecReports* );
    void monitorVertices();
    void monitorTrends();
    void monitorResolverpositions();

    template <typename T>
    T* fetch( const std::string& location )
    {
        T* t = this->exist<T>( location ) ? this->get<T>( location ) : nullptr;
        if ( !t && this->msgLevel( MSG::WARNING ) ) {
            Warning( " could not retrieve " + location, StatusCode::SUCCESS, 10 );
        }
        return t;
    }

    typedef std::map<std::string, std::vector<std::string>> DecToGroupType;
    DecToGroupType m_DecToGroup1;
    DecToGroupType m_DecToGroup2;

    // index in m_hlt1Alleys, m_hlt1AlleyRates , followed by binnumber inside alley
    std::map<Gaudi::StringKey, std::pair<unsigned, unsigned>> m_hlt1Line2AlleyBin;
    std::map<Gaudi::StringKey, std::pair<unsigned, unsigned>> m_hlt2Line2AlleyBin;
    std::vector<AIDA::IHistogram1D*> m_hlt1Alleys;
    std::vector<AIDA::IHistogram1D*> m_hlt2Alleys;
    std::vector<StatEntity*> m_hlt1AlleyRates;
    std::vector<StatEntity*> m_hlt2AlleyRates;

    std::string m_HltDecReportsLocation;
    std::string m_ODINLocation;
    std::string m_hlt1Decision;
    std::string m_hlt2Decision;
    std::string m_rawEventLocation;

    AIDA::IHistogram1D* m_odin;
    AIDA::IHistogram1D* m_odinHLT1;
    AIDA::IHistogram1D* m_odinHLT2;

    AIDA::IHistogram1D* m_hlt1Alley;
    AIDA::IHistogram1D* m_hlt2Alley;

    AIDA::IProfile1D* m_hltVirtTime;
    AIDA::IHistogram1D* m_hltTime;
    AIDA::IProfile1D* m_hltEventsTime;
    AIDA::IHistogram1D* m_tasks;

    AIDA::IHistogram2D* m_hlt1AlleysCorrelations;
    AIDA::IHistogram2D* m_hlt2AlleysCorrelations;

    AIDA::IProfile1D* m_hltTimeVsEvtSize;

    // AIDA::IHistogram1D* m_resolvxr50;
    // AIDA::IHistogram1D* m_resolvxr5;
    // AIDA::IHistogram1D* m_resolvxl50;
    // AIDA::IHistogram1D* m_resolvxl5;
    AIDA::IProfile1D* m_resolvxle;
    AIDA::IProfile1D* m_resolvxre;
    // AIDA::IHistogram1D* m_resolvy50;
    // AIDA::IHistogram1D* m_resolvy5;
    AIDA::IProfile1D* m_resolvye;
    AIDA::IProfile1D* m_magnetcurrent;
    AIDA::IProfile1D* m_magnetpolarity;
    AIDA::IProfile1D* m_magnetstate;
    AIDA::IProfile1D* m_rich1pressure;
    AIDA::IProfile1D* m_rich1temperature;
    AIDA::IProfile1D* m_rich2pressure;
    AIDA::IProfile1D* m_rich2temperature;
    AIDA::IProfile1D* m_lhcnbofbunches;
    AIDA::IProfile1D* m_lumipars;


    // Monitor 3D vertex filter
    Condition* m_veloCondition;
    Condition* m_magnetCondition;
    Condition* m_rich1Condition;
    Condition* m_rich2Condition;
    Condition* m_lhcfillingschemeCondition;
    Condition* m_lumiparsCondition;
    /*Condition* m_runinfoCondition;
    Condition* m_runparsCondition;*/

    typedef std::vector<AIDA::IHistogram2D*> HistoVector;
    typedef std::map<std::string, HistoVector> HistoMap;
    HistoMap m_vertexHistos;
    double m_beamSpotX;
    double m_beamSpotY;
    double m_xRC;
    double m_xLA;
    double m_Y;

    double m_magnetCurrent;
    double m_magnetPolarity;
    double m_magnetState;

    double m_rich1Temperature;
    double m_rich1Presssure;
    double m_rich2Temperature;
    double m_rich2Presssure;

    int m_lhcNCollidingBunches;

    std::vector<double> m_lumiparvalues;

    typedef std::map<std::string, std::string> LocationMap;
    LocationMap m_vertexLocations;

    class histopair
    {
      public:
        histopair( GaudiHistoAlg& parent, std::string loc,
                   const Gaudi::Histo1DDef& def, const char* yaxislabel = 0 );
        void fill( double x, double y )
        {
            m_histo->fill( x );
            m_profile->fill( x, y );
        }
        void operator()( double CPUtime )
        {
            int n = m_parent->exist<ObjectContainerBase>( m_loc )
                        ? (int)m_parent->get<ObjectContainerBase>( m_loc )
                              ->numberOfObjects()
                        : -1;
            fill( n, CPUtime );
        }

      private:
        GaudiHistoAlg* m_parent;
        AIDA::IHistogram1D* m_histo;
        AIDA::IProfile1D* m_profile;
        std::string m_loc;
    };

    std::map<std::string, Gaudi::Histo1DDef> m_correlateCPU;
    std::vector<histopair> m_CPUcorrelations;

    double m_currentTime; // seconds since start of clock
    double m_timeSize;
    double m_timeInterval;
    longlong m_startClock; // in microseconds
    longlong m_startEvent; // in microseconds
    long m_virtmem;
    int m_gpstimesec;
};
#endif // HLTGLOBALMONITOR_H
