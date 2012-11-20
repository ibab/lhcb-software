      subroutine vbf_h_init_hist
      call vbf_h_pwhgINIHIST
      end

      subroutine vbf_h_analysis(dsig)
      real * 8 dsig
      end


      subroutine vbf_h_particle_identif(HWW,HZZ)
      implicit none
      integer pdg_Higgs,pdg_Z,pdg_W,HZZ,HWW
      pdg_Higgs = 25
      pdg_Z = 23
      pdg_W = 24      
c     build an identifier for Higgs production in WW and ZZ fusion 
      HWW = 10000*pdg_W + pdg_Higgs
      HZZ = 10000*pdg_Z + pdg_Higgs
      end




c      subroutine topout
c      implicit none
c      include 'src/Lib/vbf_h/hepevt.h'
c      character * 50 title
c      integer i
c      integer maxnumplot
c      common/vbf_h_cmaxnumplot/maxnumplot
c      logical lin(100)
c      common/vbf_h_lin_scale/lin
c      character * 3 scale
cc     
cc     If histogram I contains accumulated weights and
cc     histogram I+100 contains its squared values,
cc     then a temporary copy of both is made in order
cc     to safely have intermediate results
c
c      do i=1,maxnumplot
c	call vbf_h_PWHGFINAL(i)
c        call vbf_h_PWHGCOPY(i,i+200)
c        call vbf_h_PWHGCOPY(i+100,i+300)
c        call vbf_h_PWHGOPERA(i+200,'F',i+200,i+200,1d0/dble(nevhep),0d0)
c        call pwhgerror(i+200,i+300,dble(nevhep))
c        call vbf_h_PWHGFINAL(i+200)
c        call vbf_h_PWHGFINAL(i+300)
c      enddo
c      do i=1,maxnumplot
c         call vbf_h_pwhggettitle(i+200,title)
c         if (lin(i)) then
c            scale = 'LIN'
c         else
c            scale = 'LOG'
c         endif
c         call vbf_h_PWHGMULTITOP(i+200,i+300,2,3,title,' ',scale)
c      enddo
c      end            





