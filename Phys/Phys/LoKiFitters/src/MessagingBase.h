// $Id:$
// ============================================================================
#ifndef MESSAGINGBASE_H 
#define MESSAGINGBASE_H 1
// ============================================================================
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class MessagingBase MessagingBase.h
   *
   *  Simple base class adding methods for messageing with context.
   *
   *  @author Chris Jones
   *  @date   2014-10-21
   */
  class MessagingBase : public GaudiTool
  {
    // ========================================================================    
  public:
    // ========================================================================
    /// Standard constructor
    MessagingBase
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent );
    // ========================================================================    
  protected:
    // ========================================================================
    /// Destructor
    virtual ~MessagingBase() ;
    // ========================================================================
  public: 
    // ========================================================================
    /// initialize the tool 
    virtual StatusCode initialize () ;     
    /// finalize the tool 
    virtual StatusCode finalize   () ;     
    // ========================================================================
  protected:
    // ========================================================================
    StatusCode _Warning
    ( const std::string& msg                                              ,
      const StatusCode&  code   = StatusCode( StatusCode::FAILURE, true ) ,
      const unsigned int prints = 2                                     ) const ;
    // ========================================================================
    StatusCode _Error
    ( const std::string& msg                                             ,
      const StatusCode&  code   = StatusCode( StatusCode::FAILURE, true ),
      const unsigned int prints = 2                                     ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// Access the current algorithm context name
    const std::string& myAlg() const { return m_myAlg ; }
    // ========================================================================
    /// get the correct algorithm context 
    bool getMyAlg () const ;
    // ========================================================================
  private:
    // ========================================================================    
    /// get the actual algorithm name context 
    mutable std::string m_myAlg       ;
    // ========================================================================
    /// Option to enable printing of the algorithm name context
    bool                m_printMyAlg  ;
    // ========================================================================    
    /// # of prints 
    unsigned int        m_prints  ;
    // ========================================================================    
  };
  // ==========================================================================  
}  //                                                     end of namespace LoKi
// ============================================================================
// Warning message
// ============================================================================
inline StatusCode
LoKi::MessagingBase::_Warning
( const std::string& msg,
  const StatusCode&  code,
  const unsigned int prints ) const
{
  // ==========================================================================
  code.setChecked () ;
  //
  if   ( errorsPrint() && m_printMyAlg  ) { getMyAlg      () ; }
  else                                    { m_myAlg.clear () ; }
  //
  if       ( m_printMyAlg && errorsPrint() )
  { return Warning ( msg + m_myAlg , code , std::min(prints,m_prints) ) ; }
  else if  ( errorsPrint() )
  { return Warning ( msg           , code , std::min(prints,m_prints) ) ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { warning ()
      << "'"       << msg  << "' " << m_myAlg
      << " Code =" << code << endmsg ; }
  //
  return code ;
  // ==========================================================================
}
// ============================================================================
// Error message
// ===========================================================================
inline StatusCode
LoKi::MessagingBase::_Error
( const std::string& msg,
  const StatusCode&  code,
  const unsigned int prints ) const
{
  // ==========================================================================
  code.setChecked () ;
  //
  if   ( errorsPrint() && m_printMyAlg  ) { getMyAlg      () ; }
  else                                    { m_myAlg.clear () ; }
  //
  if       ( m_printMyAlg && errorsPrint() )
  { return Error   ( msg + m_myAlg , code , std::min(prints,m_prints) ) ; }
  else if  ( errorsPrint() )
  { return Error   ( msg           , code , std::min(prints,m_prints) ) ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { error ()
      << "'"       << msg  << "' " << m_myAlg
      << " Code =" << code << endmsg ; }
  //
  return code ;
  // ==========================================================================
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // MESSAGINGBASE_H
// ============================================================================
