
c*************************************
c... simple logo for the generator.
c*************************************

      subroutine uplogo
      implicit double precision(a-h, o-z)
	implicit integer(i-n)

	common/mixevnt/imix,imixtype
      common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)
	double complex colmat,bundamp
      common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/counter/ixiccstate,nev,xmaxwgt
	common/confine/ptcut,etacut,psetacut,pscutmin,pscutmax
      common/funtrans/npdfu
	common/usertran/idpp
	common/outpdf/ioutpdf,ipdfnum
	common/newpara/ncombine
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum
	common/wbstate/ratiou,ratiod,ratios,nbound
	common/extramass/bmxicc,bmxibc,bmxibb,slqmass

      character*8 alporder
	
	write(*,'(a)') 
     &	'......................................................'
	write(*,'(a)') 
     &	'......................................................'
	write(*,'(a)')
     &	'...........    XICC GENERATOR: GENXICC  ..............'
	write(*,'(a)') 
     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
	!write(3,'(a)') 
c     &	'...........    XICC GENERATOR: GENXICC  ..............'
      !write(3,'(a)') 
c     &	'......................................................'
	write(*,'(a)') 
     &	'.... copyright (c)  c.h. chang  and x.g. wu  .........'
      !write(3,'(a)') 
c     &	'.... copyright (c)  c.h. chang  and x.g. wu  .........'
	write(*,'(a)') 
     &	'.... any suggestion email to: wuxg@cqu.edu.cn  .......'
      !write(3,'(a)') 
c     &	'.... any suggestion email to: wuxg@cqu.edu.cn  .......'
	write(*,'(a)') 
     &	'......................................................'
	write(*,'(a)') 
     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
      write(*,*)
      !write(3,*)
      write(*,'(a)')
     &  '******************************************************'
      !write(3,'(a)')
c     &  '******************************************************'
      write(*,'(a)')
     &  '**********        running information       **********'
      !write(3,'(a)')
c     &  '**********        running information       **********'
      write(*,'(a)')
     &  '******************************************************'
      !write(3,'(a)')
c     &  '******************************************************'
       write(*,*)
       !write(3,*)

c*****************************************************************

	write(*,'(a)') 
     &	'......................................................'
	write(*,'(a)')
     &	'... two steps: diquark first and then bound state ....'
	write(*,'(a)') 
     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
	!write(3,'(a)') 
c     &	'... two steps: diquark first and then bound state ....'
      !write(3,'(a)') 
c     &	'......................................................'

c*****************************************************************

	write(*,'(a)') 
     &	'......................................................'
	write(*,'(a)')
     &	'.............. initial parameters ....................'
	write(*,'(a)') 
     &	'......................................................'
      !write(3,'(a)') 
c     &	'......................................................'
	!write(3,'(a)') 
c     &	'.............. initial parameters ....................'
      !write(3,'(a)') 
c     &	'......................................................'

