// $ID: $

#ifndef SIMSVC_SIMULATIONSVC_H
#define SIMSVC_SIMULATIONSVC_H

// Include files
#include "GaudiKernel/Service.h"

#include "SimSvc/ISimulationSvc.h"

#include <map>

// Forward and external declarations
template <class TYPE> class SvcFactory;


/** @class SimulationSvc SimulationSvc.h SimDesc/SimulationSvc.h
 *
 *  this interface defines a Simulation service that is able to answer
 *  many question concerning the simulation of the detector. It
 *  is able to associate simulation attributes to logical volumes.
 *  These describe the way these volumes should be simulated
 *
 * @author Sebastien Ponce
 */
class SimulationSvc : public Service,
                      virtual public ISimulationSvc {
  
  /// Friend needed
  friend class SvcFactory<SimulationSvc>;

public:

  /**
   * Standard Constructor
   * @param name   String with service name
   * @param svc    Pointer to service locator interface
   */
  SimulationSvc (const std::string& name, ISvcLocator* svc);

  /**
   * default destructor
   */
  virtual ~SimulationSvc();

  //  inline void operator delete (void* p) throw()
  // { operator delete( const_cast<void*> (p)) ; }
  
  /**
   * Initializes the service
   * @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * Queries interfaces of Interface.
   * @param riid ID of Interface to be retrieved
   * @param ppvInterface Pointer to Location for interface pointer
   * @return status depending on the completion of the call
   */
  virtual StatusCode queryInterface (const InterfaceID& riid, void** ppvInterface);

  ///////////////////////////////////////////////////////
  // implementation of the ISimulationSvc interface //
  ///////////////////////////////////////////////////////

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const PartAttr* simAttribute (const ILVolume* vol) const;

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const PartAttr* simAttribute (const std::string volname) const;

  /**
   * This method tells whether a simulation attribute is associated to a given
   * logical volume or not
   * @param vol the logical volume
   */
  virtual bool hasSimAttribute (const ILVolume* vol) const;

  /**
   * This method tells whether a simulation attribute is associated to a given
   * logical volume or not
   * @param vol the logical volume
   */
  virtual bool hasSimAttribute (const std::string volname) const;

  /**
   * This method returns the pointer to the vector of region definitions.
   * @param 
   * @return std::vector<RegionCuts>
   */
  virtual const std::vector<RegionCuts>* regionsDefs () const;

  /**
   * this method erases the current set of attributes and loads a new set
   */
  virtual void reload ();

  /**
   * this method erases the current set of attributes
   */
  virtual void clear ();


private:

  /**
   * This defines a set of attributes. Each one is associated to a name
   */
  typedef std::map<std::string, const SimAttribute*> AttributeSet;

  /**
   * This defines a dictionnary
   */

  typedef std::map<std::string, PartAttr*> Dict;

  /// a map of attributes to be used
  AttributeSet m_attributeSet;

  /// a dictionnary linking logical volumes and SimAttributes
  Dict m_logvol2Sim;

  /// vector of regions definitions
  VectOfRegCuts m_regionsDefs;

  /// definition of a struct used for temporary purposes
  struct Prcuts
  {
    double gammacut;
    double electroncut;
    double positroncut;
    double protoncut;
    double aprotoncut;
    double neutroncut;
    double aneutroncut;
  };
    
  /// The location of the Xml file containing all definitions
  std::string m_simDbLocation;

};

#endif // SIMSVC_SIMULATIONSVC_H

