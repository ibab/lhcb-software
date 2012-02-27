// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES40_H 
#define LOKI_PARTICLES40_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ExtraInfo.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
/** @file LoKi/Particles40.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-02-27
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class SmartInfo
     *
     *  Trivial function which:
     *    - checks the presence of information in LHCb::Particle::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::SINFO 
     *  @see LoKi::ExtraInfo::GetSmartInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::ExtraInfo::hasInfo
     *  @see LoKi::ExtraInfo::setInfo
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2012-02-27
     */
    class GAUDI_API SmartInfo 
      : public LoKi::ExtraInfo::GetSmartInfo<const LHCb::Particle*>
    {
    public:
      // ======================================================================
      /** constructor from fuction, key and update-flag
       *  @param index the key in LHCb::Particle::extraInfo table 
       *  @param fun functionto be evaluated for missing keys 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const int                                                   index          , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun            , 
        const bool                                                  update = false ) ;
      /** constructor from fuction, key and update-flag
       *  @param fun functionto be evaluated for missing keys 
       *  @param index the key in LHCb::Particle::extraInfo table 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun            ,
        const int                                                   index          , 
        const bool                                                  update = false ) ;
      /// destructor 
      virtual ~SmartInfo() ;
      /// clone method (mandatory!)
      virtual  SmartInfo* clone() const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SmartInfo();                       // the default constructor is disabled 
      // ======================================================================
    };   
    // ========================================================================
  } //                                         End of namespace LoKi::Particles
  // ==========================================================================
  namespace Vertices
  {
    // ========================================================================
    /** @class SmartInfo
     *
     *  Trivial function which:
     *    - checks the presence of information in LHCb::VertexBase::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  
     *  @see LHCb::VertexBase
     *  @see LoKi::Cuts::VSINFO 
     *  @see LoKi::ExtraInfo::GetSmartInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::ExtraInfo::hasInfo
     *  @see LoKi::ExtraInfo::setInfo
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2012-02-27
     */
    class GAUDI_API SmartInfo 
      : public LoKi::ExtraInfo::GetSmartInfo<const LHCb::VertexBase*>
    {
    public:
      // ======================================================================
      /** constructor from fuction, key and update-flag
       *  @param index the key in LHCb::Particle::extraInfo table 
       *  @param fun functionto be evaluated for missing keys 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const int                                                     index          , 
        const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun            , 
        const bool                                                    update = false ) ;
      /** constructor from fuction, key and update-flag
       *  @param fun functionto be evaluated for missing keys 
       *  @param index the key in LHCb::Particle::extraInfo table 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun            ,
        const int                                                     index          , 
        const bool                                                    update = false ) ;
      /// destructor 
      virtual ~SmartInfo() ;
      /// clone method (mandatory!)
      virtual  SmartInfo* clone() const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SmartInfo();                       // the default constructor is disabled 
      // ======================================================================
    };   
    // ========================================================================
  } //                                          End of namespace LoKi::Vertices
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef SINFO 
     *  "SmartInfo"
     *  Trivial function which:
     *    - checks the presence of information in LHCb::Particle::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  @code 
     *
     *   const Fun fun           = SINFO ( 105566 , PT , true ) ;
     *   const LHCb::Particle* p = ... ;
     *   const double result     = fun ( p ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::SmartInfo 
     */
    typedef LoKi::Particles::SmartInfo                                  SINFO ;
    // ========================================================================
    /** @typedef VSINFO 
     *  "SmartInfo"
     *  Trivial function which:
     *    - checks the presence of information in LHCb::VertexBase::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  @code 
     *
     *   const VFun fun            = VSINFO ( 105566 , VZ , true ) ;
     *   const LHCb::VertexBase* v = ... ;
     *   const double result       = fun ( v ) ;
     *
     *  @endcode 
     *  @see LoKi::Vertices::SmartInfo 
     */
    typedef LoKi::Vertices::SmartInfo                                  VSINFO ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      End of namespace LoKi 
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_PARTICLES40_H
// ============================================================================
