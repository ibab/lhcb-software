// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/RivetAIDA.hh"
#include "Rivet/Tools/Logging.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/UnstableFinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "LWH/Histogram1D.h"
#include <cmath>

namespace Rivet {


  /// Generic analysis looking at various distributions of final state particles
  class MC_LHCb_GENERIC : public Analysis {
  public:

    /// Constructor
    MC_LHCb_GENERIC()
      : Analysis("MC_LHCb_GENERIC"),
        nerrinf(0), nerrnan(0)
    { 
       nphotons = 0;
       npions = 0;
       minPhtPx = 0.;
       maxPhtPx = 0.;
       minPhtPz = 0.;
       maxPhtPz = 0.;
       minPi0Px = 0.;
       maxPi0Px = 0.;
       minPi0Pz = 0.;
       maxPi0Pz = 0.;
       minPhtM = 0.; maxPhtM = 0.;
       minPi0M = 0.; maxPi0M = 0.;
    }


  public:

    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Projections
      const FinalState cnfs(-10.0, 10.0, 150*MeV);
      addProjection(UnstableFinalState(-10.0,10.0, 100*MeV), "UFS");
      addProjection(cnfs, "FS");
      addProjection(ChargedFinalState(-10.0, 10.0, 150*MeV), "CFS");

      // Histograms
      // @todo Choose E/pT ranged based on input energies... can't do anything about kin. cuts, though
      _histMult   = bookHistogram1D("Mult", 100, -0.5, 199.5);
      _histMultCh = bookHistogram1D("MultCh", 100, -0.5, 199.5);

      _histPt    = bookHistogram1D("Pt", 300, 0, 30);
      _histPtCh  = bookHistogram1D("PtCh", 300, 0, 30);

      _histPx = bookHistogram1D("Px", 1000, -50, 50);
      _histPy = bookHistogram1D("Py", 1000, -50, 50);

      _hPhtPx = bookHistogram2D("PhtPxM", 40, -0.2, 0.2, 40, -4., 4.);
      _hPi0Px = bookHistogram2D("Pi0PxM", 50, -0.5, 0.5, 40, -4., 4.);
      _hPhtPz = bookHistogram2D("PhtPzM", 40, -0.2, 0.2, 120, -600., 600.);
      _hPi0Pz = bookHistogram2D("Pi0PzM", 100, -1., 1., 200, -3400., 3400.);

      _histE    = bookHistogram1D("E", 100, 0, 200);
      _histECh  = bookHistogram1D("ECh", 100, 0, 200);

      _histEta    = bookHistogram1D("Eta", 100, -10, 10);
      _histEtaCh  = bookHistogram1D("EtaCh", 100, -10, 10);
      _tmphistEtaPlus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistEtaMinus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistEtaChPlus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistEtaChMinus.reset(new LWH::Histogram1D(50, 0, 10));

      _histEtaSumEt    = bookProfile1D("EtaSumEt", 50, 0, 10);

      _histRapidity    = bookHistogram1D("Rapidity", 100, -10, 10);
      _histRapidityCh  = bookHistogram1D("RapidityCh", 100, -10, 10);
      _tmphistRapPlus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistRapMinus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistRapChPlus.reset(new LWH::Histogram1D(50, 0, 10));
      _tmphistRapChMinus.reset(new LWH::Histogram1D(50, 0, 10));

      _histPhi    = bookHistogram1D("Phi", 50, 0, TWOPI);
      _histPhiCh  = bookHistogram1D("PhiCh", 50, 0, TWOPI);
    }



