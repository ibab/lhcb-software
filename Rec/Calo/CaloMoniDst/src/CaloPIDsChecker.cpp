// ============================================================================
// Include files
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IAxis.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/MCParticle.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "Kernel/Particle2MCLinker.h"
#include "CaloUtils/Calo2Track.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPIDsChecker CaloPIDsChecker.cpp
 *
 *  Class for monitoring the CaloPIDs quantities
 *
 *  The major properties
 *
 *  - "Input"
 *   The default value is                      ""
 *   The PID-quantity to be monitored, relation table of
 *   type IRelation<Track,float>
 *
 *  - "Inputs"
 *   The default value is :         TrackLocation::Default
 *   The list of Track containers to be monitored
 *
 *   - "Tr2MCP"
 *   The default value is :          "Rec/Relations/Tr2MCP"
 *   The location in TES the relation table of type
 *   IRelationWeighted<Track,MCParticle,double>
 *
 *    - "Particle"
 *    The default value is :         11
 *    Particle ID for "signal" hypothesis
 *
 *    - "Cut"
 *    The default value is :          0
 *    The cut on PID to be used for monitoring
 *
 *    - "Normialization"
 *    The default value is :          50 GeV
 *    The normalization factor used for histogramming
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-15
 */
class CaloPIDsChecker : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPIDsChecker>;
protected:
  typedef const IRelation<LHCb::Track,float>                          PIDTable;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
  
    const std::string mom = "tanh(P/"
      + std::to_string( m_pNorm / Gaudi::Units::GeV )
      + " GeV/c) ";
    const std::string cut  = "with cut 'DLL>"
      + std::to_string( m_cut ) + "' ";
  
    hBook1(  "1", "DLL for 'Signal'",     -5, 5 );
    hBook1(  "2", "DLL for 'Ghosts'",     -5, 5 );
    hBook1(  "3", "DLL for 'Background'", -5, 5 );

    hBook1( "11", mom +                "for 'Signal'",     0, 1.02, 51 );
    hBook1( "12", mom +                "for 'Ghosts'",     0, 1.02, 51 );
    hBook1( "13", mom +                "for 'Background'", 0, 1.02, 51 );

    hBook1( "21", mom + cut +          "for 'Signal'",     0, 1.02, 51 );
    hBook1( "22", mom + cut +          "for 'Ghosts'",     0, 1.02, 51 );
    hBook1( "23", mom + cut +          "for 'Background'", 0, 1.02, 51 );

    hBook1( "31", "Eff " + mom + cut + "for 'Signal'",     0, 1.02, 51 );
    hBook1( "32", "Eff " + mom + cut + "for 'Ghosts'",     0, 1.02, 51 );
    hBook1( "33", "Eff " + mom + cut + "for 'Background'", 0, 1.02, 51 );

    m_track2MCLink = new Object2MCLinker<LHCb::Track>( this, "", "", inputs() );
    if( m_split ){
      Warning( "No area spliting allowed for CaloPIDsChecker").ignore();
      m_split = false;
    }
    return StatusCode::SUCCESS;    
  }
  /// standard algorithm execution
  virtual StatusCode execute();
  /// standard algorithm finalization
  virtual StatusCode finalize(){ 
    StatusCode sc = divide( h1["21"], h1["11"], h1["31"] );
    if(sc.isSuccess())sc=divide( h1["22"], h1["12"], h1["32"] );
    if(sc.isSuccess())sc= divide( h1["23"], h1["13"], h1["33"] );
    if(!sc.isSuccess())error()<<"Failed dividing histograms " << endmsg;

    if( NULL != m_track2MCLink ) delete m_track2MCLink;
    m_track2MCLink = NULL;

// finalize the chain of base classes
    return CaloMoniAlg::finalize();
  }
