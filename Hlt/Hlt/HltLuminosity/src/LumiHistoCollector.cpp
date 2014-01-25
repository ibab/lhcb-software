// $Id: LumiHistoCollector.cpp,v 1.7 2009-09-30 07:24:56 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/ODIN.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

#include "LumiHistoCollector.h"

// for histogram retrieval
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiHistoCollector
//
// 2008-10-01 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiHistoCollector )

LumiHistoCollector::LumiHistoCollector( const std::string& name,
                                        ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator )
{
    declareProperty( "HistogramUpdatePeriod", m_histogramUpdatePeriod = 1 );
    declareProperty( "TrendSize", m_trendSize = 100 );
    declareProperty( "TrendInterval", m_trendInterval = 100 );
    declareProperty( "BXTypes",
                     m_BXTypes = {"BeamCrossing", "Beam1", "Beam2", "NoBeam"} );
    declareProperty( "AddBXTypes", m_addBXTypes = {"BeamCrossing", "NoBeam"} );
    declareProperty( "SubtractBXTypes", m_subtractBXTypes = {"Beam1", "Beam2"} );
    declareProperty( "InputVariables",
                     m_Variables = {"SPDMult",          "PUMult",
                                    "CaloEt",           "SPDMult_threshold",
                                    "PUMult_threshold", "CaloEt_threshold"} );
    declareProperty( "prevDir", m_prevDir = "Previous" );
    declareProperty( "resDir", m_resDir = "Results" );
    declareProperty( "PathBase", m_pathBase = "/stat/Histo" );
}

LumiHistoCollector::~LumiHistoCollector()
{
}

StatusCode LumiHistoCollector::initialize()
{

    StatusCode sc = HltBaseAlg::initialize();
    if ( sc.isFailure() ) return sc;

    // set up the hisogram lookup tables and book the result histos
    setupStore();

    // get list of all histograms and their directories
    if ( msgLevel( MSG::DEBUG ) ) printHistoList();

    debug() << "Initialised Lumi Histo Collector" << endmsg;
    return sc;
}

//---------------------------------------------------------
StatusCode LumiHistoCollector::execute()
{

    StatusCode sc = StatusCode::SUCCESS;
    counter( "#Entries" ) += 1;
    counter( "HistoInputs" ) += 1;

    // analyse the data at with time intervals
    if ( gpsTimeInterval() ) analyse();

    setDecision( true );
    return sc;
}

//---------------------------------------------------------
void LumiHistoCollector::setDecision( bool ok )
{
    setFilterPassed( ok );
}

//---------------------------------------------------------
void LumiHistoCollector::setupStore()
{
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "LumiHistoCollector::setupStore()0" << endmsg;

    //  get histogram objects
    ISvcLocator* svclocator = Gaudi::svcLocator();
    StatusCode sc2 = svclocator->service( "HistogramDataSvc", m_histogramSvc, true );

    // simple method - using known paths
    AIDA::IHistogram1D* mydataobject;
    // loop over requested histogram paths and retrieve them
    int countMaps = 0;
    std::string prefix = "";
    for ( const std::string& bx : m_BXTypes ) {
        countMaps++;
        /* create storage for the existing histograms */
        histoMap* hMap = new histoMap();
        m_histoStore[bx] = hMap;
        /* create storage for the histograms  which should hold the previous cycle */
        histoMap* prevMap = new histoMap();
        m_previousStore[bx] = prevMap;
        for ( std::vector<std::string>::iterator iVar = m_Variables.begin();
              iVar != m_Variables.end(); ++iVar ) {
            std::string var = *iVar;
            std::string path = m_pathBase + bx + "/" + var;
            StatusCode sc = m_histogramSvc->retrieveObject( path, mydataobject );
            if ( sc.isSuccess() ) {
                AIDA::IHistogram1D* theHisto = mydataobject;
                ( *hMap )[var] = theHisto; // store in map
                double sumBins = theHisto->sumBinHeights();
                double mean = theHisto->mean();
                const AIDA::IAxis& axis = theHisto->axis(); // assuming 1Dhisto
                double lowerEdge = axis.lowerEdge();
                double upperEdge = axis.upperEdge();
                long bins = axis.bins();
                /* create histogram with same name (add prefix anyway) */
                prefix = "Previous_" + bx + "_";
                AIDA::IHistogram1D* prevHisto =
                    initializeHisto( prefix + var, lowerEdge, upperEdge, bins );
                ( *prevMap )[var] = prevHisto; // store in other map
                /* the first time around create a single map to keep the results of
                 * the subtraction and so */
                if ( countMaps == 1 ) {
                    // the result histos
                    prefix = "Results_";
                    AIDA::IHistogram1D* resultHisto =
                        initializeHisto( prefix + var, lowerEdge, upperEdge, bins );
                    m_resultMap[var] = resultHisto; // store in result map
                    // the temp histos
                    prefix = "Temp_";
                    AIDA::IHistogram1D* tempHisto =
                        initializeHisto( prefix + var, lowerEdge, upperEdge, bins );
                    m_tempMap[var] = tempHisto; // store in temp map
                    // the trend histos
                    prefix = "Trend_";
                    AIDA::IHistogram1D* trendHisto = initializeHisto(
                        prefix + var, -m_trendSize * m_trendInterval, 0,
                        m_trendSize );
                    m_trendMap[var] = trendHisto; // store in trend map
                }
                if ( msgLevel( MSG::DEBUG ) )
                    debug() << theHisto->title() << " at path = " << path << " "
                            << " axis: " << bins << " " << lowerEdge << " "
                            << upperEdge << " sumBins: " << sumBins
                            << " mean: " << mean << endmsg;
            } else {
                error() << MSG::INFO << ">>>Could not retrieve object from TES ("
                        << path << ")" << endmsg;
            }
        }
    }
}

