
C--------------------------------------------------------------------------
C     MAX CLIQUE ALGORITHM BASED ON QUADRATIC ZERO-ONE PROGRAMMING
C
C
C  THE ALGORITHM IS DESCRIBED IN DETAILS IN: P.M. PARDALOS AND G.P. RODGERS,
C  A BRANCH AND BOUND ALGORITHM FOR THE MAXIMUM CLIQUE PROBLEM, COMP. AND
C  OPER. RESEARCH, VOL. 19, NO. 5 (1992), PP. 363-375.
C
C
C  AUTHORS          - PANOS M. PARDALOS AND GREGORY P. RODGERS
C
C  WARRANTY         - NO WARRANTY EXPRESSED OR IMPLIED IS APPLICABLE
C
C
C  NOTE: THE ALGORITHM CAN RUN IN PARALLEL 
C        ALL TIMING SUBROUTINES ARE USED FOR THE IBM 3090
C
C  INPUT PARAMETERS:
C  THE NUMBER OF PROCESSORS TO USE (=1 IF NOT RUN IN PARALLEL)
C  N (NUMBER OF VERTICES)
C  DENSTY (A NUMBER BETWEEN 0 AND 1)
C  DSEED (SEED FOR GENERATING RANDOM GRAPHS)
C
C--------------------------------------------------------------------------
C
C@PROCESS DC(STORE)                                                      
      PARAMETER (MAXN=3000,MAXEDG=50000,NWK=500000)
      INTEGER HA,KA,HAT,KAT,N,MAXC,WK,NWK                               
      REAL*8 E1,E2,E3,EHR,EBB                                           
      REAL   T1,T2,T3,THR,TBB,T0,OVH,T00,OMGEN                          
      LOGICAL X                                                         
      COMMON /STORE/ HA(MAXEDG),KA(MAXN+1),HAT(MAXEDG*2),
     >KAT(MAXN+1),X(MAXN),WK(NWK)                                         
      WRITE(*,*) 'ENTER THE NUMBER OF PROCESSORS TO USE'                
      READ(*,*) NPROCS                                                  
C                                                                       
C     GENERATE A RANDOM GRAPH AND CALCULATE TRANSPOSED ADJACENCY MATRIX 
C                                                                       
      CALL SECOND(T00)                                                  
      CALL MGRAPH(N,HA,KA,MAXEDG)                                       
      CALL SECOND(T0)                                                   
      OMGEN = T0-T00                                                    
      CALL TRPOSE(N,HA,KA,HAT,KAT,WK)                                   
C                                                                       
C     HEURISTIC PHASE USING THE GREEDY METHOD                           
C                                                                       
      CALL SECOND(T1)                                                   
      OVH = T1-T0                                                       
      CALL CLOCKX(E1)                                                   
      CALL GREEDY(N,HAT,KAT,MAXC,X,WK,WK(N+1),WK((2*N)+1),              
     >            WK((3*N)+1) )                                         
C                                                                       
C     BRANCH AND BOUND PHASE                                            
C                                                                       
      IHSOL= MAXC                                                       
      CALL SECOND(T2)                                                   
      CALL CLOCKX(E2)                                                   
      IF (NPROCS.NE.0)                                                  
     >CALL BBND(N, HAT,KAT,X,MAXC,WK,NWK,NSUBP,NSUBPG,NPROCS,NCHG)     
C                                                                       
C     FINISH TIMING AND PRINT RESULTS                                   
C                                                                       
      CALL SECOND(T3)                                                   
      CALL CLOCKX(E3)                                                   
      THR = T2-T1                                                       
      TBB = T3-T2                                                       
      EHR = (E2-E1)*0.000001                                            
      EBB = (E3-E2)*0.000001                                            
      RDNSTY = REAL(2*(KA(N+1)-1 )) / REAL(N*(N-1))                     
      WRITE(*,50) 'MATRIX GENERATION CPUTM=',OMGEN                      
      WRITE(*,50) 'TRANSPOSE OVERHEAD     =',OVH                        
      WRITE(*,40) 'PROBLEM DIMENSION      =',N                          
      WRITE(*,40) 'NUMBER OF EDGES        =',KA(N+1)-1                  
      WRITE(*,50) 'REAL DENSITY           =',RDNSTY                     
      WRITE(*,40) 'HEURISTIC IZE  =',IHSOL                      
      WRITE(*,40) 'MAXIMUM IZE    =',MAXC                       
      WRITE(*,40) 'NUMBER OF MIN CHAGES   =',NCHG                       
      WRITE(*,40) 'NUMBER OF SUBPROBLEMS  =',NSUBP                      
      WRITE(*,40) 'MINIMIZER SUBPROBLEM   =',NSUBPG                     
      WRITE(*,50) 'HEURISTIC CPU TIME     =',THR                        
      WRITE(*,50) 'HEURISTIC ELAPSED TIME =',EHR                        
      WRITE(*,50) 'B & B CPU TIME         =',TBB                        
      WRITE(*,50) 'B & B ELAPSED TIME     =',EBB                        
      WRITE(*,*) 'THE VERTICES IN THE CLIQUE ARE'                       
      IPTR = 0                                                          
      DO 10 I=1,N                                                       
         IF (X(I)) THEN                                                 
            IPTR = IPTR+1                                               
            WK(IPTR) = I                                                
         ENDIF                                                          
10    CONTINUE                                                          
      WRITE(*,20) (WK(I),I=1,IPTR)                                      
20    FORMAT(25(10I6,/))                                                
40    FORMAT(1X,A,I12)                                                  
50    FORMAT(1X,A,F12.4)                                                
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE MGRAPH(N,HA,KA,MAXEL)                                  
C                                                                       
C     MAKE A RANDOM GRAPH                                               
C                                                                       
      INTEGER    N,HA(*),KA(*)                                          
      DOUBLE PRECISION DSEED                                            
      REAL       DENSTY                                                 
      WRITE(*,*) ' ENTER N,DENSTY,DSEED'                                
      READ(*,*) N,DENSTY,DSEED                                          
C     N      = 200                                                      
C     DENSTY = 0.25                                                     
C     DSEED  = 8651167.D0                                               
      NEDGES = 0                                                        
      DO 20 I=1,N                                                       
         KA(I) = NEDGES + 1                                             
         DO 10 J=I+1,N                                                  
            IF (GGUBFS(DSEED).LT.DENSTY) THEN                           
               NEDGES     = NEDGES + 1                                  
               IF (NEDGES.GT.MAXEL) RETURN                              
               HA(NEDGES) = J                                           
            ENDIF                                                       
10       CONTINUE                                                       
20    CONTINUE                                                          
      KA(N+1) = NEDGES + 1                                              
      RETURN                                                            
C     END OF MGRAPH                                                     
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE TRPOSE(N,HA,KA,HAT,KAT,IT)                             
C                                                                       
C     CREATE THE TRANSPOSE GRAPH                                        
C                                                                       
      INTEGER N,HA(*),KA(*),HAT(*),KAT(*),IT(*),NEDGES                  
      NEDGES = KA(N+1)-1                                                
      KAT(N+1) = (2*NEDGES) + 1                                         
C     INITIALIZE KAT WITH SIZE OF EACH ROW OF HA                        
      DO 5 I=1,N                                                        
