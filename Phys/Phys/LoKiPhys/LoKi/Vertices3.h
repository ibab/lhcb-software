// $Id$
// ============================================================================
#ifndef LOKI_VERTICES3_H 
#define LOKI_VERTICES3_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV ibelyaev@cern.ch
 *  @date   2010-12-05
 *   
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace  Vertices 
  {
    // ========================================================================
    /** @class RecVertex2TrackMin
     *  @see LoKi::Cuts::RV_TrMIN 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackMin
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackMin
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
        const double                                    bad   ) ;
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackMin
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackMin () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackMin* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackMin () ;            // the default constructor is disabled
      // ======================================================================
    protected:
      // ======================================================================
      /// the function itself 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Track*>::Function>::Type m_fun ;
      /// "bad"-value 
      double         m_bad   ;  // "bad"-value 
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackMin
    // ========================================================================
    /** @class RecVertex2TrackMax
     *  @see LoKi::Cuts::RV_TrMAX 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackMax 
      : public LoKi::Vertices:: RecVertex2TrackMin 
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackMax
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
        const double                                    bad   ) ;
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackMax
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackMax () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackMax* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackMax () ;            // the default constructor is disabled
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackMax
    // ========================================================================
    /** @class RecVertex2TrackSum
     *  @see LoKi::Cuts::RV_TrSUM 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackSum
      : public LoKi::Vertices:: RecVertex2TrackMax
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackSum
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
        const double                                    bad   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackSum () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackSum* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackSum () ;            // the default constructor is disabled
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackSum
    // ========================================================================
    /** @class RecVertex2TrackFun
     *  @see LoKi::Cuts::RV_TrFUN 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackFun
      : public LoKi::Vertices:: RecVertex2TrackSum
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackFun
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
        const unsigned short                            index , 
        const double                                    bad   ) ;
      /// constructor from track cuts, track index 
      RecVertex2TrackFun
      ( const LoKi::Functor<const LHCb::Track*,double>& fun   , 
        const unsigned short                            index ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackFun() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackFun* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackFun() ; // the default constructor is disabled
      // ======================================================================
    private :
      // ======================================================================
      /// the track index :
      unsigned short m_index ;  // the track index 
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackFun
    // ========================================================================
    /** @class RecVertex2TrackHas
     *  @see LoKi::Cuts::RV_TrHAS
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackHas
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index 
      RecVertex2TrackHas
      ( const LoKi::Functor<const LHCb::Track*,bool>& cut   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackHas () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackHas* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackHas () ;            // the default constructor is disabled
      // ======================================================================
    protected:
      // ======================================================================
      /// the function itself 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Track*>::Predicate>::Type m_cut ;
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackHas
    // ========================================================================
    /** @class RecVertex2TrackCut
     *  @see LoKi::Cuts::RV_TrCUT
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackCut : public RecVertex2TrackHas 
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index 
      RecVertex2TrackCut
      ( const LoKi::Functor<const LHCb::Track*,bool>& cut   , 
        const unsigned short                          index ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackCut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackCut* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackCut() ; // the default constructor is disabled
      // ======================================================================
    private :
      // ======================================================================
      /// the track index :
      unsigned short m_index ;  // the track index 
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackCut
    // ========================================================================
    /** @class RecVertex2TrackNum
     *  @see LoKi::Cuts::RV_TrMNUM 
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    class GAUDI_API RecVertex2TrackNum
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
      // ======================================================================
    public  :
      // ======================================================================
      /// constructor from track cuts, track index and "bad" value 
      RecVertex2TrackNum
      ( const LoKi::Functor<const LHCb::Track*,bool>& cut      , 
        const double                                  bad = -1 ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~RecVertex2TrackNum () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertex2TrackNum* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertex2TrackNum () ;            // the default constructor is disabled
      // ======================================================================
    protected:
      // ======================================================================
      /// the function itself 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Track*>::Predicate>::Type m_cut ;
      /// "bad"-value 
      double         m_bad   ;  // "bad"-value 
      // ======================================================================
    }; //                       end of class LoKi::Vertices::RecVertex2TrackMin
    // ========================================================================
    /** @class RecVertexMomentum 
     *  Evaluate the momenum of reco vertex from tracks 
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::RV_P
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */
    class GAUDI_API RecVertexMomentum
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      // ======================================================================
      /// constructor
      RecVertexMomentum() ;
      /// MANDATORY: virtual destructor
      virtual ~RecVertexMomentum () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertexMomentum* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RecVertexPt
     *  Evaluate the transverse momentum of reco vertex from tracks 
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::RV_PT
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */
    class GAUDI_API RecVertexPt
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      // ======================================================================
      /// constructor
      RecVertexPt() ;
      /// MANDATORY: virtual destructor
      virtual ~RecVertexPt () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertexPt* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RecVertexMass 
     *  Evaluate the mass of RecVertex using the certain mass-hypotheses 
     *  @see LHCb::RecVertex
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */
    class GAUDI_API RecVertexMass 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      // ======================================================================
      /// constructor from vector of masses 
      RecVertexMass ( const std::vector<double>&           masses ) ;
      /// constructor from vector of IDs  
      RecVertexMass ( const std::vector<LHCb::ParticleID>& ids    ) ;
      /// constructor from vector of IDs  
      RecVertexMass ( const std::vector<std::string>&      ids    ) ;
      /// constructor from IDS 
      RecVertexMass ( const std::string& id1 , 
                      const std::string& id2 ) ;
      /// constructor from IDS 
      RecVertexMass ( const std::string& id1 , 
                      const std::string& id2 ,
                      const std::string& id3 ) ;
      /// MANDATORY: virtual destructor
      virtual ~RecVertexMass () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RecVertexMass* clone() const ;
      /// MANDATORY: the only one essential mehtod 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      RecVertexMass () ;                 // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual vector of masses 
      std::vector<double> m_masses ;             // the actual vector of masses 
      // ======================================================================      
    };
    // ========================================================================
  } //                                          end of namespace LoKi::Vertices 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef RV_TrNUM
     *  simple meta-functor that counts "track"-function
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrNUM  fun = RV_TrNUM ( TrPT > 1 * GeV ) ;
     *  
     *  const double num = fun ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackNum
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackNum                        RV_TrNUM ;
    // ========================================================================
    /** @typedef RV_TrMIN
     *  simple meta-functor that delagates the evaluation of "track"-function
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrMIN  fun = RV_TrMIN ( TrPT , -1 ) ;
     *  
     *  const double pt1 = fun ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackMin
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackMin                        RV_TrMIN ;
    // ========================================================================
    /** @typedef RV_TrMAX
     *  simple meta-functor that delagates the evaluation of "track"-function
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrMAX fun = RV_TrMIN ( TrCHI2PDOF , -1 ) ;
     *  
     *  const double chi2pdof = fun ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackMax
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackMax                        RV_TrMAX ;
    // ========================================================================
    /** @typedef RV_TrSUM
     *  simple meta-functor that delagates the evaluation of "track"-function
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrSUM sum = RV_TrSUM ( TrPT , -1 ) ;
     *  
     *  const double sumpt = fun ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackSum
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackSum                        RV_TrSUM ;
    // ========================================================================
    /** @typedef RV_TrFUN 
     *  simple meta-functor that delagates the evaluation of "track"-function
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  // constrcut the functor to get the pt of the first track in vertex:
     *  const RV_TrFUN  fun = RV_TrFUN ( TrPT , 0 ) ;
     *  
     *  const double pt1 = fun ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackFun 
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackFun                       RV_TrFUN ;
    // ========================================================================
    /** @typedef RV_TrCUT
     *  simple meta-functor that delagates the evaluation of "track"-predicate
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrCUT cut = RV_TrCUT ( 1 * GeV < TrPT , 0 ) ;
     *  
     *  const bool ok = cut ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackCut
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackCut                       RV_TrCUT ;
    // ========================================================================
    /** @typedef RV_TrHAS
     *  simple meta-functor that delegates the evaluation of "track"-predicate
     * 
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_TrHAS has = RV_TrHAS ( 1 * GeV < TrPT  ) ;
     *  
     *  const bool ok = cut ( v )  ;
     *
     *  @endcode  
     *  @see LoKi::Vertices::RecVertex2TrackHas
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex 
     *  @see LHCb::Track
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-05
     */
    typedef LoKi::Vertices::RecVertex2TrackHas                       RV_TrHAS ;
    // ========================================================================
    /** @typedef RV_MASS
     *  Simple evaluator of "mass" for RecVertex 
     *
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const RV_MASS mass = RV_MASS ( "pi+" , "pi-" ) ;
     *  
     *  const double m = mass ( v )  ;
     *
     *  @endcode  
     *  @see LHCb::RecVertex
     *  @see LoKi::Vertices::RecVertexMass
     *  @see LoKi::PhysKinematics::mass
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-07
     */
    typedef LoKi::Vertices::RecVertexMass                             RV_MASS ;
    // ========================================================================
    /** @var RV_P
     *  Simple evaluator of "momentum" for RecVertex 
     *
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const double p = RV_P( v )  ;
     *
     *  @endcode  
     *  @see LHCb::RecVertex
     *  @see LoKi::Vertices::RevVertexMomentum
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-07
     */
    const LoKi::Vertices::RecVertexMomentum                              RV_P ;
    // ========================================================================
    /** @var RV_PT
     *  Simple evaluator of pt for RecVertex 
     *
     *  @code 
     *
     *  const LHCb::VertexBase*  v  = ... ;
     *
     *  const double pt = RV_PT ( v )  ;
     *
     *  @endcode  
     *  @see LHCb::RecVertex
     *  @see LoKi::Vertices::RecVertexPt
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @date   2010-12-07
     */
    const LoKi::Vertices::RecVertexPt                                   RV_PT ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VERTICES3_H
// ============================================================================
