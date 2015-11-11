
c**********************************************
c...typical error messages for input parameters.
c**********************************************

      subroutine uperror(wronginput)
      implicit double precision(a-h, o-z)
	implicit integer(i-n)

      common/counter/ixiccstate,nev,xmaxwgt
	common/mixevnt/imix,imixtype
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum
	common/loggrade/ievntdis,igenerate,ivegasopen,igrade,iusecurdir
	common/vegcross/vegsec,vegerr,iveggrade

	logical wronginput

c...typical error messages.
      if(imix.eq.0) then
	 if((ixiccstate.gt.6).or.(ixiccstate.lt.1)) then
	  write(*,*) 'only six channels+states are programmed!'
	  wronginput=.true.
	 endif
	end if

	if(isubonly.eq.1 .and. imix.eq.1) then
	 write(*,*) 'mixed events for subprocess is not programmed!'
	 wronginput=.true.
	end if

	if(mgenxi.eq.1 .and. isubonly.eq.1) then
	 if(ixiccstate.gt.2) then
	 write(*,*) 'only gluon channel is programmed for subprocess!'
	 wronginput=.true.
	 end if
	end if

	if(mgenxi.eq.2) then
	 if(ixiccstate.gt.5) then
	 write(*,*) 'only gluon channel is programmed for xibc!'
	 wronginput=.true.
	 endif
	end if

	if(mgenxi.eq.3) then
	 if(ixiccstate.gt.2) then
	 write(*,*) 'only gluon channel is programmed for xibb!'
	 wronginput=.true.
	 endif
	end if

	if(mgenxi.eq.3 .and. imixtype.gt.1) then
	 write(*,*) 'only gluon channel mixed events is programmed 
     &	 xibb!'
	 wronginput=.true.	
	end if

	if(mgenxi.eq.2 .and. imixtype.gt.2) then
	 write(*,*) 'only gluon channel mixed events is programmed 
     &	 xibc!'
	 wronginput=.true.	
	end if

	if(ivegasopen.eq.1 .and. iveggrade.eq.1 .and. isubonly.eq.1) then
	 write(*,*) 
     &	 'useless to get new grade from existed one for subprocess!'
	 wronginput=.true.
	end if

c... more messages of the users' own to be added here ....

	return
	end

