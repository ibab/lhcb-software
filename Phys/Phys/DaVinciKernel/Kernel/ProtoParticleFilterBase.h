
//-----------------------------------------------------------------------------
/** @file ProtoParticleFilterBase.h
 *
 * Header file for algorithm ProtoParticleDLLFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleFilterBase.h,v 1.2 2007-03-02 12:36:53 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef DAVINCIKERNEL_PROTOPARTICLEFILTERBASE_H
#define DAVINCIKERNEL_PROTOPARTICLEFILTERBASE_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciKernel
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/ProtoParticleSelection.h"

// Event
#include "Event/ProtoParticle.h"

/** @class ProtoParticleFilterBase ProtoParticleFilterBase.h
 *
 *  Base class for all ProtoParticle Filter tools
 *
 *  @author Chris Jones
 *  @date   02/05/2006
 */

class ProtoParticleFilterBase : public GaudiTool,
                                virtual public IProtoParticleFilter
{

public: // Gaudi methods

  /// Standard constructor
  ProtoParticleFilterBase( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~ProtoParticleFilterBase( ) = 0; ///< Destructor

  /// Initialisation
  virtual StatusCode initialize();

  /// Finalisation
  virtual StatusCode finalize();

protected: // Definitions

  /// Type for job options selection descriptions
  typedef std::vector<std::string> SelectionDescs;

public: // tool interface methods

  /// Test if filter is satisfied.
  virtual bool isSatisfied( const LHCb::ProtoParticle* const & proto ) const;

protected:

  /// Converts a string to entirely upper case
  inline std::string to_upper( const std::string & in )
  {
    std::string out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  }

  /* Decode the ProtoParticle selection as defined in the job options
   *  into the internal selection object
   *  @return StatusCode
   *  @retval StatusCode::SUCCESS ProtoParticle selection successfully defined
   *  @retval StatusCode::FAILURE ProtoParticle selection failed to be defined
   */
  virtual StatusCode defineSelections();

  /// Decode a single selection string
  virtual StatusCode decodeSelOpts( const std::string & description );

  /** @brief Create a cut object from decoded cut options
   *   @param tag   The cut type
   *   @param delim The cut delimiter (<,>,<=,>=)
   *   @param value The cut description
   *   @return Pointer to the cut object that was created
   *   @retval non-Null Cut information was successfully decoded.
   *   @retval NULL     Cut information was not successfully decoded.
   */
  virtual const ProtoParticleSelection::Cut * createCut( const std::string & tag,
                                                         const std::string & delim,
                                                         const std::string & value ) const = 0;

  /** @brief Create a DetectorRequirements object
   *   @param tag   The detector type
   *   @param value The detector requirement
   *   @return Pointer to the detector requirement object that was created
   *   @retval non-NULL Requirement information was successfully decoded.
   *   @retval NULL     Requirement information was not successfully decoded.
   */
  virtual const ProtoParticleSelection::DetectorRequirements * createDetReq( const std::string & tag,
                                                                             const std::string & value ) const = 0;

  /// Read access to ProtoParticleSelection::Vector
  inline const ProtoParticleSelection::Vector & protoSels() const { return m_protoSels; }

private: // utility classes

  /** @class Parser ProtoParticleFilterBase.h
   *
   *  Simple class for decoding filtering options
   *
   *  @author Chris Jones
   *  @date   02/05/2006
   */
  class Parser
  {
  public:
    /** @class Parser ProtoParticleFilterBase.h
     *
     *  A single decoded result for the Parser
     *
     *  @author Chris Jones
     *  @date   02/05/2006
     */
    class Result : public std::pair<std::string,std::string>
    {
    public:
      /// Standard Constructor from tag and value
      Result( const std::string & tag, const std::string & value )
        : std::pair<std::string,std::string>(tag,value) { }
      /// Returns the tag type
      const std::string & tag()   const { return this->first;  }
      /// Returns the tag value
      const std::string & value() const { return this->second; }
    };
    typedef std::vector<Result> Results;
  public:
    /// Standard Constructor
    Parser() {}
    /// Analyse the given string, usin g the given delimiter
    bool analyse( const std::string & desc, const std::string & delim );
    /// Returns access to the decoded results
    const Results & results() const {  return m_results; }
  private:
    Results m_results; ///< List of results of the parsing
  };

private: // data

  /// ProtoParticle Selection(s)
  ProtoParticleSelection::Vector m_protoSels;

  /// Job option description of the particle selection
  SelectionDescs m_selectionOpts;

};

#endif // DAVINCIKERNEL_PROTOPARTICLEFILTERBASE_H