5        KAT(I)=KA(I+1)-KA(I)                                           
C     COMPUTE SIZE OF ROWS OF HAT                                       
      DO 20 K=1,NEDGES                                                  
         KAT(HA(K))=KAT(HA(K))+1                                        
20    CONTINUE                                                          
C     COMPUTE STARTING POSITIONS OF ROWS OF HAT                         
      DO 10 I=N,1,-1                                                    
         KAT(I) = KAT(I+1) - KAT(I)                                     
10    CONTINUE                                                          
C     INITIALIZE FIRST UNFILLED POSITION IN EACH ROW                    
      DO 30 K=1,N                                                       
         IT(K)=KAT(K)                                                   
30    CONTINUE                                                          
C     FILL LOWER TRIANGULAR PART OF HAT, WHILE UPDATING IT()            
      DO 45 J=1,N                                                       
         DO 40 K=KA(J),KA(J+1)-1                                        
C     HA(K)=VERTEX TO FILL IN THE EDGE IN LOWER TIANG. PART OF HAT      
C     IT(HA(K))=FIRST FREE POSITION FOR VERTEX TO BE FILLED             
            HAT(IT(HA(K)))=J                                            
C     UPDATE FIRST FREE POSITION                                        
            IT(HA(K))=IT(HA(K))+1                                       
40       CONTINUE                                                       
45    CONTINUE                                                          
C     COPY UPPER TRIANGULAR PART FROM HA TO HAT                         
      DO 60 K=1,N                                                       
         DO 50 I=KA(K),KA(K+1)-1                                        
            HAT(IT(K))=HA(I)                                            
            IT(K)=IT(K)+1                                               
50       CONTINUE                                                       
60    CONTINUE                                                          
      RETURN                                                            
C     END OF TRPOSE                                                     
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE GREEDY(N,HAT,KAT,NUM1S,X,DEGFRE,FREE,P,DEGFX1)         
      INTEGER    N,HAT(*),KAT(*),NUM1S,DEGFRE(N),P(N),DEGFX1(N)         
      LOGICAL    X(N),FREE(N),VALUE                                     
C                                                                       
      IMAX = 1                                                          
      DO 10 I=1,N                                                       
         P(I) = I                                                       
         DEGFRE(I) = KAT(I+1)-KAT(I)                                    
         IF (DEGFRE(I).GT.DEGFRE(IMAX)) IMAX= I                         
         DEGFX1(I) = 0                                                  
         FREE(I) = .TRUE.                                               
10    CONTINUE                                                          
C                                                                       
C     PUT THE HIGHEST CONNECTED NODE INTO THE GREEDY            
      NUM1S      = 0                                                    
      X(IMAX)    = .TRUE.                                               
      P(1)       = IMAX                                                 
      P(IMAX)    = 1                                                    
      FREE(IMAX) = .FALSE.                                              
C                                                                       
      LEV  = 1                                                          
20    CONTINUE                                                          
         LEVP1 = LEV+1                                                  
C    --- UPDATE THE VECTORS DEGFRE, DEGFX1, AND NUM1S                   
         LP = P(LEV)                                                    
C        LP WAS THE LAST VARIABLE THAT WAS FIXED                        
         IF (X(LP)) THEN                                                
            NUM1S = NUM1S+1                                             
            DO 70 K=KAT(LP),KAT(LP+1)-1                                 
               J = HAT(K)                                               
               IF (FREE(J)) THEN                                        
                  DEGFRE(J)  = DEGFRE(J)  - 1                           
                  DEGFX1(J)  = DEGFX1(J)  + 1                           
               ENDIF                                                    
70          CONTINUE                                                    
         ELSE                                                           
C           LAST VERTEX WAS THROWN OUT SO DONT BOTHER WITH DEGFX1       
            DO 90 K=KAT(LP),KAT(LP+1)-1                                 
               DEGFRE(HAT(K)) = DEGFRE(HAT(K)) - 1                      
90          CONTINUE                                                    
         ENDIF                                                          
         IF (LEV.EQ.N) GOTO 1000                                        
C                                                                       
C    --- DETERMINE IF ANY VARIABLE CAN BE FIXED                         
         DO 130 I=LEVP1,N                                               
            IP = P(I)                                                   
C                                                                       
C           TWO TESTS TO SEE IF NODE IP CAN BE FIXED                    
C                                                                       
C       -1- IS IT DISCONNECTED TO A NODE CURRENTLY FIXED TO ONE?        
            IF (DEGFX1(IP).LT.NUM1S) THEN                               
               VALUE = .FALSE.                                          
               IVAR = I                                                 
               GOTO 150                                                 
            ENDIF                                                       
C                                                                       
C       -2- AFTER THE 1ST TEST WE KNOW IP IS CONNECTED TO               
C           ALL NODES FIXED TO ONE.  SO NOW WE CAN TEST IF IT           
C           IS CONNECTED TO ALL REMAINING FREE NODES.  IF IT IS         
C           THEN FIX IP TO 1                                            
            IF (DEGFRE(IP).GE.N-LEVP) THEN                              
               VALUE = .TRUE.                                           
               IVAR = I                                                 
               GOTO 150                                                 
            ENDIF                                                       
C                                                                       
130      CONTINUE                                                       
C                                                                       
C    --- NONE COULD BE FORCED SO FIND VARIABLE WITH HIGHEST DEGREE      
         IVAR   = LEVP1                                                 
         DO 140 I=LEVP1+1,N                                             
            IF (DEGFRE(P(I)).GT.DEGFRE(P(IVAR))) IVAR=I                 
               IVAR = I                                                 
140      CONTINUE                                                       
C                                                                       
C    --- SET TO ONE AND STACK THE SUBPROBLEM WITH X(IP) = 0             
         VALUE = .TRUE.                                                 
C                                                                       
C    --- IVAR IS NEW FIXED VARIABLE, INCREASE LEV & SWAP IN P           
150      LEV      = LEVP1                                               
         IT       = P(IVAR)                                             
         P(IVAR)  = P(LEV)                                              
         P(LEV)   = IT                                                  
         X(IT)    = VALUE                                               
         FREE(IT) = .FALSE.                                             
         GOTO 20                                                        
1000  CONTINUE                                                          
      RETURN                                                            
C     END GREEDY                                                        
      END                                                               
C = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
      SUBROUTINE COPYL(N,LFROM,LTO)                                     
      LOGICAL LFROM(N),LTO(N)                                           
      INTEGER N                                                         
      DO 10 I=1,N                                                       
         LTO(I) = LFROM(I)                                              
10    CONTINUE                                                          
      RETURN                                                            
C     END OF COPYL                                                      
      END                                                               
C = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
      SUBROUTINE BBND(N, HAT,KAT,XMIN,MAXC,IWK,NWK,NSUBP,NSUBPG,NPROCS,
     >   NCHG)                                                          
C     IMPLICIT LOGICAL (A-Z)                                            
      INTEGER N,NWK,HAT(*),KAT(*),MAXC,IWK(NWK),NSUBP,NSUBPG,NPROCS     
      LOGICAL XMIN(N)                                                   
