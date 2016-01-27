
//-----------------------------------------------------------------------------
/** @file ChargedProtoCombineDLLsAlg.h
 *
 * Header file for algorithm ChargedProtoCombineDLLsAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/11/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoCombineDLLsAlg_H
#define GLOBALRECO_ChargedProtoCombineDLLsAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoCombineDLLsAlg ChargedProtoCombineDLLsAlg.h
 *
 *  Algorithm to calculate Combined DLL values for ProtoParticles, and store
 *  this information in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 15/11/2006
 */
//-----------------------------------------------------------------------------

class ChargedProtoCombineDLLsAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoCombineDLLsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoCombineDLLsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  /// Converts a string to entirely upper case
  inline std::string to_upper( const std::string & in )
  {
    std::string out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  }

private: // utility classes

  //-----------------------------------------------------------------------------
  /** @class CombinedLL ChargedProtoPAlg.h
   *
   *  Utility class holding the combined LL values for a ProtoParticle
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date 29/03/2006
   */
  //-----------------------------------------------------------------------------
  class CombinedLL
  {
  public:
    /// Standard constructor with initialisation value
    CombinedLL( const double init = 0 )
      : elDLL(init), muDLL(init), piDLL(init), kaDLL(init), prDLL(init), deDLL(init) {};
    double elDLL; ///< Electron Log Likelihood
    double muDLL; ///< Muon Log Likelihood
    double piDLL; ///< Pion Log Likelihood
    double kaDLL; ///< Kaon Log Likelihood
    double prDLL; ///< Proton Log Likelihood
    double deDLL; ///< Deuteron Log Likelihood
  public:
    /// Implement ostream << method
    friend inline std::ostream& operator << ( std::ostream& s,
                                              const CombinedLL& dlls )
    {
      return s << "[ " << dlls.elDLL << " " << dlls.muDLL << " " 
               << dlls.piDLL << " " << dlls.kaDLL << " " << dlls.prDLL << " " << dlls.deDLL << " ]";
    }
  };

private: // methods

  /// Add the Rich DLL information to the combined DLL
  bool addRich( LHCb::ProtoParticle * proto, CombinedLL & combDLL );

  /// Add the Muon DLL information to the combined DLL
  bool addMuon( LHCb::ProtoParticle * proto, CombinedLL & combDLL );

  /// Add the Calo DLL information to the combined DLL
  bool addCalo( LHCb::ProtoParticle * proto, CombinedLL & combDLL );

private: // data

  std::string m_protoPath;    ///< Location in TES of ProtoParticles
  std::vector<std::string>m_elDisable,m_muDisable,m_kaDisable,m_prDisable,m_piDisable,m_deDisable;
  int                     m_elCombDll,m_muCombDll,m_prCombDll,m_piCombDll,m_kaCombDll,m_deCombDll;
  std::map<std::string,int> m_maskTechnique;
};

#endif // GLOBALRECO_ChargedProtoCombineDLLsAlg_H




