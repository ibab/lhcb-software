#ifndef DETELEMFINDER_H 
#define DETELEMFINDER_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/HashMap.h"
#include "DetDesc/IDetElemFinder.h"

/** @class DetElemFinder DetElemFinder.h
 *  
 *  Implementation of IDetElemFinder.
 *
 *  @author Marco Clemencic
 *  @date   2006-09-01
 */
class DetElemFinder: public extends<Service,IDetElemFinder> {
public: 

  /// Standard constructor
  DetElemFinder( const std::string& name, ISvcLocator* svcloc ); 

  ~DetElemFinder( ) override; ///< Destructor

  /** Query interfaces (\see{IInterface})
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */

  /// Find the detector element instance associated to a given physical volume path.
  /// Returns 0 if there is no suitable detector element.
  const IDetectorElement * detectorElementForPath(const std::string &path) const override;

  /// Initialize Service
  StatusCode initialize() override;
  
  /// Finalize Service
  StatusCode finalize() override;


private:

  /// Recursively add the given detector element and its children.
  /// The parameter parent_path is the used to speed up recursion.
  StatusCode insert(const IDetectorElement *de, const std::string &parent_path = "");

  /// Find the detector element path and fill the variable path.
  /// If parent_path is not specified, it retrieved recursively.
  StatusCode detector_element_path(const IDetectorElement *de, std::string &path,
                                   const std::string &parent_path = "") const;

  /// Name of the Data Provider (set by the option DetDataSvc, by default "DetectorDataSvc").
  std::string m_detDataSvcName;

  /// Path to the detector element to start from. (DetElemFinder.RootElement = "/dd/Structure/LHCb")
  std::string m_rootElement;

  /// Flag to dump the retrieved map of detector elements. (DetElemFinder.DumpMap = false)
  bool m_dumpMap;

  /// Structure for the mapping
  GaudiUtils::HashMap<std::string, const IDetectorElement *> m_map;

};
#endif // DETELEMFINDER_H