protected:
  /** Standard constructor
   *  @param name nam eof algorithm instance
   *  @param isvc pointer to  Service Locator
   */
  CaloPIDsChecker( const std::string &name, ISvcLocator *isvc )
    : CaloMoniAlg( name, isvc )
    , m_track2MCLink(NULL)
    , m_pid( 11 )
    , m_cut( 0 )
    , m_pNorm( 50 * Gaudi::Units::GeV )
  { declareProperty( "Particle",      m_pid = 11);
    declareProperty( "Cut",           m_cut = 0. );
    declareProperty( "Normalization", m_pNorm = 50.*Gaudi::Units::GeV);
    declareProperty( "TrackAcceptance"   , m_accept = "Rec/Calo/InAccEcal");
    declareProperty( "TrackType"    , m_typ = "ALL");
    addToInputs( LHCb::TrackLocation::Default );
  }
  /// virtual destructor
  virtual ~CaloPIDsChecker() {}
private:
  // default constructor is disabled
  CaloPIDsChecker();
  // copy constructor is disabled
  CaloPIDsChecker( const CaloPIDsChecker& );
  // assignement operator is disabled
  CaloPIDsChecker &operator=( const CaloPIDsChecker& );
protected:
  /// transformation function for momentum
  inline double pFunc( const double value ) const
  { return tanh( value / m_pNorm );
  }
  /** extract the momentum from Track
   *  @param  track pointer to the track
   *  @return the momentum of the track
   */
  double momentum( const LHCb::Track *track  ) const;
  /// h3=h1/h2
  StatusCode divide( AIDA::IHistogram1D *hh1, AIDA::IHistogram1D *hh2,
                     AIDA::IHistogram1D *hh3 ) const;
private:
  // Track -> MC associator
  Object2MCLinker<LHCb::Track>* m_track2MCLink;
  // particle to be identified
  unsigned int         m_pid;
  // cut value
  double      m_cut;
  // momentum normalization
  double      m_pNorm;
  ///
  std::string m_accept;
  std::string m_typ;
  

};

DECLARE_ALGORITHM_FACTORY( CaloPIDsChecker )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloPIDsChecker::execute()
{ typedef const LHCb::Track::Container Tracks;

// produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;



  if ( inputs().empty() ) return Error( "No input data are specified" );
// get Tr -> CaloPID relation table 
  if ( !exist<PIDTable>( inputData()))return StatusCode::SUCCESS;
  const PIDTable *pidTable = get<PIDTable>( inputData() );
  for( std::vector<std::string>::const_iterator input = inputs().begin();
        inputs().end() != input; ++input )
  { Tracks *tracks = get<Tracks>( *input );
    if ( 0 == tracks ) return StatusCode::FAILURE;
// loop over tracks
    for( Tracks::const_iterator itrack = tracks->begin();
           tracks->end() != itrack; ++itrack )
    { const LHCb::Track *track = *itrack;
      if ( 0 == track ) continue;


// Select track
      // track type
      std::stringstream s("");
      s << track -> type() ;
      // track acceptance
      bool inAcc = true;
      if( 0 != m_accept.size() ){
        if( exist<LHCb::Calo2Track::ITrAccTable>( m_accept )){
          LHCb::Calo2Track::ITrAccTable* accTable = get<LHCb::Calo2Track::ITrAccTable>( m_accept );
          LHCb::Calo2Track::ITrAccTable::Range range = accTable ->relations( track );
          if( 0 == range.front().to() ) inAcc= false ;
        }
        else{
          inAcc = false;
          return Error( "Acceptance table not found at "+m_accept );
        }
      }
      if( s.str() != m_typ && m_typ !="ALL")continue;
      if( !inAcc )continue;
      


// get MC truth
      double maxWeight = 0;
      const LHCb::MCParticle* mc = m_track2MCLink->first(track, maxWeight);
      LHCb::MCParticle* mcp = (LHCb::MCParticle*) mc;      
      if( NULL != mc ){
        do{
          double weight =0;
          mc = m_track2MCLink->next(weight);
          if(weight>maxWeight){
            maxWeight=weight;
            mcp = (LHCb::MCParticle*) mc;
          }
        }while(mc !=NULL);
      }
      

      
// retrieve calo pid information
      PIDTable::Range calo = pidTable->relations( track );
// get THE LAST pid

      const double DLL = calo.empty() ? -1000.0 : calo.back().to();
      
      if ( 0 == mcp )                                 hFill1("2",  DLL); //ghosts
      else if ( mcp->particleID().abspid() == m_pid ) hFill1("1",  DLL); //signal
      else                                            hFill1("3",  DLL); //background

// evaluate the function of momentum
      const double pMom = pFunc( momentum( track ) );
      if ( 0 == mcp )                       hFill1("12", pMom); //ghosts
      else if ( mcp->particleID().abspid() == m_pid ) hFill1("11", pMom); //signal
      else                                       hFill1("13", pMom); //background

// apply DLL cut 
      if ( DLL < m_cut ) continue;
      if ( 0 == mcp )                       hFill1("22", pMom); //ghosts
      else if ( mcp->particleID().abspid() == m_pid ) hFill1("21", pMom); //signal
      else                                       hFill1("23", pMom); //background
    }
  }
  return StatusCode::SUCCESS;
}
// ============================================================================
/** extract the momentum from Track
 *  @param  track pointer to the track
 *  @return the momentum of the track
 */
