// $Id: RelationsClonerAlg.h,v 1.8 2009-04-30 12:58:55 jpalac Exp $
#ifndef MICRODST_RELATIONSCLONERALG_H 
#define MICRODST_RELATIONSCLONERALG_H 1

// Include files
#include <boost/function.hpp>
#include <boost/bind.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"
#include "MicroDST/RelTableFunctors.h"
// from Boost
#include <boost/type_traits/remove_pointer.hpp>
/** @class RelationsClonerAlg RelationsClonerAlg.h MicroDST/RelationsClonerAlg.h
 *
 *  Templated algorithm to clone a relations table between Froms and Tos. The
 *  Tos are also cloned.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2009-04-14
 */

namespace MicroDST 
{

  template <typename TABLE>
  class RelationsClonerAlg : public MicroDSTAlgorithm {

  private:

    typedef Defaults<TABLE> DEFAULTS;
    typedef Location<TABLE> LOCATION;
    typedef typename BindType2Cloner<TABLE>::toCloner CLONER;
    typedef MicroDST::TableCloner<TABLE> TableCloner;

  public:
    //===========================================================================
    /// Standard constructor
    RelationsClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      :
      MicroDSTAlgorithm ( name , pSvcLocator ),
      m_cloner(0),
      m_clonerType(DEFAULTS::clonerType),
      m_tableCloner(boost::bind(&RelationsClonerAlg<TABLE>::cloneFrom, &(*this), _1),
                    boost::bind(&RelationsClonerAlg<TABLE>::cloneTo, &(*this), _1))               
    {
      declareProperty("ClonerType", m_clonerType);
    }
    //===========================================================================
    ~RelationsClonerAlg( ) {} ///< Destructor
    //===========================================================================
    StatusCode initialize() 
    {
      StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

      debug() << "==> Initialize" << endmsg;

      if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

      if (inputTESLocation()=="")  {
        verbose() << "changing input TES location to " 
                  << LOCATION::Default << endmsg;
        setInputTESLocation(LOCATION::Default);
      }
      verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      m_cloner = tool<CLONER>(m_clonerType, this);

      if (m_cloner) {
        debug() << "Found cloner " << m_clonerType << endmsg;
      } else {
        sc = Error("Failed to find cloner " + m_clonerType,
                   StatusCode::FAILURE, 10);
      }

      return sc;

    }
    //===========================================================================
    StatusCode execute() 
    {

      debug() << "==> Execute" << endmsg;
      verbose() << "Going to clone relations from " 
                << inputTESLocation()
                << " into " << fullOutputTESLocation() << endmsg;

      setFilterPassed(true);

      if (exist<TABLE>(inputTESLocation()) )
      {
        verbose() << "Retrieving relations table from " 
                  << inputTESLocation() << endmsg;
        const TABLE* table = get<TABLE>(inputTESLocation());
        if (table) {
          verbose() << "found table!" << endmsg;
          TABLE* cloneTable = m_tableCloner(table);
          verbose() << "Going to store relations table from " 
                    << inputTESLocation()
                    << " into " << fullOutputTESLocation() << endmsg;
          put( cloneTable, fullOutputTESLocation() );
          return StatusCode::SUCCESS;
        }
        return StatusCode::FAILURE;
      } else {
        verbose() << "Found no table at " << inputTESLocation() << ". storing empty table"<< endmsg;
        TABLE* cloneTable = new TABLE();
        put( cloneTable, fullOutputTESLocation() );
        return StatusCode::SUCCESS;
      }

      return StatusCode::SUCCESS;

    }
    //===========================================================================
    StatusCode finalize() 
    {
      debug() << "==> Finalize" << endmsg;
      return MicroDSTAlgorithm::finalize();
    }
    //===========================================================================

  private:

    typedef typename BindType2Cloner<TABLE>::toType TO_TYPE;
    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;


  private:
    inline const CLONER* cloner() { return m_cloner; }
    inline const typename TABLE::From cloneFrom(const typename TABLE::From from) 
    {
      return getStoredClone< _From >(from);
    }
    inline const typename TABLE::To cloneTo(const typename TABLE::To to) 
    {
      const typename TABLE::To storedTo = getStoredClone< TO_TYPE >(to);
      return (0!=storedTo) ? storedTo : (*m_cloner)( to );
    }
  
    //===========================================================================
  
  private:

    CLONER* m_cloner;
    std::string m_clonerType;
    TableCloner m_tableCloner;
    
  };



 
}

#endif // MICRODST_RELATIONSCLONERALG_H
