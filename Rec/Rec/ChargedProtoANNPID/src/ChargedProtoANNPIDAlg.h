
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
#include "TMVAImpFactory.h"

// Event Model
#include "Event/ProtoParticle.h"

// NeuroBayes
#ifdef _ENABLE_NEUROBAYES
#include "NeuroBayesExpert.hh"
#endif

// TMVA
#include "TMVA/Reader.h"

// boost
#include "boost/lexical_cast.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"

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
      /// Typedef for list of inputs
      typedef ChargedProtoANNPIDAlgBase::Input::ConstVector Inputs;
    protected:
      /// No default constructor
      ANNHelper() : m_ok( false ) { }
    public:
      /** Constructor from information
       *  @param inputs The list of inputs needed for this network
       *  @param parent Point to parent algorithm
       */
      ANNHelper( const ChargedProtoANNPIDAlgBase::StringInputs& inputs,
                 const ChargedProtoANNPIDAlgBase *              parent )
        : m_inputs ( parent->getInputs(inputs) ),
          m_ok     ( false  ) { }
      /// Destructor
      virtual ~ANNHelper() 
      {
        for ( Inputs::const_iterator iIn = m_inputs.begin();
              iIn != m_inputs.end(); ++iIn ) { delete *iIn; }
      }
    public:
      /// Are we configured properly
      inline bool isOK() const { return m_ok; }
      /// Compute the ANN output for the given ProtoParticle
      virtual double getOutput( const LHCb::ProtoParticle * proto ) const = 0;
      /// Number of inputs to the ANN
      inline unsigned int nInputs() const { return m_inputs.size(); }
    protected:
      /// The list of inputs for this network
      Inputs m_inputs;
      /// Is this reader configured properly
      bool m_ok; 
    };

