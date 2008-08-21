#ifndef RESETHITS_H
#define RESETHITS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"

  /** @class ResetHits ResetHits.h
   *  Clean up used flags of hits, e.g. to run offline on hits from HLT
   *
   *  @author Stephanie Hansmann-Menzemer
   *  @date   2008-08-20 Initial version
   *  
   */

  class ResetHits : public GaudiAlgorithm {
  public:
    /// Standard constructor
    ResetHits( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~ResetHits( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
        
  private:

    bool m_veloHits; 
    bool m_TTHits;
    bool m_OTHits;
    bool m_ITHits;

   
    /// The underlying OT hit creator
    ToolHandle<Tf::IOTHitCreator> m_othitcreator ;

    /// The underlying IT hit creator
    ToolHandle<Tf::IITHitCreator> m_ithitcreator ;

     /// The underlying OT hit creator
    ToolHandle<Tf::ITTHitCreator> m_tthitcreator ;

    /// The underlying OT hit creator
    inline const Tf::IOTHitCreator * otHitCreator() const { return &*m_othitcreator; }

    /// The underlying IT hit creator
    inline const Tf::IITHitCreator * itHitCreator() const { return &*m_ithitcreator; }

    /// The underlying TT hit creator
    inline const Tf::ITTHitCreator * ttHitCreator() const { return &*m_tthitcreator; }

  };
#endif // ResetHits_H