c----------------
	write(*,*)
	!write(3,*)

	if (mgenxi.eq.1) then
	  write(*,'(a)') 'to generate the events from (cc)-diquark'
	  !write(3,'(a)') 'to generate the events from (cc)-diquark'
	  if(nbound.eq.1) then
	   write(*,'(a)') 'to generate the events for Xi_{cc}^++'
	   !write(3,'(a)') 'to generate the events for Xi_{cc}^++'
	  end if
	  if(nbound.eq.2) then
	   write(*,'(a)') 'to generate the events for Xi_{cc}^+'
	   !write(3,'(a)') 'to generate the events for Xi_{cc}^+'
	  end if
	  if(nbound.eq.3) then
	   write(*,'(a)') 'to generate the events for Omega_{cc}^+'
	   !write(3,'(a)') 'to generate the events for Omega_{cc}^+'
	  end if
	  if(nbound.eq.4) then
	   write(*,'(a)') 'to generate the events for (cc)-diquark'
	   !write(3,'(a)') 'to generate the events for (cc)-diquark'
	  end if
	end if

	if (mgenxi.eq.2) then
	  write(*,'(a)') 'to generate the events from (bc)-diquark'
	  !write(3,'(a)') 'to generate the events from (bc)-diquark'
	  if(nbound.eq.1) then
	   write(*,'(a)') 'to generate the events for Xi_{bc}^+'
	   !write(3,'(a)') 'to generate the events for Xi_{cc}^+'
	  end if
	  if(nbound.eq.2) then
	   write(*,'(a)') 'to generate the events for Xi_{bc}^0'
	   !write(3,'(a)') 'to generate the events for Xi_{bc}^0'
	  end if
	  if(nbound.eq.3) then
	   write(*,'(a)') 'to generate the events for Omega_{bc}^0'
	   !write(3,'(a)') 'to generate the events for Omega_{bc}^0'
	  end if
	  if(nbound.eq.4) then
	   write(*,'(a)') 'to generate the events for (bc)-diquark'
	   !write(3,'(a)') 'to generate the events for (bc)-diquark'
	  end if
	end if

	if (mgenxi.eq.3) then
	  write(*,'(a)') 'to generate the events from (bb)-diquark'
	  !write(3,'(a)') 'to generate the events from (bb)-diquark'
	  if(nbound.eq.1) then
	   write(*,'(a)') 'to generate the events for Xi_{bb}^0'
	   !write(3,'(a)') 'to generate the events for Xi_{bb}^0'
	  end if
	  if(nbound.eq.2) then
	   write(*,'(a)') 'to generate the events for Xi_{bb}^-'
	   !write(3,'(a)') 'to generate the events for Xi_{bb}^-'
	  end if
	  if(nbound.eq.3) then
	   write(*,'(a)') 'to generate the events for Omega_{bb}^-'
	   !write(3,'(a)') 'to generate the events for Omega_{bb}^-'
	  end if
	  if(nbound.eq.4) then
	   write(*,'(a)') 'to generate the events for (bb)-diquark'
	   !write(3,'(a)') 'to generate the events for (bb)-diquark'
	  end if
	end if

c-----------------------------
	if (isubonly.eq.1) then
	  write(*,*)
	  write(*,'(a)') 'only to generate information of subprocess'
	  !write(3,*)
	  !write(3,'(a)') 'only to generate information of subprocess'
	end if

c----------------------------
      if(imix.eq.1) go to 111
      
	if (ixiccstate.eq.1) then
	  write(*,'(a)') 'to get the result for gg channel+(3^s_1)_cbar3'
	  !write(3,'(a)') 'to get the result for gg channel+(3^s_1)_cbar3'
	end if

	if (ixiccstate.eq.2) then
	  write(*,'(a)') 'to get the result for gg channel+(1^s_0)_c6'
	  !write(3,'(a)') 'to get the result for gg channel+(1^s_0)_c6'
	end if

	if(mgenxi.eq.1) then
	 if (ixiccstate.eq.3) then
	  write(*,'(a)') 'to get the result for gc channel+(3^s_1)_cbar3'
	  !write(3,'(a)') 'to get the result for gc channel+(3^s_1)_cbar3'
	 end if

	 if (ixiccstate.eq.4) then
	  write(*,'(a)') 'to get the result for gc channel+(1^s_0)_c6'
	  !write(3,'(a)') 'to get the result for gc channel+(1^s_0)_c6'
	 end if

	 if (ixiccstate.eq.5) then
	  write(*,'(a)') 'to get the result for cc channel+(3^s_1)_cbar3'
	  !write(3,'(a)') 'to get the result for cc channel+(3^s_1)_cbar3'
	 end if

	 if (ixiccstate.eq.6) then
	  write(*,'(a)') 'to get the result for cc channel+(1^s_0)_c6'
	  !write(3,'(a)') 'to get the result for cc channel+(1^s_0)_c6'
	 end if
	end if

	if(mgenxi.eq.2) then
	 if (ixiccstate.eq.3) then
	  write(*,'(a)') 'to get the result for gg channel+(3^s_1)_c6'
	  !write(3,'(a)') 'to get the result for gg channel+(3^s_1)_c6'
	 end if

	 if (ixiccstate.eq.4) then
	  write(*,'(a)') 'to get the result for gg channel+(1^s_0)_cbar3'
	  !write(3,'(a)') 'to get the result for gg channel+(1^s_0)_cbar3'
	 end if
	end if

111   continue

