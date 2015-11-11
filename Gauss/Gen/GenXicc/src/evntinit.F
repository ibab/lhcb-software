
c*******************************************************
c*******************************************************

      subroutine evntinit
      implicit double precision(a-h, o-z)
	implicit integer(i-n)
#include "genxicc_constants.h"
!CC#include "invegas.h"

c------------pythia common block -----------------------

c...three pythia functions return integers, so need declaring.
      integer pyk,pychge,pycomp
c...external statement.
      external totfun

c...pythia common block.
      common/pyjets/n,npad,k(4000,5),p(4000,5),v(4000,5)
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &vtimup(maxnup),spinup(maxnup)
      save /hepeup/

      integer pdfgup,pdfsup,lprup
      common/heprup/idbmup(2),ebmup(2),pdfgup(2),pdfsup(2),
     &idwtup,nprup,xsecup(maxpup),xerrup(maxpup),xmaxup(maxpup),
     &lprup(maxpup)
      save /heprup/

c...user process event common block.                             
      common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)
      common/pydat3/mdcy(500,3),mdme(8000,2),brat(8000),kfdp(8000,5)

c****************************************************
c*************** define most of the common block used
c****************************************************

	double complex colmat,bundamp
      common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/counter/ixiccstate,nev
	common/mixevnt/imix,imixtype
      common/vegasbin/nvbin  ! F. Zhang 20-04-2011 IMPORTANT MODIFICATION !!!!
                             ! ":1,$s/NVEGBIN/nvbin/g"
	!common/grade/xi(nvbin,10)
	common/grade/xi(MAXNVBIN,10)
	common/vegcross/vegsec,vegerr,iveggrade

c...transform of running information.
	common/ptpass/ptmin,ptmax,crossmax,etamin,etamax,
     &	smin,smax,ymin,ymax,psetamin,psetamax
	common/histcol/inx

c...generate---switch for full events. vegasopen---switch for using
c...vegas.
	logical generate,vegasopen,usegrade,ggsub
	common/genefull/generate

c...parameters transformtion.
      common/funtrans/npdfu
	common/usertran/idpp
      common/vegasinf/number,nitmx

c...to get the distribution of an extra factor z=(2(k1+k2).p_bc)/shat.
      common/extraz/zfactor,zmin,zmax
	common/outpdf/ioutpdf,ipdfnum
      common/loggrade/ievntdis,igenerate,ivegasopen,igrade,iusecurdir
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

      character fn2*23, ffnew*24
      CHARACTER*472 BASENAME2,FULLPATHNAME,ENERGYNAME
	common/usedbyveg/fn2,BASENAME2,ENERGYNAME ! F. Zhang 23-04-2011

	dimension nfin(20),alamin(20)
c...data:lambda and n_f values for parton distributions. this is obtained
c...from pythia and used for vegas running.
      data alamin/0.177d0,0.239d0,0.247d0,0.2322d0,0.248d0,0.248d0,
     &0.192d0,0.326d0,2*0.2d0,0.2d0,0.2d0,0.29d0,0.2d0,0.4d0,5*0.2d0/,
     &nfin/20*4/


c  GG 16/02/2012 protect against nvbin>MAXNVBIN
      if (nvbin .gt. MAXNVBIN) then
         print*, 'GENXICC WARNING: NVBIN=',nvbin,
     &        ' too high: limited to ',MAXNVBIN
         nvbin=MAXNVBIN
      end if
c************************************************
c... open recording files.--record mainly the 
c... vegas running information
c************************************************
	call upopenfile

c************************************************
c...initial logical values.
	generate =.false.
	if(igenerate.eq.1) generate =.true.

	vegasopen=.false.
	if(ivegasopen.eq.1) vegasopen=.true.

	usegrade=.false.
	if(igrade.eq.1) usegrade=.true.

c--------------------------------------------
c...reference light-like momentum, which can be choosed arbitrarily.
c...this can be used to check tht rightness of the program.
c...--used only for s-wave gluon fusion mechanism.
	ggsub=.false.

	if(mgenxi.eq.1) then
	 if(imix.eq.0 .and. (ixiccstate.eq.1 .or. ixiccstate.eq.2))then
	  ggsub=.true.
	 end if
	 if(imix.eq.1 .and. imixtype.eq.1) ggsub=.true.
	end if

	if(mgenxi.eq.2)  ggsub=.true.
	if(mgenxi.eq.3)  ggsub=.true.

	if(isubonly.eq.1) ggsub=.true.

c..arbitrary light-like reference momentum, to be freely chosen.
	if(ggsub) then
	 pmomup(1,8)=3.0d0
       pmomup(2,8)=4.0d0
	 pmomup(3,8)=5.0d0
	 pmomup(4,8)=dsqrt(pmomup(1,8)**2+pmomup(2,8)**2+pmomup(3,8)**2)
	 pmomup(5,8)=0.0d0
	end if

c-----------------------
c...output program logo.
      call uplogo

c-----------------------
c...setting the alphas.
c   mstu(111)=0---fixed; d=1---fir order; =2---sec order
	mstp(2)  =mstu(111)

c---------------------------------------------------------
c...choose lambda value to be used in case of using vegas,
c...to ensure they are the same inside or outside pythia.
	if(vegasopen) then
	   if(ioutpdf.eq.0) then
