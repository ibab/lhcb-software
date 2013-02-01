
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDAlg.h
 *
 *  Declaration file for ANN Combined PID algorithm ANNGlobalPID::ChargedProtoANNPIDAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/09/2010
 */
//-------------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDAlg_H
#define ChargedProtoANNPID_ChargedProtoANNPIDAlg_H 1

// STL
#include <sstream>
#include <fstream>

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoANNPIDAlgBase.h"

// Event Model
#include "Event/ProtoParticle.h"

// NeuroBayes (only on linux with gcc for the moment)
#ifdef __GNUC__
#include "NeuroBayesExpert.hh"
#endif

// interfaces
#include "TrackInterfaces/ITrackSelector.h"

// boost
#include "boost/assign/list_of.hpp"

// FPE exception protection
#include "Kernel/FPEGuard.h"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDAlg ChargedProtoANNPIDAlg.h
   *
   *  Adds ANN PID information to ProtoParticles
   *
   *  @author Chris Jones
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDAlg : public ChargedProtoANNPIDAlgBase
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDAlg( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private:

    /** @class ANNHelper ChargedProtoANNPIDAlg.h
     *
     *  Base class for ANN helpers
     *
     *  @author Chris Jones
     *  @date   2010-03-09
     */
    class ANNHelper
    {
    protected:
      /// No default constructor
      ANNHelper() : m_parent(NULL) { }
    public:
      /** Constructor from information
       *  @param inputs The list of inputs needed for this network
       *  @param parent Point to parent algorithm
       */
      ANNHelper( const ChargedProtoANNPIDAlgBase::IntInputs& inputs,
                 const ChargedProtoANNPIDAlgBase *           parent )
        : m_inputs ( inputs ),
          m_parent ( parent ) { }
      /// Destructor
      virtual ~ANNHelper() { }
    public:
      /// Compute the ANN output for the given ProtoParticle
      virtual double getOutput( const LHCb::ProtoParticle * proto ) const = 0;
      /// Number of inputs to the ANN
      inline unsigned int nInputs() const { return m_inputs.size(); }
    protected:
      ChargedProtoANNPIDAlgBase::IntInputs m_inputs; ///< The list of inputs for this network
      const ChargedProtoANNPIDAlgBase *    m_parent; ///< Pointer to parent algorithm
    };

#ifdef __GNUC__
    /** @class ANNHelper ChargedProtoANNPIDAlg.h
     *
     *  Helper class for NeuroBayes networks
     *
     *  @author Chris Jones
     *  @date   2010-03-09
     */
    class NeuroBayesANN : public ANNHelper
    {
    private:
      /// No default constructor
      NeuroBayesANN() : m_expert(NULL) { }
    public:
      /** Constructor from information
       *  @param paramFileName Network tuning parameter file
       *  @param inputs The list of inputs needed for this network
       *  @param parent Point to parent algorithm
       */
      NeuroBayesANN( const std::string &                  paramFileName,
                     const ChargedProtoANNPIDAlgBase::IntInputs& inputs,
                     const ChargedProtoANNPIDAlgBase *           parent,
                     const bool suppressPrintout = true )
        : ANNHelper ( inputs, parent ),
          m_expert  ( new Expert(paramFileName.c_str(),-2) ),
          m_inArray ( new float[inputs.size()]             ),
          m_suppressPrintout (            suppressPrintout )
      { }
      /// Destructor
      virtual ~NeuroBayesANN() { delete m_expert; delete[] m_inArray; }
    public:
      /// Compute the ANN output for the given ProtoParticle
      virtual double getOutput( const LHCb::ProtoParticle * proto ) const;
    private:
      Expert * m_expert;  ///< Pointer to the NeuroBayes 'Expert'
      float * m_inArray;  ///< Working array for network inputs
      bool m_suppressPrintout; ///< Suppress any printout from NeuroBayes
    };
#endif

  private:

    /// Checks if the ProtoParticle has any RICH information
    bool hasRichInfo( const LHCb::ProtoParticle * proto ) const
    {
      return ( getInput(proto,"RichUsedAero")  > 0.5 ||
               getInput(proto,"RichUsedR1Gas") > 0.5 ||
               getInput(proto,"RichUsedR2Gas") > 0.5 );
    }

    /// Checks if the ProtoParticle has any ECAL information
    bool hasEcalInfo( const LHCb::ProtoParticle * proto ) const
    {
      return ( getInput(proto,"InAccEcal")  > 0.5 );
    }

    /// Run track preselection
    bool trackPreSel( const LHCb::ProtoParticle * proto ) const;

  private:

    /// Track selector type
    std::string m_trSelType;

    /// Track selector tool
    ITrackSelector * m_trSel;

    /// Track Pre-selection
    std::string m_trackPreSel;

    /// Configuration file
    std::string m_configFile;  

    /// Network Helper
    ANNHelper * m_netHelper;     

    /// The extra info to fill on the ProtoParticle
    LHCb::ProtoParticle::additionalInfo m_protoInfo;

    /// The version of the PID networks trianing to use
    std::string m_netVersion;

    /// Suppress all printout from the ANN experts
    bool m_suppressANNPrintout;

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDAlg_H
