
//-----------------------------------------------------------------------------
/** @file ProtoParticleSelection.h
 *
 * Header file for utility class ProtoParticleSelection
 *
 * CVS Log :-
 * $Id: ProtoParticleSelection.h,v 1.7 2009-06-11 15:30:44 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef DAVINCIKERNEL_PROTOPARTICLESELECTION_H
#define DAVINCIKERNEL_PROTOPARTICLESELECTION_H 1

// Include files

// Event model
#include "Event/ProtoParticle.h"

/** @class ProtoParticleSelection ProtoParticleSelection.h Kernel/ProtoParticleSelection.h
 *
 *  Utility class describing selection criteria for ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */

class ProtoParticleSelection
{

public: // definitions

  /// type for list of ProtoParticleSelections
  typedef std::vector<ProtoParticleSelection> Vector;

public:

  /// Standard constructor
  ProtoParticleSelection( ) { }

  ~ProtoParticleSelection( ); ///< Destructor

public:

  /// Disable copy constructor
  ProtoParticleSelection( const ProtoParticleSelection & sel )
    : m_detectors ( sel.cloneDetReqs() ),
      m_cuts      ( sel.cloneCuts()    ) { }

  /// Disable assignment operator
  ProtoParticleSelection& operator = ( const ProtoParticleSelection & sel )
  {
    m_detectors = sel.cloneDetReqs();
    m_cuts      = sel.cloneCuts();
    return *this;
  }

public: // Helper classes

  /** @class Cut ProtoParticleSelection.h Kernel/ProtoParticleSelection.h
   *
   *  Utility class describing a single cut on a ProtoParticle variable
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-05-03
   */
  class Cut
  {
  public:

    /// Enum describing the cut type
    enum Delim
      {
        UndefinedDelim=-1, ///< Undefined delimiter type
        LT=0               ///< Less than
        , GT               ///< Greater than
      };
    /// Type for list of cuts
    typedef std::vector<const ProtoParticleSelection::Cut*> Vector;

  public:

    /// Standard constructor
    Cut( const Delim delim               = Cut::LT,
         const double cutValue           = -999999,
         const std::string & description = "Unknown" )
      : m_delim       ( delim       ),
        m_cutValue    ( cutValue    ),
        m_description ( description ) { }

    /// virtual destructor
    virtual ~Cut() { }

  public:

    /// Clone method
    virtual Cut * clone() const = 0;

  public:

    /** @brief Checks if the given ProtoParticle passes the selection cut
     *  @param proto Pointer to the ProtoParticle to test
     *  @return boolean indicating if cut was satisfied or not
     *  @retval true  ProtoParticle satisfies the cut
     *  @retval false ProtoParticle fails the cut
     */
    virtual bool isSatisfied( const LHCb::ProtoParticle * proto ) const = 0;

  public: // Getters and setters

    /// Access the delimiter type
    ProtoParticleSelection::Cut::Delim delim() const { return m_delim; }
    /// Access the cut value
    double cutValue() const { return m_cutValue; }
    /// Access the cut description
    const std::string & description() const { return m_description; }

    /// Set the delimiter type
    void setDelim( const ProtoParticleSelection::Cut::Delim delim ) { m_delim=delim; }
    /// Set the cut value
    void setCutValue( const double cut )            { m_cutValue    = cut;  }
    /// Set the cut description
    void setDescription( const std::string & desc ) { m_description = desc; }

    /// Returns the cut delimiter type for a given string descriptor
    static Delim delimiter( const std::string & delim )
    {
      const ProtoParticleSelection::Cut::Delim Delim =
        ( delim == "<" ? ProtoParticleSelection::Cut::LT :
          delim == ">" ? ProtoParticleSelection::Cut::GT :
          ProtoParticleSelection::Cut::UndefinedDelim );
      if ( Delim == ProtoParticleSelection::Cut::UndefinedDelim )
      {
        throw GaudiException( "Unknown delimiter "+delim,
                              "*ProtoParticleSelection::Cut::delimiter*",
                              StatusCode::FAILURE );
      }
      return Delim;
    }

  protected:

    /// Method to apply the correct selection criteria based on the delimiter
    inline bool testCut( const double value, const double cut ) const
    {
      switch ( delim() )
      {
      case ProtoParticleSelection::Cut::GT:   return value > cut;
      case ProtoParticleSelection::Cut::LT:   return value < cut;
      default:                                return false;
      }
    }

  private:
    Delim m_delim;                     ///< The cut type (<,> etc.)
    double m_cutValue;                 ///< The cut value
    std::string m_description;         ///< A string description of the cut
  };

  /** @class SingleVariableCut ProtoParticleSelection.h Kernel/ProtoParticleSelection.h
   *
   *  Utility class describing a cut on the a single ProtoParticle variable
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-05-03
   */
  class SingleVariableCut : public Cut
  {
  public:

    /// Standard constructor
    SingleVariableCut( const LHCb::ProtoParticle::additionalInfo variable = LHCb::ProtoParticle::NoPID,
                       const Delim delim               = Cut::LT,
                       const double cutValue           = -999999,
                       const std::string & description = "Unknown" )
      : Cut ( delim, cutValue, description ), m_variable(variable) { }

    /// Destructor
    virtual ~SingleVariableCut() { }

  public:

    /// Clone method
    virtual Cut * clone() const;

  public:

    // test if a ProtoParticle passes the cut or not
    virtual bool isSatisfied( const LHCb::ProtoParticle * proto ) const;

  public: // Setters and getters

    /// Access the variable type
    LHCb::ProtoParticle::additionalInfo variable() const { return m_variable; }

    /// Set the variable type
    void setVariable( const LHCb::ProtoParticle::additionalInfo variable ) { m_variable=variable; }


  private:

    LHCb::ProtoParticle::additionalInfo m_variable; ///< The ProtoParticle variable to apply the cut on

  };

  /** @class DLLCut ProtoParticleSelection.h Kernel/ProtoParticleSelection.h
   *
   *  Utility class describing a cut on the difference of two ProtoParticle DLL values
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-05-03
   */
  class DLLCut : public Cut
  {
  public:

    /// Standard constructor
    DLLCut( const LHCb::ProtoParticle::additionalInfo dll1 = LHCb::ProtoParticle::NoPID,
            const LHCb::ProtoParticle::additionalInfo dll2 = LHCb::ProtoParticle::NoPID,
            const Delim delim               = Cut::LT,
            const double cutValue           = -999999,
            const std::string & description = "Unknown" )
      : Cut ( delim, cutValue, description ), m_dll1(dll1), m_dll2(dll2) { }

    /// Destructor
    virtual ~DLLCut() { }

  public:

    /// Clone method
    virtual Cut * clone() const;

  public:

    /// test if a ProtoParticle passes the cut or not
    virtual bool isSatisfied( const LHCb::ProtoParticle * proto ) const;

  public: // setters and getters

    /// Access the variable type
    LHCb::ProtoParticle::additionalInfo dll1() const { return m_dll1; }
    /// Access the variable type
    LHCb::ProtoParticle::additionalInfo dll2() const { return m_dll2; }

    /// Set the variable types
    inline void setDLLs( const LHCb::ProtoParticle::additionalInfo dll1,
                         const LHCb::ProtoParticle::additionalInfo dll2 )
    {
      m_dll1 = dll1;
      m_dll2 = dll2;
    }

  private:

    /// The first DLL value (X) in constructing the DLL difference X-Y
    LHCb::ProtoParticle::additionalInfo m_dll1;

    /// The second DLL value (Y) in constructing the DLL difference X-Y
    LHCb::ProtoParticle::additionalInfo m_dll2;

  };

  /** @class DetectorRequirements ProtoParticleSelection.h Kernel/ProtoParticleSelection.h
   *
   *  Utility class describing a certain detector requirement
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-05-03
   */
  class DetectorRequirements
  {

  public:

    /// Enum describing the various PID detector
    enum Detector
      {
        UndefinedDet=-1, ///< Undefined detector type
        // General DLL information flags
        RICH,            ///< Requires RICH DLL information
        CALO,            ///< Requires CALO DLL information
        MUON,            ///< Requires MUON DLL information
        // Rich detector flags
        RICH_AEROGEL,    ///< Must have RICH1 aerogel information
        RICH_RICH1GAS,   ///< Must have RICH1 gas information
        RICH_RICH2GAS,   ///< Must have RICH2 gas information
        // calo detector flags
        CALO_SPD,        ///< Must be in the CALO SPD acceptance
        CALO_PRS,        ///< Must be in the CALO PRS acceptance
        CALO_ECAL,       ///< Must be in the CALO ECAL acceptance
        CALO_HCAL,       ///< Must be in the CALO HCAL acceptance
        CALO_BREM,       ///< Must be in the CALO BREM acceptance
      };
    /// Enum describing the various PID detector
    enum Requirement
      {
        UndefinedReq=-1, ///< Undefined detector requirement type
        MustHave=0,      ///< Information from this detector must be available
        MustNotHave,     ///< Information from this detector must NOT be available
        OnlyHave         ///< Information from this detector ONLY must be available
      };
    /// Type for list of Detectors
    typedef std::vector<const ProtoParticleSelection::DetectorRequirements*> Vector;

  public:

    /** Test whether the given ProtoParticle satisfies the detector requirements
     *  @return boolean indicating if detector requirement was satisfied or not
     *  @retval true  ProtoParticle satisfies the detector requirement
     *  @retval false ProtoParticle fails the detector requirement
     */
    virtual bool isSatisfied( const LHCb::ProtoParticle * proto ) const;

