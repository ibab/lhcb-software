// $ID: $

#ifndef VISDESC_VISUALIZATIONSVC_H
#define VISDESC_VISUALIZATIONSVC_H

// Include files
#include "GaudiKernel/Service.h"

#include "VisSvc/IVisualizationSvc.h"

#include <map>

// Forward and external declarations
template <class TYPE> class SvcFactory;

/** @class VisualizationSvc VisualizationSvc.h VisDesc/VisualizationSvc.h
 *
 *  this interface defines a Visualization service that is able to answer
 *  many question concerning the visualization of the detector. It
 *  is able to associate visualization attributes to materials and logical
 *  volumes. These describe the way these materials and volumes should be
 *  displayed
 *
 * @author Sebastien Ponce
 */
class VisualizationSvc : public Service,
                         virtual public IVisualizationSvc {
  
  /// Friend needed
  friend class SvcFactory<VisualizationSvc>;
  
public:

  /**
   * Standard Constructor
   * @param name   String with service name
   * @param svc    Pointer to service locator interface
   */
  VisualizationSvc (const std::string& name, ISvcLocator* svc);

  /**
   * default destructor
   */
  virtual ~VisualizationSvc() {};

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
  // implementation of the IVisualizationSvc interface //
  ///////////////////////////////////////////////////////

  /**
   * This method returns the visualization attribute associated to a given
   * material or 0 if there is no attribute associated to it.
   * @param mat the material
   * @return the visualization attribute that should be used to display
   * this material
   */
  virtual const VisAttribute visAttribute (const Material* mat) const;

  /**
   * This method returns the visualization attribute associated to a given
   * logical volume or 0 if there is no attribute associated to it.
   * @param vol the logical volume
   * @return the visualization attribute that should be used to display
   * this logical volume
   */
  virtual const VisAttribute visAttribute (const ILVolume* vol) const;

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
  typedef std::map<std::string, VisAttribute> AttributeSet;

  /**
   * This defines a dictionnary
   */
  typedef std::map<std::string, std::string> Dictionnary;

  /// a map of attributes to be used
  AttributeSet m_attributeSet;

  /// a dictionnary linking materials and VisAttributes
  Dictionnary m_material2Vis;

  /// a dictionnary linking logical volumes and VisAttributes
  Dictionnary m_logvol2Vis;
  Dictionnary m_logvol_regex_2Vis;

  /// The location of the Xml file containing all definitions
  std::string m_colorDbLocation;

};

#endif // VISDESC_VISUALIZATIONSVC_H

