
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

// STL 
#include <map>
#include <unordered_map>

// RichKernel
#include "RichKernel/RichDAQDefinitions.h"

// RichKernel
#include "RichKernel/RichDAQHeaderPD_V4.h"
#include "RichKernel/RichDAQParityFooter.h"
#include "RichKernel/RichDAQL1IngressHeader.h"

namespace Rich
{
  namespace DAQ
  {

    /** @class HPDInfo RichKernel/RichDecodedData.h
     *  Decoded HPD information
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/04/2007
     */
    class HPDInfo
    {

    public:

      /// The HPD Header type
      typedef RichDAQHeaderV4::RichDAQHeaderPD Header;
      /// The HPD Footer type
      typedef ParityFooter                     Footer;
      
    public:

      /// Default Constructor
      HPDInfo() { }

      /// Constructor from info
      HPDInfo( const LHCb::RichSmartID& input,
               const Header&            header,
               const Footer&            footer )
        : m_hpdID  ( input  ),
          m_header ( header ),
          m_footer ( footer ) 
      { }

    public:

      /**  Access the HPD ID (LHCb::RichSmartID) for this HPD
       *   @attention It is possible this HPDID is invalid (for instance
       *              if the L0ID->RichSmartID database lookup failed.
       *              Users show check for validity using the method
       *              RichSmartID::isvalid()
       */
      inline const LHCb::RichSmartID& hpdID() const { return m_hpdID; }
      /// Access the Header words
      inline const Header& header()           const { return m_header; }
      /// Access the Footer words
      inline const Footer& footer()           const { return m_footer; }

      /// set the Level1Input
      inline void setHpdID( const LHCb::RichSmartID& input ) { m_hpdID = input; }
      /// Set the Header
      inline void setHeader( const Header & header ) { m_header = header; }
      /// Set the footer
      inline void setFooter( const Footer & footer ) { m_footer = footer; }

      /// Read access to the RichSmartIDs for the hit pixels in this HPD
      inline const LHCb::RichSmartID::Vector& smartIDs() const { return m_smartIds; }
      /// Write access to the RichSmartIDs for the hit pixels in this HPD
      inline       LHCb::RichSmartID::Vector& smartIDs()       { return m_smartIds; }

    private:

      LHCb::RichSmartID m_hpdID;            ///< The RichSmartID for this HPD
      Header m_header;                      ///< The HPDHeader word
      Footer m_footer;                      ///< The HPDFooter word
      LHCb::RichSmartID::Vector m_smartIds; ///< The decoded pixels in this HPD

    };

    /// Map for RICH HPD data, sorted by HPD number
    typedef std::unordered_map< const Rich::DAQ::Level1Input, HPDInfo > HPDMap;

    /** @class IngressInfo RichKernel/RichDecodedData.h
     *  Information for each L1 ingress
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/04/2007
     */
    class IngressInfo
    {

    public:

      /// Default Constructor
      IngressInfo() { }

    public:

      /// Get the L1IngressHeader
      inline const L1IngressHeader& ingressHeader() const { return m_ingressHeader; }
      /// Read access to the HPD data for this Ingress
      inline const HPDMap & hpdData() const { return m_hpdData; }
      /// Write access to the HPD data for this Ingress
      inline       HPDMap & hpdData()       { return m_hpdData; }
      /// Set the L1IngressHeader
      inline void setIngressHeader( const L1IngressHeader& head ) { m_ingressHeader = head; }

    private:

      L1IngressHeader m_ingressHeader; ///< The ingress header word
      HPDMap          m_hpdData;       ///< The HPD data blocks for this ingress

    };

    /// Map for HPD data for each L1 ingress
    typedef std::unordered_map< const L1IngressID, IngressInfo >      IngressMap;

    /// Map for RICH HPD data, sorted by Level1 board number
    typedef std::unordered_map< const Level1HardwareID, IngressMap >  L1Map;

  }
}

#endif // RICHKERNEL_RICHDECODEDDATA_H
