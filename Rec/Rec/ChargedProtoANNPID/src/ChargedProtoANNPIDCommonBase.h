
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDCommonBase.h
 *
 *  Declaration file for ANNGlobalPID::ChargedProtoANNPIDCommonBase
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   03/02/2011
 */
//-------------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_CHARGEDPROTOANNPIDCOMMONBASE_H
#define ChargedProtoANNPID_CHARGEDPROTOANNPIDCOMMONBASE_H 1

// STL
#include <string>
#include <vector>
#include <map>
#include <sstream>

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Event Model
#include "Event/ProtoParticle.h"
#include "Event/RecSummary.h"

//-----------------------------------------------------------------------------
/** @namespaceANNGlobalPID
 *
 *  General namespace for Global PID ANN software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2010-03-09
 */
//-----------------------------------------------------------------------------

namespace ANNGlobalPID
{

  /** @class ChargedProtoANNPIDCommonBase ChargedProtoANNPIDCommonBase.h
   *
   *  Common base class
   *
   *  @author Chris Jones
   *  @date   03/02/2011
   */

  template <class PBASE>
  class ChargedProtoANNPIDCommonBase : public PBASE,
                                       virtual public IIncidentListener
  {

  public:

    /// Standard algorithm-like constructor
    ChargedProtoANNPIDCommonBase( const std::string& name,
                                  ISvcLocator* pSvcLocator );

    /// Standard tool-like constructor
    ChargedProtoANNPIDCommonBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

    virtual ~ChargedProtoANNPIDCommonBase( ); ///< Destructor

  public:

    /** Initialization of the algorithm after creation
     *
     * @return The status of the initialization
     * @retval StatusCode::SUCCESS Initialization was successful
     * @retval StatusCode::FAILURE Initialization failed
     */
    virtual StatusCode initialize();

    /** Finalization of the algorithm before deletion
     *
     * @return The status of the finalization
     * @retval StatusCode::SUCCESS Finalization was successful
     * @retval StatusCode::FAILURE Finalization failed
     */
    virtual StatusCode finalize();

    /// Implement the handle method for the Incident service.
    void handle( const Incident& incident );

  protected:

    /// Type for list of inputs
    typedef std::vector<std::string> StringInputs;

  public:

    /** @class Input ChargedProtoANNPIDCommonBase.h
     *
     *  Base class for all 'input' classes, that handle getting
     *  a particular value from a given ProtoParticle
     *
     *  @author Chris Jones
     *  @date   15/04/2013
     */
    class Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const = 0;
    public:
      /// Type for a vector of inputs
      typedef std::vector<const Input *> ConstVector;
    };

