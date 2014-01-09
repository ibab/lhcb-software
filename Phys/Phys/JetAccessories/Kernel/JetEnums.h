#ifndef LHCB_JETEXTRAINFO_ENUMS_H
#define LHCB_JETEXTRAINFO_ENUMS_H 1

namespace LHCb {
  namespace ParticleFlowType {
      const int KEY = 900;
      enum ParticleFlowTypes {
        Unknown=0,
        // Good Particles
        Charged = 1,
        ChargedHadron,
        Muon,
        Electron,
        Neutral = 10,
        Photon,
        Pi0,
        MergedPi0,
        ResolvedPi0,
        NeutralHadron,
        NeutralRecovery ,
        // Composite particles
        Composite = 100 ,
        V0,
        D,
        B,
        // Strange/Bad particles
        BadParticle = 1000,
        Charged0Momentum ,
        ChargedInfMomentum,
        BadPhotonMatchingT,
        BadPhoton,
        IsolatedPhoton,
        // Conmpostie Daughter
        Daughter = 10000
      };
  }
  namespace JetIDInfo {
      enum JetIDInfos {
        StartJetIDInfo         = 9000,
        Ntracks                = 9001,
        N90                    = 9002,
        MTF                    = 9003,
        NSatCalo               = 9004,
        NHasPV                 = 9005,
        CPF                    = 9006,
        JetWidth               = 9007,
        NSatECAL               = 9008,
        NSatHCAL               = 9009,
        NIPChi2Inf4            = 9010,
        MPT                    = 9011,
      };
  }
  namespace JECInfo {
      enum JECInfos {
        JEC                    = 9101,
        NPVsForJEC             = 9102,
        JECError               = 9103,
      };
  }
  namespace JetEnergyFractionInfo {
    enum JetEnergyFractionInfos {
        Charged                = 9201,
        ChargedHadron          = 9202,
        Muon                   = 9203,
        Electron               = 9204,
        Neutral                = 9205,
        Photon                 = 9206,
        Pi0                    = 9207,
        MergedPi0              = 9208,
        ResolvedPi0            = 9209,
        NeutralHadron          = 9210,
        NeutralRecovery        = 9211,
        Composite              = 9212,
        V0                     = 9213,
        D                      = 9214,
        B                      = 9215,
        BadParticle            = 9216,
        Charged0Momentum       = 9217,
        ChargedInfMomentum     = 9218,
        BadPhotonMatchingT     = 9219,
        BadPhoton              = 9220,
        IsolatedPhoton         = 9221,
    };
  }
}
#endif // LHCB_JETEXTRAINFO_ENUMS_H 1