c*************************************************

	if(imix.eq.1) then

	 if(mgenxi.eq.1) then
	   write(*,'(a)') 'to get the mixing results from (cc)-diquark'
	   !write(3,'(a)') 'to get the mixing results from (cc)-diquark'
	 end if
	 if(mgenxi.eq.2) then
	   write(*,'(a)') 'to get the mixing results from (bc)-diquark'
	   !write(3,'(a)') 'to get the mixing results from (bc)-diquark'
	 end if
	 if(mgenxi.eq.3) then
	   write(*,'(a)') 'to get the mixing results from (bb)-diquark'
	   !write(3,'(a)') 'to get the mixing results from (bb)-diquark'
	 end if

	 if(imixtype.eq.1) then
	   write(*,'(a)') 
     &	'mixed events of gg-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	   !write(3,'(a)')
c     &	'mixed events of gg-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	 end if

	 if(mgenxi.eq.1) then
	  if(imixtype.eq.2) then
	   write(*,'(a)') 
     &	'mixed events of gc-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	   !write(3,'(a)')
c     &	'mixed events of gc-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	  end if
	  if(imixtype.eq.3) then
	   write(*,'(a)') 
     &	'mixed events of cc-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	   !write(3,'(a)')
c     &	'mixed events of cc-channel of [(3^s_1)_cbar3+(1^s_0)_c6]'
	  end if
	 end if

	 if(mgenxi.eq.2) then
	  if(imixtype.eq.2) then
	   write(*,'(a)') 
     &	'mixed events of gg-channel of [(3^s_1)_c6+(1^s_0)_cbar3]'
	   !write(3,'(a)')
c     &	'mixed events of gg-channel of [(3^s_1)_c6+(1^s_0)_cbar3]'
	  end if
	 end if
	end if

c*************************************************
      if(mgenxi.eq.1) then
	 if(imix.eq.0 .and. (ixiccstate.eq.3 .or. ixiccstate.eq.4)) then
	  if(ncombine.eq.0) then
	   write(*,'(a)') 
     &     'subtraction not included to avoid duble counting of gg'
	   !write(3,'(a)')
c     &     'subtraction not included to avoid duble counting of gg'
	  end if
	  if(ncombine.eq.1) then
	   write(*,'(a)') 
     &     'subtraction included to avoid duble counting of gg'
	   !write(3,'(a)')
c     &     'subtraction included to avoid duble counting of gg'
	  end if
	 end if
c-------------------
       if(imix.eq.1 .and. imixtype.eq.2) then
	  if(ncombine.eq.0) then
	   write(*,'(a)') 
     &     'subtraction not included to avoid duble counting of gg'
	   !write(3,'(a)')
c     &     'subtraction not included to avoid duble counting of gg'
	  end if
	  if(ncombine.eq.1) then
	   write(*,'(a)') 
     &     'subtraction included to avoid duble counting of gg'
	   !write(3,'(a)')
c     &     'subtraction included to avoid duble counting of gg'
	  end if
	 end if	   
	end if

c*************************************************

	if(npdfu.eq.1) then
	  !write(3,41)'generate evnts', nev,'teva energy(gev)',ecm
	  write(*,41)'generate evnts', nev,'teva energy(gev)',ecm
	end if
	if(npdfu.eq.2) then
	  !write(3,41) 'generate evnts', nev,'lhc energy(gev)',ecm
	  write(*,41) 'generate evnts', nev,'lhc energy(gev)',ecm
	end if
	if(npdfu.eq.3) then
	  !write(3,41)'generate evnts', nev,'selex energy(gev)',ecm
	  write(*,41)'generate evnts', nev,'selex energy(gev)',ecm
	end if

      write(*,'(a)') '***********************************************'
      !write(3,'(a)') '***********************************************'

c*************************************************

      write(*,'(a)') '***********************************************'
      !write(3,'(a)') '***********************************************'
	
	if(mgenxi.eq.1) then
	  write(*,32) pmxicc,pmc,fxicc
	  !write(3,32) pmxicc,pmc,fxicc
	end if

	if(mgenxi.eq.2) then
	  write(*,33) pmxicc,pmb,pmc,fxicc
	  !write(3,33) pmxicc,pmb,pmc,fxicc
	end if

	if(mgenxi.eq.3) then
	  write(*,34) pmxicc,pmb,fxicc
	  !write(3,34) pmxicc,pmb,fxicc
	end if

	write(*,*) 'light quark mass is (current mass)',slqmass,' GeV'
	!write(3,*) 'light quark mass is (current mass)',slqmass,' GeV'

