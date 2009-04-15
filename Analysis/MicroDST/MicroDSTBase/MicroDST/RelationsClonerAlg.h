// $Id: RelationsClonerAlg.h,v 1.2 2009-04-15 17:01:48 jpalac Exp $
#ifndef MICRODST_RELATIONSCLONERALG_H 
#define MICRODST_RELATIONSCLONERALG_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"
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
public:
  //===========================================================================
  /// Standard constructor
  RelationsClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
    :
    MicroDSTAlgorithm ( name , pSvcLocator ),
    m_cloner(0),
    m_clonerType(DEFAULTS::clonerType)
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
        TABLE* cloneTable = tableOfClones(table);
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

  TABLE* tableOfClones(const TABLE* table) 
  {

    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;

    TABLE* cloneTable = new TABLE();

    typename TABLE::Range relations = table->relations();
    verbose() << "Found " << relations.size() 
              << " relations" << endmsg;

    for (typename TABLE::Range::const_iterator iRel = relations.begin();
         iRel != relations.end();
         ++iRel ) {
      const typename TABLE::From from = iRel->from();
      if (from) {
        typename TABLE::Range range = table->relations(from);
        for (typename TABLE::Range::const_iterator iRange = range.begin();
             iRange != range.end();
             ++iRange) {
          verbose() << "\nFrom " << *from 
                    << " is related to To " 
                    << *(iRange->to()) 
                    << " with weight " << iRange->weight() << endmsg;
          const typename TABLE::From clonedFrom = 
            getStoredClone< _From >(from);
          const typename TABLE::To clonedTo = (*m_cloner)( iRange->to()  );
          if (clonedFrom&&clonedTo) {
            cloneTable->relate(clonedFrom, clonedTo, iRange->weight());
            verbose() << "\ncloned From "    
                      << *clonedFrom 
                      << " is related to cloned To " 
                      << *clonedTo 
                      << " with weight " << iRange->weight() << endmsg;
          } // if cloned From AND cloned To
        } // loop on related Tos
      } // if From* found
    } // loop on all relations
  
    return cloneTable;

  }
  //===========================================================================

private:

  CLONER* m_cloner;
  std::string m_clonerType;
  
};
 
}

#endif // MICRODST_RELATIONSCLONERALG_H
