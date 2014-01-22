// ============================================================================
#ifndef LOKI_UPGRADEVERTICES_H
#define LOKI_UPGRADEVERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/VertexBase.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/UpgradeTool.h"
#include "TrUpgrade.h"
// ============================================================================
// ============================================================================
/** @file
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  Implementation of UpgradeVertices, an adaptation of VxUpgrade from streamer
 *  of RecVertices to Hlt::Candidates.
 *  Could be aligned with UpgradeTracks and UpgradeMultiTracks by moving the
 *  actual logic into UpgradeTool, cfr. TrUpgrade.cpp, UpgradeTool.cpp
 *
 *  @author Pieter David pieter.david@cern.ch
 *  @date   2012-02-24
 */
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    /** @class UpgradeVertices
     *  Functor which "upgrades" the candidates.
     *  The upgraded candidates are registered in Hlt Data Service
     *  Only "vertex" candidates are upgraded - first both tracks are upgraded,
     *  then the vertices are recreated using VertexCreator
     *  @see ITracksFromTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TC_UPGRADE_VX
     *  @author Pieter David pieter.david@cern.ch
     *  @date 2012-02-24
     */
    class GAUDI_API UpgradeVertices
      : public Upgrade
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters
       *  @param output  the output selection name
       *  @param config  the tool configuration
       */
      UpgradeVertices
      ( std::string              output  ,  // output selection name/key
        const LoKi::Hlt1::UpgradeConf&  config  ,  //             configuration
        bool                      allow1Fail = false                                   ,
        std::string              clonedTracksLocation = "Hlt/Track/Tmp" 
      ) ;
      // ======================================================================
      /** constructor from all configuration parameters
       *  @param output  the output selection name
       *  @param config  the tool configuration
       */
      UpgradeVertices
      ( std::string              output  ,  // output selection name/key
        const LoKi::Hlt1::UpgradeTool&  config  ,  //             configuration
        bool                      allow1Fail = false                                   ,
        std::string              clonedTracksLocation = "Hlt/Track/Tmp"                         
      ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~UpgradeVertices () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  UpgradeVertices* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      UpgradeVertices();                 // the default constructor is disabled
      // ======================================================================
      bool m_allow1Fail;
      // ======================================================================
      int  m_failKey;
      // ======================================================================
      std::string m_clonedTracksLocation;
      // ======================================================================
      /** upgrade the candidates
       *  @param input   input recvertices
       *  @param output  output container of upgraded recvertices (cumulative)
       *  @return status code
       */
      StatusCode upgradeVertices
      ( const Hlt::Candidate::ConstVector& input   ,
        Hlt::Candidate::ConstVector&       output  ) const ;
      // ======================================================================
      /** upgrade the candidate
       *  @param input   input recvertices
       *  @param output  output container of upgraded recvertices (cumulative)
       *  @param otracks output container of upgraded tracks (cumulative)
       *  @return status code
       */
      StatusCode _i_upgrade_recvertex_j
      ( const Hlt::Candidate*              input   ,
        Hlt::Candidate::ConstVector&       output  ,
        LHCb::Track::Container*            otracks ) const ;
      // ======================================================================
      /**
       *  Retrieve the ExtraInfo key for failed tracks from the HltANN service
       */
      void retrieveFailKey() ;
      // ======================================================================
    private:
      /** make the vertices
       *  @param tracks1 the first set of tracks
       *  @param tracks2 the second set of tracks
       *  @param vertices (cumulative) container of created vertices
       *  @return number of created vertices
       */
      size_t make
      ( const LHCb::Track::ConstVector& tracks1 ,
        const LHCb::Track::ConstVector& tracks2 ,
        std::vector<LHCb::RecVertex*>& vertices  ) const ;
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef TC_UPGRADE_VX
     * functor to upgrade vertices
     *
     * @see LoKi::Hlt1::VxUpgrade
     * @author Pieter David pieter.david@cern.ch
     * @date 2012-02-24
     */
    typedef LoKi::Hlt1::UpgradeVertices                         TC_UPGRADE_VX ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_UPGRADEVERTICES_H
// ============================================================================
