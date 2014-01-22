// $Id$
#ifndef LOKI_TRFILTER_H 
#define LOKI_TRFILTER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// Interface
// ============================================================================
#include <TrackInterfaces/ITrackSelector.h>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
// ============================================================================
namespace LoKi
{
   // ==========================================================================
   namespace Hlt1
   {
      // ============================================================
      /** @class FilterTracks
       *  Simple functor which "upgrades" the candidates.
       *  The upgraded candidates are registered in Hlt Data Service 
       *  Both track & mutlitrack candidated are upgraded 
       *  @see ITrackSelector
       *  @see LoKi::Hlt1::FilterConf
       *  @see LoKi::Cuts::TC_FILTER_TR
       *  @author Roel Aaij roel.aaij@cern.ch
       *  @date   2011-04-08
       */
      class GAUDI_API FilterTracks 
         : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe 
      {
      public:
         // ======================================================================
         /** constructor from all configuration parameters 
          *  @param output  the output selection name 
          *  @param config  the tool configuration 
          */
         FilterTracks( std::string output,     // output selection name/key 
                       std::string tool );     //            tool type/name
         /// MANDATORY: virtual destructor 
         virtual ~FilterTracks() ;
         /// MANDATORY: clone method ("virtual constructor")
         virtual  FilterTracks* clone () const ; 
         /// MANDATORY: the only one essential method 
         virtual result_type operator() ( argument a ) const ;
         /// OPTIONAL: nice printout 
         virtual std::ostream& fillStream ( std::ostream& s ) const ;
         /// recoID 
         int recoID() const { return m_recoID; }
         /// underlying tool name
         const std::string& tool() const { return m_tool; }
      private:
         /// the default constructor is disabled 
         FilterTracks() ;                        // the default constructor is disabled 
         /// the method which does the actual filtering
         virtual bool filter( const LHCb::Track* track ) const;
         /// "sink":  register the selection for Hlt Data Service 
         LoKi::Hlt1::Sink m_sink;                                      // the sink
         /// the algorithm 
         const GaudiAlgorithm* m_alg;
         /// the tool
         mutable LoKi::Interface<ITrackSelector> m_selector;
         /// ID of the filter tool 
         mutable int m_recoID;                     // ID of the recontruction tool 
         /// the underlying tool
         const std::string m_tool;
         // ======================================================================
      };
   }
   namespace Cuts 
   {
      // ========================================================================
      /** @typedef TC_FILTER_TR
       *  Functor to filter tracks in Hlt::Candidate using ITrackSelector.
       *
       *  @see LoKi::Hlt1::FilterTracks 
       *  @author Roel Aaij roel.aaij@cern.ch
       *  @date 2011-04-10
       */
      typedef LoKi::Hlt1::FilterTracks                              TC_FILTER_TR;
      // ========================================================================
   } //                                              end of namespace LoKi::Cuts 
}
#endif // LOKI_TRFILTER_H