C                                                                       
C     LOCAL VARIABLES                                                   
      INTEGER CSUBP,CYCLES                                              
      INTEGER NBUSY,NFREE,BUSY(6),FREE(6),STRWK(6),SPLIT(6)             
      INTEGER LDX,LDF,LSTK,LP,LX,LFR,LXM,LTOP,LLEV,LMIN,LNSB,LNSG,LNWK, 
     >        LEND                                                      
      INTEGER NSPLIT,IPT,IPTZ,IPTY,IMIN,IMAX,MINSPL,WSIZE,Y,Z,I,MINSB   
      LOGICAL PREMPT,NEWMAX                                             
C                                                                       
C   --- INITIALIZE STATISTICS                                           
      NCHG   = 0                                                        
      NSUBP  = 0                                                        
      NSUBPG = 0                                                        
      CSUBP  = 0                                                        
      CYCLES = 0                                                        
C                                                                       
C   --- INITILIZE THE MININUM # OF SUBPROBLEMS TO SOLVE IN EACH CYCLE   
      MINSB = 800                                                       
C                                                                       
C   --- DIVIDE WORKSPACE AMONG NPROCS PROCESSORS                        
C                                                                       
      WSIZE = NWK/NPROCS                                                
      STRWK(1) = 0                                                      
      DO 10 I=2,NPROCS                                                  
         STRWK(I) = STRWK(I-1)+WSIZE                                    
10    CONTINUE                                                          
C                                                                       
C   --- SET UP STORAGE POINTERS                                         
      LDX =  1                                                          
      LDF =  LDX + (   N    )                                           
      LSTK=  LDF + (   N    )                                           
      LP  = LSTK + ( 3*(N+1))                                           
      LX  =   LP + (   N    )                                           
      LFR =   LX + (   N    )                                           
      LXM =  LFR + (   N    )                                           
      LTOP=  LXM + (   N    )                                           
      LLEV= LTOP + (   1    )                                           
      LMIN= LLEV + (   1    )                                           
      LNSB= LMIN + (   1    )                                           
      LNSG= LNSB + (   1    )                                           
      LNWK= LNSG + (   1    )                                           
      LEND= LNWK + (   1    )                                           
      IF (LEND.GE.WSIZE) THEN                                           
         WRITE(6,200) LEND*NPROCS,NWK                                   
         RETURN                                                         
      ENDIF                                                             
C                                                                       
C   --- INITIALIZE SUBPROBLEM 1                                         
      DO 20 I=1,N                                                       
         IWK(LP+I-1)= I                                                 
20    CONTINUE                                                          
      IWK(LTOP)=  1                                                     
      IWK(LSTK)= -1                                                     
      IWK(LLEV)=  0                                                     
      IWK(LNWK)=  0                                                     
      IWK(LMIN)= MAXC                                                   
C                                                                       
C   --- SET UP BUSY AND FREE LISTS                                      
      NBUSY     = 1                                                     
      BUSY(1)   = 1                                                     
      NFREE     = NPROCS-1                                              
      DO 30 I=1,NFREE                                                   
         FREE(I) = I+1                                                  
30    CONTINUE                                                          
C-----------------------------------------------------------------------
C                                                                       
C     SPLIT SUBPROBLEMS TILL ALL PROCS ARE MARKED BUSY                  
C                                                                       
C-----------------------------------------------------------------------
C     WHILE (NBUSY.LT.NPROCS.AND.NBUSY.GT.0)                            
1000  IF (NBUSY.LT.NPROCS.AND.NBUSY.GT.0) THEN                          
C                                                                       
C        FIND ALL THE SPLIT POINTS IN THE NBUSY SUBPROBLEMS             
         NSPLIT=0                                                       
         DO 40 I=1,NBUSY                                                
            IPT = STRWK(BUSY(I))                                        
C           IS THE TOP OF THE STACK GREATER THAN 1?                     
            IF (IWK(IPT+LTOP).GT.1) THEN                                
C              GET THE LEVEL FROM THE TOP ELEMENT OF THE STACK          
               SPLIT(I) = IWK(IPT + LSTK + 3)                           
               NSPLIT   = NSPLIT  + 1                                   
            ELSE                                                        
               SPLIT(I) = 0                                             
            ENDIF                                                       
40       CONTINUE                                                       
C                                                                       
         IF (NSPLIT.EQ.0) THEN                                          
C           IF THERE ARE NO PROBLEMS TO SPLIT THEN FORCE SPLITTING      
C           OF THE FIRST PROCESS IN THE BUSY LIST BY (RE)STARTING       
C           THE BRANCH AND BOUND ALGORITHM                              
50          IPT = STRWK(BUSY(1))                                        
            CALL BBSPL1(N,HAT,KAT,WSIZE,IWK(IPT+1),NCHG)                
            NSUBP = NSUBP + IWK(IPT+LNSB)                               
            CSUBP = CSUBP + IWK(IPT+LNSB)                               
C           DID BBSPL1 FIND A NEW MINIMUM ?                             
            IF (IWK(IPT+LMIN).GT.MAXC) THEN                             
               MAXC = IWK(IPT+LMIN)                                     
               CALL COPYL(N,IWK(IPT+LXM),XMIN)                          
               DO 60 Y=1,NPROCS                                         
                  IWK(STRWK(Y)+LMIN) = MAXC                             
60             CONTINUE                                                 
               NSUBPG = CSUBP + IWK(IPT+LNSG)                           
            ENDIF                                                       
C           DID BBSPL1 SOLVE THE PROBLEM ?                              
            IF (IWK(IPT+LLEV).EQ.-1) THEN                               
C              ATTEMPTED SPLIT SOLVED THE PROBLEM                       
C              TAKE Y OFF BUSY LIST AND ADD IT TO THE FREE LIST         
               NFREE = NFREE + 1                                        
               NBUSY = NBUSY - 1                                        
               FREE(NFREE) = BUSY(1)                                    
               DO 70  I=1,NBUSY                                         
                  BUSY(I) = BUSY(I+1)                                   
70             CONTINUE                                                 
C              GO BACK AND TRY TO SPLIT ANOTHER SUBPROBLEM              
               IF (NBUSY.GT.0) GOTO 50                                  
            ELSE                                                        
C              NO, THEN A NEW SPLIT POINT WAS FOUND                     
               SPLIT(1) = IWK(IPT + LSTK + 3)                           
               NSPLIT = 1                                               
            ENDIF                                                       
         ENDIF                                                          
C                                                                       
         IF (NBUSY.EQ.0) GOTO 3000                                      
C                                                                       
C        NSPLIT PROBLEMS ARE NOW SPLITABLE                              
C        FIND LOWEST SPLITPOINT (HIGHEST PT IN TREE) AND SPLIT          
         IMIN   = 0                                                     
         MINSPL = N+1                                                   
C        EXAMINE SPLITPOINTS OF ALL BUSY SUBPROBLEMS                    
         DO 80 I=1,NBUSY                                                
            IF (SPLIT(I).NE.0.AND.SPLIT(I).LT.MINSPL) THEN              
               IMIN = I                                                 
               MINSPL = SPLIT(IMIN)                                     
            ENDIF                                                       
