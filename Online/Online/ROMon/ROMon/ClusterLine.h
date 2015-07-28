// $Id: FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
#ifndef ROMON_CLUSTERLINE_H
#define ROMON_CLUSTERLINE_H 1

// C++ include files
#include <string>
#include <vector>

// Framework includes

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class FarmLineDisplay;

  /**@class ClusterLine ROMon.h GaudiOnline/Display.h
   *
   *   Display entry for the boot status of one subfarm
   *
   *   @author M.Frank
   */
  class ClusterLine  {
  public:
    typedef std::vector<std::pair<std::string,long> > Summary;
  protected:
    std::string        m_name;
    std::string        m_partition;
    int                m_svc;
    size_t             m_position;
    FarmLineDisplay*   m_parent;
    void*              m_data;
    char*              m_ptr;

    /// Start update cycle.
    void begin_update(const char* text=0);
    /// End update cycle.
    void end_update();

  public:
    /// Initializing constructor
    ClusterLine(FarmLineDisplay* par, const std::string& p, const std::string& n);
    /// Default destructor
    virtual ~ClusterLine();
    /// Connect to data service
    void connect(const std::string& name);
    /// Name of the clusterline (=subfarm name)
    const std::string& name() const           { return m_name;      }
    /// Name of the clusterline (=subfarm name)
    const std::string& partition() const      { return m_partition; }
    /// Access to the cluster data
    template <class T> const T* data() const  { return (T*)m_data;  }
    /// The line position within the parent
    size_t position() const                   { return m_position;  }
    /// Update position on reconfiguring the display
    void setPosition(size_t pos)              { m_position = pos;   }
    /// Check for data time stamp(s)
    void check(time_t now);
    /// Hightlight cursor if current
    void set_cursor();
    /// Display function drawing on pasteboard the initial display
    virtual void initialDisplay();
    /// Display function drawing on pasteboard of current display
    virtual void display();
    /// Collect summary data
    virtual void collect(Summary& /* summary */ )  const {          }
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
  };

  typedef ClusterLine* (*ClusterLineCreator_t)(FarmLineDisplay* parent, 
                                               const std::string& partition, 
                                               const std::string& title);

  /// Factory method: create a cluster line according to a given type
  ClusterLine* createClusterLine(const std::string& type, 
                                 FarmLineDisplay* parent, 
                                 const std::string& partition,
                                 const std::string& title);

  /// Registry function
  void _registerCreator(const char* name,ClusterLineCreator_t func);

  template<class T> class ClusterLineFactory  {
  public:
    static ClusterLine* create(FarmLineDisplay* parent, const std::string& partition, const std::string& title)
    { return new T(parent,partition,title);            }
    ClusterLineFactory(const char* name) 
    { _registerCreator(name,create);                   }
  };


}      // End namespace ROMon
#endif /* ROMON_CLUSTERLINE_H */
