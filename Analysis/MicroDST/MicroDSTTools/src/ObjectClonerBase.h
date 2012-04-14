// $Id: $
#ifndef MICRODST_OBJECTCLONERBASE_H 
#define MICRODST_OBJECTCLONERBASE_H 1

#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/Functors.hpp>

/** @class ObjectClonerBase ObjectClonerBase.h
 *  
 *  Base class for cloning tools
 *  
 *  @author Chris Jones
 *  @date   2012-03-30
 */
class ObjectClonerBase : public MicroDSTTool
{

public: 

  /// Standard constructor
  ObjectClonerBase( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~ObjectClonerBase( ); ///< Destructor

protected:

  /// Check to see if a given object should be cloned, or is VETO'ed
  template<class TYPE>
  bool isVetoed( const TYPE * obj ) const
  {
    const bool veto =
      ( obj && obj->parent() &&
        !m_tesVetoList.empty() &&
        m_tesVetoList.end() != std::find( m_tesVetoList.begin(),
                                          m_tesVetoList.end(),
                                          obj->parent()->registry()->identifier() ) );
    if ( veto )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Object in " << obj->parent()->registry()->identifier()
                << " is VETO'ed from cloning. Returning original pointer" << endmsg;
    }
    return veto;
  }

private:

  std::vector<std::string> m_tesVetoList;

};

#endif // MICRODST_OBJECTCLONERBASE_H
