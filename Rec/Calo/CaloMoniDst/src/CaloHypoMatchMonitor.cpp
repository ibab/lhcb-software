// $Id: CaloHypoMatchMonitor.cpp,v 1.5 2007-07-25 19:49:12 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 12:16:38  odescham
// v2r0 - adapt to the new Track Event Model
//
// Revision 1.3  2005/05/13 12:58:35  cattanem
// fixes for windows
//
// Revision 1.2  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted2D.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/Track.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/Calo2Track.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloHypoMatchMonitor CaloHypoMatchMonitor.cpp
 *
 *  The algorithm for trivial monitoring of matching of
 *  "CaloClusters" with Tracks.
 *  It produces 5 histograms:
 *
 *  <ol>
 *  <li> @p log10(#Relations+1)  distribution               </li>
 *  <li> Link multiplicity       distribution               </li>
 *  <li> Minimal Weight          distribution               </li>
 *  <li> Maximal Weight          distribution               </li>
 *  <li>         Weight          distribution               </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where
 *  @ "Name" is the name of the algorithm
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloHypoMatchMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoMatchMonitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    std::string common = "'" + inputData() + "' " + name();
    hBook1( "1", "log10(#Links+1) " + common, 0,    4, 100 );
    hBook1( "2", "Rels per Hypo   " + common, 0,   25,  50 );
    hBook1( "3", "Min weight      " + common, 0,  100, 200 );
    hBook1( "4", "Max weight      " + common, 0, 1000, 200 );
    hBook1( "5", "Weights         " + common, 0, 1000, 500 );
    return StatusCode::SUCCESS;
  }
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloHypoMatchMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ) {}
  /// destructor (virtual and protected)
  virtual ~CaloHypoMatchMonitor() {}
private:
  /// default  construstor  is  private
  CaloHypoMatchMonitor();
  /// copy     construstor  is  private
  CaloHypoMatchMonitor( const CaloHypoMatchMonitor& );
  /// assignement operator  is  private
  CaloHypoMatchMonitor &operator=( const CaloHypoMatchMonitor& );
};

DECLARE_ALGORITHM_FACTORY( CaloHypoMatchMonitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloHypoMatchMonitor::execute()
{ typedef LHCb::CaloHypo::Container                            Hypos;
  typedef LHCb::Calo2Track::ITrHypoTable2D                     Table;

  if ( !produceHistos() ) return StatusCode::SUCCESS;

// check relations
  Table *table = get<Table>( inputData() );
  if ( 0 == table ) return StatusCode::FAILURE;

// logarithm of ( total number of links + 1 )
  hFill1( "1", log10( table-> inverse()->relations().size() + 1. ) );

  if ( inputs().empty() ) return Error( "No input data are specified" );

// loop over all containers
  for( std::vector<std::string>::const_iterator input = inputs().begin();
       inputs().end() != input; ++input )
  { Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) return StatusCode::FAILURE;
// loop over all hypos
    for( Hypos::const_iterator hypo = hypos->begin();
         hypos->end() != hypo; ++hypo )
    { const LHCb::Calo2Track::ITrHypoTable2D::InverseType::Range range = table-> inverse()->relations( *hypo );
// number of related tracks
      hFill1( "2", range.size() );
      if ( range.empty() ) continue;
// minimal weight
      hFill1( "3", range.front().weight() );
// maximal weight
      hFill1( "4", range.back().weight() );
// all weights
      for( LHCb::Calo2Track::ITrHypoTable2D::InverseType::iterator relation=range.begin(); range.end()!=relation; ++relation )
      { hFill1( "5", relation->weight() );
      }
    } // end of loop over hypos 
  } // end of loop over containers 
  return StatusCode::SUCCESS;
}
