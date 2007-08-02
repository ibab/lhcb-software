C $Id: alpgen_main.F,v 1.1 2007-08-02 11:23:04 ibelyaev Exp $
      SUBROUTINE alpgen
c     Version to allow reading in a standard event, and perform the kt
C     clustering and alphas rescaling
C-----------------------------------------------------
c**********
c**********      program alpgen
c**********
C-------------------------------------------------------------------------
C 
C driver for multi-parton matrix element generator based on ALPHA
C
C--------------------------------------------------------------------------
C
      call alsprc
c
c     Routine location: XXX.f
c     Purpose: 
c        -  assign the hard process code for selected process XXX
c
C setup running defaults:
c
      call alsdef
c
c     Routine location: alpgen.f
c     Purpose: 
c        -  initialise the default generation parameters (e.g. beam
c           type, energy, PDF set) 
c        -  initialise default mass and couplings for particles
c
c
C setup  event generation options, bookkeeping, etc
      call alhset
c
c     Routine location: XXX.f
c     Purpose: 
c        - setup event generation options, bookkeeping, etc, for
c              the specific hard process XXX
c        - write run information on stat, unwpar files

c initialise alpha parameters
      call alinit
c     Routine location: alpgen.f
c     Purpose: 
c        - evaluate parameter-dependent quantities (e.g. Higgs width)
c        - fill the apar array of ALPHA (which contains all parameters
c          required by ALPHA
c
c
C setup internal bookkeeping histograms
      call alsbkk
c
c     Routine location: alpgen.f
c     Purpose: initialise histograms common to all processes, and needed
C     for internal purposes
c
c
C setup user histograms
      call alshis
c
c     Routine location: XXXusr.f
c     Purpose: initialise histograms
c
c
c
C setup integration grids, including optimization if required
c
      call alsgrd
c
c     Routine location: XXX.f
c     Purpose: setup integration grid variables
      call aligrd
c
c     Routine location: alpgen.f
c     Purpose: initialise grid with warm-up iterations, if required
c
c
C generate events
c
      Call alegen
c
c     Routine location: alpgen.f
c     Purpose: generates events, calling in a standad format the
c          the process-depepdent phase-space and flavour-selection
c          routines, contained in XXX.f

C finalise histograms
c
      call alfhis
c
c     Routine location: XXXusr.f
c     Purpose: finalize analysis and histograms
      call alfbkk
c
c     Routine location: alpgen.f
c     Purpose: finalize internal histograms
c
c
      end