c...this part is from the inner part of pythia.
	      if(mstp(51).ge.1 .and. mstp(51).le.20) then
              paru(112)=alamin(mstp(51))
              mstu(112)=nfin(mstp(51))
            end if
	   else
	      if(ipdfnum.eq.100) then
			 paru(112)=0.1750d0    !grv98l
		     mstu(112)=4
	      end if
		  if(ipdfnum.eq.200) then
			 paru(112)=0.220d0     !msrt2001l
	         mstu(112)=4
	      end if
		  if(ipdfnum.eq.300) then
			 paru(112)=0.216d0     !leading order alpha running
	         mstu(112)=4
	      end if
	   end if
	end if

c******************************************************************
c...using vegas to get the importance function and then the 
c...sampling stratage: xi(), which will be stored in grade files.
c******************************************************************
	if(ggsub) then
	   ndim =7   ! gg-mechanism
	   if(isubonly.eq.1) ndim=5
	else
	   ndim =4   ! gc-, cc-mechanisms
	end if

      if(vegasopen) then
	  call vegaslogo()   !logo for vegas.
        if(imix.eq.0) then
c--------------------------------------
c... using existed grade to generate more precise grade.
c--------------------------------------
		if(iveggrade.eq.1) then
	     write(*,'(a)')
     &		'using existed grade to generate more precise grade'
c		 write(3,'(a)')
c     &		'using existed grade to generate more precise grade'
		 
		 do i=1,nvbin
	        read(11,*) (xi(i,j),j=1,ndim)
	     end do

	     ffnew(1:11)=fn2(1:11)
	     ffnew(12:12)='n'
		 ffnew(13:24)=fn2(12:23)
            ! Added by F. Zhang 23-04-2011 
            if(iusecurdir.eq.1) then
            FULLPATHNAME = ffnew(12:24)
            else 
            FULLPATHNAME = BASENAME2(1:LEN_TRIM(BASENAME2))//'/'      
     +                 //ENERGYNAME(1:len_trim(ENERGYNAME))
     +                 //FFNEW(1:LEN_TRIM(FFNEW))   
            endif
c	     open(unit=29,file=ffnew,status='unknown')
	     open(unit=29,file=FULLPATHNAME,status='unknown')

		end if
            
		call vegas(totfun,ndim,number,nitmx,2)

		do i=1,nvbin
	      if(iveggrade.eq.1) then
			write(29,*) (xi(i,j),j=1,ndim)
	      else
	        write(11,*) (xi(i,j),j=1,ndim)
		  end if
		end do
	  end if

c*************************************************
c*************************************************

        if(imix.eq.1) then
c--------------------------------------
c... using existed grade to generate more precise grade.
c--------------------------------------
		if(iveggrade.eq.1) then
	     write(*,'(a)')
     &		  'existed grade to generate more precise grade'
c		 write(3,'(a)')
c     &		  'existed grade to generate more precise grade'
		 
		 do i=1,nvbin
	       read(11,*) (xi(i,j),j=1,ndim)
	     end do

	     ffnew(1:11)=fn2(1:11)
	     ffnew(12:12)='n'
		 ffnew(13:24)=fn2(12:23)
             if (iusecurdir.eq.1) then 
             FULLPATHNAME = ffnew(12:24)
          else
      FULLPATHNAME = BASENAME2(1:LEN_TRIM(BASENAME2))//'/'
     &                //ENERGYNAME(1:LEN_TRIM(ENERGYNAME)) 
     &                //FFNEW(1:LEN_TRIM(FFNEW))
           endif
c	     open(unit=29,file=ffnew,status='unknown')
	     open(unit=29,file=FULLPATHNAME,status='unknown')
	    end if

		call vegas(totfun,ndim,number,nitmx,2)

		do i=1,nvbin
	      if(iveggrade.eq.1) then
			  write(29,*) (xi(i,j),j=1,ndim)
	      else
	          write(11,*) (xi(i,j),j=1,ndim)
		  end if
		end do
	  end if

	  call vegasend(vegasopen,ievntdis,usegrade)
      else
	  call vegasend(vegasopen,ievntdis,usegrade)

c***********************************************************
c...initialize the grade.


	  rc=1.0d0/nvbin
	  do 77 j=1,ndim
           xi(nvbin,j)=1.0d0
           dr=0.0d0
        do 77 i=1,nvbin-1
           dr=dr+rc
           xi(i,j)=dr
77      continue

c...using the existed grade. one thing should be care is that
c...existed grade should be formed under the same parameters.
	  if(usegrade) then
	    write(*,'(a)') 'using the existed vegas grade'
c		write(3,'(a)') 'using the existed vegas grade'
		do i=1,nvbin
	      read(11,*) (xi(i,j),j=1,ndim)
	    end do
	  else
	    write(*,'(a)') 'using trivial MC to generate events'
c		write(3,'(a)') 'using trivial MC to generate events'
	  end if
      end if

c...expanded event listing (required only for histogramming).
      mstp(125)=2

c...pyinit--->to initialize the generation procedure.
c      call pyinit('user',' ',' ',0d0)  ! commented by F. Zhang 26-04-2011

c...set xi+_cc or xi++_cc stable.
      mdcy(pycomp(4412),1)=0 
      mdcy(pycomp(4422),1)=0
      mdcy(pycomp(4432),1)=0

c...set xibc0 and xibc+ stable.
	mdcy(pycomp(5142),1)=0
	mdcy(pycomp(5242),1)=0
	mdcy(pycomp(5342),1)=0

c...set xibb- and xibb0 stable.
	mdcy(pycomp(5512),1)=0
	mdcy(pycomp(5522),1)=0
	mdcy(pycomp(5532),1)=0

      return
	end

