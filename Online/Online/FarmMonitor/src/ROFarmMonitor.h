// $Id: ROFarmMonitor.h,v 1.3 2010-10-18 06:25:08 ocallot Exp $
#ifndef ROFARMMONITOR_H
#define ROFARMMONITOR_H 1

// Include files
#include "RTL/rtl.h"
#include "RODimLineService.h"
#include "RODimFsmService.h"
#include "RONodeCounter.h"
#include "ROShifter.h"
#include "dim/dic.hxx"
#include "dim/dis.hxx"
#include "Trending/TrendWriter.h"

// Forward declarations
class Nodeset;

/** @class ROFarmMonitor ROFarmMonitor.h
 *
 *
 *  @author Olivier Callot
 *  @date   2008-03-17
 */
class ROFarmMonitor : public DimClient {
public:
  struct Descriptor {
    size_t length;
    size_t actual;
    char*  pointer;
    Descriptor() : length(0), actual(0), pointer(0) {                                    }
    void   release()              { if(pointer) ::free(pointer); pointer=0; length=0; actual=0;}
    template<class T> T* data()   {	return (T*)pointer;                                }
    char*  reserve(size_t len)    {
      if ( len > length ) {
        release();
        pointer = (char*)::malloc(length=len);
      }
      return pointer;
    }
  };

  struct PartitionDesc {
    std::string name;
    int         partitionID;
    std::vector<std::string> hltNodes;
    std::vector<std::string> calibNodes;
    std::vector<std::string> moniNodes;
    std::vector<std::string> recoNodes;
    std::vector<std::string> defNodes;
    int packingFactor;
    DimInfo*    dimPartitionID;
    DimInfo*    dimHltNodes;
    DimInfo*    dimCalibNodes;
    DimInfo*    dimPackingFactor;
    DimInfo*    dimDefNodes;
    std::vector<RODimLineService*> lineServices;
    std::vector<RODimLineService*> moniServices;
    std::vector<RODimFsmService*>  fsmServices;
    std::vector<RONodeCounter>     prevCounters;
    TrendWriter*                   trendWriter;
    PartitionDesc( std::string a ) {
      name        = a;
    }    
  };
  
  /// Standard constructor
  ROFarmMonitor( int argc, char** argv) ;
  
  virtual ~ROFarmMonitor( ); ///< Destructor

  void initialize();
  
  /// Update all farm nodes
  virtual void update( );

  static void nodeInfoHandler(void* tag, void* address, int* size);

  void infoHandler();
  
protected:

  std::string nodeNameFromAddress( char* add );
  
private:
  int                      m_delay;    /// Time delay in milliseconds between 2 updates
  int                      m_print;    /// Print level
  int                      m_test;     /// Non zero -> testing, no publication
  std::vector<std::string> m_names;    /// List of service names
  std::vector<Descriptor*> m_datas;    /// Variable size data buffer
  std::vector<int>         m_svc;      /// List of Dim services
  std::vector<std::string> m_stateName; /// Name of FSM states
  longlong                 m_lastTime;  /// time of the last update
  int                      m_offsetTime; /// Offset to make time readable.
 
  DimServer*                   m_dimServer;
  std::vector<PartitionDesc*>  m_partitions;  /// list of partitions
  ROShifter*               m_shifter;   /// tool to get the shifter name
  char                     m_shiftLeader[80];
  char                     m_dataManager[80];
  char                     m_production[80];
  DimService*              m_dimShiftLeader;
  DimService*              m_dimDataManager;
  DimService*              m_dimProduction;
};
#endif // ROFARMMONITOR_H