80       CONTINUE                                                       
C                                                                       
C      --- NOW SPLIT THE PROCESS DENOTED BY BUSY(IMIN)                  
         Y = BUSY(IMIN)                                                 
         Z = FREE(NFREE)                                                
         IPTY = STRWK(Y)                                                
         IPTZ = STRWK(Z)                                                
C        TAKE Z OFF FREE LIST AND ADD IT TO THE BUSY LIST               
         NFREE = NFREE - 1                                              
         NBUSY = NBUSY + 1                                              
         BUSY(NBUSY) = Z                                                
C                                                                       
C      --- INITIALIZE SUBPROBLEM Z FROM SUBPROBLEM Y                    
C        PERMUTATION VECTOR P                                           
         DO 90 I=1,N                                                    
            IWK((IPTZ+LP-1)+I)= IWK((IPTY+LP-1)+I)                      
90       CONTINUE                                                       
C        INITIALIZE TOP OF STACK AND STACK AS EMPTY                     
         IWK(IPTZ+LTOP) =  1                                            
         IWK(IPTZ+LSTK) = -1                                            
         IWK(IPTZ+LNWK) =  0                                            
C        LEVEL OF EXECUTION                                             
         IWK(IPTZ+LLEV) =  SPLIT(IMIN)                                  
C        INCUMBENT                                                      
         IWK(IPTZ+LMIN)= MAXC                                           
C        ZERO-ONE VARIABLES X                                           
         CALL COPYL(N,IWK(IPTY+LX),IWK(IPTZ+LX))                        
C        CHANGE ONE X VARIABLE                                          
         CALL CHANGL(IWK(IPTZ+LX),IWK(IPTZ+LP),SPLIT(IMIN))             
C        -->  SUBROUTINE CHANGL(X,P,LEV)                                
C        -->  X(P(LEV)) = .NOT.X(P(LEV))                                
C        REMOVE SUBPROBLEM FROM Y'S STACK                               
         CALL PLLSTK(IWK(IPTY+LTOP),IWK(IPTY+LSTK))                     
         IWK(IPTY+LNWK) =  0                                            
C        -->  SUBROUTINE PLLSTK(TOPSTK,STACK)                           
C        -->  INTEGER TOPSTK,STACK(3,*)                                 
      GOTO 1000                                                         
      ENDIF                                                             
C     ENDWHILE(NBUSY.LT.NPROCS.AND.NBUSY.GT.0)                          
C                                                                       
C-----------------------------------------------------------------------
C                                                                       
C     SOLVE SUBPROBLEMS IN PARALLEL                                     
C                                                                       
C-----------------------------------------------------------------------
C     WHILE (NBUSY.EQ.NPROCS)                                           
2000  IF (NBUSY.EQ.NPROCS) THEN                                         
C        EXECUTE NPROCS IN PARALEL   THIS IS CALLED ONE CYCLE           
C        EACH PROC WILL SEARCH ITS SUBPROBLEM FOR MAXVT VERTICES        
         CYCLES = CYCLES + 1                                            
         PREMPT = .FALSE.                                               
         DO 100 Y=1,NPROCS-1                                            
            IPT = STRWK(Y)+1                                            
C           SOLVE SUBPROBLEM Y                                          
            CALL BBP(N,HAT,KAT,MINSB,WSIZE,IWK(IPT),PREMPT,NCHG)        
CCCC        CALL DSPTCH('BBP',N,HAT,KAT,MINSB,WSIZE,IWK(IPT),PREMPT)    
100      CONTINUE                                                       
         IPT = STRWK(NPROCS)+1                                          
         CALL BBP(N,HAT,KAT,MINSB,WSIZE,IWK(IPT),PREMPT,NCHG)           
C                                                                       
C        SYNCHRONIZE SUBPROBLEMS (WAIT FOR ALL TO FINISH)               
CCCC     IF (NPROCS.GT.1) CALL SYNCRO                                   
C-----------------------------------------------------------------------
C                                                                       
C        ANALYSIS AND SYNCHRONIZATION PHASE                             
C                                                                       
C-----------------------------------------------------------------------
         NBUSY  = 0                                                     
         NFREE  = 0                                                     
         NEWMAX = .FALSE.                                               
         IMAX   = 0                                                     
         DO 110 Y=1,NPROCS                                              
            IPT = STRWK(Y)                                              
C           IWK(IPT+LNSB)IS THE # OF SUBPROBLEMS SOLVED FOR THIS PROBLEM
            NSUBP = NSUBP + IWK(IPT+LNSB)                               
C           FIND WHICH PROBLEM DID THE MOST TO CALCULATE CRITICAL PATH  
            IF (IWK(IPT+LNSB).GT.IMAX) IMAX=IWK(IPT+LNSB)               
            IF (IWK(IPT+LMIN).GT.MAXC) THEN                             
               NSUBPG = CSUBP + IWK(IPT+LNSG)                           
               NEWMAX = .TRUE.                                          
               MAXC   = IWK(IPT+LMIN)                                   
               CALL COPYL(N,IWK(IPT+LXM),XMIN)                          
            ENDIF                                                       
            IF (IWK(IPT+LLEV).NE.-1) THEN                               
               NBUSY = NBUSY + 1                                        
               BUSY(NBUSY) = Y                                          
            ELSE                                                        
               NFREE = NFREE + 1                                        
               FREE(NFREE) = Y                                          
            ENDIF                                                       
110      CONTINUE                                                       
C        KEEP TRACK OF CRITICAL PATH COUNT OF SUBPROBLEMS               
         CSUBP = CSUBP + IMAX                                           
C                                                                       
C        UPDATE NEW BOUND IF ONE WAS FOUND                              
         IF (NEWMAX) THEN                                               
            DO 120 Y=1,NPROCS                                           
               IWK(STRWK(Y)+LMIN) = MAXC                                
120         CONTINUE                                                    
         ENDIF                                                          
      GOTO 2000                                                         
      ENDIF                                                             
C     ENDWHILE(NBUSY.EQ.NPROCS)                                         
C                                                                       
C     REPEAT UNTIL ALL SUBPROBLEMS HAVE BEEN SOLVED (NBUSY.EQ.0)        
      IF (NBUSY.NE.0) GOTO 1000                                         
C--------------------------------------------------------------------   
C                                                                       
C     END OF CYETURN TO Q01                                     
C                                                                       
C--------------------------------------------------------------------   
3000  CONTINUE                                                          
      WRITE(*,*) 'PROBLEM SOLVED IN ',CYCLES,' CYCLES'                  
      WRITE(*,*) 'CRITICAL PATH SUBPROBLEM COUNT = ',CSUBP              
200   FORMAT(' NOT ENOUGH WORKSPACE:',I8,' NEEDED',I8,' SPECIFIED')     
      RETURN                                                            
C                                                                       
C     END OF                                                    
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE CHANGL(X,P,LEV)                                        
C                                                                       
C     CHANGE THE ZERO-ONE VALUE IN X(P(LEV))                            
C                                                                       
      INTEGER P(*),LEV                                                  
      LOGICAL X(*)                                                      
      X(P(LEV)) = .NOT.X(P(LEV))                                        
      RETURN                                                            
C                                                                       
C     END OF CHANGL                                                     
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE PLLSTK(TOPSTK,STACK)                                   
C                                                                       
C     PULL THE TOP MOST ELEMENT OFF OF THE STACK                        
C                                                                       
      INTEGER TOPSTK,STACK(3,*)                                         
      TOPSTK = TOPSTK-1                                                 
