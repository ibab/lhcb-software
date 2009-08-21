// $Id: RelationsClonerAlg.h,v 1.13 2009-08-21 15:50:35 jpalac Exp $
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
 *  Templated algorithm to clone a relations table between Froms and Tos. The Froms
 *  are assumed to be cloner already. The Tos are cloned, unless the ClonerType 
 *  property is set to "NONE", in which case the cloner table points to the original 
 *  Tos.
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

      if ( inputTESLocation()=="" )  {
        verbose() << "Setting input TES location to default: " 
                  << LOCATION::Default << endmsg;
        setInputTESLocation(LOCATION::Default);
      }
      verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      if (m_clonerType=="NONE") {
        sc = this->Warning("ClonerType set to NONE. No cloning of To performed.", 
                           StatusCode::SUCCESS);
      } else {
        m_cloner = tool<CLONER>(m_clonerType, this);
        if (m_cloner) {
          debug() << "Found cloner " << m_clonerType << endmsg;
          sc = StatusCode::SUCCESS;
        } else {
          sc = Error("Failed to find cloner " + m_clonerType,
                     StatusCode::FAILURE);
        }
      }

      return sc;

    }
    //===========================================================================
    StatusCode execute() 
    {

      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

      const std::string outputLocation = 
        this->outputTESLocation( this->inputTESLocation() );

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Going to clone relations from " 
                  << inputTESLocation()
                  << " into " << outputLocation << endmsg;
      }
      
      setFilterPassed(true);

      if (exist<TABLE>(outputLocation) )
      {
        return this->Warning("Object "+ outputLocation + " already exists. Not cloning.", 
                             StatusCode::SUCCESS,10) ;
      }
          

      if (exist<TABLE>(inputTESLocation()) )
      {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Retrieving relations table from " 
                    << inputTESLocation() << endmsg;
        }
        const TABLE* table = get<TABLE>(inputTESLocation());
        if (table) {
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "found table with "<< table->relations().size() 
                      << " entries!" << endmsg;
          }
          TABLE* cloneTable = m_tableCloner(table);
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "Going to store relations table from " 
                      << inputTESLocation()
                      << " into " << outputLocation << endmsg;
            verbose() << "Number of relations in cloned table: "
                      << cloneTable->relations().size() << endmsg;
          }
          put( cloneTable, outputLocation );
          return StatusCode::SUCCESS;
        }
        return StatusCode::FAILURE;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Found no table at " << inputTESLocation() 
                    << ". storing empty table"<< endmsg;
        }
        
        TABLE* cloneTable = new TABLE();
        put( cloneTable, outputLocation );
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

      if (0==from) {
        error() << "FROM is NUL!!!!" << endmsg;
        return 0;
      }
      return getStoredClone< _From >(from);

    }

    inline const typename TABLE::To cloneTo(const typename TABLE::To to) 
    {
      if (0==m_cloner) return to;
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
