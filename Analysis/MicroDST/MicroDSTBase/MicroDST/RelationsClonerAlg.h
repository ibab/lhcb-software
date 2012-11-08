// $Id: RelationsClonerAlg.h,v 1.23 2010-09-07 10:28:53 jpalac Exp $
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
// From DaVinci
#include "DaVinciUtils/Guards.h"
// from Boost
#include <boost/type_traits/remove_pointer.hpp>

namespace MicroDST
{

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

  template <typename TABLE>
  class RelationsClonerAlg : public MicroDSTAlgorithm
  {

  private:

    typedef Defaults<TABLE> DEFAULTS;
    typedef Location<TABLE> LOCATION;
    typedef typename BindType2Cloner<TABLE>::ToCloner CLONER;
    typedef MicroDST::TableCloner<TABLE> TableCloner;

  public:

    //===========================================================================
    /// Standard constructor
    RelationsClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      :
      MicroDSTAlgorithm ( name , pSvcLocator ),
      m_cloner(NULL),
      m_clonerType(DEFAULTS::clonerType),
      m_tableCloner( boost::bind(&RelationsClonerAlg<TABLE>::cloneFrom, &(*this), _1),
                     boost::bind(&RelationsClonerAlg<TABLE>::cloneTo,   &(*this), _1) )
    {
      declareProperty("ClonerType", m_clonerType);
      //setProperty( "OutputLevel", 2 );
    }

    //===========================================================================

    ~RelationsClonerAlg( ) {} ///< Destructor

    //===========================================================================

    virtual StatusCode initialize()
    {
      StatusCode sc = MicroDSTAlgorithm::initialize();
      if ( sc.isFailure() ) return sc;

      if ( inputTESLocations().empty() )
      {
        setInputTESLocation(LOCATION::Default);
      }
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      if ( m_clonerType.empty() || m_clonerType == "NONE" )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() <<"ClonerType set to NONE. No cloning of To performed." << endmsg;
      }
      else
      {
        m_cloner = tool<CLONER>(m_clonerType, this);
        if ( m_cloner )
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << "Found cloner " << m_clonerType << endmsg;
          sc = StatusCode::SUCCESS;
        }
        else
        {
          sc = Error( "Failed to find cloner " + m_clonerType );
        }
      }

      return sc;
    }

    //=========================================================================

    virtual StatusCode execute()
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

      setFilterPassed(true);

      for ( std::vector<std::string>::const_iterator inputLoc = this->inputTESLocations().begin();
            inputLoc != this->inputTESLocations().end(); ++inputLoc )
      {
        copyTableFromLocation(*inputLoc);
      }
      return StatusCode::SUCCESS;
    }

    //=========================================================================

  protected:

    void copyTableFromLocation( const std::string& inputLocation )
    {

      const std::string outputLocation =
        this->outputTESLocation( inputLocation );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Going to clone relations from "
                  << inputLocation
                  << " into " << outputLocation << endmsg;
      }

      if (exist<TABLE>(outputLocation) )
      {
        this->Warning("Object "+ outputLocation + " already exists. Not cloning.",
                      StatusCode::SUCCESS, 0).ignore() ;
        return;
      }

      if (exist<TABLE>(inputLocation) )
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Retrieving relations table from "
                    << inputLocation << endmsg;
        }
        const TABLE* table = get<TABLE>(inputLocation);
        if ( table && !table->relations().empty() )
        {

          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "found table with "<< table->relations().size()
                      << " entries!" << endmsg;
          }
          TABLE* cloneTable = m_tableCloner(table);
          DaVinci::Utils::DataObjectGuard guard(cloneTable);
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Going to store relations table from "
                      << inputLocation
                      << " into " << outputLocation << endmsg;
            verbose() << "Number of relations in cloned table: "
                      << cloneTable->relations().size() << endmsg;
          }
          if ( !cloneTable->relations().empty() )
          {
            put( cloneTable, outputLocation );
          }
        }
        else
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            this->Warning("Found no table at "+inputLocation,
                          StatusCode::FAILURE, 0).ignore();
          }
        }

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          this->Warning("Found no table at "+inputLocation,
                        StatusCode::FAILURE, 0).ignore();
        }
      }

    }

    //===========================================================================

  private:

    typedef typename BindType2Cloner<TABLE>::ToType TO_TYPE;
    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;

  private:

    inline const CLONER* cloner() { return m_cloner; }

    inline typename TABLE::From cloneFrom(const typename TABLE::From from)
    {
      if ( ! from )
      {
        error() << "FROM is NULL !!!!" << endmsg;
        return 0;
      }
      if ( ! from->parent() )
      {
        Warning("From is not in TES. Cannot clone!", StatusCode::FAILURE,0).ignore();
        return 0;
      }
      return getStoredClone< _From >(from);
    }

    inline typename TABLE::To cloneTo(const typename TABLE::To to)
    {
      if ( ! m_cloner ) return to;

      if ( ! to->parent() )
      {
        Warning("To is not in TES. Cannot clone!", StatusCode::FAILURE,0).ignore();
        return 0;
      }

      const typename TABLE::To storedTo = getStoredClone< TO_TYPE >(to);
      return ( storedTo ? storedTo : (*m_cloner)( to ) );
    }

    //===========================================================================

  private:

    CLONER* m_cloner;
    std::string m_clonerType;
    TableCloner m_tableCloner;

  };

}

#endif // MICRODST_RELATIONSCLONERALG_H
