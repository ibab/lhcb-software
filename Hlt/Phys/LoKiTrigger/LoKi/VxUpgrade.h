// $Id$
// ============================================================================
#ifndef LOKI_VXUPGRADE_H 
#define LOKI_VXUPGRADE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/UpgradeTool.h"
#include "LoKi/VxCreator.h"
// ============================================================================
/** @file 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-31
 * 
 *  @todo VxUpgrade: check if we really need this code 
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class VxUpgrade VxUpgrade.h LoKi/VxUpgrade.h
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-13
     */
    class VxUpgrade 
      : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe
      , public LoKi::Hlt1::UpgradeTool 
      , public LoKi::Hlt1::VxCreator
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       *  @param creator vertex creator 
       *  @param location TES location of upgraded vertices 
       */
      VxUpgrade 
      ( std::string                    output    ,// output selection name/key 
        const LoKi::Hlt1::UpgradeConf& config    ,//             configuration 
        const LoKi::Hlt1::VxCreator&   creator   =
        LoKi::Hlt1::VxCreator ()                 ,//            vertex creator  
        std::string             location  = 
        "Hlt1/Vertex/VertexUpgradeBank"  ) ; //   location of upgraded vertices
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       *  @param creator vertex creator 
       *  @param location TES location of upgraded vertices 
       */
      VxUpgrade 
      ( std::string                    output    ,// output selection name/key 
        const LoKi::Hlt1::UpgradeConf& config    ,//             configuration 
        std::string                    location  ,//         upgraded vertices
        const LoKi::Hlt1::VxCreator&   creator   =
        LoKi::Hlt1::VxCreator ()                 ) ; //          vertex creator  
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      VxUpgrade 
      ( std::string                    output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeTool& config  ,   //             configuration 
        const LoKi::Hlt1::VxCreator&   creator   =
        LoKi::Hlt1::VxCreator ()               ,//            vertex creator  
        std::string                    location  = 
        "Hlt1/Vertex/VertexUpgradeBank"  ) ; // location of upgdarded vertices
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      VxUpgrade 
      ( std::string                    output   ,  // output selection name/key 
        const LoKi::Hlt1::UpgradeTool& config   ,  //             configuration 
        std::string                    location ,  //         upgraded vertices
        const LoKi::Hlt1::VxCreator&   creator  =
        LoKi::Hlt1::VxCreator ()                ) ;//            vertex creator  
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~VxUpgrade() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VxUpgrade* clone () const { return new VxUpgrade (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================    
    public:
      // ======================================================================
      /// output selection 
      const Gaudi::StringKey& output  () const { return m_sink.output  () ; }
      const std::string& selName () const { return m_sink.selName () ; }
      // ======================================================================      
    public:
      // ======================================================================
      /// TES location of vertices 
      const std::string& location () const { return m_location ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default coinstructor is disabled 
      VxUpgrade() ;                     // the default coinstructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// "sink": register selection in Hlt Data Service 
      LoKi::Hlt1::VxSink m_sink ; // register selection in Hlt Data Service 
      // ======================================================================
      /// TES location of vertices 
      std::string  m_location ;                     // TES location of vertices 
      // ======================================================================      
    } ;
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VXUPGRADE_H
// ============================================================================