c--------------------------------

	if(mstu(111).eq.0 .or. isubonly.eq.1) alporder='const'
	if(mstu(111).eq.1) alporder='lo'
	if(mstu(111).eq.2) alporder='nlo'

	write(*,'(a,3x,a,a)')
     & 'Q2=square of baryon transverse mass;','alphas order=',alporder
      !write(3,'(a,3x,a,a)')
c     & 'Q2=square of baryon transverse mass;','alphas order=',alporder

	if(ioutpdf.eq.0) then
	  write(*,'(a)') 'the innter pdfs which are from pythia'
	  !write(3,'(a)') 'the innter pdfs which are from pythia'
c...you may add some more from pythia manual book.
	  if(mstp(51).eq.1) then
	    write(*,'(a)')'PDF: cteq 3l'
	    !write(3,'(a)')'PDF: cteq 3l'
	  end if
	  if(mstp(51).eq.2) then
	    write(*,'(a)')'PDF: cteq 3m'
	    !write(3,'(a)')'PDF: cteq 3m'
	  end if
	  if(mstp(51).eq.7) then
	    write(*,'(a)')'PDF: cteq 5l'
	    !write(3,'(a)')'PDF: cteq 5l'
	  end if
	  if(mstp(51).eq.8) then
	    write(*,'(a)')'PDF: cteq 5m1'
	    !write(3,'(a)')'PDF: cteq 5m1'
	  end if
	  if(mstp(51).eq.13) then
	    write(*,'(a)')'PDF: ehlq set 2'
	    !write(3,'(a)')'PDF: ehlq set 2'
	  end if
	end if

	if(ioutpdf.eq.1) then
	  write(*,'(a)') 'new version of outer pdfs which are from www'
	  !write(3,'(a)') 'new version of outer pdfs which are from www'
	  if(ipdfnum.eq.100) then
	     write(*,'(a)')'PDF: grv98lo'
		 !write(3,'(a)')'PDF: grv98lo'
	  end if
	  if(ipdfnum.eq.200) then
	     write(*,'(a)')'PDF: mstr2001lo'
		 !write(3,'(a)')'PDF: mstr2001lo'
	  end if
	  if(ipdfnum.eq.300) then
	     write(*,'(a)')'PDF: cteq6hq'
		 !write(3,'(a)')'PDF: cteq6hq'
	  end if
	end if

      write(*,'(a,i3)') 'using pythia idwtup=',idpp
	!write(3,'(a,i3)') 'using pythia idwtup=',idpp

	write(*,'(a,g10.4,a)') 'ptcut=',ptcut,'gev'
	!write(3,'(a,g10.4,a)') 'ptcut=',ptcut,'gev'

	if(etacut.gt.1.0d+2) then
	   write(*,'(a)') 'no rapidity cut'
	   !write(3,'(a)') 'no rapidity cut'
	else
	   write(*,'(a,g10.4)') 'rapidity cut=',etacut
	   !write(3,'(a,g10.4)') 'rapidity cut=',etacut
	end if

         if(psetacut.gt.1.0d+2 .and. pscutmin.lt.(-1.0d+2) 
     &      .and. pscutmax.gt.1.0d+2) then
            write(*,'(a)') 'no pseudo-rapidity cut'
         else
            if(psetacut.lt.1.0d+2) then
               write(*,'(a,g10.2)') 'pseudo-rapidity cut: |eta|<',psetacut
            else
               write(*,'(a,g10.2,a,g10.2)') 'pseudo-rapidity cut: ',
     &             pscutmin,'<eta<',pscutmax
            end if
         end if

      write(*,'(a)') '------------------------------------'
      !write(3,'(a)') '------------------------------------'

32    format('m_{xicc}=',g10.4,'m_{c}=',g10.4,'R_cc(0)=',g10.5)
33    format('m_{xibc}=',g10.4,'m_{b}=',g10.4,'m_{c}=',g10.4,
     & 'R_bc(0)=',g10.5)
34    format('m_{xibb}=',g10.4,'m_{b}=',g10.4,'R_bb(0)=',g10.5)

41    format(a,1x,i9,';',3x,a,g11.3,1x)

	return
	end

