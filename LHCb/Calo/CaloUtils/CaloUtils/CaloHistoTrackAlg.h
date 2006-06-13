// $Id: CaloHistoTrackAlg.h,v 1.5 2006-06-13 14:13:52 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/06/06 11:59:52  ibelyaev
//  new base classes for PIDs & rewritten TrackUse
//
// ============================================================================
#ifndef CALOUTILS_CALOHistoTrackAlg_H 
#define CALOUTILS_CALOHistoTrackAlg_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h"
// ============================================================================
// Track Event 
// ============================================================================
#include "Event/TrackUse.h"
// ============================================================================

// ============================================================================
/** @class CaloHistoTrackAlg CaloHistoTrackAlg.h
 *  
 *  Helper base algorithm form implementation 'track'-related algorithms
 *
 *  It helps to select the appropriate tracks for further processing
 *  Track is 'selected' if it fullfills 
 *
 *   - general features
 *   - category 
 *   - "algorithm" 
 *
 *  @see TrackUse 
 *
 *
 *  @see CaloHistoAlg
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-10-26
 */
// ============================================================================
class CaloHistoTrackAlg : public GaudiHistoAlg 
{
public:  
  /// standard algorithm initialization 
  virtual StatusCode initialize(); 
  /// standard algorithm finalization 
  virtual StatusCode finalize  ();
protected:
  /// Standard constructor
  CaloHistoTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  /// destructor (virtual and protected)
  virtual ~CaloHistoTrackAlg();
private:
  /// default  construstor  is  private 
  CaloHistoTrackAlg(); 
  /// copy     construstor  is  private 
  CaloHistoTrackAlg
  ( const CaloHistoTrackAlg& );
  /// assignement operator  is  private 
  CaloHistoTrackAlg& operator=
  ( const CaloHistoTrackAlg& );
protected:
  /// check if the track to be used @see TrackUse 
  inline bool use ( const LHCb::Track* track ) const { return m_use ( track ) ; }
  /// print the short infomration about track flags 
  inline MsgStream& print
  ( MsgStream&         stream , 
    const LHCb::Track* track  ) const ;
  /// print the short infomration about track flags 
  inline MsgStream& print 
  ( const LHCb::Track* track             , 
    const MSG::Level   level = MSG::INFO ) const ;
protected :
  // the actual object 
  TrackUse m_use ;
};
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
CaloHistoTrackAlg::print ( MsgStream&         stream , 
                           const LHCb::Track* track  ) const 
{ return stream.isActive() ? m_use.print ( stream , track ) : stream ; }
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
CaloHistoTrackAlg::print ( const LHCb::Track* track  , 
                           const MSG::Level   level  ) const 
{ return print ( msgStream ( level ) , track ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOHistoTrackAlg_H
// ============================================================================
