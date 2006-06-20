// $Id: TrackUse.h,v 1.2 2006-06-20 20:00:43 erodrigu Exp $
// ============================================================================
#ifndef EVENT_TRACKUSE_H 
#define EVENT_TRACKUSE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// Event model
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// forward declarations 
// ============================================================================
class Algorithm ;
class Service   ;
class AlgTool   ;
// ============================================================================

/** @class TrackUse TrackUse.h Event/TrackUse.h
 *
 *  the helper class to feed the algorithms/services/tools with 
 *  proper (and uniform) treatment of "Track properties" for 
 *  selection (or) rejection of tracks 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-10-27
 */
class TrackUse 
  : public std::unary_function<const LHCb::Tracks,bool>
{
public: 
  typedef std::vector<int> Types ;
public:
  /** declare 'own' properties for the algorithm 
   * 
   *  @code 
   *
   *  // algorithm contructor 
   *  MyAlg::MyAlg  ( const std::string& name , 
   *                  ISvcLocator*       svc  )
   *   : Algorithm  ( name , svc ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param alg algorithm 
   *  @return status code 
   */
   StatusCode declareProperties ( Algorithm* alg ) ;
  /** declare 'own' properties for the service 
   * 
   *
   *  @code 
   *
   *  // service contructor 
   *  MySvc::MySvc  ( const std::string& name  , 
   *                  ISvcLocator*       isvc  )
   *   : Service    ( name , isvc ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param svc service 
   *  @return status code 
   */
  StatusCode declareProperties ( Service*  svc ) ;
  /** declare 'own' properties for the tool 
   * 
   *
   *  @code 
   *
   *  // tool contructor 
   *  MyTool::MyTool  ( const std::string& type , 
   *                    const std::string& name , 
   *                    const IInterface*  parent  )
   *   : AlgTool      ( type , name , parent ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param tool tool
   *  @return status code 
   */
  StatusCode declareProperties ( AlgTool*  tool ) ;  
  /** the basic method which defines if track selected for further processing
   *  @param track pointer to Track object 
   *  @return decsion
   */
  inline bool use  ( const LHCb::Track* track ) const ;  
  /** the main method (STL functor interface) 
   *  @param track pointer to Track object 
   *  @return decsion
   */  
  inline bool operator() ( const LHCb::Track* track ) const
  { return use ( track ) ; }
public:
  //
  inline bool check        () const  { return m_check ; }
  inline bool skipClones   () const  { return m_skipClones   ; }
  inline bool skipInvalid  () const  { return m_skipInvalid  ; }
  inline bool skipBackward () const  { return m_skipBackward ; }
  /// get the list of accepted fit status
  size_t acceptedFitStatus ( std::vector<LHCb::Track::FitStatus>&  s ) const ;
  /// get the list of accepted types  
  size_t acceptedType      ( std::vector<LHCb::Track::Types>&      t ) const ;
  /// get the list of rejected history
  size_t rejectedHistory   ( std::vector<LHCb::Track::History>&    h ) const ;
public:
  inline bool acceptedFitStatus ( const LHCb::Track::FitStatus v ) const ;
  inline bool acceptedType      ( const LHCb::Track::Types     v ) const ;
  inline bool rejectedHistory   ( const LHCb::Track::History   v ) const ;
public:
  /// printout to MsgStream 
  MsgStream&    fillStream ( MsgStream&    stream ) const ;
  /// printout of the track into the staream 
  MsgStream&    print      ( MsgStream&         stream , 
                             const LHCb::Track* track  ) const ;
protected:
  /** the basic method for delegation of properties 
   *  @param object property manager 
   *  @return statsu code 
   */
  template <class TYPE>
  inline StatusCode i_declareProperties( TYPE* object ) ;
public:
  /// Standard constructor
  TrackUse( );  
  /// destructor 
  virtual ~TrackUse() ;
private:
  // check the track
  bool               m_check        ;
  // reject clones 
  bool               m_skipClones   ;  
  // reject invalid 
  bool               m_skipInvalid  ;
  // reject backward 
  bool               m_skipBackward ;
  //
  typedef std::vector<int> Shorts ;
  /// accepted fit status 
  Shorts m_fitstatus  ;
  /// accepted type 
  Shorts m_type    ;
  /// rejected history 
  Shorts m_history ;
};
// ============================================================================
/// status to be accepted
// ============================================================================
inline bool TrackUse::acceptedFitStatus ( const LHCb::Track::FitStatus v ) const 
{
  return 
    m_fitstatus.end() != std::find( m_fitstatus.begin() , m_fitstatus.end() , v ) ;  
} ;
// ============================================================================
/// type to be accepted
// ============================================================================
inline bool TrackUse::acceptedType    ( const LHCb::Track::Types  v ) const 
{
  return 
    m_type.end() != std::find( m_type.begin() , m_type.end() , v ) ;  
} ;
// ============================================================================
/// history to be rejected 
// ============================================================================
inline bool TrackUse::rejectedHistory ( const LHCb::Track::History v ) const 
{
  return 
    m_history.end() != std::find( m_history.begin() , m_history.end() , v ) ;  
} ;
// ============================================================================
/** the basic method which defines if track selected for further processing
 *  @param track pointer to Track object 
 *  @return decision
 */
// ============================================================================
inline bool
TrackUse::use  ( const LHCb::Track* track ) const 
{
  if ( 0 == track ) { return false ; }                              // RETURN 
  if ( !check()   ) { return true  ; }                              // RETURN 
  /// check for flags 
  if ( skipClones  ()  && 
       track->checkFlag ( LHCb::Track::Clone    ) )   { return false ; }
  /// check for flags 
  if ( skipInvalid ()  && 
       track->checkFlag ( LHCb::Track::Invalid  ) )   { return false ; }
  /// check for flags 
  if ( skipBackward()  && 
       track->checkFlag ( LHCb::Track::Backward ) )   { return false ; }
  /// accepted fit status ?
  if ( !acceptedFitStatus ( track -> fitStatus() )  ) { return false ; }
  /// accepted type   ?
  if ( !acceptedType    ( track -> type    () )   )   { return false ; }
  /// rejected history ?
  if (  rejectedHistory ( track -> history () )   )   { return false ; }
  // end! 
  return true ;
} ;
// ============================================================================
inline MsgStream&    
operator<<( MsgStream&      stream , 
            const TrackUse& trUse  ) 
{ return trUse.fillStream ( stream ) ; }
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // EVENT_TRACKUSE_H
// ============================================================================