//---------------------------------------------------------
StatusCode LumiHistoCollector::analyse()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "analyse()" << endmsg;
    // look at what we have in store
    info() << "summary of store contents" << endmsg;
    std::string prefix = "";
    for ( histoStore::iterator iBx = m_histoStore.begin(); iBx != m_histoStore.end();
          ++iBx ) {
        histoMap* theMap = iBx->second;
        std::string bx = iBx->first;
        histoMap* prevMap = m_previousStore[bx];
        for ( histoMap::iterator iVar = theMap->begin(); iVar != theMap->end();
              ++iVar ) {
            std::string var = iVar->first;
            AIDA::IHistogram1D* theHist = iVar->second;
            AIDA::IHistogram1D* prevHist = ( *prevMap )[var];
            std::string title = theHist->title();
            double sumBins = theHist->sumBinHeights();
            double sumAllBins = theHist->sumAllBinHeights();
            double prevSumBins = prevHist->sumBinHeights();
            double prevSumAllBins = prevHist->sumAllBinHeights();
            double mean = theHist->mean();
            double prevMean = prevHist->mean();
            if ( msgLevel( MSG::DEBUG ) )
                debug() << bx << " / " << var << " : " << title
                        << " sumBins: " << sumBins << " (" << sumAllBins
                        << ") mean: " << mean << " prevSumBins: " << prevSumBins
                        << " (" << prevSumAllBins << ") prevMean: " << prevMean
                        << endmsg;
        }
    }

    // take delta from all histos
    for ( const std::string& var : m_Variables ) {
        AIDA::IHistogram1D* resultHist = m_resultMap[var];
        resultHist->reset();
        double meanR = 0; // for R calculation
        for ( const std::string& bx : m_BXTypes ) {
            histoMap* theMap = m_histoStore[bx];
            histoMap* prevMap = m_previousStore[bx];
            AIDA::IHistogram1D* theHist = ( *theMap )[var];
            AIDA::IHistogram1D* prevHist = ( *prevMap )[var];
            AIDA::IHistogram1D* tempHist = m_tempMap[var];
            tempHist->reset();
            // subtract the previous results from the present ones and store in the
            // previous
            long prevEntries = prevHist->entries();
            double prevSumAll = prevHist->sumAllBinHeights();
            double theSumAll = theHist->sumAllBinHeights();
            double prevMean = prevHist->mean();
            double theMean = theHist->mean();
            double calcMean = 0;
            if ( theSumAll - prevSumAll ) {
                calcMean = ( theSumAll * theMean - prevSumAll * prevMean ) /
                           ( theSumAll - prevSumAll );
            }
            prevHist->scale( -1. );
            tempHist->add( *theHist );
            tempHist->add( *prevHist );
            double tempMean = tempHist->mean();

            prevHist->add( *theHist );
            if ( msgLevel( MSG::DEBUG ) )
                debug() << " subtraction result: " << bx << "/" << var
                        << " entries: " << theHist->entries() << " - " << prevEntries
                        << " = " << prevHist->entries()
                        << " *** sum: " << theHist->sumAllBinHeights() << " - "
                        << prevSumAll << " = " << prevHist->sumAllBinHeights()
                        << endmsg;
            // add and subtract according to crossing type
            double deltaSumAll = prevHist->sumAllBinHeights();
            double deltaMean = prevHist->mean();
            if ( msgLevel( MSG::DEBUG ) )
                debug() << " check means " << deltaMean << " and " << calcMean
                        << " from " << theMean << " and " << prevMean << " temp "
                        << tempMean << endmsg;
            double scale = 0; // if histo not wanted scale to zero
            for ( const std::string&  ABx : m_addBXTypes ) {
                if ( ABx == bx ) {
                    scale = deltaSumAll;
                    meanR += deltaMean;
                    break;
                }
            }
            for ( const std::string& SBx : m_subtractBXTypes ) {
                if (  SBx  == bx ) {
                    scale = -1 * deltaSumAll;
                    meanR -= deltaMean;
                    break;
                }
            }
            // add or subtract this crossing type
            prevHist->scale( scale );
            resultHist->add( *prevHist );
            // dispose of intermediate data and save the data in store for the next
            // time
            prevHist->reset();
            prevHist->add( *theHist );
        }
        // R-calculation
        double histoR = resultHist->mean();
        info() << var << ": R from means: " << meanR << "  R from histos: " << histoR
               << endmsg;
        // store trend
        storeTrend( var, meanR );
    }

    return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode LumiHistoCollector::printHistoList()
{
    // walk through the tree recursively and print the nodes
    ISvcLocator* svclocator = Gaudi::svcLocator();
    StatusCode sc = svclocator->service( "HistogramDataSvc", m_histogramSvc, true );
    if ( sc.isSuccess() ) {
        std::string m_rootName;
        SmartDataPtr<DataObject> root( m_histogramSvc, m_rootName );
        std::string store_name { "Unknown" };
        IRegistry* pReg = root->registry();
        IRegistry* pObj;
        if ( pReg ) {
            SmartIF<IService> isvc( pReg->dataSvc() );
            if ( isvc ) store_name = isvc->name();
        }
        pObj = root->registry();
        SmartIF<IDataManagerSvc> mgr( m_histogramSvc );
        if ( mgr ) printHistoLeaf( m_histogramSvc, mgr, pObj );
    } else {
        error() << "printHistoList cannot get HistogramDataSvc" << endmsg;
    }
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------
bool LumiHistoCollector::printHistoLeaf( IHistogramSvc* histogramSvc,
                                         SmartIF<IDataManagerSvc> mgr,
                                         IRegistry* pObj )
{
    // print the node name and work down the tree if new leaves
    typedef std::vector<IRegistry*> Leaves;
    Leaves leaves;
    StatusCode sc = mgr->objectLeaves( pObj, leaves );
    if ( sc.isSuccess() ) {
        if ( leaves.size() ) {
            // this is a leaf
            for ( auto& i : leaves ) {
                if ( printHistoLeaf( histogramSvc, mgr, i ) ) {
                    // this is not a histogram
                    info() << MSG::INFO << "--Node found with id " << i->identifier() << endmsg;
                }
            }
        } else {
            const std::string& id = pObj->identifier();
            info() << MSG::INFO << "**Hist found with id " << id << endmsg;
            return false;
        }
    }
    return true;
}

//=============================================================================
ulonglong LumiHistoCollector::gpsTime()
{

    // get ODIN bank
    LHCb::ODIN* odin = getIfExists<LHCb::ODIN>( LHCb::ODINLocation::Default );
    if ( !odin ) {
        err() << "ODIN cannot be loaded" << endmsg;

        // StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
        return 0;
    }
    return odin->gpsTime(); // in microseconds
}

//=============================================================================
bool LumiHistoCollector::gpsTimeInterval()
{
    // check if time interval has elapsed
    ulonglong now = gpsTime();
    verbose() << "gps: " << now << endmsg;

    if ( m_gpsPrevious ) {
        double delta = ( now - m_gpsPrevious );
        double interval = ( m_trendInterval );

        if ( delta > ( interval * ( 1.0E6 ) ) ) {
            m_gpsPrevious = now;
            return true;
        }
        return false;
    }
    m_gpsPrevious = now;
    return false;
}

//=============================================================================
void LumiHistoCollector::storeTrend( std::string varname, double lumiValue )
{
    // store trends for
    AIDA::IHistogram1D* theHist = m_trendMap[varname];
    const AIDA::IAxis& axis = theHist->axis();
    long bins = axis.bins();
    if ( msgLevel( MSG::DEBUG ) ) debug() << "trend " << varname << ": ";
    for ( long i = 0; i < bins; ++i ) {
        double binValue = theHist->binHeight( i );
        double nextValue;
        if ( i < bins - 1 ) {
            nextValue = theHist->binHeight( i + 1 );
        } else {
            nextValue = lumiValue;
        }
        double x = 0.5 * ( axis.binUpperEdge( i ) + axis.binLowerEdge( i ) );
        theHist->fill( x, nextValue - binValue );
        if ( msgLevel( MSG::DEBUG ) ) debug() << theHist->binHeight( i ) << " ";
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << endmsg;
}