C     -1 IS AT TOP OF STACK.  DO NOT TOUCH IT                           
      DO 10 I=2,TOPSTK                                                  
         STACK(1,I) = STACK(1,I+1)                                      
         STACK(2,I) = STACK(2,I+1)                                      
C        CAUSE BOUNDS TO BE RECALCULATED (SAVE SYNCHRONIZATION TIME)    
         STACK(3,I) = -1                                                
10    CONTINUE                                                          
      RETURN                                                            
C                                                                       
C     END OF PLLSTK                                                     
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE BBP(N,HAT,KAT,MINSB,NWK,IWK,PREMPT,NCHG)               
      INTEGER N,HAT(*),KAT(*),MINSB,NWK,IWK(NWK)                        
      LOGICAL PREMPT                                                    
C                                                                       
C     LOCAL VARIABLES                                                   
      INTEGER MAXWK                                                     
C   --- SET UP STORAGE POINTERS                                         
      LDX =  1                                                          
      LDF =  LDX + (   N    )                                           
      LSTK=  LDF + (   N    )                                           
      LP  = LSTK + ( 3*(N+1))                                           
      LX  =   LP + (   N    )                                           
      LFR =   LX + (   N    )                                           
      LXM =  LFR + (   N    )                                           
      LTOP=  LXM + (   N    )                                           
      LLEV= LTOP + (   1    )                                           
      LMIN= LLEV + (   1    )                                           
      LNSB= LMIN + (   1    )                                           
      LNSG= LNSB + (   1    )                                           
      LNWK= LNSG + (   1    )                                           
      LEND= LNWK + (   1    )                                           
      MAXWK = NWK-LEND                                                  
      CALL BB(N,HAT,KAT,MINSB,MAXWK,                                    
     >    IWK(LDX),IWK(LDF),IWK(LSTK),IWK(LP),IWK(LX),                  
     >    IWK(LFR),IWK(LXM),                                            
     >    IWK(LTOP),IWK(LLEV),IWK(LMIN),IWK(LNSB),IWK(LNSG),IWK(LNWK),  
     >    IWK(LEND),PREMPT,NCHG)                                        
      RETURN                                                            
C                                                                       
C     END OF BBP                                                        
C = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
      END                                                               
CPROCESS VECTOR(REPORT) DIRECTIVE('*VDIR:')                             
      SUBROUTINE BB(N,HAT,KAT,MAXS,MAXWK,                               
     >              DEGFX1,DEGFRE,STACK,P,X,FREE,XMIN,                  
     >              TOPSTK,LEV,MAXC,NSUBP,NSUBPG,ENDWK,                 
     >              IWK,PREMPT,NCHG)                                    
C                                                                       
C  PARAMETERS                                                           
      INTEGER N,MAXS,MAXWK,TOPSTK,LEV,MAXC,NSUBP,NSUBPG,ENDWK           
      INTEGER HAT(*),KAT(*),DEGFX1(N),DEGFRE(N),STACK(3,N),P(N),        
     >        IWK(MAXWK)                                                
      LOGICAL X(N),XMIN(N),FREE(N),PREMPT                               
C                                                                       
C  LOCAL VARIABLES                                                      
      INTEGER G,LEVP1,IP,LP,I,J,IT,DGNEED,NUM1S,NMLEVP                  
      INTEGER NFREE,NEWEWK,ITEMP,IPTR,IVAR                              
      LOGICAL USELST,VALUE                                              
C                                                                       
C*VDIR:  PREFER SCALAR ON                                               
      NSUBP  = 0                                                        
      NSUBPG = 0                                                        
      USELST = .FALSE.                                                  
C     CALCULATE THE UPPER BOUND ON MAXC, G                              
      G = N                                                             
      DO 10 I=1,LEV                                                     
         IP = P(I)                                                      
         FREE(IP) = .FALSE.                                             
         IF(.NOT.X(IP)) G = G-1                                         
10    CONTINUE                                                          
      DO 20 I=LEV+1,N                                                   
         FREE(P(I)) = .TRUE.                                            
20    CONTINUE                                                          
                                                                        
30    IF (LEV.EQ.-1.OR.(NSUBP.GE.MAXS.AND.PREMPT)) GOTO 1000            
C                                                                       
         LEVP1  = LEV    + 1                                            
C                                                                       
         IF (G.LE.MAXC.OR.LEV.EQ.N) THEN                                
C                                                                       
C           TERMINAL NODE FOUND                                         
C                                                                       
C         --- CHECK TO SEE IF IT WAS A LEAF                             
            IF (G.GT.MAXC) THEN                                         
               MAXC = G                                                 
               DO 40 J=1,N                                              
                  XMIN(J) = X(J)                                        
40             CONTINUE                                                 
               NSUBPG = NSUBP+1                                         
               NCHG = NCHG+1                                            
            ENDIF                                                       
C                                                                       
C         --- GET NEW SUBPROBLEM FROM THE STACK                         
            LEV    = STACK(1,TOPSTK)                                    
            G      = STACK(2,TOPSTK)                                    
            ITEMP  = STACK(3,TOPSTK)                                    
            TOPSTK = TOPSTK - 1                                         
            IF (LEV.NE.-1) THEN                                         
               X(P(LEV)) = (.NOT.X(P(LEV)))                             
               DO 50 I=LEV+1,N                                          
                  FREE(P(I)) = .TRUE.                                   
50             CONTINUE                                                 
C         ---  GET DEGREE COUNTS FROM STORAGE IF THERE WAS ENOUGH ROOM  
               IF (ITEMP.NE.-1) THEN                                    
                  ENDWK = ITEMP                                         
                  ITEMP = ENDWK + (N-LEV)                               
                  IPTR = 0                                              
                  DO 55 I=1,N                                           
                     IF (FREE(I)) THEN                                  
                        IPTR = IPTR+1                                   
C                       THE FREE LIST IN P IS NOW IN ORDER              
                        P(LEV+IPTR) = I                                 
                        DEGFRE(I) = IWK(ENDWK+IPTR)                     
                        DEGFX1(I) = IWK(ITEMP+IPTR)                     
                     ENDIF                                              
55                CONTINUE                                              
                  USELST = .TRUE.                                       
               ELSE                                                     
C                 THERE WASN'T ENOUGH ROOM, COUNTS MUST BE RECALCULATED 
                  USELST = .FALSE.                                      
               ENDIF                                                    
            ENDIF                                                       
            NSUBP = NSUBP + 1                                           
C                                                                       
         ELSE                                                           
C                                                                       
C           PROCEED DEPTH FIRST                                         
C                                                                       
C         --- CALCULATE THE DEGREE OF FREE NODES TO FREE NODES          
            IF (USELST) THEN                                            
               LP = P(LEV)                                              
C              LP WAS THE LAST VARIABLE THAT WAS FIXED                  
               IF (X(LP)) THEN                                          
C*VDIR: PREFER VECTOR                                                   
C*VDIR: IGNORE RECRDEPS(DEGFRE,DEGFX1)                                  
                  DO 70 K=KAT(LP),KAT(LP+1)-1                           
                     DEGFRE(HAT(K))  = DEGFRE(HAT(K))  - 1              
                     DEGFX1(HAT(K))  = DEGFX1(HAT(K))  + 1              
