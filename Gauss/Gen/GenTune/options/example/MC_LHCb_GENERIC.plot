.*/MC_LHCb_GENERIC/.*::LogY=1

# BEGIN PLOT .*/MC_LHCb_GENERIC/E$
Title=Energy of all particles
XLabel=$E$ [GeV]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}E$ [GeV$^{-1}$]
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/ECh
Title=Energy of charged particles
XLabel=$E$ [GeV]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}E$ [GeV$^{-1}$]
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/Eta
XLabel=$\eta$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}\eta$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/Eta$
Title=Pseudorapidity of all particles
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/EtaCh
Title=Pseudorapidity of charged particles
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/Eta.*PMRatio
XLabel=$|\eta|$
YLabel=$\mathrm{d}\sigma/\mathrm{d}\eta_+ \; / \; \mathrm{d}\sigma/\mathrm{d}\eta_-$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/EtaPMRatio
Title=Pseudorapidity $+/-$ ratio of all particles
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/EtaChPMRatio
Title=Pseudorapidity $+/-$ ratio of charged particles
# END PLOT

# BEGIN PLOT .*/MC_LHCb_GENERIC/EtaSumEt
Title=Pseudorapidity distribution of $\langle \sum E_\perp \rangle$
XLabel=$\eta$
YLabel=$\langle \sum E_\perp \rangle$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/MultCh
Title=Charged multiplicity of events
XLabel=$N_\text{ch}$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}N_\text{ch}$
LogY=0
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Mult$
Title=Total multiplicity of events
XLabel=$N$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}N$
LogY=0
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Phi$
Title=Azimuthal distribution of all particles
XLabel=$\phi$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}\phi$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/PhiCh
Title=Azimuthal distribution of charged particles
XLabel=$\phi$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}\phi$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Pt$
Title=Transverse momentum of all particles
XLabel=$p_\perp$ [GeV]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}p_\perp$ [GeV$^{-1}$]
FullRange=1
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/PtCh
Title=Transverse momentum of charged particles
XLabel=$p_\perp$ [GeV]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}p_\perp$ [GeV$^{-1}$]
FullRange=1
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Rapidity$
Title=Rapidity of all particles
XLabel=$y$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}y$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/RapidityPMRatio
Title=Rapidity $+/-$ ratio of all particles
XLabel=$|y|$
YLabel=$\mathrm{d}\sigma/\mathrm{d}y_+ \; / \; \mathrm{d}\sigma/\mathrm{d}y_-$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/RapidityCh$
Title=Rapidity of charged particles
XLabel=$y$
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}y$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/RapidityChPMRatio
Title=Rapidity $+/-$ ratio of charged particles
XLabel=$|y|$
YLabel=$\mathrm{d}\sigma/\mathrm{d}y_+ \; / \; \mathrm{d}\sigma/\mathrm{d}y_-$
LogY=0
LegendYPos=0.5
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Px$
Title=X-component of momentum of all particles
XLabel=$p_x$ [GeV/c]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}p_x$ [GeV$^{-1} c$]
FullRange=1
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Py$
Title=Y-component of momentum of all particles
XLabel=$p_y$ [GeV/c]
YLabel=$1/\sigma \; \mathrm{d}\sigma/\mathrm{d}p_y$ [GeV$^{-1} c$]
FullRange=1
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/PhtPxM
Title=Final state photons $p_x$ vs. $m^2$
XLabel=$m_{0}^{2}$ [GeV$^2$/$c$]
YLabel=$p_{x}^{\gamma}$ [GeV/$c$]
LogY=0
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/PhtPzM
Title=Final state photons $p_z$ vs. $m^2$
XLabel=$m_{0}^{2}$ [GeV$^2$/$c$]
YLabel=$p_{z}^{\gamma}$ [GeV/$c$]
LogY=0
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Pi0PxM
Title=$p_x$ vs. $m^2$ for $\pi^0$
XLabel=$m_{\pi^0}^{2}$ [GeV$^2$/$c$]
YLabel=$p_{x}$ [GeV/$c$]
LogY=0
# END PLOT

# BEGIN PLOT /MC_LHCb_GENERIC/Pi0PzM
Title=$p_z$ vs. $m^2$ for $\pi^0$
XLabel=$m_{\pi^0}^{2}$ [GeV$^2$/$c$]
YLabel=$p_{z}$ [GeV/$c$]
LogY=0
# END PLOT

