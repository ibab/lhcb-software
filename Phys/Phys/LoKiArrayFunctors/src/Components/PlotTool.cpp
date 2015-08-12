// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram.h"
#include "AIDA/IBaseHistogram.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/Fill.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPlotTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
#include "Preambulo.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class PlotTool
     *  Simple "hybrid-based" implementation of abstract interface IPlotTool
     *
     *
     *  The tool have the following important properties:
     *
     *    - <b>Factory</b>: the name of the Hybrid Factory to be used for
     *                      creation of the functors, the default value is  
     *                      <c>"LoKi::Hybrid::Tool/HybridFactory:PUBLIC"</c>
     *
     *    - <b>Selector</b>: the python predicate to be used for selection
     *                      of particles, the default value is <c>ALL</c>
     *
     *    - <b>Histos</b>: the map  { functor : histogram }, default is the empty map. 
     *
     *
     *  The examples of "{ functor : histogram }"-map:
     *
     *  @code 
     *
     *  Plots.Plots.Histos = 
     *    { "P/1000"  : ('momentum',0,100) , 
     *      "PT/1000" : ('pt_%1%',0,5,500) , 
     *      "M"       : ('mass in MeV_%1%_%2%_%3%',0,6000) } ;
     *
     *  @endcode 
     *
     *  Please note "%1%" format descriptors.
     *  The actual title of the histogram is constructed from
     *  the base title (used as "format"-string) and three fields:
     *    -# <c>trailer</c>
     *    -# <c>functor</c>
     *    -# <c>selector</c>
     *
     *  Therefore *if* the base title conatains the format directives,
     *  they will be expanded, e.g. the third histogram will have the 
     *  actual title  <c>"mass in GeV__M_TRUE"</c> (assuming empty <c>trailer</c>)
     *
     *  One is free to choose the format, reshuffel and skip the certain fields,
     *  e.g. the following configuration
     *  @code 
     *
     *  Plots.Plots.Histos = 
     *    {  ... , 
     *      "M"       : ('The histogram Cut:%3% Function:%2%',0,6000) } ;
     *
     *  @endcode 
     *  produces the title equal to <c>"The histogram Cut:TRUE Function:M"</c>
     *  
     *  More sophisticated formats are available. For detailes see 
     *   Boost.Format library
     *  
     *  @see IPlotTool
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-16
     */
    class PlotTool
      : public virtual IPlotTool 
      , public    GaudiHistoTool 
    {
      // ======================================================================
      // friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::PlotTool> ;
      // ======================================================================
    public:
      // ======================================================================
      /// the standard initialization of the tool 
      virtual StatusCode initialize () 
      {
        StatusCode sc = GaudiHistoTool::initialize () ;
        if ( sc.isFailure() ) { return sc ; }
        //
        svc<IService> ( "LoKiSvc"          , true ) ;
        svc<IService> ( "HistogramDataSvc" , true ) ;
        //
        sc = initSelector () ; // initialize the selector 
        if ( sc.isFailure() ) 
        { return Error ( "Unable to decode the 'Selector'", sc ) ; }
        //
        sc = initHistos   () ; // initialize the histograms 
        if ( sc.isFailure() ) 
        { return Error ( "Unable to decode the 'Histos'"  , sc ) ; }
        //
        if ( m_histos.size() != m_map.size() ) 
        { return Error ( "Something wrong in the configurtaion!") ; }
        //
        return StatusCode::SUCCESS ;
      }
      // ======================================================================
      /// finalize: reset functors
      virtual StatusCode finalize() 
      {
        /// reset all functors
        m_histos.clear() ;
        /// finalize the base
        return GaudiHistoTool::finalize () ;        
      }
      // ======================================================================
    public:
      // ======================================================================
      /** the helper structure to keep the all information related 
       *  to the basic" bunch" of the histograms
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-03-17
       */
      class Histo
      {
      public:
        // ===================================================================
        /// the actual type of "trailer->histos" map
        typedef GaudiUtils::VectorMap<std::string, AIDA::IHistogram1D*> HMap ;
        // ===================================================================
      public:
        // ===================================================================
        /// the default constructor 
        Histo ()
          : m_fun    ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant ( -1.e+10 ) )
          , m_desc   (   )
          , m_histos (   )
        {}
        // ====================================================================
      public:
        // ====================================================================
        /// the function to be evaluated 
        LoKi::Types::Fun    m_fun    ;
        /// the histogram descriptor
        Gaudi::Histo1DDef   m_desc   ;             // the histogram description 
        /// the list of histogram 
        HMap                m_histos ;
        // ====================================================================
      } ;
      // ======================================================================
  public:
      // ======================================================================
      /** Fill the plots using a LHCb::Particle::ConstVector
       *  @see IPlotTool 
       *  @param particles vector of particles 
       *  @param trailer   the trailer 
       *  @return status code 
       */
      virtual StatusCode fillPlots
      ( const LHCb::Particle::ConstVector& particles ,
        const std::string                  trailer   ) ;
      // ======================================================================
      /** Fill plots using a single Particle
       *  @see IPlotTool 
       *  @param particle  particle 
       *  @param trailer   the trailer 
       *  @return status code 
       */
      virtual StatusCode fillPlots
      ( const LHCb::Particle* particle ,
        const std::string     trailer  ) ;
      // ======================================================================
      /** Define plot directory
       *  @see IPlotTool
       *  @param path new path for the histograms 
       *  @return status code 
       */
      virtual StatusCode setPath ( const std::string& path ) 
      {
        setHistoDir ( path ) ;
        return StatusCode::SUCCESS;  
      }
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler for the histograms 
      void propHandler1 ( Property& /* p */ )  
      { 
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        StatusCode sc = initHistos   () ; 
        Assert ( sc.isSuccess() , "Unable to set 'Histos'" , sc ) ;
      }
      /// the update handler for the selector 
      void propHandler2 ( Property& /* p */ )  
      { 
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        StatusCode sc = initSelector () ;
        Assert ( sc.isSuccess() , "Unable to set 'Selector'" , sc ) ;
      }
      /// the update handler for factory & preambulo
      void propHandler3 ( Property& /* p */ )  
      { 
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        StatusCode sc1 = initHistos   () ; 
        Assert ( sc1.isSuccess() , "Unable to set 'Histos'"   , sc1 ) ;
        StatusCode sc2 = initSelector () ;
        Assert ( sc2.isSuccess() , "Unable to set 'Selector'" , sc2 ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// inititalize the histograms 
      StatusCode initHistos   () ;  // inititalize the histograms
      /// inititalize the selector 
      StatusCode initSelector () ;  // inititalize the selector 
      // ======================================================================
    protected:
      // ======================================================================
      /** the standard consructor 
       *  @param type tool type (?)
       *  @param name tool instance name 
       *  @param parent the parent
       */
      PlotTool
      ( const std::string& type   ,    // tool type (?)
        const std::string& name   ,    // tool instance name 
        const IInterface*  parent )    // the parent
        : GaudiHistoTool ( type , name , parent ) 
        , m_map         () 
        , m_histos      () 
        , m_factory     ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC") 
        , m_selector    ( "ALL" )
        , m_cut         ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( true ) )
        , m_preambulo   () 
      {
        declareInterface <IPlotTool> ( this ) ;
        //
        if      ( 0 == name.find("Hlt1") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find("Hlt2") ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        //
        declareProperty 
          ( "Factory" , 
            m_factory , 
            "Type/Name for C++/Python Hybrid Factory" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PlotTool::propHandler3 , this ) ;
        //
        declareProperty 
          ( "Histos" ,  m_map      , 
            "The map of variables and the histogram desctriptors" ) -> 
          declareUpdateHandler 
          ( &LoKi::Hybrid::PlotTool::propHandler1 , this ) ;
        //
        declareProperty 
          ( "Selector"    , 
            m_selector    , 
            "The selection criteria for the particles" ) -> 
          declareUpdateHandler 
          ( &LoKi::Hybrid::PlotTool::propHandler2 , this ) ;
        //
        declareProperty 
          ( "Preambulo"   , 
            m_preambulo   , 
            "The preambulo to be used for Bender/Python script" ) ->
          declareUpdateHandler 
          ( &LoKi::Hybrid::PlotTool::propHandler3 , this ) ;
        //
        setProperty ( "HistoPrint" , true ).ignore() ;
        //
      }
      /// virtual and protected destructor 
      virtual ~PlotTool(){} ; // virtual and protected destructor  
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PlotTool  () ;  // no default constructor 
      /// the copy  constructor is disabled 
      PlotTool  ( const PlotTool& ) ; // no copy constructor 
      /// the assignement operator is disabled 
      PlotTool& operator=( const PlotTool& ) ; // no assigenement
      // ======================================================================
    public:
      // ======================================================================
      /** book the new histogram using the "base" description and the trailer 
       * 
       *  @attention the actual title of the histogram is constructed 
       *             from the base title, the trailer, the funtor and 
       *             the selector 
       *  
       *  @param h the base description
       *  @param trailer the trailer 
       *  @return fresh booked histogram
       */
      inline AIDA::IHistogram1D* bookHisto 
      ( const Histo&        h  , 
        const std::string&  trailer ) const 
      {
        Gaudi::Histo1DDef hist ( h.m_desc ) ;
        // construct the new title 
        boost::format fmt ( h.m_desc.title() ) ;
        using namespace boost::io ;  
        // allow various number of arguments 
        fmt.exceptions ( all_error_bits ^ ( too_many_args_bit | too_few_args_bit ) ) ;
        // form a few title 
        fmt % trailer % h.m_fun.printOut() % m_selector ;          // ATTENTION!
        hist.setTitle ( fmt.str() ) ;
        
        // book the histogram using the basic tools 
        return this -> book ( hist ) ;        
      }
      // ======================================================================
    protected:
      // ======================================================================
      std::string preambulo() const { return _preambulo ( m_preambulo ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type for var -> histo mapping
      typedef std::map<std::string,Gaudi::Histo1DDef> Map    ;
      typedef std::vector<Histo>                      Histos ;
      /// the storage of the variables and the histogram descriptors
      Map    m_map    ; // the storage of the variables and the histogram descriptors
      /// the actual structure to keep all information: 
      Histos m_histos ;         // the actual structure to keep all information
      /// the type/name for the factory
      std::string          m_factory  ;        // the type/name for the factory
      /// the general selector name 
      std::string          m_selector ;            // the general selector name 
      /// the general selector  
      LoKi::PhysTypes::Cut m_cut ;                      // the general selector  
      /// the preambulo 
      std::vector<std::string> m_preambulo ;                   // the preambulo 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// initialize the histograms 
// ============================================================================
StatusCode LoKi::Hybrid::PlotTool::initHistos () 
{
  // clear the existing histos
  m_histos.clear() ;
  
  // locate the factory
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  
  StatusCode sc = StatusCode ( StatusCode::SUCCESS , true ) ;
  // set the histograms 
  for ( Map::const_iterator item = m_map.begin() ; m_map.end() != item ; ++item ) 
  {
    Histo histo ;
    histo.m_desc = item->second ;
    sc = factory->get ( item->first , histo.m_fun , preambulo() ) ;
    if ( sc.isFailure() ) 
    { return Error 
        ( "Unable to decode the function: \"" + item->first + "\"" , sc ) ; }
    m_histos.push_back ( histo ) ;  
  }
  //
  release ( factory ) ;
  
  return sc ;
}
// ============================================================================
// initialize the selector 
// ============================================================================
StatusCode LoKi::Hybrid::PlotTool::initSelector () 
{
  // locate the factory
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  
  StatusCode sc = factory->get ( m_selector , m_cut ) ;
  
  if ( sc.isFailure() )
  { return Error 
      ( "Unable to decode the selector: \"" + m_selector + "\"" , sc ) ;}
  //
  release ( factory ) ;
  
  return StatusCode::SUCCESS ; 
}
// ============================================================================
/*  Fill plots using a single Particle
 *  @see IPlotTool 
 *  @param particles vector of particles 
 *  @param trailer   the trailer 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Hybrid::PlotTool::fillPlots
( const LHCb::Particle* particle ,
  const std::string     trailer  ) 
{
  if ( 0 == particle ) 
  { Warning ( "fillPlots(): LHCb::Particle* points to NULL, proceed" ) ; }
  // select the particle? 
  if ( !m_cut ( particle ) ) { return StatusCode::SUCCESS ; }
  // loop over all booked histograms 
  for ( Histos::iterator ihisto = m_histos.begin() ; 
        m_histos.end() != ihisto ; ++ihisto ) 
  {
    // get the histogram
    Histo::HMap&  hmap = ihisto->m_histos ;
    AIDA::IHistogram1D* h = hmap[trailer] ;
    // book it if not done yet 
    if ( 0 == h ) 
    {
      h = bookHisto ( *ihisto , trailer ) ;
      hmap.erase  ( trailer     ) ;
      hmap.insert ( trailer , h ) ;
    }
    // fill the histogram
    fill 
      ( h                          ,   // the histogram 
        ihisto->m_fun ( particle ) ,   // the value 
        1.0                        ,   // the weight 
        ihisto->m_desc.title()     ) ; // the title 
  } // end of the loop over the histograms 
  return StatusCode::SUCCESS ;                              // RETURN 
}
// ============================================================================
/*  Fill the plots using a LHCb::Particle::ConstVector
 *  @see IPlotTool 
 *  @param particles vector of particles 
 *  @param trailer   the trailer 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Hybrid::PlotTool::fillPlots
( const LHCb::Particle::ConstVector& particles ,
  const std::string                  trailer   ) 
{
  // loop over all booked histograms 
  for ( Histos::iterator ihisto = m_histos.begin() ; 
        m_histos.end() != ihisto ; ++ihisto ) 
  {
    // get the histogram
    Histo::HMap&  hmap = ihisto->m_histos ;
    AIDA::IHistogram1D* h = hmap[trailer] ;
    // book it if not done yet 
    if ( 0 == h ) 
    {
      h = bookHisto ( *ihisto , trailer ) ;
      hmap.erase  ( trailer     ) ;
      hmap.insert ( trailer , h ) ;
    }
    if ( 0 == h ) { continue ; }                // CONITNUE 
    // loop over the particles 
    for ( LHCb::Particle::ConstVector::const_iterator ip = 
            particles.begin() ; particles.end() != ip ; ++ip ) 
    {
      // select the particle? 
      if ( !m_cut ( *ip ) ) { continue ; }    // CONTINUE 
      // fill the histogram
      fill 
        ( h                          ,       // the histogram 
          ihisto->m_fun ( *ip )      ,       // the value 
          1.0                        ,       // the weight 
          ihisto->m_desc.title()     ) ;     // the title 
    } //                                     end of the loop over the particles 
  } //                                      end of the loop over the histograms 
  return StatusCode::SUCCESS ;                                        // RETURN 
}
// ============================================================================
/// Declare the mandatory factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,PlotTool)
// ============================================================================
// The END 
// ============================================================================
  