70                CONTINUE                                              
               ELSE                                                     
C                 LAST VERTEX WAS THROWN OUT SO DONT BOTHER WITH DEGFX1 
C*VDIR: PREFER VECTOR                                                   
C*VDIR: IGNORE RECRDEPS(DEGFRE)                                         
                  DO 90 K=KAT(LP),KAT(LP+1)-1                           
                     DEGFRE(HAT(K)) = DEGFRE(HAT(K)) - 1                
90                CONTINUE                                              
               ENDIF                                                    
            ELSE                                                        
C              CALCULATE FREE NODE CONNECTIVITY FROM SCRATCH            
               DO 120 I = LEVP1,N                                       
                  IP = P(I)                                             
                  DEGFRE(IP) = 0                                        
                  DEGFX1(IP) = 0                                        
                  DO 110 K=KAT(IP),KAT(IP+1)-1                          
                     J = HAT(K)                                         
                     IF (FREE(J)) THEN                                  
                        DEGFRE(IP) = DEGFRE(IP) + 1                     
                     ELSE                                               
                        IF (X(J)) DEGFX1(IP) = DEGFX1(IP) + 1           
                     ENDIF                                              
110               CONTINUE                                              
120            CONTINUE                                                 
            ENDIF                                                       
C                                                                       
C         --- DETERMINE IF ANY VARIABLE CAN BE FIXED                    
            NUM1S = LEV + G - N                                         
            DGNEED = MAXC - NUM1S -1                                    
            NMLEVP = N - LEVP1                                          
            DO 130 I=LEVP1,N                                            
               IP = P(I)                                                
C                                                                       
C              THREE TESTS TO SEE IF NODE IP CAN BE FIXED               
C                                                                       
C            -1- IS IT DISCONNECTED TO A NODE CURRENTLY FIXED TO ONE?   
               IF (DEGFX1(IP).LT.NUM1S) THEN                            
                  VALUE = .FALSE.                                       
                  IVAR = I                                              
                  G = G - 1                                             
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
C            -2- AFTER THE 1ST TEST WE KNOW IP IS CONNECTED TO          
C                ALL NODES FIXED TO ONE.  SO NOW WE CAN TEST IF IT      
C                IS CONNECTED TO ALL REMAINING FREE NODES.  IF IT IS    
C                THEN FIX IP TO 1                                       
               IF (DEGFRE(IP).GE.NMLEVP) THEN                           
                  VALUE = .TRUE.                                        
                  IVAR = I                                              
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
C            -3- IS ITS TOTAL DEGREE + 1 SMALLER THAN A KNOWN   
C              EACH FREE NODE NEEDS AT LEAST DEGREE DGNEED TO MAKE      
C              A F SIZE MAXC. IF NOT WE CAN THROW NODE AWAY.    
               IF (DEGFRE(IP).LE.DGNEED) THEN                           
C                 FIX TO 0                                              
                  VALUE = .FALSE.                                       
                  G     = G - 1                                         
                  IVAR  = I                                             
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
130         CONTINUE                                                    
C                                                                       
C           NOTE: AT THIS POINT WE KNOW THAT ALL FREE NODES ARE         
C                 CONNECTED TO ALL NODES FIXED TO ONE AND THAT THEY     
C                 ARE ALSO DISCONNECTED TO SOME FREE NODES.  SO         
C                 WE MAY CHOOSE VARIABLE TO BRANCH ON BASED ON HOW      
C                 MUCH THEY ARE CONNECTED OR DISCONNECTED TO OTHER      
C                 FREE NODES.                                           
C                                                                       
C         --- NONE COULD BE FIXED SO FIND VARIABLE I TO BRANCH ON.      
CCC           RULE1: CHOOSE NODE WITH HIGHEST FREE CONNECTIVITY         
C             RULE2: CHOOSE NODE WITH LOWEST FREE CONNECTIVITY          
C             RULE 2 IS FASTER IN VERIFYING OPTIMALITY AND IS USED HERE 
C             RULE 1 IS THE GREEDY METHOD.                              
            IVAR   = LEVP1                                              
            DO 140 I=LEVP1+1,N                                          
               IF (DEGFRE(P(I)).LT.DEGFRE(P(IVAR))) IVAR= I             
140         CONTINUE                                                    
C                                                                       
C         --- SET TO ONE AND STACK THE SUBPROBLEM WITH X(IP) = 0        
            VALUE = .TRUE.                                              
            IF (G-1.GT.MAXC) THEN                                       
C              ONLY STACK SUBPROBLEMS THAT ARE NOT SUBOPTIMAL           
               TOPSTK = TOPSTK + 1                                      
               STACK(1,TOPSTK) = LEVP1                                  
               STACK(2,TOPSTK) = G-1                                    
               NFREE = N-LEVP1                                          
               NEWEWK = ENDWK + (NFREE+NFREE)                           
               IF (NEWEWK.LE.MAXWK) THEN                                
                  STACK(3,TOPSTK) = ENDWK                               
                  FREE(P(IVAR)) = .FALSE.                               
                  ITEMP = ENDWK + NFREE                                 
                  IPTR = 0                                              
                  DO 145 I=1,N                                          
C                    NOTE: THE FOLLOWING REQUIRES THAT THE FREE LIST IN 
C                          THE PERMUTATION P BE IN ORDER                
                     IF (FREE(I)) THEN                                  
                        IPTR = IPTR+1                                   
                        IWK(ENDWK+IPTR) = DEGFRE(I)                     
                        IWK(ITEMP+IPTR) = DEGFX1(I)                     
                     ENDIF                                              
145               CONTINUE                                              
                  ENDWK = NEWEWK                                        
               ELSE                                                     
                  STACK(3,TOPSTK) = -1                                  
               ENDIF                                                    
            ENDIF                                                       
C                                                                       
C         --- IVAR IS NEW FIXED VARIABLE, INCREASE LEV & SWAP IN P      
150         LEV      = LEVP1                                            
            IT       = P(IVAR)                                          
            P(IVAR)  = P(LEV)                                           
            P(LEV)   = IT                                               
            X(IT)    = VALUE                                            
            FREE(IT) = .FALSE.                                          
            USELST   = .TRUE.                                           
C                                                                       
         ENDIF                                                          
         GOTO 30                                                        
C                                                                       
1000  PREMPT = .TRUE.                                                   
      RETURN                                                            
C                                                                       
C     END OF BB                                                         
      END                                                               
C = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
      REAL FUNCTION GGUBFS (DSEED)                                      
      DOUBLE PRECISION   DSEED                                          
      DOUBLE PRECISION   D2P31M,D2P31                                   
C     D2P31M=(2**31) - 1                                                
C     D2P31 =(2**31)(OR AN ADJUSTED VALUE)                              
      DATA               D2P31M/2147483647.D0/                          
      DATA               D2P31 /2147483648.D0/                          
      DSEED = DMOD(16807.D0*DSEED,D2P31M)                               
      GGUBFS = DSEED / D2P31                                            
      RETURN                                                            
      END                                                               
C = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
      SUBROUTINE SECOND(ECPU)                                           
C                                                                       
C  THIS PROCEDURE USES AN IBM UTILITIY THAT GIVES MANY TIMEING VALUES.  
C  WE ARE ONLY INTERESTED IN THE DOUBLE PRECESION CPU TIME.             
C  SEE EXPLANATION OF OTHER VALUES IN IBM UTILITY MANUAL.               
C  THE FOLLOWING CMS COMMAND MUST BE EXECUTED ONCE BEFORE RUNNING FV??? 
C  TO GET ACCESS TO THE IBM UTILITY DATETM.                             
C                                                                       
C    GLOBAL TXTLIB UTILITY                                              
C                                                                       
C  IT IS SUGGESTED THAT THIS COMMAND BE PUT IN YOUR PROFILE EXEC .      
C                                                                       
      LOGICAL*1 DATTIM(23)                                              
      REAL*4 ECPU,ETIME,ETCPU                                           
      INTEGER LD                                                        
      LD = 23                                                           
      CALL DATETM(DATTIM,LD,ECPU,ETIME,ETCPU)                           
C     ECPU = 0.0                                                        
      RETURN                                                            
      END                                                               
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      SUBROUTINE BBSPL1(N,HAT,KAT,NWK,IWK,NCHG)                         
      INTEGER N,HAT(*),KAT(*),NWK,IWK(NWK)                              
C   --- SET UP STORAGE POINTERS                                         
      LDX =  1                                                          
      LDF =  LDX + (   N    )                                           
      LSTK=  LDF + (   N    )                                           
      LP  = LSTK + ( 3*(N+1))                                           
      LX  =   LP + (   N    )                                           
      LFR =   LX + (   N    )                                           
      LXM =  LFR + (   N    )                                           
      LTOP=  LXM + (   N    )                                           
      LLEV= LTOP + (   1    )                                           
      LMIN= LLEV + (   1    )                                           
      LNSB= LMIN + (   1    )                                           
      LNSG= LNSB + (   1    )                                           
      LNWK= LNSG + (   1    )                                           
      LEND= LNWK + (   1    )                                           
      MAXWK = NWK-LEND                                                  
C     BBSPL2 WILL STOP AFTER THE FIRST SUBPROBLEM IS STACKED            
      CALL BBSPL2(N,HAT,KAT,MAXWK,                                      
     >    IWK(LDX),IWK(LDF),IWK(LSTK),IWK(LP),IWK(LX),                  
     >    IWK(LFR),IWK(LXM),                                            
     >    IWK(LTOP),IWK(LLEV),IWK(LMIN),IWK(LNSB),IWK(LNSG),IWK(LNWK),  
     >    IWK(LEND),NCHG)                                               
      RETURN                                                            
C                                                                       
C     END OF BBSPL1                                                     
C= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      END                                                               
CPROCESS VECTOR(REPORT) DIRECTIVE('*VDIR:')                             
      SUBROUTINE BBSPL2(N,HAT,KAT,MAXWK,                                
     >              DEGFX1,DEGFRE,STACK,P,X,FREE,XMIN,                  
     >              TOPSTK,LEV,MAXC,NSUBP,NSUBPG,ENDWK,IWK,NCHG)        
C                                                                       
C  PARAMETERS                                                           
      INTEGER N,MAXWK,TOPSTK,LEV,MAXC,NSUBP,NSUBPG,ENDWK                
      INTEGER HAT(*),KAT(*),DEGFX1(N),DEGFRE(N),STACK(3,N),P(N),        
     >        IWK(MAXWK)                                                
      LOGICAL X(N),XMIN(N),FREE(N)                                      
C                                                                       
C  LOCAL VARIABLES                                                      
      INTEGER G,LEVP1,IP,LP,I,J,IT,DGNEED,NUM1S,NMLEVP                  
      INTEGER NFREE,NEWEWK,ITEMP,IPTR,IVAR                              
      LOGICAL USELST,VALUE                                              
C*VDIR: PREFER SCALAR ON                                                
C                                                                       
      NSUBP  = 0                                                        
      NSUBPG = 0                                                        
      USELST = .FALSE.                                                  
C     CALCULATE THE UPPER BOUND ON MAXC, G                              
      G = N                                                             
      DO 10 I=1,LEV                                                     
         IP = P(I)                                                      
         FREE(IP) = .FALSE.                                             
         IF(.NOT.X(IP)) G = G-1                                         
10    CONTINUE                                                          
      DO 20 I=LEV+1,N                                                   
         FREE(P(I)) = .TRUE.                                            
20    CONTINUE                                                          
                                                                        
30    IF (LEV.EQ.-1.OR.TOPSTK.GT.1) GOTO 1000                           
C                                                                       
         LEVP1  = LEV    + 1                                            
C                                                                       
         IF (G.LE.MAXC.OR.LEV.EQ.N) THEN                                
C                                                                       
C           TERMINAL NODE FOUND                                         
C                                                                       
C         --- CHECK TO SEE IF IT WAS A LEAF                             
            IF (G.GT.MAXC) THEN                                         
               MAXC = G                                                 
               DO 40 J=1,N                                              
                  XMIN(J) = X(J)                                        
40             CONTINUE                                                 
               NSUBPG = NSUBP+1                                         
               NCHG = NCHG + 1                                          
            ENDIF                                                       
C                                                                       
C         --- GET NEW SUBPROBLEM FROM THE STACK                         
            LEV    = STACK(1,TOPSTK)                                    
            G      = STACK(2,TOPSTK)                                    
            ITEMP  = STACK(3,TOPSTK)                                    
            TOPSTK = TOPSTK - 1                                         
            IF (LEV.NE.-1) THEN                                         
               X(P(LEV)) = (.NOT.X(P(LEV)))                             
               DO 50 I=LEV+1,N                                          
                  FREE(P(I)) = .TRUE.                                   
50             CONTINUE                                                 
C         ---  GET DEGREE COUNTS FROM STORAGE IF THERE WAS ENOUGH ROOM  
               IF (ITEMP.NE.-1) THEN                                    
                  ENDWK = ITEMP                                         
                  ITEMP = ENDWK + (N-LEV)                               
                  IPTR = 0                                              
                  DO 55 I=1,N                                           
                     IF (FREE(I)) THEN                                  
                        IPTR = IPTR+1                                   
C                       THE FREE LIST IN P IS NOW IN ORDER              
                        P(LEV+IPTR) = I                                 
                        DEGFRE(I) = IWK(ENDWK+IPTR)                     
                        DEGFX1(I) = IWK(ITEMP+IPTR)                     
                     ENDIF                                              
55                CONTINUE                                              
                  USELST = .TRUE.                                       
               ELSE                                                     
C                 THERE WASN'T ENOUGH ROOM, COUNTS MUST BE RECALCULATED 
                  USELST = .FALSE.                                      
               ENDIF                                                    
            ENDIF                                                       
            NSUBP = NSUBP + 1                                           
C                                                                       
         ELSE                                                           
C                                                                       
C           PROCEED DEPTH FIRST                                         
C                                                                       
C         --- CALCULATE THE DEGREE OF FREE NODES TO FREE NODES          
            IF (USELST) THEN                                            
               LP = P(LEV)                                              
