#ifndef LHCBSTATUS_LHCBSTATUS_H
#define LHCBSTATUS_LHCBSTATUS_H

#include <cstdlib>
#include <map>

/*
 *  namespace declaration
 */
namespace LHCbStatus {
  struct ExperimentStatus;
  struct ServiceMap {
    struct PubItem  {
      int svc;
      int typ;
      ServiceMap* entries;
      size_t      offset;
      char        buffer[32];
    };
    typedef std::map<std::string,PubItem*> Services;
    Services          services;
    ExperimentStatus* current;
    ExperimentStatus* previous;
  };

  enum { SUBFARM_NAME_LENGTH=14, NUM_SUBFARM=54 };

  /** @struct SubDetector
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct SubDetector {
    char             name[16];
    char             hvStatus[16];
    char             dcsStatus[16];
    char             daiStatus[16];
    char             daqStatus[16];
    char             hwStatus[16];
    char             hwLock[16];
    char             hwIncluded[16];
    char             hwComplete[16];
    char             _pad[32];
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct DaqSystem
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct DaqSystem {
    char             name[16];
    char             status[16];
    unsigned char    metaStatus[4];
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _LHC
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _LHC {
    int              FillNumber;
    char             Mode[32];
    char             State[32];
    int              Energy;
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _Magnet
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _Magnet {
    struct _Item {
      float Current;
      int   Polarity;
      /// Print information to standard output
      void             dump();
    } Set, Measured;
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _HLTSubfarm
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _HLTSubfarm {
    char             name[12];
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _HLT
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _HLT {
    int              nSubFarms;
    char             subfarms[SUBFARM_NAME_LENGTH][NUM_SUBFARM];
    float            hltRate;
    float            runHltRate;
    long long int    hltNTriggers;
    /// Standard constructor
    _HLT();
    /// Access name of single subfarm
    char*            subFarm(int i) {  return subfarms[i]; }
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _TFC
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _TFC {
    char             OdinData[16];
    char             OdinMEPRequest[16];
    long long int    numTriggers;
    float            triggerRate;
    float            runTriggerRate;
    float            triggerRateWidget;
    float            runTriggerRateWidget;
    float            lifeTime;
    float            runLifeTime;
    float            deadTime;
    float            runDeadTime;
    int              nTriggersU;
    int              lostTriggers;
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _Trigger
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _Trigger {
    int              TCK;
    char             TCKLabel[64];
    char             onlineVersion[32];
    char             gaudiVersion[32];
    char             mooreVersion[32];
    char             dataflowVersion[32];
    char             HLTType[12];
    char             swPath[64];
    char             L0Type[64];
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _General
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _General {
    int              status;
    int              activePartId;
    int              runNumber;
    int              TAE;
    int              L0Gap;
    int              fillNumber;
    int              runNMinutes;
    long long int    runNTriggers;
    char             partName[32];
    char             runType[32];
    char             dataType[32];
    time_t           runStartTime, runStopTime, runInitialStartTime;
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct _VELO
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct _VELO  {
    /** @struct _MotionSystem
     *
     *  @author  M.Frank
     *  @date    01/06/2009
     *  @version 1.0
     */
    struct _MotionSystem   {
      float            resolPosRC;
      float            resolPosLA;
      float            resolPosY;
      /// Print information to standard output
      void             dump();
    } MotionSystem;
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };

  /** @struct ExperimentStatus
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  struct ExperimentStatus {
    char             partition[16];
    int              numDetectors;
    int              numSystems;
    SubDetector      _detector[16];
    DaqSystem        _system[40];
    _LHC              LHC;
    _Magnet           Magnet;
    _HLT              HLTFarm;
    _TFC              TFC;
    _Trigger          Trigger;
    _VELO             VELO;
    _General          general;

    const SubDetector* detector(int i) const {  return &_detector[i];      }
    SubDetector*     detector(int i)         {  return &_detector[i];      }
    const DaqSystem* system(int i) const     {  return &_system[i];        }
    DaqSystem*       system(int i)           {  return &_system[i];        }
    size_t           sizeOf() const          {  return sizeof(ExperimentStatus); }
    /// Print information to standard output
    void             dump();
    /// Publish data contents
    int publish(ServiceMap& s);
  };


  /** @struct Server
   *
   *  @author  M.Frank
   *  @date    01/06/2009
   *  @version 1.0
   */
  class Server {
  protected:
    /// Reference to status section
    ServiceMap        m_services;
    /// vector of PVSS datapoints containing information
    //std::vector<PVSS::DataPoint*> m_datapoints;
    
  public:
    /// Standard constructor
    Server();
    /// Standard destructor
    virtual ~Server();
    /// Access Status information block
    ExperimentStatus* info() const { return m_services.current; }
    void swapBuffers();
    /// Publish data contents
    int publish();
    /// Connect datapoints to PVSS
    bool connect();
    /// Load information from PVSS
    bool load();
    /// Start serving data to clients
    void start();
  };


}        /* End namespace LHCbStatus  */
#endif   /* LHCBSTATUS_LHCBSTATUS_H   */