#ifdef _ENABLE_NEUROBAYES
    /** @class NeuroBayesANN ChargedProtoANNPIDAlg.h
     *
     *  Helper class for NeuroBayes networks
     *
     *  @author Chris Jones
     *  @date   2013-03-09
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
       *  @param suppressPrintout Supress all output from NeuroBayes
       */
      NeuroBayesANN( const std::string &                     paramFileName,
                     const ChargedProtoANNPIDAlgBase::StringInputs& inputs,
                     const ChargedProtoANNPIDAlgBase *              parent,
                     const bool suppressPrintout = true )
        : ANNHelper ( inputs, parent ),
          m_expert  ( new Expert(paramFileName.c_str(),-2) ),
          m_inArray ( new float[inputs.size()]             ),
          m_suppressPrintout ( suppressPrintout            )
      { m_ok = true; }
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

    /** @class TMVAReaderANN ChargedProtoANNPIDAlg.h
     *
     *  Helper class for TMVA ANN networks via the Reader
     *
     *  @author Chris Jones
     *  @date   2013-03-09
     */
    class TMVAReaderANN : public ANNHelper
    {
    private:
      /// No default constructor
      TMVAReaderANN() : m_reader(NULL) { }
    public:
      /** Constructor from information
       *  @param paramFileName Network tuning parameter file
       *  @param inputs The list of inputs needed for this network
       *  @param parent Point to parent algorithm
       */
      TMVAReaderANN( const std::string &                     paramFileName,
                     const ChargedProtoANNPIDAlgBase::StringInputs& inputs,
                     const ChargedProtoANNPIDAlgBase *              parent )
        : ANNHelper ( inputs, parent ),
          m_reader  ( new TMVA::Reader( parent->msgLevel(MSG::DEBUG) ?
                                        "!Color:!Silent" : "!Color:Silent" ) ),
          m_inArray ( new float[inputs.size()] )
      {
        int i = 0;
        for ( ChargedProtoANNPIDAlgBase::StringInputs::const_iterator iIn = inputs.begin();
              iIn != inputs.end(); ++iIn, ++i )
        {
          m_reader->AddVariable( (*iIn).c_str(), &(m_inArray[i]) );
        }
        m_reader->BookMVA( "PID", paramFileName.c_str() );
        m_ok = true;
      }
      /// Destructor
      virtual ~TMVAReaderANN() { delete m_reader; delete[] m_inArray; }
    public:
      /// Compute the ANN output for the given ProtoParticle
      virtual double getOutput( const LHCb::ProtoParticle * proto ) const;
    private:
      TMVA::Reader * m_reader; ///< The TMVA reader
      float * m_inArray;  ///< Working array for network inputs
    };

    /** @class TMVAImpANN ChargedProtoANNPIDAlg.h
     *
     *  Helper class for TMVA ANN networks via the C++ implementation
     *
     *  @author Chris Jones
     *  @date   2013-03-09
     */
    class TMVAImpANN : public ANNHelper
    {
    private:
      /// No default constructor
      TMVAImpANN() : m_reader(NULL) { }
    public:
      /** Constructor from information
       *  @param paramFileName Network tuning parameter file
       *  @param inputs The list of inputs needed for this network
       *  @param parent Point to parent algorithm
       */
      TMVAImpANN( const std::string & config,
                  const std::string & particle,
                  const std::string & track,
                  const ChargedProtoANNPIDAlgBase::StringInputs& inputs,
                  const ChargedProtoANNPIDAlgBase *              parent )
        : ANNHelper ( inputs, parent ),
          m_reader  ( tmvaFactory().create( config, particle, track, inputs ) ),
          m_vars    ( inputs.size() )
      {
        m_ok = ( m_reader && m_reader->IsStatusClean() );
      }
      /// Destructor
      virtual ~TMVAImpANN() { delete m_reader; }
    public:
      /// Compute the ANN output for the given ProtoParticle
      virtual double getOutput( const LHCb::ProtoParticle * proto ) const;
    private:
      IClassifierReader * m_reader; ///< The TMVA reader
      mutable std::vector<double> m_vars; ///< the input variables
    };

  private:

    /** @class Cut ChargedProtoANNPIDAlg.h
     *
     *  ProtoParticle selection cut
     *
     *  @author Chris Jones
     *  @date   2010-03-09
     */
    class Cut
    {
    public:
      /// Vector of cuts
      typedef std::vector<const Cut*> ConstVector;
    private:
      /// Delimitor enum
      enum Delim { UNDEFINED = -1, GT, LT, GE, LE };
    public:
      /// Constructor
      Cut( const std::string& desc = "NOTDEFINED",
           const ChargedProtoANNPIDAlgBase * parent = NULL );
      /// Destructor
      ~Cut( ) { delete m_variable; }
    private:
      /// No Copy Constructor
      Cut( const Cut& ) { }
    public:
      /// Is this object well defined
      bool isOK() const { return m_OK; }
      /// Does the ProtoParticle pass the cut
      bool isSatisfied( const LHCb::ProtoParticle * proto ) const
      {
        const double var = m_variable->value(proto);
        return ( m_delim == GT ? var >  m_cut :
                 m_delim == LT ? var <  m_cut :
                 m_delim == GE ? var >= m_cut :
                 m_delim == LE ? var <= m_cut :
                 false );
      }
      /// Cut description
      const std::string description() const { return m_desc; }
    public:
      /// Overload output to ostream
      friend inline std::ostream& operator << ( std::ostream& s, const Cut & cut )
      { return s << "'" << cut.description() << "'" ; }
    private:
      /// Set the delimitor enum from a string
      bool setDelim( const std::string & delim )
      {
        bool ok = false;
        if      ( ">"  == delim ) { m_delim = GT; ok = true; }
        else if ( "<"  == delim ) { m_delim = LT; ok = true; }
        else if ( ">=" == delim ) { m_delim = GE; ok = true; }
        else if ( "<=" == delim ) { m_delim = LE; ok = true; }
        return ok;
      }
    private:
      std::string m_desc; ///< The cut description
      bool m_OK;          ///< Is this cut well defined
      const ChargedProtoANNPIDAlgBase::Input * m_variable;  ///< The variable to cut on
      double m_cut;       ///< The cut value
      Delim m_delim;      ///< The delimitor
    };

  private:

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

    /// Vector of cuts to apply
    Cut::ConstVector m_cuts;

    /// The track type for this instance
    LHCb::Track::Types m_tkType;

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDAlg_H
