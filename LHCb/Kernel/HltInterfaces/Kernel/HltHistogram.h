#ifndef KERNEL_HLTHISTOGRAM_H
#define KERNEL_HLTHISTOGRAM_H 1

// Include files

// Gaudi
#include <GaudiKernel/HistoDef.h>

// boost
#include <boost/numeric/conversion/cast.hpp>

// local
#include <Kernel/IHltMonitorSvc.h>

/** @class HltHistogram HltHistogram.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-01
 */
class HltHistogram {
public:
   /// Standard constructor
   HltHistogram(IHltMonitorSvc* const monSvc, Gaudi::StringKey id,
                double left, double right, size_t bins)
      : m_id{id},
        m_monSvc{monSvc},
        m_def{id.str(), left, right, boost::numeric_cast<int>(bins)}
   {
      m_diff = (right - left) / bins;
   }

   virtual ~HltHistogram( ) { }; ///< Destructor

   // Count at time t
   void fill(double value) {
      if (value < left()) {
         return;
      }
      auto bin = boost::numeric_cast<size_t>((value - left()) / m_diff);
      if (bin > bins())
         return;
      m_monSvc->fill(m_id, bin);
   }

   Gaudi::StringKey id() const
   {
      return m_id;
   }

   double left()  const { return m_def.lowEdge(); }
   double right() const { return m_def.highEdge(); }
   size_t bins() const { return m_def.bins(); }
   const Gaudi::Histo1DDef& def() const { return m_def; }

private:

   const Gaudi::StringKey m_id;
   IHltMonitorSvc* const m_monSvc;
   Gaudi::Histo1DDef m_def;
   double m_diff;

};

#endif // KERNEL_HLTHISTOGRAM_H
