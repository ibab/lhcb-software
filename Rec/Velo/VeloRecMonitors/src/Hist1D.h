// $Id: Hist1D.h,v 1.4 2010-05-05 13:17:00 krinnert Exp $
#ifndef INCLUDE_HIST1D_H
#define INCLUDE_HIST1D_H 1
#include <numeric>

/** @class Hist1D Hist1D.h
 * 
 * A wrapper for ROOT TH1D the speeds up filling and
 * setting bin contents by a factor of 10. 
 *
 * @author Kurt Rinnert <kurt.rinnert@cern.ch>
 * @date   2010-03-27
 */
namespace Velo {
class Hist1D {

	public:

		inline Hist1D(TH1D* h)
			: m_nBins(h->GetNbinsX())
			, m_min(h->GetXaxis()->GetXmin())
		  , m_max(h->GetXaxis()->GetXmax())
		 	, m_binWidth((m_max-m_min)/m_nBins)
		 	, m_entries(static_cast<unsigned int>(h->GetEntries()))
		 	, m_h(h)
		 	, m_array(h->GetArray())
	{ ; }

		/// slow fill method. under/overflow is handled properly
		inline void fill(const double& x)
		{
			++m_array[( x < m_min ? 0 : ( x > m_max ? m_nBins+1 : static_cast<unsigned int>((x-m_min)/m_binWidth)+1 ))];
			++m_entries;
		}

		/// fast fill method. clients must guarantee there is no under/overflow
		inline void fillFast(const double& x)
		{
			++m_array[static_cast<unsigned int>((x-m_min)/m_binWidth)+1];
			++m_entries;
		}

		/// no checks performed, bin must be a valid index
		inline void setBinContent(const unsigned int bin, const double& x)
		{
			m_array[bin] = x;
			++m_entries;
		}

		/// no checks performed, bin must be a valid index
		inline double getBinContent(const unsigned int bin)
		{
			return m_array[bin];
		}

		/// fast integral method, simply sums all bin contents
		inline double integral() const
		{
			return std::accumulate(m_array+1,m_array+m_nBins+1,0.0);
		}	

    /// multiply all bin contents by scale factor
    inline void scale(const double& s)
    {
      for (unsigned int i=1; i<m_nBins+1; ++i) {
        m_array[i] *= s;
      }
    }

    /// set number of entries, trigger update
    inline void setEntries(unsigned int n) 
    {
      m_entries = n;
			m_h->SetEntries(m_entries);
    }
    
		/// call this to sync the number of entries
		inline void updateEntries()
		{
			m_h->SetEntries(m_entries);
		}

		inline void reset()
		{
			m_h->Reset();
			m_entries = 0;
		}

	private:
		const unsigned int m_nBins;
		const double m_min;
		const double m_max;
		const double m_binWidth;
		unsigned int m_entries;
		TH1D* m_h;
		double* m_array;
};
}
#endif // INCLUDE_HIST1D_H

