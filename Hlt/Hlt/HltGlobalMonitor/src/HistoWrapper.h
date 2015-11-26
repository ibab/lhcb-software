// $Id: $
#ifndef HISTOWRAPPER_H
#define HISTOWRAPPER_H 1

// stdlib
#include <string>

// STL
#include <vector>
#include <array>

// boost
#include <boost/numeric/conversion/cast.hpp>

// Gaudi/LHCb
#include <GaudiAlg/GaudiHistoAlg.h>
#include <Event/HltSelReports.h>
#include <GaudiUtils/Aida2ROOT.h>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

// Kernel
#include <Kernel/HltHistogram.h>

// ROOT
#include <TH1.h>

#include <iostream>

namespace LHCb {
   class ODIN;
}

/** @class HistoWrapper HistoWrapper.h
 *  Simple wrapper class which contains some info about
 *
 *  @author Roel Aaij
 *  @date   2011-04-23
 */

class HltHistogram;
class HltMassMonitor;

class HistoWrapper  {
public:
   /// Standard constructor
   HistoWrapper( HltMassMonitor* algo, const std::string& name,
                 const std::string& decision,
                 const std::vector< double >& massDef);

   HistoWrapper( HltMassMonitor* algo, const std::string& name,
                 const std::string& decision,
                 const std::vector< double >& massDef,
                 const std::vector< double >& massStruct );

   virtual ~HistoWrapper( ); ///< Destructor

   void fill(const LHCb::ODIN* odin, const LHCb::HltSelReports* selReports);

private:

   TH1* toROOT( AIDA::IHistogram1D* histo )
   {
      return Gaudi::Utils::Aida2ROOT::aida2root( histo );
   }

   double left()       const { return m_massDef[ 0 ]; }
   double right()      const { return m_massDef[ 1 ]; }
   unsigned int bins() const
   {
      return boost::numeric_cast< unsigned int >( m_massDef[ 2 ] );
   }

   const std::string& name()     const { return m_name;     }
   const std::string& decision() const { return m_decision; }

   // Data members
   const std::string m_name;
   const std::string m_decision;

   class Wrapper {
   public:

      Wrapper() = default;
      Wrapper(AIDA::IHistogram1D* a, HltHistogram* h) : m_hlt(h), m_aida(a) {}
      Wrapper(const Wrapper& other) : m_hlt(other.m_hlt),
                                      m_aida(other.m_aida) {}

      Wrapper& operator=(const Wrapper& other) {
         if (&other == this) {
            return *this;
         }
         m_hlt = other.m_hlt;
         m_aida = other.m_aida;
         return *this;
      }

      void fill(double value)
      {
         if (m_hlt) {
            m_hlt->fill(value);
         }
         if (m_aida) {
            m_aida->fill(value);
         }
      }

   private:

      HltHistogram* m_hlt = nullptr;
      AIDA::IHistogram1D* m_aida = nullptr;

   };

   struct CandInfo {
      CandInfo() : mass(0), pT(0) { }

      CandInfo(double _mass, double _pT)
         : mass(_mass), pT(_pT) {}
      double mass;
      double pT;
   };

   Wrapper m_mass;
   Wrapper m_pT;

   uint32_t m_initial;
   std::vector<CandInfo> m_masses;
   const std::vector<double> m_massDef;
   const std::vector<double> m_massStruct;

};
#endif // HISTOWRAPPER_H
