#ifndef MESSAGINGBASE_H 
#define MESSAGINGBASE_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

namespace LoKi
{

  /** @class MessagingBase MessagingBase.h
   *
   *  Simple base class adding methods for messageing with context.
   *
   *  @author Chris Jones
   *  @date   2014-10-21
   */

  class MessagingBase : public GaudiTool
  {

  public:

    /// Standard constructor
    MessagingBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

    /// Destructor
    virtual ~MessagingBase( );

    /// initialize the tool 
    virtual StatusCode initialize () ;     

  protected:

    // ========================================================================

    StatusCode _Warning
    ( const std::string& msg                                             ,
      const StatusCode&  code   = StatusCode( StatusCode::FAILURE, true ),
      const unsigned int prints = 2 ) const;

    // ========================================================================

    StatusCode _Error
    ( const std::string& msg                                             ,
      const StatusCode&  code   = StatusCode( StatusCode::FAILURE, true ),
      const unsigned int prints = 2 ) const;

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

    /// get the actual algorithm name context 
    mutable std::string m_myAlg       ;

    /// Option to enable printing of the algorithm name context
    bool                m_printMyAlg  ;

    /// # of prints 
    unsigned int        m_prints ;

  };

}

#endif // MESSAGINGBASE_H
