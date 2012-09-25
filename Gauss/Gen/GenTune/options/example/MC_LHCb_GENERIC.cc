// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/RivetAIDA.hh"
#include "Rivet/Tools/Logging.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "LWH/Histogram1D.h"
#include <cmath>

namespace Rivet {


  /// Generic analysis looking at various distributions of final state particles
  class MC_LHCb_GENERIC : public Analysis {
  public:

    /// Constructor
    MC_LHCb_GENERIC()
      : Analysis("MC_LHCb_GENERIC")
    {    }


  public:

    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Projections
      const FinalState cnfs(-10.0, 10.0, 150*MeV);
      addProjection(cnfs, "FS");
      addProjection(ChargedFinalState(-10.0, 10.0, 150*MeV), "CFS");

      // Histograms
      // @todo Choose E/pT ranged based on input energies... can't do anything about kin. cuts, though
      _histMult   = bookHistogram1D("Mult", 100, -0.5, 199.5);
      _histMultCh = bookHistogram1D("MultCh", 100, -0.5, 199.5);

      _histPt    = bookHistogram1D("Pt", 300, 0, 30);
      _histPtCh  = bookHistogram1D("PtCh", 300, 0, 30);

      _histPx = bookHistogram1D("Px", 2000, -100, 100);
      _histPy = bookHistogram1D("Py", 2000, -100, 100);

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
        if (isinf(rapidity)) {MSG_ERROR("Rapidity reached infinity!"); continue;};
        if (isnan(rapidity)) {MSG_ERROR("Rapidity is NAN for particle " << p.pdgId() << " !"); continue; };
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
        if (isnan(rapidity)) continue;
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

      histogramFactory().divide(histoPath("EtaPMRatio"), *_tmphistEtaPlus, *_tmphistEtaMinus);
      histogramFactory().divide(histoPath("EtaChPMRatio"), *_tmphistEtaChPlus, *_tmphistEtaChMinus);
      histogramFactory().divide(histoPath("RapidityPMRatio"), *_tmphistRapPlus, *_tmphistRapMinus);
      histogramFactory().divide(histoPath("RapidityChPMRatio"), *_tmphistRapChPlus, *_tmphistRapChMinus);
    }

    //@}


  private:

    /// Temporary histos used to calculate eta+/eta- ratio plot
    shared_ptr<LWH::Histogram1D> _tmphistEtaPlus, _tmphistEtaMinus;
    shared_ptr<LWH::Histogram1D> _tmphistEtaChPlus, _tmphistEtaChMinus;
    shared_ptr<LWH::Histogram1D> _tmphistRapPlus, _tmphistRapMinus;
    shared_ptr<LWH::Histogram1D> _tmphistRapChPlus, _tmphistRapChMinus;

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
    //@}

  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(MC_LHCb_GENERIC);

}
