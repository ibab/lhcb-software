
//---------------------------------------------------------------------------------
/** @file RichDecodedData.h
 *
 *  Header file for Rich::DAQ::DecodedData
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHDECODEDDATA_H
#define RICHKERNEL_RICHDECODEDDATA_H 1

// RichKernel
#include "RichKernel/RichDAQDefinitions.h"
#include "RichKernel/RichDAQHeaderPD_V4.h"
#include "RichKernel/RichDAQParityFooter.h"
#include "RichKernel/RichDAQL1IngressHeader.h"

//#include "RichKernel/RichMap.h"
#include "RichKernel/RichPoolMap.h"
//#include "RichKernel/RichUnorderedMap.h"

namespace Rich
{
  namespace DAQ
  {

    /** @class HPDInfo RichKernel/RichDecodedData.h
     *  Decoded HPD information
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/04/2007
     */
    class HPDInfo // : public LHCb::MemPoolAlloc<HPDInfo>
    {

    public:

      /// The HPD Header type
      using Header = RichDAQHeaderV4::RichDAQHeaderPD;
      /// The HPD Footer type
      using Footer = ParityFooter;
      
    public:

      /// Default Constructor
      HPDInfo() = default;

      /// Destructor
      ~HPDInfo() = default;

      /// Default Copy Constructor
      HPDInfo( const HPDInfo& ) = default;

      /// Default Copy operator
      HPDInfo& operator=( const HPDInfo& ) = default;

      /// Default Move Constructor
      HPDInfo( HPDInfo&& ) = default;

      /// Default Move operator
      HPDInfo& operator=( HPDInfo&& ) = default;

      /// Constructor from HPD data bank information
      HPDInfo( const LHCb::RichSmartID& input,
               const Header&            header,
               const Footer&            footer )
        : m_hpdID  ( input  ),
          m_header ( header ),
          m_footer ( footer )
      { }

      /// Constructor from HPD data bank information
      HPDInfo( const LHCb::RichSmartID&   input,
               const Header::HeaderWords& header,
               const Footer::FooterWords& footer )
        : m_hpdID  ( input  ),
          m_header ( header ),
          m_footer ( footer )
      { }

      /**  Access the HPD ID (LHCb::RichSmartID) for this HPD
       *   @attention It is possible this HPDID is invalid (for instance
       *              if the L0ID->RichSmartID database lookup failed.
       *              Users should check for validity using the method
       *              RichSmartID::isvalid()
       */
      inline const LHCb::RichSmartID& hpdID() const & noexcept { return m_hpdID; }
      /// Access the Header words
      inline const Header&           header() const & noexcept { return m_header; }
      /// Access the Footer words
      inline const Footer&           footer() const & noexcept { return m_footer; }

      /// set the Level1Input
      inline void setHpdID( const LHCb::RichSmartID& input ) noexcept { m_hpdID  = input;  }
      /// Set the Header
      inline void setHeader( const Header & header )         noexcept { m_header = header; }
      /// Set the footer
      inline void setFooter( const Footer & footer )         noexcept { m_footer = footer; }

      /// Read access to the RichSmartIDs for the hit pixels in this HPD
      inline const LHCb::RichSmartID::Vector& smartIDs() const & { return m_smartIds; }
      /// Write access to the RichSmartIDs for the hit pixels in this HPD
      inline       LHCb::RichSmartID::Vector& smartIDs()       & { return m_smartIds; }

    public:

      /// Move the Header words
      inline Header&& header() && { return std::move(m_header); }
      /// Move the Footer words
      inline Footer&& footer() && { return std::move(m_footer); }
      /// Move the smart IDs
      inline LHCb::RichSmartID::Vector&& smartIDs() && { return std::move(m_smartIds); }
      /// Move the HPD ID
      inline LHCb::RichSmartID && hpdID() && { return std::move(m_hpdID); }

    private:

      LHCb::RichSmartID m_hpdID;            ///< The RichSmartID for this HPD
      Header m_header;                      ///< The HPDHeader word
      Footer m_footer;                      ///< The HPDFooter word
      LHCb::RichSmartID::Vector m_smartIds; ///< The decoded pixels in this HPD

    };

    /// Map for RICH HPD data, sorted by HPD number
    using HPDMap = Rich::PoolMap< Rich::DAQ::Level1Input, HPDInfo >;

    /** @class IngressInfo RichKernel/RichDecodedData.h
     *  Information for each L1 ingress
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/04/2007
     */
    class IngressInfo // : public LHCb::MemPoolAlloc<IngressInfo>
    {

    public:

      /// Default Constructor
      IngressInfo() = default;

      /// Destructor
      ~IngressInfo() = default;

      /// Default Copy Constructor
      IngressInfo( const IngressInfo& ) = default;

      /// Default Copy operator
      IngressInfo& operator=( const IngressInfo& ) = default;

      /// Default Move Constructor
      IngressInfo( IngressInfo&& ) = default;

      /// Default Move operator
      IngressInfo& operator=( IngressInfo&& ) = default;

      /// Constructor with ingress header
      explicit IngressInfo( const L1IngressHeader& head ) : m_ingressHeader(head) { }

    public:

      /// Get the L1IngressHeader
      inline const L1IngressHeader& ingressHeader() const & noexcept { return m_ingressHeader; }
      /// Read access to the HPD data for this Ingress
      inline const HPDMap &               hpdData() const & noexcept { return m_hpdData; }
      /// Write access to the HPD data for this Ingress
      inline       HPDMap &               hpdData()       & noexcept { return m_hpdData; }
      /// Set the L1IngressHeader
      inline void setIngressHeader( const L1IngressHeader& head ) { m_ingressHeader = head; }

    public:

      /// Move the L1IngressHeader
      inline L1IngressHeader&& ingressHeader() && { return std::move(m_ingressHeader); }
      /// Move the HPD data for this Ingress
      inline HPDMap &&               hpdData() && { return std::move(m_hpdData); }

    private:

      L1IngressHeader m_ingressHeader; ///< The ingress header word
      HPDMap          m_hpdData;       ///< The HPD data blocks for this ingress

    };

    /// Map for HPD data for each L1 ingress
    using IngressMap = Rich::PoolMap< L1IngressID, IngressInfo >;

    /// Map for RICH HPD data, sorted by Level1 board number
    using L1Map      = Rich::PoolMap< Level1HardwareID, IngressMap >;

  }
}

#endif // RICHKERNEL_RICHDECODEDDATA_H
