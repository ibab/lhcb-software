// $Id: PMakerSelection.h,v 1.1.1.1 2004-08-24 06:47:48 pkoppenb Exp $
#ifndef PMAKERSELECTION_H 
#define PAKERSELECTION_H 1

// Include files
#include <string>
#include <vector>
#include <utility>

/** @class PMakerSelection PMakerSelection.h
 *
 *  Helper class to hold parameters for selection for a particle type.
 *  Contains detectors flags and cuts to be satisfied in this case.
 *
 *  @author Gloria Corti
 *  @date   2003-05-26
 */
class PMakerSelection {
public:
  enum DLLCuts { 
    DLLE_PI=0,  DLLE_MU=1,   DLLE_K=2,   DLLE_P=3,
    DLLMU_PI=4, DLLMU_E=5,   DLLMU_K=6,  DLLMU_P=7,
    DLLK_PI=8,  DLLK_E=9,    DLLK_MU=10, DLLK_P=11,
    DLLP_PI=12, DLLP_E=13,   DLLP_MU=14, DLLP_K=15,
    DLLPI_E=16, DLLPI_MU=17, DLLPI_K=18, DLLPI_P=19 
  };

  /// Standard constructor
  PMakerSelection()
     : m_detectors(0) {
     for(int i=0; i<20; ++i ) {
       m_dllCuts.push_back(-999.0);  
     }
  }

  virtual ~PMakerSelection( ) {} ///< Destructor

  /// Retrieve Detector flags
  unsigned long detectors() const; 
  
  /// Update Detector flags
  void setDetectors(unsigned long value);


  /// Update Boolean: detector to be present
  void setHasRich(unsigned long value);

  /// Retrieve Boolean: detector is present
  unsigned long HasRich() const;

  /// Update Boolean: detector to be present
  void setHasCalo(unsigned long value);

  /// Retrieve Boolean: detector is present
  unsigned long HasCalo() const;

  /// Update Boolean: detector to be present
  void setHasMuon(unsigned long value);

  /// Retrieve Boolean: detector is present
  unsigned long HasMuon() const;
    
  /// Retrieve list of Dll cuts to apply
  const std::vector<double>& dllCuts() const;

  /// Retrieve list of Dll cuts to apply
  std::vector<double>& dllCuts();
  
  /// Set list of Dll cuts to apply
  void setDllCuts( const std::vector< double >& value );  

  std::string cutType( const PMakerSelection::DLLCuts& cut );
  
protected:

private:

  /// Offsets of bitfield Status
  enum DetectorsBits {HasRichBits = 0, HasCaloBits = 1, HasMuonBits = 2}; 
  /// Bitmasks for bitfield Status
  enum DetectorsMasks {HasRichMask = 0x00000001, HasCaloMask = 0x00000002,
                       HasMuonMask = 0x00000004}; 
  /// Flags for detectors to be present
  unsigned long m_detectors;
  /// Cut values for delta Log Likelihood between hypothesis     
  std::vector< double > m_dllCuts;
};


// ----------------------------------------------------------------------------
//   end of class
// ----------------------------------------------------------------------------
inline unsigned long PMakerSelection::detectors() const 
{
  return m_detectors;
}

inline void PMakerSelection::setDetectors(unsigned long value)
{
  m_detectors = value; 
}

inline void PMakerSelection::setHasRich(unsigned long value)
{
  m_detectors &= ~HasRichMask;
  m_detectors |= ((value << HasRichBits) & HasRichMask);
}

inline unsigned long PMakerSelection::HasRich() const
{
  return ((m_detectors & HasRichMask) >> HasRichBits);
}

inline void PMakerSelection::setHasCalo(unsigned long value)
{
  m_detectors &= ~HasCaloMask;
  m_detectors |= ((value << HasCaloBits) & HasCaloMask);
}

inline unsigned long PMakerSelection::HasCalo() const
{
  return ((m_detectors & HasCaloMask) >> HasCaloBits);
}

inline void PMakerSelection::setHasMuon(unsigned long value)
{
  m_detectors &= ~HasMuonMask;
  m_detectors |= ((value << HasMuonBits) & HasMuonMask);
}

inline unsigned long PMakerSelection::HasMuon() const
{
  return ((m_detectors & HasMuonMask) >> HasMuonBits);
}

inline const std::vector<double>& PMakerSelection::dllCuts() const
{
  return m_dllCuts;
}

inline std::vector<double>& PMakerSelection::dllCuts()
{
  return m_dllCuts;
}

inline void PMakerSelection::setDllCuts( const std::vector< double >& value ) 
{
  m_dllCuts = value;
}

std::string PMakerSelection::cutType( const PMakerSelection::DLLCuts& cut ) {
  switch( cut ) {
  case PMakerSelection::DLLE_PI:      return "DLLE_PI";
  case PMakerSelection::DLLE_MU:      return "DLLE_MU";
  case PMakerSelection::DLLE_K:       return "DLLE_K";
  case PMakerSelection::DLLE_P:       return "DLLE_P";
  case PMakerSelection::DLLMU_PI:     return "DLLMU_PI";
  case PMakerSelection::DLLMU_E:      return "DLLMU_E";
  case PMakerSelection::DLLMU_K:      return "DLLMU_K";
  case PMakerSelection::DLLMU_P:      return "DLLMU_P";
  case PMakerSelection::DLLK_PI:      return "DLLK_PI";
  case PMakerSelection::DLLK_E:       return "DLLK_E";
  case PMakerSelection::DLLK_MU:      return "DLLK_MU";
  case PMakerSelection::DLLK_P:       return "DLLK_P";
  case PMakerSelection::DLLP_PI:      return "DLLP_PI";
  case PMakerSelection::DLLP_E:       return "DLLP_E";
  case PMakerSelection::DLLP_MU:      return "DLLP_MU";
  case PMakerSelection::DLLP_K:       return "DLLP_P";
  case PMakerSelection::DLLPI_E:      return "DLLPI_E";
  case PMakerSelection::DLLPI_MU:     return "DLLPI_MU";
  case PMakerSelection::DLLPI_K:      return "DLLPI_K";
  case PMakerSelection::DLLPI_P:      return "DLLPI_P";
  default:                   return "?"; // should never happen
  }
}


#endif // PMAKERSELECTION_H