  public:

    /// Access the detector type
    int detector() const { return m_detector; }
    /// Access the detector requirement type
    int requirement() const { return m_requirement; }
    /// Access the detector requirement description
    const std::string & description() const { return m_description; }

    /// Set the detector type
    void setDetector( const int det )    { m_detector = det;    }
    /// Set the detector requirement type
    void setRequirement( const int req ) { m_requirement = req; }
    /// Set the detector requirement description
    void setDescription( const std::string & desc ) { m_description = desc; }

    /// Returns the detector enum for a given string
    static Detector detector( const std::string & det );

  public:

    /// Standard constructor
    DetectorRequirements( const Detector det    = ProtoParticleSelection::DetectorRequirements::UndefinedDet,
                          const Requirement req = ProtoParticleSelection::DetectorRequirements::UndefinedReq,
                          const std::string & description = "Unknown" )
      : m_detector    ( det ),
        m_requirement ( req ),
        m_description ( description ) { }

    /// Destructor
    virtual ~DetectorRequirements() { }

  public:

    /// Clone method
    virtual DetectorRequirements * clone() const;

  private:

    // Methods to test a ProtoParticle has specific RICH radiators

    /// Does this ProtoParticle have RICH information from the aerogel radiator
    inline bool hasRichAerogel( const LHCb::ProtoParticle * proto ) const
    {
      return ( hasRichDLL(proto) && NULL != proto->richPID() ?
               proto->richPID()->usedAerogel() : false );
    }

    /// Does this ProtoParticle have RICH information from the Rich1Gas radiator
    inline bool hasRich1Gas( const LHCb::ProtoParticle * proto ) const
    {
      return ( hasRichDLL(proto) && NULL != proto->richPID() ?
               proto->richPID()->usedRich1Gas() : false );
    }

    /// Does this ProtoParticle have RICH information from the Rich2Gas radiator
    inline bool hasRich2Gas( const LHCb::ProtoParticle * proto ) const
    {
      return ( hasRichDLL(proto) && NULL != proto->richPID() ?
               proto->richPID()->usedRich2Gas() : false );
    }

    // Methods to test a ProtoParticle has specific CALO information

    /// Does this ProtoParticle have CALO SPD information
    inline bool hasCaloSPD( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::InAccSpd );
    }

    /// Does this ProtoParticle have CALO PRS information
    inline bool hasCaloPRS( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::InAccPrs );
    }

    /// Does this ProtoParticle have CALO ECAL information
    inline bool hasCaloECAL( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::InAccEcal );
    }

    /// Does this ProtoParticle have CALO HCAL information
    inline bool hasCaloHCAL( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::InAccHcal );
    }

    /// Does this ProtoParticle have CALO Brem information
    inline bool hasCaloBREM( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::InAccBrem );
    }

    // Methods to test if a ProtoParticle has any sub-det DLL information

    /// Does this ProtoParticle have RICH DLL information
    inline bool hasRichDLL( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::RichPIDStatus );
    }

    /// Does this ProtoParticle have MUON DLL information
    inline bool hasMuonDLL( const LHCb::ProtoParticle * proto ) const
    {
      return proto->hasInfo( LHCb::ProtoParticle::MuonPIDStatus );
    }

    /// Does this ProtoParticle have CALO DLL information
    inline bool hasCaloDLL( const LHCb::ProtoParticle * proto ) const
    {
      return ( hasCaloECAL(proto) || hasCaloPRS(proto) );
    }

  private: // data

    // Store these as ints, to allow users to dynamically extend the types that can be stored.
    int    m_detector;         ///< The detector the requirement refers to
    int m_requirement;         ///< The requirement type (requires, only use etc.)
    std::string m_description; ///< A string description of the cut

  };

public: // accessors and setters etc.

  /// read Access to the list of cuts
  inline const ProtoParticleSelection::Cut::Vector & cuts() const
  {
    return m_cuts;
  }

  /// read access to the list of detector requirements
  inline const ProtoParticleSelection::DetectorRequirements::Vector & detReqs() const
  {
    return m_detectors;
  }

  /// Add a new new
  inline void addToCuts( const Cut * cut )
  {
    m_cuts.push_back( cut );
  }

  /// Add a new detector requirement
  inline void addToDetReqs( const DetectorRequirements * detreqs )
  {
    m_detectors.push_back( detreqs );
  }

public: // cloning methods

  /// Clone the vector of DetectorRequirements
  DetectorRequirements::Vector cloneDetReqs() const;

  /// Clone the vector of Cuts
  Cut::Vector cloneCuts() const;

private: // data

  /// List of detector requirements
  DetectorRequirements::Vector m_detectors;

  /// List of cuts to apply
  Cut::Vector m_cuts;

};

#endif // DAVINCIKERNEL_PROTOPARTICLESELECTION_H
