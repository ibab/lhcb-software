
//----------------------------------------------------------------------------
/** @file DeRichBase.h
 *
 *  Header file for detector description class : DeRichBase
 *
 *  @author Chris Jones    jonesc@hep.phy.cam.ac.uk
 *  @date   26-07-2009
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHBASE_H
#define RICHDET_DERICHBASE_H 1

// STL
#include <memory>

// Gaudi
#include "GaudiKernel/MsgStream.h"

// DetDesc
#include "DetDesc/DetectorElement.h"

// RichDet
#include "RichDet/DeRichLocations.h"

class DeRichSystem;

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
  DeRichBase( const std::string & name = "" ) : DetectorElement ( name ) { }

  /// Destructor
  virtual ~DeRichBase( ) { } 

protected:

  /// Set the name for this detector element
  inline void setMyName ( const std::string & name ) const
  {
    m_myname = name;
    // force a new MsgStream object using this new name
    m_msgStream.reset( nullptr );
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

  /** Create a MsgStream object on the fly, when first needed
   *
   *  @return Reference to the predefined stream
   */
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream.reset( new MsgStream( msgSvc(), myName() ) );
    return *m_msgStream.get();
  }

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
    return msgStream() << level ;
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

  /// Return the current message level
  inline MSG::Level msgLevel() const 
  {
    return msgStream().level(); 
  }

  /// Test the message level for the cached MsgStream object
  inline bool msgLevel( const MSG::Level level ) const 
  {
    return UNLIKELY( msgLevel() <= level ); 
  }

  /// Test the message level for a given MsgStream object
  inline bool msgLevel( const MSG::Level level,
                        MsgStream & msgStream ) const 
  {
    return UNLIKELY( msgStream.level() <= level ); 
  }

protected:

  /// Access DeRichSystem on demand
  DeRichSystem * deRichSys() const;

private:

  mutable std::string m_myname;    ///< The name of this detector element
  mutable std::unique_ptr<MsgStream> m_msgStream; ///< Message Stream Object
  mutable DeRichSystem* m_deRichS = nullptr; ///< Pointer to the overall RICH system object

};

#endif // RICHDET_DERICHBASE_H
