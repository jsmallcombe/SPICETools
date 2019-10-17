      real*8 FUNCTION THREEJ(RJ1,RM1,RJ2,RM2,RJ3,RM3)

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      IF ( MOD(RJ1-RJ2-RM3,2.d0) .EQ. 0 ) THEN
        PHZ=1.d0
      ELSE
        PHZ=-1.d0
      END IF
      THREEJ=PHZ*CLEB(RJ1,RJ2,RJ3,RM1,RM2,-RM3)/SQRT(2.*RJ3+1.)
      RETURN
      END
