#ifndef RICHDET_PARTICLECODE_H
#define RICHDET_PARTICLECODE_H 1

typedef int ParticleCode;

enum {
  UnknownParticle = -1,
  Electron,
  Muon,
  Pion,
  Kaon,
  Proton,
  MaxParticleCode
};

#endif // RICHDET_PARTICLECODE_H
