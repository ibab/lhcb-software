// $Id: LogVolumeException.h,v 1.3 2003-01-17 14:03:38 sponce Exp $
// ============================================================================
// CVS tag $Name:
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/18 15:48:50  ibelyaev
//  bug fix for Win2k
//
// Revision 1.1  2001/11/18 15:32:44  ibelyaev
//  update for Logical Assemblies
// 
// ============================================================================
#ifndef DETDESC_LOGVOLUMEEXCEPTION_H 
#define DETDESC_LOGVOLUMEEXCEPTION_H 1
// Include files
#include "GaudiKernel/GaudiException.h"
// forward declarations
class ILVolume ; ///< detdesc package 

/** @class LogVolumeException LogVolumeException.h DetDesc/LogVolumeException.h
 *  
 *  The exception class used for any implementation of ILVolume interface
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   18/11/2001
 */


class LogVolumeException :
  public GaudiException
{
  
public:
  
  /** standard constructor 
   *  @param name   exception name 
   *  @param logvol pointer to ILVolume interface 
   *  @param sc     status code
   */
  LogVolumeException
  ( const std::string&    name                         ,
    const ILVolume*       logvol = 0                   ,
    const StatusCode&     sc     = StatusCode::FAILURE );
  
  /** standard constructor 
   *  @param name   exception name 
   *  @param excp   "previous" exception in the linked list
   *  @param logvol pointer to ILVolume interface 
   *  @param sc     status code
   */
  LogVolumeException
  ( const std::string&    name                         ,
    const GaudiException& excp                         ,
    const ILVolume*       logvol = 0                   ,
    const StatusCode&     sc     = StatusCode::FAILURE );
  
  /** destructor 
   */
  virtual ~LogVolumeException() throw();
  
  /** printout to standard STD/STL stream 
   *  @param os reference to the stream 
   *  @return reference to the stream 
   */  
  virtual std::ostream&     printOut
  ( std::ostream& os = std::cerr ) const ; 
  
  /** printout to standard Message stream 
   *  @param os reference to the stream 
   *  @return reference to the stream 
   */  
  virtual MsgStream&        printOut
  ( MsgStream&    os             ) const ;
  
  /** clone method ("virtual constructor")
   *  @return pointer to newly created object
   */
  virtual GaudiException* clone() const ;
  
private:
  
  /** default constructor is private!
   */
  LogVolumeException();
  
private:
  
  const ILVolume* m_logvol ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // DETDESC_LOGVOLUMEEXCEPTION_H
// ============================================================================
