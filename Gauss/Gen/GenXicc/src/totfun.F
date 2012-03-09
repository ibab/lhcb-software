
c****************************************************
c****************************************************

      double precision function totfun(ny,yvar,papawt)
	implicit double precision (a-h,o-z)
	implicit integer (i-n)

	common/mixevnt/imix,imixtype
	common/wbstate/ratiou,ratiod,ratios,nbound

      dimension yvar(ny)

	external sepfun,xmixfun

      totfun=0.0d0

	if (papawt.lt.1.0d-16) then
	  return
      else
	  if(imix.eq.0) totfun=sepfun(ny,yvar,papawt)
	  if(imix.eq.1) totfun=xmixfun(imixtype,ny,yvar,papawt)
	end if

c...set the probability for grabing u,d,s quarks
      if(nbound.eq.1) totfun=totfun*(ratiou)/(ratiou+ratiod+ratios)
      if(nbound.eq.2) totfun=totfun*(ratiod)/(ratiou+ratiod+ratios)
      if(nbound.eq.3) totfun=totfun*(ratios)/(ratiou+ratiod+ratios)
      if(nbound.eq.4) totfun=totfun

	return
	end

