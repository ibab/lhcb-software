// $Id: ROFarmMonitor.h,v 1.2 2010-03-24 07:44:00 ocallot Exp $
#ifndef ROFARMMONITOR_H
#define ROFARMMONITOR_H 1

// Include files
#include "RTL/rtl.h"
#include "RODimLineService.h"
#include "RODimFsmService.h"
#include "RONodeCounter.h"
#include "dim/dic.hxx"
#include "dim/dis.hxx"

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
    DimInfo*    dimPartitionID;
    DimInfo*    dimHltNodes;
    DimInfo*    dimCalibNodes;
    std::vector<RODimLineService*> lineServices;
    std::vector<RODimLineService*> moniServices;
    std::vector<RODimFsmService*>  fsmServices;
    std::vector<RONodeCounter>     prevCounters;
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
  std::vector<std::string> m_names;    /// List of service names
  std::vector<Descriptor*> m_datas;    /// Variable size data buffer
  std::vector<int>         m_svc;      /// List of Dim services
  std::vector<std::string> m_stateName; /// Name of FSM states
  longlong                 m_lastTime;  /// time of the last update
 
  DimServer*                   m_dimServer;
  std::vector<PartitionDesc*>  m_partitions;  /// list of partitions
};
#endif // ROFARMMONITOR_H