    /// Perform the per-event analysis
    void analyze(const Event& event) {
      const double weight = event.weight();

      // Iterate through unstable particle list
      const UnstableFinalState& ufs = applyProjection<UnstableFinalState>(event, "UFS");
      foreach (const Particle&p, ufs.particles()) {
        if ( p.pdgId() == 111 ) {
          _hPi0Px->fill(p.momentum().mass2()/GeV/GeV, p.momentum().px()/GeV, weight);
          _hPi0Pz->fill(p.momentum().mass2()/GeV/GeV, p.momentum().pz()/GeV, weight);
          npions++;
          if (minPi0Px > p.momentum().px()) minPi0Px = p.momentum().px();
          if (maxPi0Px < p.momentum().px()) maxPi0Px = p.momentum().px();
          if (minPi0Pz > p.momentum().pz()) minPi0Pz = p.momentum().pz();
	  if (maxPi0Pz < p.momentum().pz()) maxPi0Pz = p.momentum().pz();
	  if (minPi0M > p.momentum().mass2()) minPi0M = p.momentum().mass2();
          if (maxPi0M < p.momentum().mass2()) maxPi0M = p.momentum().mass2();
        };
      }

      // Charged + neutral final state
      const FinalState& cnfs = applyProjection<FinalState>(event, "FS");
      MSG_DEBUG("Total multiplicity = " << cnfs.size());
      _histMult->fill(cnfs.size(), weight);
      foreach (const Particle& p, cnfs.particles()) {
        const double eta = p.momentum().eta();
        MSG_DEBUG("Filling eta!");
        _histEta->fill(eta, weight);
        MSG_DEBUG("Filling eta sum E_T!");
        _histEtaSumEt->fill(fabs(eta), p.momentum().Et(), weight);
        if (eta > 0) {
          MSG_DEBUG("Filling eta plus!");
          _tmphistEtaPlus->fill(fabs(eta), weight);
        } else {
          MSG_DEBUG("Filling eta minus!");
          _tmphistEtaMinus->fill(fabs(eta), weight);
        }
        const double rapidity = p.momentum().rapidity();
        MSG_DEBUG("Filling rapidity (" << (float)rapidity << ")!");
        if (isinf(rapidity)) {
          if (nerrinf < 100) MSG_ERROR("Rapidity is Inf for particle " << p.pdgId() << " (status=" << p.genParticle().status()  << ")..."); 
          nerrinf++;
          continue;
        };
        if (isnan(rapidity)) {
          if (nerrnan < 100) MSG_ERROR("Rapidity is NaN for particle " << p.pdgId() << " (status=" << p.genParticle().status() << ")...");
          nerrnan++;
          continue;
        };
        _histRapidity->fill(rapidity, weight);
        if (rapidity > 0) {
          MSG_DEBUG("Filling rapidity plus!");
          _tmphistRapPlus->fill(fabs(rapidity), weight);
        } else {
          MSG_DEBUG("Filling rapidity minus!");
          _tmphistRapMinus->fill(fabs(rapidity), weight);
        }
        if ((&p) == NULL) {MSG_ERROR("Null particle reference!"); continue;};
        MSG_DEBUG("px = " << p.momentum().px()/GeV << " GeV/c");
        MSG_DEBUG("py = " << p.momentum().py()/GeV << " GeV/c");
        if ((&AIDA::IHistogram1D::fill) == NULL) {MSG_ERROR("Null fill reference!"); continue;};
        MSG_DEBUG("Filling px!");
        _histPx->fill(p.momentum().px()/GeV, weight);
        MSG_DEBUG("Filling py!");
        _histPy->fill(p.momentum().py()/GeV, weight);
        MSG_DEBUG("Filling pT!");
        _histPt->fill(p.momentum().pT()/GeV, weight);
        MSG_DEBUG("Filling E!");
        _histE->fill(p.momentum().E()/GeV, weight);
        MSG_DEBUG("Filling phi!");
        _histPhi->fill(p.momentum().phi(), weight);
        if ( p.pdgId() == 22 ) {
          _hPhtPx->fill(p.momentum().mass2()/GeV/GeV, p.momentum().px()/GeV, weight);
          _hPhtPz->fill(p.momentum().mass2()/GeV/GeV, p.momentum().pz()/GeV, weight);
          nphotons++;
          if (minPhtPx > p.momentum().px()) minPhtPx = p.momentum().px();
          if (maxPhtPx < p.momentum().px()) maxPhtPx = p.momentum().px();
          if (minPhtPz > p.momentum().pz()) minPhtPz = p.momentum().pz();
	  if (maxPhtPz < p.momentum().pz()) maxPhtPz = p.momentum().pz();
	  if (minPhtM > p.momentum().mass2()) minPhtM = p.momentum().mass2();
          if (maxPhtM < p.momentum().mass2()) maxPhtM = p.momentum().mass2();
        };
      }
      const FinalState& cfs = applyProjection<FinalState>(event, "CFS");
      MSG_DEBUG("Total charged multiplicity = " << cfs.size());
      _histMultCh->fill(cfs.size(), weight);
      foreach (const Particle& p, cfs.particles()) {
        const double eta = p.momentum().eta();
        _histEtaCh->fill(eta, weight);
        if (eta > 0) {
          _tmphistEtaChPlus->fill(fabs(eta), weight);
        } else {
          _tmphistEtaChMinus->fill(fabs(eta), weight);
        }
        const double rapidity = p.momentum().rapidity();
        if (isnan(rapidity) || isinf(rapidity)) continue; //avoid events with bad data!
        _histRapidityCh->fill(rapidity, weight);
        if (rapidity > 0) {
          _tmphistRapChPlus->fill(fabs(rapidity), weight);
        } else {
          _tmphistRapChMinus->fill(fabs(rapidity), weight);
        }
        _histPtCh->fill(p.momentum().pT()/GeV, weight);
        _histECh->fill(p.momentum().E()/GeV, weight);
        _histPhiCh->fill(p.momentum().phi(), weight);
      }


    }



