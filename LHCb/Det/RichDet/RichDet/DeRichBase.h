
//----------------------------------------------------------------------------
/** @file DeRichBase.h
 *
 *  Header file for detector description class : DeRichBase
 *
 *  CVS Log :-
 *  $Id: DeRichBase.h,v 1.1 2009-07-26 18:13:17 jonrob Exp $
 *
 *  @author Chris Jones    jonesc@hep.phy.cam.ac.uk
 *  @date   26-07-2009
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHBASE_H
#define RICHDET_DERICHBASE_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// DetDesc
#include "DetDesc/DetectorElement.h"

// RichDet
#include "RichDet/DeRichLocations.h"

/** @class DeRichBase RichDet/DeRichBase.h
 *
 *  Base class for all Rich DetectorElements
 *
 *  @author Chris Jones
 *  @date   2009-07-26
 */
class DeRichBase : public DetectorElement
{

public:

  /// Standard constructor
  DeRichBase( const std::string & name = "" ) 
    : DetectorElement ( name ),
      m_msgStream     ( NULL )  { }

  /// Destructor
  virtual ~DeRichBase( ) { delete m_msgStream; } 

protected:

  /// Set the name for this detector element
  inline void setMyName ( const std::string & name ) const
  {
    m_myname = name;
    // force a new MsgStream object using this new name
    delete m_msgStream;
    m_msgStream = NULL;
  }

  /** Returns the name of this particular Detector Element
   *  @return name
   */
  inline const std::string & myName() const 
  { 
    if ( m_myname.empty() ) { setMyName( name() ); }
    return m_myname; 
  }

protected:

  /** Predefined configurable message stream for the efficient printouts
   *
   *  @code
   *
   *  if ( a < 0 ) { msgStream( MSG::ERROR ) << "a = " << endmsg ; }
   *
   *  @endcode
   *
   *  @return Reference to the predefined stream
   */
  inline MsgStream & msgStream( const MSG::Level level ) const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), myName() );
    return *m_msgStream << level ;
  }

  /// shortcut for the method msgStream ( MSG::ALWAYS )
  inline MsgStream&  always () const { return msgStream ( MSG::ALWAYS ) ; }
  /// shortcut for the method msgStream ( MSG::FATAL   )
  inline MsgStream&   fatal () const { return msgStream ( MSG::FATAL ) ; }
  /// shortcut for the method msgStream ( MSG::ERROR   )
  inline MsgStream&   error () const { return msgStream ( MSG::ERROR ) ; }
  /// shortcut for the method msgStream ( MSG::WARNING )
  inline MsgStream& warning () const { return msgStream ( MSG::WARNING ) ; }
  /// shortcut for the method msgStream ( MSG::INFO    )
  inline MsgStream&    info () const { return msgStream ( MSG::INFO ) ; }
  /// shortcut for the method msgStream ( MSG::DEBUG   )
  inline MsgStream&   debug () const { return msgStream ( MSG::DEBUG ) ; }
  /// shortcut for the method msgStream ( MSG::VERBOSE )
  inline MsgStream& verbose () const { return msgStream ( MSG::VERBOSE ) ; }

  /// Temporary Message Stream object with a custom name
  MsgStream msgStream( const std::string & name ) const
  {
    return MsgStream( msgSvc(), name );
  }

private:

  mutable std::string m_myname;    ///< The name of this detector element
  mutable MsgStream * m_msgStream; ///< Message Stream Object

};

#endif // RICHDET_DERICHBASE_H
