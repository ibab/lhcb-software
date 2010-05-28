// $Id: RelationsFromClonerAlg.h,v 1.5 2010-05-28 19:36:35 jpalac Exp $
#ifndef MICRODST_RELATIONSFROMCLONERALG_H 
#define MICRODST_RELATIONSFROMCLONERALG_H 1

// Include files
#include <boost/function.hpp>
#include <boost/bind.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/Defaults.h"
#include "MicroDST/RelTableFunctors.h"
// from Boost
#include <boost/type_traits/remove_pointer.hpp>
/** @class RelationsFromClonerAlg RelationsFromClonerAlg.h MicroDST/RelationsFromClonerAlg.h
 *
 *  Templated algorithm to clone a relations table between Froms and Tos. The Froms
 *  are assumed to be cloned already. The Tos are not cloned, so the resulting cloned table 
 *  points to the original Tos.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2010-02-24
 */

namespace MicroDST 
{

  template <typename TABLE>
  class RelationsFromClonerAlg : public MicroDSTAlgorithm {

  private:

    typedef Location<TABLE> LOCATION;
    typedef MicroDST::TableCloner<TABLE> TableCloner;

  public:
    //===========================================================================
    /// Standard constructor
    RelationsFromClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      :
      MicroDSTAlgorithm ( name , pSvcLocator ),
      m_tableCloner(boost::bind(&RelationsFromClonerAlg<TABLE>::cloneFrom, &(*this), _1),
                    boost::bind(&RelationsFromClonerAlg<TABLE>::cloneTo, &(*this), _1))               
    {
    }
    //===========================================================================
    ~RelationsFromClonerAlg( ) {} ///< Destructor
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

      return sc;

    }
    //===========================================================================
    StatusCode execute() 
    {

      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

      setFilterPassed(true);

      std::vector<std::string>::const_iterator iBegin = this->inputTESLocations().begin();
      std::vector<std::string>::const_iterator iEnd = this->inputTESLocations().end();
      for (std::vector<std::string>::const_iterator inputLoc = iBegin;
           inputLoc != iEnd;
           ++inputLoc) {
        copyTableFromLocation(*inputLoc);
      }
      return StatusCode::SUCCESS;
    }

    //=========================================================================
    void copyTableFromLocation(const std::string& inputLocation) 
    {
      const std::string outputLocation = 
        this->outputTESLocation( inputLocation );

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Going to clone relations from " 
                  << inputLocation
                  << " into " << outputLocation << endmsg;
      }
      


      if (exist<TABLE>(outputLocation) )
      {
        this->Warning("Object "+ outputLocation + " already exists. Not cloning.", 
                      StatusCode::SUCCESS,0).ignore() ;
        return;
      }
          

      if (exist<TABLE>(inputLocation) )
      {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Retrieving relations table from " 
                    << inputLocation << endmsg;
        }
        const TABLE* table = get<TABLE>(inputLocation);
        if (table and  !table->relations().empty() ) {
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "found table with "<< table->relations().size() 
                      << " entries!" << endmsg;
          }
          TABLE* cloneTable = m_tableCloner(table);
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "Going to store relations table from " 
                      << inputLocation
                      << " into " << outputLocation << endmsg;
            verbose() << "Number of relations in cloned table: "
                      << cloneTable->relations().size() << endmsg;
          }
          if (!cloneTable->relations().empty()) {
            put( cloneTable, outputLocation );
          } 
        } else {
          if ( msgLevel(MSG::VERBOSE) ) {
            this->Warning("Found no table at "+inputLocation,
                        StatusCode::FAILURE).ignore();  
          }
        }
        
      } else {
        if ( msgLevel(MSG::VERBOSE) ) {
          this->Warning("Found no table at "+inputLocation,
                        StatusCode::FAILURE, 0).ignore();
        }
      }

    }
    //===========================================================================
    StatusCode finalize() 
    {
      debug() << "==> Finalize" << endmsg;
      return MicroDSTAlgorithm::finalize();
    }
    //===========================================================================

  private:

    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;


  private:

    inline typename TABLE::From cloneFrom(const typename TABLE::From from) 
    {

      if (0==from) {
        error() << "FROM is NUL!!!!" << endmsg;
        return 0;
      }
      if (0==from->parent()) {
        Warning("From is not in TES. Cannot clone!", StatusCode::FAILURE,0).ignore();
        return 0;
      }
      return getStoredClone< _From >(from);

    }

    inline typename TABLE::To cloneTo(const typename TABLE::To to) 
    {
      if (0==to->parent()) {
        Warning("To is not in TES. Cannot clone!", StatusCode::FAILURE,0).ignore();
        return 0;
      }
      return to;
    }
  
    //===========================================================================
  
  private:

    TableCloner m_tableCloner;
    
  };



 
}

#endif // MICRODST_RELATIONSCLONERALG_H
