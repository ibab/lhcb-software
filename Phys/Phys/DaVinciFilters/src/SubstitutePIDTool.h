// $Id: $
#ifndef SUBSTITUTEPIDTOOL_H 
#define SUBSTITUTEPIDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ISubstitutePID.h"            // Interface


/** @class SubstitutePIDTool SubstitutePIDTool.h
 *
 *  Tool to substitute PID of a DecayChain.
 *
 *  All Code is extracted from SubstitutePID Algorithm by Vanya.
 *  See this algorithm for configuration. The options are the same.
 *
 *  @see SubstitutePID
 *
 *  @author Patrick Koppenburg
 *  @date   2011-12-07
 *  @author  Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-05-22
 */
class SubstitutePIDTool : public GaudiTool, 
                          virtual public ISubstitutePID 
{

public: 

  /// Standard constructor
  SubstitutePIDTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~SubstitutePIDTool( ); ///< Destructor

private:

  // ==========================================================================
  struct  Substitution
  {
    // ========================================================================
    Substitution () 
      : m_finder ( Decays::Trees::Types_<const LHCb::Particle*>::Invalid() ) 
      , m_pid    ( 0 ) 
      , m_used   ( 0 ) 
    {}  
    Substitution 
    ( Decays::IDecay::iTree&  tree , 
      const LHCb::ParticleID& pid  ) 
      : m_finder ( tree ) 
      , m_pid    ( pid  ) 
      , m_used   ( 0    ) 
    {}
    // ========================================================================
    Decays::IDecay::Finder m_finder  ;     //                 the decay finder 
    LHCb::ParticleID       m_pid     ;     //                              PID 
    unsigned long long     m_used    ;     //                            #used 
    // ========================================================================
  } ;
  // ==========================================================================
  typedef std::vector<Substitution> Substitutions ;
  // ========================================================================== 
  /// mapping : { 'decay-component' : "new-pid" } (property)
  SubstitutionMap  m_map  ; // mapping : { 'decay-component' : "new-pid" }
  /// the actual substitution engine 
  Substitutions    m_subs ; // the actual substitution engine 
  bool  m_to_be_updated1 ; ///< update property
public:
  /// initialize
  StatusCode initialize(  ) ;
  /// decode the substitution code 
  StatusCode decodeCode( const SubstitutionMap& newMap ) ;
  /// loop over particles
  StatusCode substitute( const LHCb::Particle::ConstVector& input,
                         LHCb::Particle::ConstVector& output );
  void updateHandler ( Property& p ); ///< update properties
  /// perform the actual substitution 
  unsigned int substitute ( LHCb::Particle* p );
private:
  /// perform the recursive 4-momentum correction
  unsigned int correctP4 ( LHCb::Particle* p );
  ///< internal check
  bool m_initialized ;
};
#endif // SUBSTITUTEPIDTOOL_H