    /// ProtoParticle Extra Info object
    class InProtoExInfo : public Input
    {
    public:
      InProtoExInfo( const LHCb::ProtoParticle::additionalInfo info,
                     const double def = -999 )
        : m_info(info), m_def(def) { }
    private:
      LHCb::ProtoParticle::additionalInfo m_info;
      double m_def;
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return proto->info( m_info, m_def );
      }
    };

    /// Track Extra Info object
    class InTrackExInfo : public Input
    {
    public:
      InTrackExInfo( const int info ) : m_info(info) { }
    private:
      int m_info;
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return proto->track()->info( m_info, -999 );
      }
    };

    /// Track Momentum
    class InTrackP : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const double MaxP = 5000.0 * Gaudi::Units::GeV;
        const double var = proto->track()->p();
        return ( var < MaxP ? var : -999 );
      }
    };

    /// Track Transverse Momentum
    class InTrackPt : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const double MaxPt = 1000.0 * Gaudi::Units::GeV;
        const double var = proto->track()->pt();
        return ( var < MaxPt ? var : -999 );
      }
    };

    /// Track Likelihood
    class InTrackLikelihood : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const double var = proto->track()->likelihood();
        return ( var > -120.0 ? var : -999 );
      }
    };

    /// Track Ghost Probability
    class InTrackGhostProb : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const double var = proto->track()->ghostProbability();
        return ( var > 0.00001 ? var : -999 );
      }
    };

    /// Track Clone distance
    class InTrackCloneDist : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const double var = proto->track()->info(LHCb::Track::CloneDist,-999);
        return ( var >= 0 ? var : -999 );
      }
    };

    /// Used RICH Aerogel
    class InRichUsedAerogel : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->usedAerogel() : 0 );
      }
    };

    /// Used RICH Rich1 Gas
    class InRichUsedR1Gas : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->usedRich1Gas() : 0 );
      }
    };

    /// Used RICH Rich2 Gas
    class InRichUsedR2Gas : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->usedRich2Gas() : 0 );
      }
    };

    /// RICH above electron threshold
    class InRichAboveElThres : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->electronHypoAboveThres() : 0 );
      }
    };

    /// RICH above muon threshold
    class InRichAboveMuThres : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->muonHypoAboveThres() : 0 );
      }
    };

    /// RICH above pion threshold
    class InRichAbovePiThres : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->pionHypoAboveThres() : 0 );
      }
    };

    /// RICH above kaon threshold
    class InRichAboveKaThres : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->kaonHypoAboveThres() : 0 );
      }
    };

    /// RICH above proton threshold
    class InRichAbovePrThres : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->protonHypoAboveThres() : 0 );
      }
    };

    /// RICH DLL accessor
    class InRichDLL : public Input
    {
    public:
      InRichDLL( const Rich::ParticleIDType type ) 
        : m_type( type ) { }
    private:
      const Rich::ParticleIDType m_type;
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->richPID() ? proto->richPID()->particleDeltaLL(m_type) : -999 );
      }
    };

    /// Muon IsMuon flag
    class InMuonIsMuon : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->muonPID() ? proto->muonPID()->IsMuon() : 0 );
      }
    };

    /// Muon IsMuonLoose flag
    class InMuonIsMuonLoose : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->muonPID() ? proto->muonPID()->IsMuonLoose() : 0 );
      }
    };

    /// Muon # shared hits
    class InMuonNShared : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return proto->info(LHCb::ProtoParticle::MuonNShared,-1.0) + 1.0;
      }
    };

    /// Muon Muon likelihood
    class InMuonLLMu : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->muonPID() && proto->muonPID()->IsMuonLoose() ? 
                 proto->muonPID()->MuonLLMu() : -999 );
      }
    };

    /// Muon background likelihood
    class InMuonLLBkg : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return ( proto->muonPID() && proto->muonPID()->IsMuonLoose() ?
                 proto->muonPID()->MuonLLBg() : -999 );
      }
    };

    /// Number ProtoParticles
    class InNumProtos : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        const LHCb::ProtoParticles * protos = 
          dynamic_cast<const LHCb::ProtoParticles *>(proto->parent());
        return ( protos ? static_cast<double>(protos->size()) : -999 );
      }
    };

    /// Number of CALO Hypos
    class InNumCaloHypos : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        return static_cast<double>(proto->calo().size());
      }
    };

    /// Access a variable from the RecSummary
    class InRecSummary : public Input
    {
    public:
      InRecSummary( const int info,
                    const ChargedProtoANNPIDCommonBase<PBASE> * parent )
        : m_info( info ), m_parent( parent ) { }
    private:
      const int m_info;   ///< The info variable to access
      const ChargedProtoANNPIDCommonBase<PBASE> * m_parent; ///< Pointer to parent
    public:
      virtual double value( const LHCb::ProtoParticle * ) const
      {
        return m_parent->recSummary()->info( m_info, 0 );
      }
    };

    /// Calo Ecal chi^2
    class InCaloEcalChi2 : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        double var = proto->info(LHCb::ProtoParticle::CaloEcalChi2,-999);
        if      ( var < -100    ) { var = -999; }
        else if ( var > 9999.99 ) { var = -999; }
        return var;
      }
    };

    /// Calo Brem chi^2
    class InCaloBremChi2 : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        double var = proto->info(LHCb::ProtoParticle::CaloBremChi2,-999);
        if      ( var < -100    ) { var = -999; }
        else if ( var > 9999.99 ) { var = -999; }
        return var;
      }
    };

    /// Calo Cluster chi^2
    class InCaloClusChi2 : public Input
    {
    public:
      virtual double value( const LHCb::ProtoParticle * proto ) const
      {
        double var = proto->info(LHCb::ProtoParticle::CaloClusChi2,-999);
        if      ( var < -100   ) { var = -999; }
        else if ( var > 999.99 ) { var = -999; }
        return var;
      }
    };

  public:

    /** Get the Input object for a given input name
     *  @attention Created on the heap, and user takes ownership
     */
    const Input * getInput( const std::string& name ) const;

    /** Get a vector of input objects for a given set of names
     *  @attention Created on the heap, and user takes ownership
     */
    typename Input::ConstVector getInputs( const StringInputs& names ) const;

  private:

    /// Common Constructor initisalisations
    void initCommonConstructor();

    /// Access on demand the RecSummary object
    const LHCb::RecSummary * recSummary() const;

  protected:

    std::string m_protoPath; ///< Location in TES of ProtoParticles

    /// Cached pointer to the RecSummary object
    mutable const LHCb::RecSummary * m_summary;

  };

}

#endif // ChargedProtoANNPID_CHARGEDPROTOANNPIDCOMMONBASE_H
