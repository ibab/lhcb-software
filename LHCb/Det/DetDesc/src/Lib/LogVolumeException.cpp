// $Id: LogVolumeException.cpp,v 1.3 2003-01-17 14:03:38 sponce Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/18 15:48:50  ibelyaev
//  bug fix for Win2k
//
// Revision 1.1  2001/11/18 15:32:45  ibelyaev
//  update for Logical Assemblies
// 
// ============================================================================
// Include files
// local
#include "DetDesc/LogVolumeException.h"
#include "DetDesc/ILVolume.h"

// ============================================================================
/** @file LogVolumeException.cpp
 *
 *  Implementation file for class : LogVolumeException
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 18/11/2001 
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @param name   exception name 
 *  @param logvol pointer to ILVolume interface 
 *  @param sc     status code
 */
// ============================================================================
LogVolumeException::LogVolumeException
( const std::string&    name   ,
  const ILVolume*       logvol ,
  const StatusCode&     sc     )
  : GaudiException( name , "*LogVolumeException*" , sc )
  , m_logvol ( logvol )
{};

// ============================================================================
/** standard constructor 
 *  @param name   exception name 
 *  @param excp   "previous" exception in the linked list
 *  @param logvol pointer to ILVolume interface 
 *  @param sc     status code
 */
// ============================================================================
LogVolumeException::LogVolumeException
( const std::string&    name   ,
  const GaudiException& excp   ,
  const ILVolume*       logvol ,
  const StatusCode&     sc     )
  : GaudiException( name , "*LogVolumeException*" , sc , excp )
  , m_logvol ( logvol )
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
LogVolumeException::~LogVolumeException() throw()
{ m_logvol = 0 ; };

// ============================================================================
/** printout to standard STD/STL stream 
 *  @param os reference to the stream 
 *  @return reference to the stream 
 */  
// ============================================================================
std::ostream& LogVolumeException::printOut
( std::ostream& os             ) const 
{
  /// print THIS exception 
  os << " \t" << tag() << " \t " << message() ; 
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    }
  os << std::endl ;
  /// print the logvol 
  os << " Exception logvol: " 
     << m_logvol 
     << std::endl  ; 
  /// print all previous (linked) exceptions     
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ;
};

// ============================================================================
/** printout to standard Message stream 
 *  @param os reference to the stream 
 *  @return reference to the stream 
 */  
// ============================================================================
MsgStream&    LogVolumeException::printOut
( MsgStream&    os             ) const 
{
  /// print THIS exception 
  os << " " << tag() << " \t " << message() ; 
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    } 
  os << endreq ;
  /// print the logvol 
  os << " Exception logvol: " 
     << m_logvol 
     << endreq  ; 
  /// print all previous (linked) exceptions     
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ;
};

// ============================================================================
/** clone method ("virtual constructor")
 *  @return pointer to newly created object
 */
// ============================================================================
GaudiException* LogVolumeException::clone() const 
{ return new LogVolumeException( *this ); }

// ============================================================================
// The End 
// ============================================================================
