#ifndef LHCBEVENT_VELORDIGIT_H
#define LHCBEVENT_VELORDIGIT_H 1


/// Includes
//#include "Gaudi/DataSvc/SmartDataPtr.h"
//#include "Gaudi/Interfaces/ISvcLocator.h"
#include "VeloEvent/VeloDigit.h"

/// Forward declarations
class VeloRDetectorElement;

// Externals 
extern const CLID& CLID_VeloRDigit;

/**
 * Digitisation of an \f$R\f$ strip.
 * This class represents the digitisation of an \f$R\f$ strip.
 */

class VeloRDigit : public VeloDigit {

public:

  /// Constructors
  VeloRDigit() {
    
  }
  
  /// Destructor
  virtual ~VeloRDigit() {

  }
  
  /// Reference to the detector element for the \f$R\f$ detector
  //virtual VeloRDetectorElement& VeloRDigit::det();
  
  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloRDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloRDigit; 
  }
  
private:

  /// Interface to the detector data provider service
  //IDataProviderSvc* m_detSvc;
  /// Name of the detector element for the \f$R\f$ detector
  //std::string  m_detName;
  
};

#include "Det/VeloDet/VeloRDetectorElement.h"

/// Reference to the detector element for the \f$R\f$ detector
//inline VeloRDetectorElement& VeloRDigit::det() {
//  StatusCode sc = serviceLocator()->getService("DetectorDataSvc",
//						 IID_IDataProviderSvc, 
//			    			 (IInterface*&)m_detSvc);
//  SmartDataPtr<VeloRDetectorElement> det(m_detSvc,m_detName);
//
//  if (det) return dynamic_cast<VeloRDetectorElement>&(det); 
//
//}
//inline const SmartDataPtr<VeloRDetectorElement> VeloRDigit::det() const{
//  return m_det;
//}


#endif // LHCBEVENT_VELORDIGIT_H







