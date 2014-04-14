#ifndef ELECPRESEL_H
#define ELECPRESEL_H 1

// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticleTransporter.h"            // Interface
#include "GaudiKernel/SmartRefVector.h"

// from Event
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/State.h"
#include "Event/CaloDigit.h"
// *******************
// *** From Calo **
// *******************
#include "CaloDet/DeCalorimeter.h"
// From Kernel
#include "Kernel/CaloCellID.h"

class ITrackExtrapolator;

using namespace LHCb;

namespace ElecPreselDets
{
   enum Calorimeters {Spd = 0, Prs, Ecal, Hcal};
}

namespace Kali 
{
  class ElecPresel : public DVAlgorithm {
    
  public:
    /// Standard methods...
    /// Standard constructor
    ElecPresel ( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~ElecPresel ();  ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
    /// Specific methods
    
    void  PropagateToZ (const ProtoParticle *proto, double z, double *x,  
                        double *y);
    void  StoreCaloInfo ();
    bool  CheckHCal (const ProtoParticle *proto);
    
  private:
    
    ITrackExtrapolator* m_Extra;        ///< Track extrapolator 
    
    std::string m_Input;       ///< Location of ProtoParticles
    std::string m_Calos[4];    ///< Location of calorimeters in DET
    std::string m_Digits[4];   ///< Location of calorimeters in RAW
    
    const DeCalorimeter *m_Calo[4];       ///< Calorimeters in DET
    LHCb::CaloDigits    *m_RawDigits[4];  ///< Digits in RAW
    
    double m_zShowerMax[4];
    double zShowerLength[4];
    
    // Electrons collected
    int m_NETracks;                  ///< Total number of "Electron" candidates
    float m_ClosestTrack[100];      ///< Distance to the closest track at ECAL
    
    // Complete tracks info
    const Particle      *m_AllParts[10000];
    const ProtoParticle *m_AllProto[10000];
    int            m_Mask[10000];
    int m_NAllTracks;
    double m_XYEcal[2][10000];
    int     m_Ele[10000];
    float   m_Vcharge[10000];  
    
    double  m_ECALisolation;  ///< Tracks isolation distance (in mm)
    
  };
}

#endif // ELECPRESEL_H

  
