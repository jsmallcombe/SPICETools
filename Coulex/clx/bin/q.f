      SUBROUTINE Q
C
C	COMPUTE QLM FUNCTIONS
C

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      DIMENSION LAMDA(5)
      REAL*8 R_QLM(5,5), I_QLM(5,5)
      COMMON /QLM/W,EPS,EROOT,LAMMAX,LAMDA,R_QLM,I_QLM,RALFA,ZPOL
C
      EW=EXP(W)
      COSHY=.5d0*(EW+1.d0/EW)
      SINHY=.5d0*(EW-1.d0/EW)
      DEN=EPS*COSHY+1.d0
      DEN1=DEN*DEN
      DEN2=DEN1*DEN1
      SH1=EROOT*SINHY
      SH2=SH1*SH1
      SH3=SH2*SH1
      CH1=COSHY+EPS
      CH2=CH1*CH1
      DO I1=1,LAMMAX
c
c	transfer
c
	if(lamda(i1).eq.6) then
          if ((((-1.2d0)*den*52.d0*66.d0*1.78d0)/650.d0)+21.d0
     &       .lt. -15.d0) then
            r_qlm(i1,1)=0
	  else
	    r_qlm(i1,1)=
     &         den*exp((((-1.2d0)*den*52.d0*66.d0*1.78d0)/650.d0)
     &         +20.d0)
	  endif
	  i_qlm(i1,1)=0.d0
	  write(6,*) r_qlm(i1,1)
	endif
C
C	E1
C
        IF ( LAMDA(I1) .EQ. 1 ) THEN
          R_QLM(I1,1)=.5d0*CH1/DEN1
          I_QLM(I1,1)=0.d0
          R_QLM(I1,2)=0.d0
          I_QLM(I1,2)=(-.3535534d0)*SH1/DEN1
C
C	E2
C
        ELSE IF ( LAMDA(I1) .EQ. 2 ) THEN
C
C	POL ACCOUNTS FOR E1-POLARIZATION
C
          POL=1.0d0-ZPOL/DEN
          R_QLM(I1,1)=.75d0*(2.d0*CH2-SH2)/DEN2*POL
          I_QLM(I1,1)=0.d0
          R_QLM(I1,2)=0.d0
          I_QLM(I1,2)=(-1.8371173d0)*CH1*SH1/DEN2*POL
          R_QLM(I1,3)=(-.9185587d0)*SH2/DEN2*POL
          I_QLM(I1,3)=0.d0
C
C	E3
C
        ELSE IF ( LAMDA(I1) .EQ. 3 ) THEN
          DEN3=DEN2*DEN1
          R_QLM(I1,1)=1.875d0*CH1*(2.d0*CH2-3.d0*SH2)/DEN3
          I_QLM(I1,1)=0.d0
          R_QLM(I1,2)=0.d0
          I_QLM(I1,2)=(-1.6237976d0)*(4.d0*CH2-SH2)*SH1/DEN3
          R_QLM(I1,3)=(-5.1348989d0)*CH1*SH2/DEN3
          I_QLM(I1,3)=0.d0
          R_QLM(I1,4)=0.d0
          I_QLM(I1,4)=2.0963137d0*SH3/DEN3
C
C	E4
C
        ELSE IF ( LAMDA(I1) .EQ. 4 ) THEN
          DEN4=DEN2*DEN2
          CH4=CH2*CH2
          SH4=SH2*SH2
          R_QLM(I1,1)=1.09375d0*(8.d0*CH4-24.d0*CH2*SH2+3.d0*SH4)/DEN4
          I_QLM(I1,1)=0.d0
          R_QLM(I1,2)=0.d0
          I_QLM(I1,2)=(-4.8913987d0)*CH1*(4.d0*CH2-3.d0*SH2)*SH1/DEN4
          R_QLM(I1,3)=(-3.4587411d0)*(6.d0*CH2-SH2)*SH2/DEN4
          I_QLM(I1,3)=0.d0
          R_QLM(I1,4)=0.d0
          I_QLM(I1,4)=12.9414244d0*CH1*SH3/DEN4
          R_QLM(I1,5)=4.5754844d0*SH4/DEN4
          I_QLM(I1,5)=0.d0
C
C	M1
C
        ELSE IF ( LAMDA(I1) .EQ. 5 ) THEN
          R_QLM(I1,1)=0.d0
          I_QLM(I1,1)=0.d0
          R_QLM(I1,2)=(-.3535534d0)*EROOT/DEN1
          I_QLM(I1,2)=0.d0
        END IF
      END DO
      RALFA=EPS*SINHY+W
      RETURN
      END