// ============================================================================
double CaloPIDsChecker::momentum( const LHCb::Track *track ) const
{ if( 0 == track )
  { error() << "momentum(): Track* points to NULL, return -100*GeV" << endmsg;
    return -100 * Gaudi::Units::GeV;
  }
// get the momentum from the state nearest to 0,0,0
  const LHCb::State state = track->closestState( 0.0 );
  //OD if( 0 == state ) 
  //  { 
  //  Error ( "momentum(): Track has no state closest to z=0, return -100*GeV");  
  //  return -100 * GeV ;
  //}
  //const TrStateP* stateP = dynamic_cast<const TrStateP*>( state );
  //if( 0 == stateP ) 
  //{ 
  //  Error ( "momentum(): TrState is not TrStateP! ,return -100*GeV");  
  //  return -100 * GeV ;
  // }
  return state.p();
}
// ============================================================================
// Divide the histograms
// ============================================================================
StatusCode CaloPIDsChecker::divide( AIDA::IHistogram1D *hh1
                                  , AIDA::IHistogram1D *hh2
                                  , AIDA::IHistogram1D *hh3 ) const
{ if (( 0 == hh1 ) || ( 0 == hh2 ) || ( 0 == hh3 )){ 
    return Error( "AIDA::IHistogram1D* points to NULL" );
  }
  const IAxis &axis1 = hh1->axis();
  const IAxis &axis2 = hh2->axis();
  const IAxis &axis3 = hh3->axis();
  const int nBins = axis3.bins();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "DIVIDE nbins " << nBins << endmsg;
  
  if (( axis1.bins() != nBins ) || ( axis2.bins() != nBins ))
  { return Error( "Different histogram specifications" );
  }
  double count = 0;
  for( int nBin = 0; nBin < nBins; ++nBin )
  { const double v1 = hh1->binHeight( nBin );
    const double v2 = hh2->binHeight( nBin );
    const double x  = 0.5 * ( axis3.binLowerEdge ( nBin ) +
                              axis3.binUpperEdge ( nBin ) );

// fill the histogram 
    if ( 0 != v2 ) hh3->fill( x, v1/v2 );
    if ( 0 != v1) count += 1.;
  }
  
  const double content = hh3->sumBinHeights();
  if ( 0 != count ){
    info() << "'" << hh3->title()   << "' = "
           << (content/count)*100 << "[%]" << endmsg;
  }else{
    info() << "'" << hh1->title()
           << "' contains no filled bins - histogram ratio undefined" << endmsg;
  }
  
  return StatusCode::SUCCESS;
}



