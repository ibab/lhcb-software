// $Id: $
// stdlib
#include <exception>
#include <math.h>
#include <iostream>

// boost
#include <boost/foreach.hpp>

// AIDA
#include <AIDA/IHistogram1D.h>
#include <AIDA/IHistogram2D.h>
#include <AIDA/IProfile1D.h>
#include <AIDA/IAxis.h>

#include <GaudiKernel/IIncidentSvc.h>

// local
#include "HistoWrapper.h"
#include "HltMassMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HistoWrapper
//
// 2011-04-23 : Roel Aaij
//-----------------------------------------------------------------------------

//=============================================================================
HistoWrapper::HistoWrapper( HltMassMonitor* algo, const std::string& histoName,
                            const std::string& decision,
                            const std::vector< double >& massDef)
   : HistoWrapper(algo, histoName, decision, massDef, {0.0})
{

}

//=============================================================================
HistoWrapper::HistoWrapper( HltMassMonitor* algo, const std::string& histoName,
                            const std::string& decision,
                            const std::vector< double >& massDef,
                            const std::vector< double >& massStruct)
   :  m_name( histoName ),
      m_decision( decision ),
      m_massDef( massDef ),
      m_massStruct( massStruct )
{
   if ( m_massDef.size() != 3 ) {
      throw std::exception();
   }

   auto monSvc = algo->hltMonSvc();
   if (monSvc.isValid()) {
      auto bins = boost::numeric_cast<size_t>(massDef[2]);
      HltHistogram& mass = monSvc->histogram(algo->name() + "/" + name() + "_mass",
                                             massDef[0], massDef[1], bins);
      m_mass = Wrapper(&mass);
      HltHistogram& pt = monSvc->histogram(algo->name() + "/" + name() + "_pT",
                                           0., 6000., 100);
      m_pT = Wrapper(&pt);
   } else {
      m_mass = Wrapper(algo->book1D(name() + "_mass", name() + " invariant mass",
                                     left(), right(), bins()));
      m_pT   = Wrapper(algo->book1D(name() + "_pT",  0., 6000., 100));
   }
}

//=============================================================================
HistoWrapper::~HistoWrapper()
{

}

//=============================================================================
void HistoWrapper::fill(const LHCb::HltSelReports* selReports)
{
   const LHCb::HltObjectSummary* selReport = selReports->selReport(decision());
   if (!selReport) return;

   int iteration=0;
   for (const auto& candidate : selReport->substructure()) {
      iteration++;

      float pT=0.;
      float tx=0.;
      float ty=0.;
      float qoverp=0.;
      float p=0.;
      float normfactor = 0.;
      tx = candidate->numericalInfo()[ "3#Track.firstState.tx" ];
      ty = candidate->numericalInfo()[ "4#Track.firstState.ty" ];
      qoverp = candidate->numericalInfo()[ "5#Track.firstState.qOverP" ];
      if (qoverp > 0) p = 1 / qoverp;
      if (qoverp < 0) p = -1 / qoverp;
      normfactor = sqrt(tx * tx + ty * ty + 1);
      pT = sqrt(tx * tx + ty * ty) * p / normfactor;
      if (pT > 0) {
         m_pT.fill(pT);
      }

      //compute mass depending on structure
      int massdepth = m_massStruct.size();
      float mass = 0.;
      if (massdepth == 1) {
         auto info = candidate->numericalInfo();
         if (!info.count("1#Particle.measuredMass"))
            continue;
         mass = candidate->numericalInfo()[ "1#Particle.measuredMass" ];
      } else {
         //We need to dig into the substructure
         float cand_E = 0., cand_px = 0.,cand_py = 0.,cand_pz = 0.;
         int index = -1;
         float candcharge = 1.;
         for (const auto& child : candidate->substructure()) {
           ++index;
           //Check that the child is a track or we need to dig deeper
           auto childtrack = child.data();
           while (true) {
             if (childtrack->numericalInfo().size()==0) {
               childtrack = childtrack->substructure()[0].data();
             } else {
               break;
             }
           }
           float thismass = m_massStruct[index];
           tx = childtrack->numericalInfo()[ "3#Track.firstState.tx" ];
           ty = childtrack->numericalInfo()[ "4#Track.firstState.ty" ];
           p  = fabs(1./childtrack->numericalInfo()[ "5#Track.firstState.qOverP" ]);
           candcharge *= childtrack->numericalInfo()[ "5#Track.firstState.qOverP" ]*p;
           normfactor = sqrt(tx*tx+ty*ty+1);
           cand_px += tx*p/normfactor;
           cand_py += ty*p/normfactor;
           cand_pz += 1.*p/normfactor;
           cand_E  += sqrt(thismass*thismass + p*p);
         }
         float m2 = cand_E*cand_E - cand_px*cand_px - cand_py*cand_py - cand_pz*cand_pz;
         if ((m2 < 0.) || (candcharge > 0)) {
            mass = -999.; //Should raise exception but can't figure out what to include
                          //for this to happen
         } else {
            mass = sqrt(m2);
         }
      }
      m_mass.fill(mass);
   }
}