    /// Finalize
    void finalize() {
      scale(_histMult, 1/sumOfWeights());
      scale(_histMultCh, 1/sumOfWeights());

      scale(_histEta, 1/sumOfWeights());
      scale(_histEtaCh, 1/sumOfWeights());

      scale(_histRapidity, 1/sumOfWeights());
      scale(_histRapidityCh, 1/sumOfWeights());

      scale(_histPt, 1/sumOfWeights());
      scale(_histPtCh, 1/sumOfWeights());

      scale(_histPx, 1/sumOfWeights());
      scale(_histPy, 1/sumOfWeights());

      scale(_histE, 1/sumOfWeights());
      scale(_histECh, 1/sumOfWeights());

      scale(_histPhi, 1/sumOfWeights());
      scale(_histPhiCh, 1/sumOfWeights());

      scale(_hPhtPx, 1/sumOfWeights());
      scale(_hPi0Px, 1/sumOfWeights());
      scale(_hPhtPz, 1/sumOfWeights());
      scale(_hPi0Pz, 1/sumOfWeights());

      histogramFactory().divide(histoPath("EtaPMRatio"), *_tmphistEtaPlus, *_tmphistEtaMinus);
      histogramFactory().divide(histoPath("EtaChPMRatio"), *_tmphistEtaChPlus, *_tmphistEtaChMinus);
      histogramFactory().divide(histoPath("RapidityPMRatio"), *_tmphistRapPlus, *_tmphistRapMinus);
      histogramFactory().divide(histoPath("RapidityChPMRatio"), *_tmphistRapChPlus, *_tmphistRapChMinus);
      
      cout << "=======================================" << std::endl;
      cout << "Number of photons: " << nphotons << std::endl;
      cout << "min px, max px: " << minPhtPx << " , " << maxPhtPx << std::endl;
      cout << "min pz, max pz: " << minPhtPz << " , " << maxPhtPz << std::endl;
      cout << "min M, max M: " << minPhtM << " , " << maxPhtM << std::endl;
      cout << "=======================================" << std::endl;
      cout << "Number of neutral pions: " << npions << std::endl;
      cout << "min px, max px: " << minPi0Px << " , " << maxPi0Px << std::endl;
      cout << "min pz, max pz: " << minPi0Pz << " , " << maxPi0Pz << std::endl;
      cout << "min M, max M: " << minPi0M << " , " << maxPi0M << std::endl;
      cout << "=======================================" << std::endl;
      cout << "Number of particles with rapidity=NaN: " << nerrnan << std::endl;
      cout << "Number of particles with rapidity=Inf: " << nerrinf << std::endl;
      cout << "=======================================" << std::endl;
    }

    //@}


  private:

    /// Temporary histos used to calculate eta+/eta- ratio plot
    shared_ptr<LWH::Histogram1D> _tmphistEtaPlus, _tmphistEtaMinus;
    shared_ptr<LWH::Histogram1D> _tmphistEtaChPlus, _tmphistEtaChMinus;
    shared_ptr<LWH::Histogram1D> _tmphistRapPlus, _tmphistRapMinus;
    shared_ptr<LWH::Histogram1D> _tmphistRapChPlus, _tmphistRapChMinus;
    /// Counters for error suppression
    unsigned int nerrinf;
    unsigned int nerrnan;

    /// Temp variables:
    unsigned int nphotons;
    double maxPhtPx, minPhtPx, minPhtPz, maxPhtPz, minPhtM, maxPhtM;
    unsigned int npions;
    double maxPi0Px, minPi0Px, minPi0Pz, maxPi0Pz, minPi0M, maxPi0M;

    /// @name Histograms
    //@{
    AIDA::IHistogram1D *_histMult, *_histMultCh;
    AIDA::IProfile1D   *_histEtaSumEt;
    AIDA::IHistogram1D *_histEta, *_histEtaCh;
    AIDA::IHistogram1D *_histRapidity, *_histRapidityCh;
    AIDA::IHistogram1D *_histPt, *_histPtCh;
    AIDA::IHistogram1D *_histPx, *_histPy;
    AIDA::IHistogram1D *_histE, *_histECh;
    AIDA::IHistogram1D *_histPhi, *_histPhiCh;
    AIDA::IHistogram2D *_hPhtPx, *_hPi0Px;
    AIDA::IHistogram2D *_hPhtPz, *_hPi0Pz;
    //@}

  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(MC_LHCb_GENERIC);

}