C              LP WAS THE LAST VARIABLE THAT WAS FIXED                  
               IF (X(LP)) THEN                                          
C*VDIR: PREFER VECTOR                                                   
C*VDIR: IGNORE RECRDEPS(DEGFRE,DEGFX1)                                  
                  DO 70 K=KAT(LP),KAT(LP+1)-1                           
                     DEGFRE(HAT(K))  = DEGFRE(HAT(K))  - 1              
                     DEGFX1(HAT(K))  = DEGFX1(HAT(K))  + 1              
70                CONTINUE                                              
               ELSE                                                     
C                 LAST VERTEX WAS THROWN OUT SO DONT BOTHER WITH DEGFX1 
C*VDIR: PREFER VECTOR                                                   
C*VDIR: IGNORE RECRDEPS(DEGFRE)                                         
                  DO 90 K=KAT(LP),KAT(LP+1)-1                           
                     DEGFRE(HAT(K)) = DEGFRE(HAT(K)) - 1                
90                CONTINUE                                              
               ENDIF                                                    
            ELSE                                                        
C              CALCULATE FREE NODE CONNECTIVITY FROM SCRATCH            
               DO 120 I = LEVP1,N                                       
                  IP = P(I)                                             
                  DEGFRE(IP) = 0                                        
                  DEGFX1(IP) = 0                                        
                  DO 110 K=KAT(IP),KAT(IP+1)-1                          
                     J = HAT(K)                                         
                     IF (FREE(J)) THEN                                  
                        DEGFRE(IP) = DEGFRE(IP) + 1                     
                     ELSE                                               
                        IF (X(J)) DEGFX1(IP) = DEGFX1(IP) + 1           
                     ENDIF                                              
110               CONTINUE                                              
120            CONTINUE                                                 
            ENDIF                                                       
C                                                                       
C         --- DETERMINE IF ANY VARIABLE CAN BE FIXED                    
            NUM1S = LEV + G - N                                         
            DGNEED = MAXC - NUM1S -1                                    
            NMLEVP = N - LEVP1                                          
            DO 130 I=LEVP1,N                                            
               IP = P(I)                                                
C                                                                       
C              THREE TESTS TO SEE IF NODE IP CAN BE FIXED               
C                                                                       
C            -1- IS IT DISCONNECTED TO A NODE CURRENTLY FIXED TO ONE?   
               IF (DEGFX1(IP).LT.NUM1S) THEN                            
                  VALUE = .FALSE.                                       
                  IVAR = I                                              
                  G = G - 1                                             
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
C            -2- AFTER THE 1ST TEST WE KNOW IP IS CONNECTED TO          
C                ALL NODES FIXED TO ONE.  SO NOW WE CAN TEST IF IT      
C                IS CONNECTED TO ALL REMAINING FREE NODES.  IF IT IS    
C                THEN FIX IP TO 1                                       
               IF (DEGFRE(IP).GE.NMLEVP) THEN                           
                  VALUE = .TRUE.                                        
                  IVAR = I                                              
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
C            -3- IS ITS TOTAL DEGREE + 1 SMALLER THAN A KNOWN   
C              EACH FREE NODE NEEDS AT LEAST DEGREE DGNEED TO MAKE      
C              A F SIZE MAXC. IF NOT WE CAN THROW NODE AWAY.    
               IF (DEGFRE(IP).LE.DGNEED) THEN                           
C                 FIX TO 0                                              
                  VALUE = .FALSE.                                       
                  G     = G - 1                                         
                  IVAR  = I                                             
                  GOTO 150                                              
               ENDIF                                                    
C                                                                       
130         CONTINUE                                                    
C                                                                       
C           NOTE: AT THIS POINT WE KNOW THAT ALL FREE NODES ARE         
C                 CONNECTED TO ALL NODES FIXED TO ONE AND THAT THEY     
C                 ARE ALSO DISCONNECTED TO SOME FREE NODES.  SO         
C                 WE MAY CHOOSE VARIABLE TO BRANCH ON BASED ON HOW      
C                 MUCH THEY ARE CONNECTED OR DISCONNECTED TO OTHER      
C                 FREE NODES.                                           
C                                                                       
C         --- NONE COULD BE FIXED SO FIND VARIABLE I TO BRANCH ON.      
CCC           RULE1: CHOOSE NODE WITH HIGHEST FREE CONNECTIVITY         
C             RULE2: CHOOSE NODE WITH LOWEST FREE CONNECTIVITY          
C             RULE 2 IS FASTER IN VERIFYING OPTIMALITY AND IS USED HERE 
C             RULE 1 IS THE GREEDY METHOD.                              
            IVAR   = LEVP1                                              
            DO 140 I=LEVP1+1,N                                          
               IF (DEGFRE(P(I)).LT.DEGFRE(P(IVAR))) IVAR=I              
140         CONTINUE                                                    
C                                                                       
C         --- SET TO ONE AND STACK THE SUBPROBLEM WITH X(IP) = 0        
            VALUE = .TRUE.                                              
            IF (G-1.GT.MAXC) THEN                                       
C              ONLY STACK SUBPROBLEMS THAT ARE NOT SUBOPTIMAL           
               TOPSTK = TOPSTK + 1                                      
               STACK(1,TOPSTK) = LEVP1                                  
               STACK(2,TOPSTK) = G-1                                    
               NFREE = N-LEVP1                                          
               NEWEWK = ENDWK + (NFREE+NFREE)                           
               IF (NEWEWK.LE.MAXWK) THEN                                
                  STACK(3,TOPSTK) = ENDWK                               
                  FREE(P(IVAR)) = .FALSE.                               
                  ITEMP = ENDWK + NFREE                                 
                  IPTR = 0                                              
                  DO 145 I=1,N                                          
C                    NOTE: THE FOLLOWING REQUIRES THAT THE FREE LIST IN 
C                          THE PERMUTATION P BE IN ORDER                
                     IF (FREE(I)) THEN                                  
                        IPTR = IPTR+1                                   
                        IWK(ENDWK+IPTR) = DEGFRE(I)                     
                        IWK(ITEMP+IPTR) = DEGFX1(I)                     
                     ENDIF                                              
145               CONTINUE                                              
                  ENDWK = NEWEWK                                        
               ELSE                                                     
                  STACK(3,TOPSTK) = -1                                  
               ENDIF                                                    
            ENDIF                                                       
C                                                                       
C         --- IVAR IS NEW FIXED VARIABLE, INCREASE LEV & SWAP IN P      
150         LEV      = LEVP1                                            
            IT       = P(IVAR)                                          
            P(IVAR)  = P(LEV)                                           
            P(LEV)   = IT                                               
            X(IT)    = VALUE                                            
            FREE(IT) = .FALSE.                                          
            USELST   = .TRUE.                                           
C                                                                       
         ENDIF                                                          
         GOTO 30                                                        
C                                                                       
1000  RETURN                                                            
C                                                                       
C     END OF BBSPL2                                                     
      END                                                               

      subroutine datetm(a,b,c,d,e)
      logical*1 a
      integer b
      real*8 c,d,e
      a=.false.
      b=0
      c=0
      d=0
      e=0
      return
      end

      subroutine clockx(a)
      real*8 a
      a=0
      return
      end

