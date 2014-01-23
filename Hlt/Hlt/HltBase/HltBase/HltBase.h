// $Id: HltBase.h,v 1.27 2010-03-27 22:08:30 graven Exp $
#ifndef HLTBASE_HLTBASE_H
#define HLTBASE_HLTBASE_H 1

// Include files
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/HistoDef.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/MsgStream.h"
#include "Kernel/IANNSvc.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"

/** @class HltBase
 *
 *  Provide services to HLT algorithms and tools
 *
 *  Functionality:
 *       Access to HltSvc, Hlt::Data
 *       Retrieve, register Hlt::Selections
 *       Histogram booking via options
 *       Counters
 *
 *  Options
 *       HistogramDescriptor
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */
namespace LHCb
{
class Track;
class RecVertex;
}

class ISvcLocator;

template <class BASE>
class HltBase : public BASE
{
  public:
    // Algorithm constructor
    HltBase( std::string name, ISvcLocator* pSvcLocator );

    // Tool constructor
    HltBase( std::string type, std::string name, const IInterface* parent );

    // destructor
    virtual ~HltBase(); ///< Destructor
    // initialize
    virtual StatusCode initialize();

  protected:
    // initialize Histo
    AIDA::IHistogram1D* initializeHisto( const std::string& name, double min = 0.,
                                         double max = 100., int nBins = 100 );

    // print info of this container
    template <class CONT>
    void printInfo( const std::string& title, const CONT& cont )
    {
        BASE::info() << title << cont.size() << endmsg;
        typedef typename CONT::const_iterator iter;
        for ( iter it = cont.begin(); it != cont.end(); ++it )
            printInfo( title, *( *it ) );
    }

    // print info from track
    void printInfo( const std::string& title, const LHCb::Track& track );

    // print info from vertex
    void printInfo( const std::string& title, const LHCb::RecVertex& ver );

    // print info from extraInfo
    void printInfo( const std::string& title,
                    const GaudiUtils::VectorMap<int, double>& info );

    IANNSvc& annSvc() const;
    Hlt::IRegister* regSvc() const;
    Hlt::IData* hltSvc() const;

    // returns the ID of the extraInfo by name
    int hltInfoID( const std::string& name );

  private:
    // returns the ID of the extraInfo by name
    std::string hltInfoName( int id );

    // delegate constructor
    virtual void hltBaseConstructor();

    // Property to book histogram from options
    SimpleProperty<std::map<std::string, Gaudi::Histo1DDef>> m_histoDescriptor;

    // hlt data provider service
    mutable Hlt::IData* m_hltSvc;
    //
    // hlt data registration service
    mutable Hlt::IRegister* m_regSvc;

    // assigned names and numbers service...
    mutable IANNSvc* m_hltANNSvc;
};
#endif // HLTBASE_HLTBASE_H
