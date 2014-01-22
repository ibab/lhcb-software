// $Id$
// ============================================================================
#ifndef LOKI_HLTTOOL_H 
#define LOKI_HLTTOOL_H 1
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
/** @file  LoKi/HltTool.h
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-31
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class Tool
     *  Helper class to hold the configuration of varios tools 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-31
     */
    class GAUDI_API Tool
    {
    public:
      // ======================================================================
      /** constructor from the tool type/name & "public" flag
       *  @param tool the tool type/name 
       *  @param flag public?
       */
      Tool 
      ( std::string  tool          , 
        bool         flag  = false ) ;
      /// virtual destructor 
      virtual ~Tool() {}
      // ======================================================================      
    public:
      // ======================================================================
      /// get the tool 
      const std::string& tool () const { return m_tool    ; }
      /// public? 
      inline bool        pub  () const { return m_public  ; }
      /// private
      inline bool        priv () const { return !m_public ; }
      // ======================================================================
    public:
      // ======================================================================
      /// output operator 
      virtual std::ostream& fillStream  ( std::ostream& s ) const = 0 ;
      /// conversion to string 
      virtual std::string   toString    () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tool () ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// tool type/name 
      std::string m_tool   ;                                  // tool type/name 
      /// public tool ? 
      bool        m_public ;                                   // public tool ? 
      // ======================================================================
    };
    // ========================================================================
    /// output operator 
    // ========================================================================
    inline std::ostream& operator<<
      ( std::ostream&           s , 
        const LoKi::Hlt1::Tool& t ) { return t.fillStream ( s ) ; }
    // ========================================================================
    /** @class TrackFunction 
     *  Helper class to hold the configuration of ITrackFunctionTool
     *  @see ITrackFunctionTool 
     *  @see LoKi::Tracks::TrFunction 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-31
     */
    class GAUDI_API TrackFunction : public LoKi::Hlt1::Tool 
    {
    public:
      // ======================================================================
      /** constructor fofrom tool type/name & "public" flag
       *  @param tool the tool type/name 
       *  @param flag public?
       */
      TrackFunction 
      ( std::string tool          , 
        bool        flag  = false ) ;
      // ======================================================================
      /// constructor from the tool 
      TrackFunction ( const LoKi::Hlt1::Tool& tool ) ;
      // ======================================================================      
    public:
      // ======================================================================
      /// output operator 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackFunction() ;                 // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class TrackBiFunction 
     *  Helper class tro hold the configuration of ITrackBiFunctionTool
     *  @see ITrackBiFunctionTool 
     *  @see LoKi::Tracks::TTrFunction 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-31
     */
    class GAUDI_API TrackBiFunction : public LoKi::Hlt1::Tool 
    {
    public:
      // ======================================================================
      /** constructor fofrom tool type/name & "public" flag
       *  @param tool the tool type/name 
       *  @param flag public?
       */
      TrackBiFunction 
      ( std::string tool          , 
        bool        flag  = false ) ;
      // ======================================================================
      /// constructor from the tool 
      TrackBiFunction ( const LoKi::Hlt1::Tool& tool ) ;
      // ======================================================================      
    public:
      // ======================================================================
      /// output operator 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackBiFunction() ;                // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MatchTVelo
     *  Helper class to hold the configuration of IMatchTVeloTracks
     *  @see IMatchTVeloTracks
     *  @see LoKi::Tracks::TTrFunction 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-31
     */
    class GAUDI_API MatchTVelo : public LoKi::Hlt1::Tool 
    {
    public:
      // ======================================================================
      /** constructor from tool type/name & "public" flag
       *  @param tool the tool type/name 
       *  @param flag public?
       */
      MatchTVelo
      ( std::string tool          , 
        bool        flag  = false ) ;
      // ======================================================================
      /// constructor from the tool 
      MatchTVelo ( const LoKi::Hlt1::Tool& tool ) ;
      // ======================================================================      
    public:
      // ======================================================================
      /// output operator 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MatchTVelo () ;                    // the default constructor is disabled 
      // ======================================================================   
    } ;
    // ========================================================================
    /** @class TrackVertexFunction 
     *  Helper class to hold the configuration of ITrackVertexBiFunctionTool
     *  @see ITrackVertexBiFunctionTool 
     *  @see LoKi::Tracks::TrVFunction 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-31
     */
    class GAUDI_API TrackVertexFunction : public LoKi::Hlt1::Tool 
    {
    public:
      // ======================================================================
      /** constructor fofrom tool type/name & "public" flag
       *  @param tool the tool type/name 
       *  @param flag public?
       */
      TrackVertexFunction 
      ( std::string tool          , 
        bool        flag  = false ) ;
      // ======================================================================
      /// constructor from the tool 
      TrackVertexFunction ( const LoKi::Hlt1::Tool& tool ) ;
      // ======================================================================      
    public:
      // ======================================================================
      /// output operator 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackVertexFunction() ;            // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_HLTTOOL_H
// ============================================================================
