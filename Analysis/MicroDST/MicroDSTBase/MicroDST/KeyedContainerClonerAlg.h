// $Id: KeyedContainerClonerAlg.h,v 1.9 2010-09-07 10:25:55 jpalac Exp $
#ifndef MICRODST_KEYEDCONTAINERCLONERALG_H
#define MICRODST_KEYEDCONTAINERCLONERALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"

namespace MicroDST
{

  /** @class KeyedContainerClonerAlg KeyedContainerClonerAlg.h MicroDST/KeyedContainerClonerAlg.h
   *
   *  Clones objects in Keyed Containers
   *  
   *  @author Juan PALACIOS
   *  @date   2008-08-29
   */

  template <typename T>
  class KeyedContainerClonerAlg : public MicroDSTAlgorithm
  {

  private:

    typedef Defaults<T> DEFAULTS;
    typedef Location<T> LOCATION;
    typedef typename BindType2Cloner<T>::Cloner CLONER;

  public:

    /// Standard constructor
    KeyedContainerClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      :
      MicroDSTAlgorithm ( name , pSvcLocator   ),
      m_cloner          ( NULL                 ),
      m_clonerType      ( DEFAULTS::clonerType )
    {
      declareProperty( "ClonerType", m_clonerType );
    }

    ~KeyedContainerClonerAlg( ) {} ///< Destructor

    StatusCode initialize()
    {
      StatusCode sc = MicroDSTAlgorithm::initialize();
      if ( sc.isFailure() ) return sc;

      if ( inputTESLocation().empty() )
      {
        setInputTESLocation(LOCATION::Default);
      }
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      m_cloner = tool<CLONER>(m_clonerType, this);

      if ( m_cloner )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Found cloner " << m_clonerType << endmsg;
      }
      else
      {
        sc = Error( "Failed to find cloner " + m_clonerType );
      }

      return sc;
    }

    StatusCode execute()
    {
      for ( std::vector<std::string>::const_iterator iLoc = this->inputTESLocations().begin();
            iLoc != this->inputTESLocations().end(); ++iLoc )
      {
        copyKeyedContainer<CLONER>( niceLocationName(*iLoc), m_cloner );
      }
      
      setFilterPassed(true);
      
      return StatusCode::SUCCESS;
    }

  private:

    CLONER* m_cloner;
    std::string m_clonerType;

  };

}

#endif // MICRODST_KEYEDCONTAINERCLONERALG_H
