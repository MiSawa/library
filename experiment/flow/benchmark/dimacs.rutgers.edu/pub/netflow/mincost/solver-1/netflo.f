C  THIS VERSION OF NETFLO WAS MODIFIED FOR COMPATIBILITY WITH SUN UNIX. 
C  THE ONLY CHANGE WAS TO REPLACE FUNCTION CALLS TO  IEOR  WITH CALLS TO 
C   XOR  .  BOTH ARE BITWISE EXCLUSIVE OR FUNCTIONS.  C. MCGEOCH 1/91
C
C  THIS VERSION OF NETFLO WAS PREPARED FOR THE IBM 3081 D.              JOY00010
C                                                                       JOY00030
C  SEE PAGES 244-256  IN ALGORITHMS FOR NETWORK PROGRAMMING, BY
C  KENNINGTON AND HELGASON, JOHN WILEY, 1980 FOR ADDITIONAL INFORMATION
C
C  NOTE:  FOR THIS VERSION IF INPUT HAS UPPER BOUND = 0 OR BLANK, THEN
C         NETFLO INTERPRETS THIS AS AN UNBOUNDED ARC.
C                          IF INPUT HAS UPPER BOUND = -1, THEN
C         NETFLO INTERPRETS THIS AS AN ARC WITH UPPER BOUND = 0.
C                                                                       JOY00050
C         IF THE DATA IN CARD SET 3 PAGE 252 IS INCORRECT, THEN
C         NETFLO WILL DISCOVER THE ERROR AND MAKE THE NECESSARY
C         CHANGES.  IT WILL PRODUCE THE CORRECT ANSWER EVEN IF THIS
C         FILE IS WRONG.  THIS INFORMATION IS SIMPLY USED TO REORDER
C         THE ARCS IN BACKWARD STAR FORMAT.  IT THE DATA IS INCORRECT
C         THE REORDERING PROCESS WILL TAKE LONGER.
C                                                                       JOY00050
C         NETFLO USES THE BIG-M METHOD WHICH CAN CAUSE DIFFICULTIES     JOY00050
C         ON SOME MACHINES.  OUR APPROXIMATION FOR INFINITY IS 10000000.JOY00050
C         IF THE ARC COSTS ARE TOO LARGE, THEN THIS VALUE CAN BE TOO    JOY00050
C         SMALL.  IF YOU INCREASE THIS VALUE, THEN YOU MAY EXCEED THE   JOY00050
C         INTEGER PRECISION OF YOUR HARDWARE.                           JOY00050
C         USE CAUTION AND HAVE FUN!                                     JOY00050
C                                                                       JOY00050
C----------------------------------------------------------             JOY00040
C                                                                       JOY00050
C     NETFLO WAS WRITTEN BY R. V. HELGASON AND J. L. KENNINGTON         JOY00060
C                           OPERATIONS RESEARCH DEPT.                   JOY00070
C                           SOUTHERN METHODIST UNIVERSITY               JOY00080
C                           DALLAS, TEXAS 75275                         JOY00090
C                           (214)-692-3072                              JOY00100
C-----------------------------------------------------------            JOY00110
C                                                                       JOY00120
C     LAST REVISED -- FEB 1988                                          JOY00130
C                                                                       JOY00140
C-----------------------------------------------------------            JOY00150
C                                                                       JOY00160
      INTEGER AAAA,NNNN                                                 JOY00170
      PARAMETER (AAAA=12400,NNNN=1230)                                  JOY00180
C                                                                       JOY00190
      INTEGER   LNODE,MNODE
      INTEGER   DOWN(NNNN),NEXT(NNNN),LEVEL(NNNN)
      INTEGER   ARCID(NNNN),FLOW(NNNN),DUAL(NNNN),CAT(NNNN)
      EQUIVALENCE (DUAL(1),CAT(1))                                      JOY00300
C                                                                       JOY00310
C                                                                       JOY00320
C     NODE FORMAT                                                       JOY00330
C                                                                       JOY00340
C                                                                       JOY00350
C     1 - PREDECESSOR OR DOWN POINTER                                   JOY00360
C     2 - THREAD OR NEXT POINTER                                        JOY00370
C     3 - LEVEL NUMBER                                                  JOY00380
C     4 - ASSOCIATED ARC IDENTIFIER                                     JOY00390
C     (+=ORIENTATION OPPOSITE TO DOWN POINTER)                          JOY00400
C     (-=ORIENTATION SAME AS DOWN POINTER)                              JOY00410
C     5 - FLOW ON ARC                                                   JOY00420
C     6 - DUAL VARIABLE VALUE                                           JOY00430
C                                                                       JOY00440
      INTEGER   LARC,MARC                                               JOY00460
      INTEGER   FROM(AAAA),COST(AAAA),CAPAC(AAAA)
      INTEGER   FLOOR(AAAA),NAME(AAAA)
      INTEGER   ARCNAM
C                                                                       JOY00530
C     ARC FORMAT                                                        JOY00540
C                                                                       JOY00550
C     1 - FROM NODE                                                     JOY00560
C     2 - COST                                                          JOY00570
C     3 - CAPACITY(-,IF AT UB)                                          JOY00580
C     4 - LOWER BOUND                                                   JOY00590
C     5 - NAME                                                          JOY00600
C                                                                       JOY00610
C     ARC(ARTY) IS GENERAL REFERENCE FOR ARTIFICIAL ARCS                JOY00620
C                                                                       JOY00630
      INTEGER   TO                                                      JOY00640
      INTEGER   SLACK,ARTIF,DUMMY,XCESS
      INTEGER   PRICE0,TOO                                              JOY00660
      INTEGER   TRY,PRICE                                               JOY00670
      INTEGER   NEWARC,NEWPR                                            JOY00680
      INTEGER   NEWFRM,NEWTO                                            JOY00690
      INTEGER   ARTY,BIG,ARTYP1                                         JOY00700
      INTEGER   THD                                                     JOY00710
      INTEGER   DW(2),CH(2),DWN,CHG                                     JOY00720
      INTEGER   THETA,JTHETA,KTHETA,POSS,JPOSS                          JOY00730
      INTEGER   FRM,LVJ                                                 JOY00740
      INTEGER   FM,LST                                                  JOY00750
      INTEGER   DWE                                                     JOY00760
      INTEGER   FLW,AID                                                 JOY00770
      INTEGER   Q1,Q2                                                   JOY00780
      INTEGER   DIR,REF                                                 JOY00790
      INTEGER   U1,U2,U3,U4                                             JOY00800
      LOGICAL   INFEAS                                                  JOY00850
      LOGICAL   OPTIM                                                   JOY00860
      LOGICAL   DMP                                                     JOY00870
      LOGICAL   PPR                                                     JOY00880
      LOGICAL   PRES                                                    JOY00890
C                                                                       JOY00900
      DATA ITDOPT/ 1 /                                                  JOY00910
      DATA ARTY/ 1 /                                                    JOY00940
      DATA BIG/  10000000 /                                             JOY00950
      DATA U1,U2,U3,U4/5,5,5,5/                                         JOY00980
C                                                                       JOY00990
      DATA SLACK/  66666/                                               JOY01000
      DATA ARTIF/  99999/                                               JOY01010
      DATA DUMMY/  77777/                                               JOY01020
      DATA XCESS/  88888/                                               JOY01030
C                                                                       JOY01040
 1000 FORMAT(8I10)                                                      JOY01050
 1001 FORMAT(1X,8I10)                                                   JOY01060
 1002 FORMAT(2X,I8,5I10)                                                JOY01070
 1003 FORMAT(1X,I10,5I10)                                               JOY01080
 1004 FORMAT(F10.0)                                                     JOY01090
 1005 FORMAT(1X,F10.5)                                                  JOY01100
 2001 FORMAT(1X,I5,1H,,4I15,1X,A10)                                     JOY01110
 2002 FORMAT(1X,I5,1H,,6I15)                                            JOY01120
 8910 FORMAT(8H0PROBLEM,I5)                                             JOY01230
C                                                                       JOY01250
C     NSTOP - EXIT CONDITION                                            JOY01260
C     MNODE - NUMBER OF NODES                                           JOY01270
C     NET -TOTAL BALANCE IN NETWORK                                     JOY01280
C     MSORC - NUMBER OF SOURCES                                         JOY01290
C     MSINK - NUMBER OF SINKS                                           JOY01300
C     MARC - NUMBER OF ARCS                                             JOY01310
C     MTREE - NUMBER OF BRANCHES ON TREE(EXCLUDING ROOT)                JOY01320
C     THD - POINTER MOVING ALONG THREAD                                 JOY01330
C     TRY - VARIABLE ENCOUNTERED DURING SETUP OR PRICING                JOY01340
C     PRICE - REDUCED  COST FOR TRY                                     JOY01350
C     NEWARC  - BEST VARIABLE TO ENTER                                  JOY01360
C     NEWPR - PRICE FOR NEWARC                                          JOY01370
C     DW - DOWN PTRS FOR RATIO TEST(FROM STEM,TO STEM)                  JOY01380
C     CH - PATH CONDITIONS FOR RATIO TEST (FROM STEM,TO STEM)           JOY01390
C     DWN - POINTER MOVING ALONG DOWN PATH                              JOY01400
C     CHG - PATH CONDITION                                              JOY01410
C     THETA - MINIMUM RATIO IN RATIO TEST                               JOY01420
C     JTHETA - UB (-) OR LB(+1) CONDITION FOR MIN THETA                 JOY01430
C     KTHETA - MIN THETA OCCURS ON FROM STEM(1) OR TO STEM (2)          JOY01440
C     POSS - CANDIDATE FOR MIN THETA                                    JOY01450
C     JPOSS - UB (-1) OR LB(+1) CONDITION FOR CANDIDATE FOR MIN THETA   JOY01460
C     DWE - ROOT OF CYCLE                                               JOY01470
C                                                                       JOY01480
      OPEN(5,FILE='NETFLO.INP')
      OPEN(6,FILE='NETFLO.OUT')
      ARTYP1 = ARTY+1
      LNODE = NNNN-2
      LARC =  AAAA-2
      LARCP1 = LARC+1                                                   JOY01740
C     UNUSED NODE NUMBER                                                JOY01750
      LNODP1 = LNODE+1                                                  JOY01760
      NPROB = 0                                                         JOY01800
      KARD = 0                                                          JOY01810
    5 CONTINUE                                                          JOY01830
      PPR = .TRUE.                                                      JOY01840
      PRES = .FALSE.                                                    JOY01850
      NPROB = NPROB+1                                                   JOY01860
C     INPUT NUMBER OF REGULAR NODES                                     JOY02060
      NSTOP = 2                                                         JOY02100
      READ(U1,1000,END=999) I                                           JOY02110
C                                                                       JOY02120
C     NUMBER OF NODES(I10)                                              JOY02130
C                                                                       JOY02140
      KARD = KARD+1                                                     JOY02150
      IF(PPR) WRITE(6,8910) NPROB                                       JOY02160
      WRITE(6,1001) I                                                   JOY02170
      NSTOP = 3                                                         JOY02190
      IF(I.LE.0) GO TO 999                                              JOY02200
      MNODE = I                                                         JOY02210
      MNODP1 = MNODE+1                                                  JOY02220
      MNODP2 = MNODE+2                                                  JOY02230
      NSTOP = 4                                                         JOY02240
      IF(MNODP1.GT.LNODE) GO TO 999                                     JOY02250
      DUAL(MNODP1) = 0                                                  JOY02260
C     INITIALIZE NODE ARRAY                                             JOY02270
      DO 10 J10=1,MNODP1                                                JOY02280
      DOWN(J10) = 0
      NEXT(J10) = 0
      LEVEL(J10) = 0
      ARCID(J10) = 0
      FLOW(J10) = 0
   10 CONTINUE
C     INITIALIZE ARTIFICIAL                                             JOY02310
      FROM(ARTY) = MNODP1                                               JOY02320
      COST(ARTY) = BIG                                                  JOY02330
      CAPAC(ARTY) = 0                                                   JOY02340
      FLOOR(ARTY) = 0                                                   JOY02350
      NAME(ARTY) = ARTIF                                                JOY02360
C                                                                       JOY02370
C     DURING SETUP WE USE THE ABSENCE OF THE UPPER BOUND FLAG           JOY02380
C     ON AN ARC TO INDICATE THAT THE ARC IS ELIGIBLE TO BECOME          JOY02390
C     PART OF THE STARTING BASIS, OTHERWISE IT IS NOT.                  JOY02400
C                                                                       JOY02410
      NET = 0                                                           JOY02420
      MSORC = 0                                                         JOY02430
      MARC = 0                                                          JOY02440
C     INITIALIZE SUPPLY AND DEMAND LISTS WITH SELF-POINTER              JOY02450
      NEXT(MNODP1) = MNODP1                                             JOY02460
      DOWN(MNODP1) = MNODP1                                             JOY02470
C     INPUT NON-ZERO NODAL BALANCES                                     JOY02480
   15 CONTINUE                                                          JOY02490
      NSTOP = 5                                                         JOY02510
      READ(U2,1000,END=999) I,J                                         JOY02520
C                                                                       JOY02530
C     NODE NUMBER(I10),BALANCE(I10)/BLANK= ESCAPE                       JOY02540
C                                                                       JOY02550
      KARD = KARD+1                                                     JOY02560
      WRITE(6,1001) I,J                                                 JOY02570
      NSTOP = 6                                                         JOY02590
      IF(I) 999,60,20                                                   JOY02600
   20 CONTINUE                                                          JOY02610
      IF(I.GT.MNODE) GO TO 999                                          JOY02620
      NSTOP = 7                                                         JOY02630
      IF(FLOW(I).NE.0) GO TO 999                                        JOY02640
      FLOW(I) = J                                                       JOY02650
      NET = NET+J                                                       JOY02660
      IF(J.LE.0) GO TO 15                                               JOY02670
      MSORC = MSORC+1                                                   JOY02680
C     SAVE ORIGINAL SUPPLY IN LEVEL                                     JOY02690
      LEVEL(I) = J                                                      JOY02700
      NEXT(I) = NEXT(MNODP1)                                            JOY02710
      NEXT(MNODP1) = I                                                  JOY02720
      GO TO 15                                                          JOY02730
   60 CONTINUE                                                          JOY02740
C     TEST FOR FEASIBILITY                                              JOY02750
      NSTOP = 8                                                         JOY02760
      IF(NET.LT.0) GO TO 999                                            JOY02770
C     INPUT NUMBER OF ARCS TO EACH NODE(IN NUMERIC ORDER)               JOY02780
      NSTOP = 9                                                         JOY02800
      READ(U3,1000,END=999) (CAT(I),I=1,MNODE)                          JOY02810
C                                                                       JOY02820
C     NUMBER OF ARCS TO EACH NODE (8I10)                                JOY02830
C                                                                       JOY02840
      KARD = KARD+1                                                     JOY02850
      WRITE(6,1001) (CAT(I),I=1,MNODE)                                  JOY02860
C                                                                       JOY02880
C     RESERVE LOCATIONS FOR INPUT ARCS BY FILLING WITH DUMMIES          JOY02890
C      CAT(N) WILL POINT TO THE NEXT OPEN LOCATION FOR STORING          JOY02900
C      ARCS WHOSE TERMINAL NODE IS N.                                   JOY02910
C                                                                       JOY02920
      NSTOP = 10                                                        JOY02930
      I = 1                                                             JOY02940
      J = ARTY                                                          JOY02950
      DO 80 J80 = 1,MNODE                                               JOY02960
      I = -I                                                            JOY02970
      K70 = MAX0(1,CAT(J80))
      IF(J+K70.GT.LARC) GO TO 999                                       JOY02990
      CAT(J80) = ISIGN(J+1,I)                                           JOY03000
      DO 70 I70=1,K70                                                   JOY03010
      J = J+1                                                           JOY03020
C     NOTE THAT DUMMY ARCS HAVE SAME INITIAL AND TERMINAL NODE          JOY03030
      FROM(J) = ISIGN(J80,I)                                            JOY03040
      COST(J) = 0                                                       JOY03050
      CAPAC(J) = -BIG                                                   JOY03060
      FLOOR(J) = 0
      NAME(J) = DUMMY
   70 CONTINUE                                                          JOY03090
   80 CONTINUE                                                          JOY03100
      MARC = J+1                                                        JOY03110
      IF(MARC.GT.LARC) GO TO 999                                        JOY03120
      FROM(MARC) = ISIGN(MNODP1,-I)                                     JOY03130
C     INPUT REGULAR ARCS                                                JOY03140
      KOST0 = 0                                                         JOY03150
   90 CONTINUE                                                          JOY03160
      NSTOP = 11                                                        JOY03180
      READ(U4,1002,END=999)    ARCNAM,I,J,K,L,M                         JOY03190
C                                                                       JOY03200
C     NAME,FROM NODE,TO NODE,COST,CAPACITY,LOWER BOUND                  JOY03210
C      (I10) , (I10) , (I10),(I10), (I10) ,    (I10)                    JOY03220
C     /BLANK = ESCAPE                                                   JOY03230
C                                                                       JOY03240
      KARD = KARD+1                                                     JOY03250
      WRITE(6,1003)    ARCNAM,I,J,K,L,M                                 JOY03260
      NSTOP = 12                                                        JOY03280
      IF(I) 999,150,100                                                 JOY03290
  100 CONTINUE                                                          JOY03300
      IF(I.GT.MNODE) GO TO 999                                          JOY03310
      IF(J.GT.MNODE) GO TO 999                                          JOY03320
      IF(J.LE.0) GO TO 999                                              JOY03330
      NSTOP = 13                                                        JOY03340
      IF(L.GE.BIG) GO TO 999                                            JOY03350
      IF(L.EQ.0) L = BIG                                                JOY03360
      IF(L.LT.0) L = 0                                                  JOY03370
      IF(M.GE.BIG) GO TO 999                                            JOY03380
      IF(M.LT.0) GO TO 999                                              JOY03390
      IF(M.GT.L) GO TO 999                                              JOY03400
      II = CAT(J)                                                       JOY03410
      JJ = IABS(II)                                                     JOY03420
C     TEST TO SEE IF CATEGORY IS FULL                                   JOY03430
      KK = ISIGN(LNODP1,II)                                             JOY03440
      IF( XOR(KK,FROM(JJ)).GT.0) GO TO 140                              JOY03450
C     MOVE REST OF ARCS DOWN TO ACCOMODATE                              JOY03460
      NSTOP = 14                                                        JOY03470
      IF(MARC.EQ.LARC) GO TO 999                                        JOY03480
      MARC = MARC+1                                                     JOY03490
      K120 = MARC-JJ                                                    JOY03500
      M120 = MARC                                                       JOY03510
      DO 120 J120=1,K120                                                JOY03520
      L120 = M120-1                                                     JOY03530
      FROM(M120)  = FROM(L120)
      COST(M120)  = COST(L120)
      CAPAC(M120) = CAPAC(L120)
      FLOOR(M120) = FLOOR(L120)
      NAME(M120)  = NAME(L120)
      M120 = L120                                                       JOY03570
  120 CONTINUE                                                          JOY03580
      DO 130 J130=J,MNODE                                               JOY03590
  130 CAT(J130) = CAT(J130)+ISIGN(1,CAT(J130))                          JOY03600
C     INSERT NEW ARC                                                    JOY03610
  140 CONTINUE                                                          JOY03620
      FROM(JJ) = ISIGN(I,II)                                            JOY03630
      COST(JJ) = K                                                      JOY03640
      KOST0 = KOST0+K*M                                                 JOY03650
      CAPAC(JJ) = L-M                                                   JOY03660
      FLOOR(JJ) = M                                                     JOY03670
      FLOW(I) = FLOW(I)-M                                               JOY03680
      FLOW(J) = FLOW(J)+M                                               JOY03690
      NAME(JJ) = ARCNAM                                                 JOY03700
      CAT(J) = ISIGN(JJ+1,II)                                           JOY03710
C     MARK FROM NODE WITH ARC RUNNING FROM IT FLAG                      JOY03720
      ARCID(I) = -1                                                     JOY03730
      GO TO 90                                                          JOY03740
  150 CONTINUE                                                          JOY03750
C                                                                       JOY03760
C     ELIMINATE NON-ESSENTIAL DUMMY ARCS                                JOY03770
C     CAT(N) WILL POINT TO LOCATION OF FIRST ARC TERMINAL AT NODE N     JOY03780
C                                                                       JOY03790
      I = LNODP1                                                        JOY03800
      K = ARTY                                                          JOY03810
      L = 0                                                             JOY03820
      MARC = MARC-1                                                     JOY03830
      DO 190 J190=ARTYP1,MARC                                           JOY03840
      J = FROM(J190)                                                    JOY03850
      IF( XOR(I,J).GT.0) GO TO 160                                      JOY03860
      I = -I                                                            JOY03870
      L = L+1                                                           JOY03880
      CAT(L) = K+1                                                      JOY03890
      GO TO 170                                                         JOY03900
  160 IF(IABS(J).EQ.L) GO TO 190                                        JOY03910
  170 K = K+1                                                           JOY03920
      IF(K.EQ.J190) GO TO 190                                           JOY03930
      FROM(K)  = FROM(J190)
      COST(K)  = COST(J190)
      CAPAC(K) = CAPAC(J190)
      FLOOR(K) = FLOOR(J190)
      NAME(K)  = NAME(J190)
  190 CONTINUE                                                          JOY03970
      MARC = K                                                          JOY03980
      MREG = K                                                          JOY03990
      NSTOP = 15                                                        JOY04000
      IF(MARC+MAX0(1,MSORC)+1.GT.LARC) GO TO 999                        JOY04010
C                                                                       JOY04020
C     ADD REGULAR SLACKS                                                JOY04030
C                                                                       JOY04040
      I = -FROM(MARC)                                                   JOY04050
      THD = NEXT(MNODP1)                                                JOY04060
      NEXT(MNODP1) = MNODP1                                             JOY04070
      IF(THD.NE.MNODP1) GO TO 192                                       JOY04080
C     NO REGULAR SLACKS, ADD DUMMY                                      JOY04090
      MARC = MARC+1                                                     JOY04100
      FROM(MARC) = ISIGN(MNODP1,I)                                      JOY04110
      COST(MARC) = 0                                                    JOY04120
      CAPAC(MARC) = -BIG                                                JOY04130
      FLOOR(MARC) = 0                                                   JOY04140
      NAME(MARC) = DUMMY                                                JOY04150
      GO TO 194                                                         JOY04160
C     FOLLOW LIST                                                       JOY04170
  192 CONTINUE                                                          JOY04180
      MARC = MARC+1                                                     JOY04190
      NAME(MARC) = SLACK                                                JOY04200
      FROM(MARC) = ISIGN(THD,I)                                         JOY04210
      COST(MARC) = 0                                                    JOY04220
      CAPAC(MARC) = LEVEL(THD)                                          JOY04230
      LEVEL(THD) = 0                                                    JOY04240
      FLOOR(MARC) = 0                                                   JOY04250
      NXT = NEXT(THD)                                                   JOY04260
      NEXT(THD) = 0                                                     JOY04270
      THD = NXT                                                         JOY04280
      IF(THD.NE.MNODP1) GO TO 192                                       JOY04290
  194 CONTINUE                                                          JOY04300
      MSLK = MARC                                                       JOY04310
C     ADD EXCESS ARC AT END OF REGULAR SLACKS                           JOY04320
      MARC = MARC+1                                                     JOY04330
      FROM(MARC) = ISIGN(MNODP2,-I)                                     JOY04340
      COST(MARC) = BIG                                                  JOY04350
      CAPAC(MARC) = 0                                                   JOY04360
      FLOOR(MARC) = 0                                                   JOY04370
      NAME(MARC) = XCESS                                                JOY04380
C                                                                       JOY04390
C     LOCATE SOURCES AND SINKS FOR PROBLEM                              JOY04400
C     ADJUSTED FOR LOWER BOUNDS                                         JOY04410
C                                                                       JOY04420
      NET = 0                                                           JOY04430
      MTREE = 0                                                         JOY04440
      THD = MNODP1                                                      JOY04450
      DO 200 I200 = 1,MNODE                                             JOY04460
      J = FLOW(I200)                                                    JOY04470
      NET = NET+J                                                       JOY04480
      IF(J) 30,200,50                                                   JOY04490
C     SINK                                                              JOY04500
   30 CONTINUE                                                          JOY04510
      FLOW(I200) = -J                                                   JOY04520
C     LINK DEMANDS IN DECREASING SIZE ORDER                             JOY04530
      DWN = MNODP1                                                      JOY04540
   35 CONTINUE                                                          JOY04550
      NXT = DOWN(DWN)                                                   JOY04560
      IF(FLOW(NXT)+J.LE.0) GO TO 40                                     JOY04570
      DWN = NXT                                                         JOY04580
      GO TO 35                                                          JOY04590
   40 CONTINUE                                                          JOY04600
      DOWN(DWN) = I200                                                  JOY04610
      DOWN(I200) = NXT                                                  JOY04620
      LEVEL(I200) = -1                                                  JOY04630
      GO TO 200                                                         JOY04640
C     SOURCE                                                            JOY04650
   50 CONTINUE                                                          JOY04660
      MTREE = MTREE+1                                                   JOY04670
      ARCID(I200) = -MARC                                               JOY04680
      FLOW(I200) = J                                                    JOY04690
      NEXT(THD) = I200                                                  JOY04700
      DOWN(I200) = MNODP1                                               JOY04710
      NEXT(I200) = MNODP1                                               JOY04720
      LEVEL(I200) = 1                                                   JOY04730
      DUAL(I200) = BIG                                                  JOY04740
      THD = I200                                                        JOY04750
  200 CONTINUE                                                          JOY04760
C     CHECK FOR FEASIBILITY                                             JOY04770
      NSTOP = 16                                                        JOY04780
      IF(NET.LT.0) GO TO 999                                            JOY04790
 2331 FORMAT(1X,I5,1H,,4I5,1X,A6)                                       JOY04800
 2332 FORMAT(1X,I5,1H,,5I8,I15)                                         JOY04810
 8339 FORMAT(10H ITERATION,I10)                                         JOY04820
 8704 FORMAT(   '**** DUMP ENABLED AT',I7)                              JOY04830
C                                                                       JOY04840
C     ADVANCED START                                                    JOY04850
C                                                                       JOY04860
C     SELECT HIGHEST RANK DEMAND ON LIST                                JOY04900
 1010 CONTINUE                                                          JOY04910
      TO = DOWN(MNODP1)                                                 JOY04920
C     IS LIST EXHAUSTED                                                 JOY04930
      IF(TO.EQ.MNODP1) GO TO 210                                        JOY04940
 1020 CONTINUE                                                          JOY04950
C     SET TO LINK TO ARTIFICIAL                                         JOY04960
      NEWARC = ARTY                                                     JOY04970
      NEWPR = BIG                                                       JOY04980
C     ANY DEMAND LEFT                                                   JOY04990
      IF(FLOW(TO).EQ.0) GO TO 1110                                      JOY05000
C                                                                       JOY05010
C     LOOK FOR SOURCES FIRST                                            JOY05020
C                                                                       JOY05030
      TRY = CAT(TO)                                                     JOY05040
      FRM = FROM(TRY)                                                   JOY05050
      LST = ISIGN(LNODP1,FRM)                                           JOY05060
 1030 CONTINUE                                                          JOY05070
C     IS IT UNAVAILABLE                                                 JOY05080
      IF(CAPAC(TRY).LE.0) GO TO 1050                                    JOY05090
      FM = IABS(FRM)                                                    JOY05100
C     IS IT FROM A NON-SOURCE                                           JOY05110
      IF(LEVEL(FM).NE.1) GO TO 1050                                     JOY05120
      IF(ARCID(FM).EQ.ARTY) GO TO 1050                                  JOY05130
      PRICE = COST(TRY)                                                 JOY05140
C     IS COST WORSE                                                     JOY05150
      IF(PRICE.GE.NEWPR) GO TO 1050                                     JOY05160
C     DOES CAPACITY EXCEED DEMAND                                       JOY05170
      IF(CAPAC(TRY).GT.FLOW(TO)) GO TO 1040                             JOY05180
C     IS THERE NOT ENOUGH SUPPLY FOR CAPACITY                           JOY05190
      IF(FLOW(FM).LT.CAPAC(TRY)) GO TO 1050                             JOY05200
      NEWARC = -TRY                                                     JOY05210
      NEWPR = PRICE                                                     JOY05220
      IF(NEWPR.EQ.0) GO TO 1055                                         JOY05230
      GO TO 1050                                                        JOY05240
 1040 CONTINUE                                                          JOY05250
C     IS THERE NOT ENOUGH SUPPLY FOR DEMAND                             JOY05260
      IF(FLOW(FM).LT.FLOW(TO)) GO TO 1050                               JOY05270
      NEWARC = TRY                                                      JOY05280
      NEWPR = PRICE                                                     JOY05290
      IF(NEWPR.EQ.0) GO TO 1055                                         JOY05300
 1050 CONTINUE                                                          JOY05310
      TRY = TRY+1                                                       JOY05320
      FRM = FROM(TRY)                                                   JOY05330
      IF( XOR(FRM,LST).GT.0) GO TO 1030                                 JOY05340
      IF(NEWARC.EQ.ARTY) GO TO 1070                                     JOY05350
 1055 CONTINUE                                                          JOY05360
C     ARE WE SENDING DEMAND                                             JOY05370
      IF(NEWARC.GT.0) GO TO 1060                                        JOY05380
C     SEND CAPACITY                                                     JOY05390
      NEWARC = -NEWARC                                                  JOY05400
      FM = IABS(FROM(NEWARC))                                           JOY05410
C     GET CAPACITY                                                      JOY05420
      FLW = CAPAC(NEWARC)                                               JOY05430
C     MARK UNAVAILABLE                                                  JOY05460
      CAPAC(NEWARC) = -FLW                                              JOY05470
C     ADJUST FLOWS                                                      JOY05480
      FLOW(FM) = FLOW(FM)-FLW                                           JOY05490
      FLOW(TO) = FLOW(TO)-FLW                                           JOY05500
      GO TO 1020                                                        JOY05510
C     SEND DEMAND                                                       JOY05520
 1060 CONTINUE                                                          JOY05530
C     MARK UNAVAILABLE                                                  JOY05540
      CAPAC(NEWARC) = -CAPAC(NEWARC)                                    JOY05550
C     ADJUST FLOWS                                                      JOY05560
      FM = IABS(FROM(NEWARC))                                           JOY05570
      FLOW(FM) = FLOW(FM)-FLOW(TO)                                      JOY05580
      K = BIG                                                           JOY05600
      GO TO 1115                                                        JOY05610
C                                                                       JOY05620
C     LOOK FOR TRANSSHIPMENT POINTS                                     JOY05630
C                                                                       JOY05640
 1070 CONTINUE                                                          JOY05650
      TRY = CAT(TO)                                                     JOY05660
      FRM = FROM(TRY)                                                   JOY05670
 1080 CONTINUE                                                          JOY05680
C     IS IT UNAVAILABLE                                                 JOY05690
      IF(CAPAC(TRY).LE.0) GO TO 1090                                    JOY05700
      FM = IABS(FRM)                                                    JOY05710
C     IS IT ALREADY LINKED                                              JOY05720
      IF(LEVEL(FM).NE.0) GO TO 1090                                     JOY05730
      PRICE = COST(TRY)                                                 JOY05740
C     IS COST WORSE                                                     JOY05750
      IF(PRICE.GE.NEWPR) GO TO 1090                                     JOY05760
      NEWARC = TRY                                                      JOY05770
      NEWPR = PRICE                                                     JOY05780
      IF(NEWPR.EQ.0) GO TO 1095                                         JOY05790
 1090 CONTINUE                                                          JOY05800
      TRY = TRY+1                                                       JOY05810
      FRM = FROM(TRY)                                                   JOY05820
      IF( XOR(FRM,LST).GT.0) GO TO 1080                                 JOY05830
      IF(NEWARC.EQ.ARTY) GO TO 1110                                     JOY05840
 1095 CONTINUE                                                          JOY05850
      FM = IABS(FROM(NEWARC))                                           JOY05860
C     DOES CAPACITY EXCEED DEMAND                                       JOY05870
      IF(CAPAC(NEWARC).GT.FLOW(TO)) GO TO 1100                          JOY05880
C     GET CAPACITY                                                      JOY05890
      FLW = CAPAC(NEWARC)                                               JOY05900
C     MARK UNAVAILABLE                                                  JOY05910
      CAPAC(NEWARC) = -FLW                                              JOY05920
C     ADJUST FLOWS                                                      JOY05930
      FLOW(FM) = FLW                                                    JOY05940
      FLOW(TO) = FLOW(TO)-FLW                                           JOY05950
C     LINK IN TO DEMAND LIST                                            JOY05960
      DOWN(FM) = TO                                                     JOY05970
      DOWN(MNODP1) = FM                                                 JOY05980
C     START NEW CHAIN                                                   JOY05990
      LEVEL(FM) = -1                                                    JOY06000
      GO TO 1010                                                        JOY06010
 1100 CONTINUE                                                          JOY06020
C     MARK UNAVAILABLE                                                  JOY06030
      CAPAC(NEWARC) = -CAPAC(NEWARC)                                    JOY06040
C     PASS ALONG FLOW DEMAND                                            JOY06050
      FLOW(FM) = FLOW(TO)                                               JOY06060
C     LINK IN NEW DEMAND NODE                                           JOY06070
      DOWN(FM) = DOWN(TO)                                               JOY06080
      DOWN(TO) = FM                                                     JOY06090
      DOWN(MNODP1) = FM                                                 JOY06100
      NEXT(FM) = TO                                                     JOY06110
      ARCID(TO) = NEWARC                                                JOY06120
      LEVEL(FM) = LEVEL(TO)-1                                           JOY06130
      DUAL(TO) = NEWPR                                                  JOY06140
      MTREE = MTREE+1                                                   JOY06150
      GO TO 1010                                                        JOY06160
C     ADD CHAIN TO TREE                                                 JOY06170
 1110 CONTINUE                                                          JOY06180
      K = 0                                                             JOY06200
 1115 CONTINUE                                                          JOY06210
C     REMOVE FROM DEMAND LIST                                           JOY06230
      DOWN(MNODP1) = DOWN(TO)                                           JOY06240
C     LINK IN AS LEFTMOST BRANCH                                        JOY06250
      FM = IABS(FROM(NEWARC))                                           JOY06260
      ARCID(TO) = NEWARC                                                JOY06270
      DUAL(TO) = NEWPR                                                  JOY06280
      DOWN(TO) = FM                                                     JOY06290
      I = NEXT(FM)                                                      JOY06300
      NEXT(FM) = TO                                                     JOY06310
      J = LEVEL(FM)-LEVEL(TO)+1                                         JOY06320
      THD = FM                                                          JOY06330
 1120 CONTINUE                                                          JOY06340
C     MOVE ALONG CHAIN                                                  JOY06350
      THD = NEXT(THD)                                                   JOY06360
C     ADJUST LEVEL AND DUAL VARIABLES                                   JOY06370
      L = LEVEL(THD)                                                    JOY06380
      LEVEL(THD) = L+J                                                  JOY06390
      K = K-DUAL(THD)                                                   JOY06400
      DUAL(THD) = K                                                     JOY06410
      IF(L.NE.-1) GO TO 1120                                            JOY06420
      NEXT(THD) = I                                                     JOY06430
      MTREE = MTREE+1                                                   JOY06440
      GO TO 1010                                                        JOY06450
  210 CONTINUE                                                          JOY06460
C     SET UP TO EXPAND TREE                                             JOY06540
      TO = 1                                                            JOY06550
      TRY = ARTYP1                                                      JOY06560
      FRM = FROM(TRY)                                                   JOY06570
  220 CONTINUE                                                          JOY06580
C     DO WE NEED TO EXPAND TREE TO REACH ALL NODES                      JOY06590
      IF(MTREE.EQ.MNODE) GO TO 285                                      JOY06600
      TOO = TO                                                          JOY06610
      NEWPR = BIG                                                       JOY06620
C     SEARCH FOR LEAST COST CONNECTABLE ARC IN CURRENT TO-GROUP         JOY06630
  230 CONTINUE                                                          JOY06640
      LVJ = LEVEL(TO)                                                   JOY06650
      LST = ISIGN(LNODP1,FRM)                                           JOY06660
  235 CONTINUE                                                          JOY06670
      IF(CAPAC(TRY).LE.0) GO TO 260                                     JOY06680
      M = COST(TRY)                                                     JOY06690
      IF(NEWPR.LT.M) GO TO 260                                          JOY06700
      FM = IABS(FRM)                                                    JOY06710
      IF(LEVEL(FM).EQ.0) GO TO 240                                      JOY06720
      IF(LVJ.NE.0) GO TO 260                                            JOY06730
C     TO END IS CONNECTABLE                                             JOY06740
      I = FM                                                            JOY06750
      J = TO                                                            JOY06760
      K = M                                                             JOY06770
      L = TRY                                                           JOY06780
      GO TO 250                                                         JOY06790
  240 CONTINUE                                                          JOY06800
      IF(LVJ.EQ.0) GO TO 260                                            JOY06810
C     FROM END IS CONNECTABLE                                           JOY06820
      I = TO                                                            JOY06830
      J = FM                                                            JOY06840
      K = -M                                                            JOY06850
      L = -TRY                                                          JOY06860
  250 CONTINUE                                                          JOY06870
      NEWPR = M                                                         JOY06880
  260 CONTINUE                                                          JOY06890
      TRY = TRY+1                                                       JOY06900
      FRM = FROM(TRY)                                                   JOY06910
C     IS TO-GROUP EXHAUSTED                                             JOY06920
      IF( XOR(FRM,LST).GT.0) GO TO 235                                  JOY06930
C     PREPARE FOR NEXT TO-GROUP                                         JOY06940
      TO = TO+1                                                         JOY06950
      IF(TO.NE.MNODP1) GO TO 270                                        JOY06960
      TO = 1                                                            JOY06970
      TRY = ARTYP1                                                      JOY06980
      FRM = FROM(TRY)                                                   JOY06990
  270 CONTINUE                                                          JOY07000
      IF(NEWPR.NE.BIG) GO TO 280                                        JOY07010
      IF(TO.NE.TOO) GO TO 230                                           JOY07020
C     NOT ALL NODES CONNECTABLE - CHECK FOR ISOLATED POINTS             JOY07030
      DO 275 I275=1,MNODE                                               JOY07040
      IF(LEVEL(I275).NE.0) GO TO 275                                    JOY07050
C     TEST FOR ARCS RUNNING FROM IT                                     JOY07060
      IF(ARCID(I275).EQ.-1) GO TO 274                                   JOY07070
C     CHECK FOR DUMMY - NO ARCS RUNNING TO IT                           JOY07080
      J275 = CAT(I275)                                                  JOY07090
      IF(IABS(FROM(J275)).NE.I275) GO TO 274                            JOY07100
      WRITE(6,8021) I275                                                JOY07110
 8021 FORMAT(6H NODE ,I10,12H IS ISOLATED)                              JOY07120
C     ADD ARTIFICIAL                                                    JOY07130
  274 CONTINUE                                                          JOY07140
      ARCID(I275) = ARTY                                                JOY07150
      FLOW(I275) = 0                                                    JOY07160
      NEXT(I275) = NEXT(MNODP1)                                         JOY07170
      NEXT(MNODP1) = I275                                               JOY07180
      DOWN(I275) = MNODP1                                               JOY07190
      LEVEL(I275) = 1                                                   JOY07200
      DUAL(I275) = -BIG                                                 JOY07210
  275 CONTINUE                                                          JOY07220
      GO TO 285                                                         JOY07230
  280 CONTINUE                                                          JOY07240
      ARCID(J) = L                                                      JOY07250
      DOWN(J) = I                                                       JOY07260
      NEXT(J) = NEXT(I)                                                 JOY07270
      NEXT(I) = J                                                       JOY07280
      LEVEL(J) = LEVEL(I)+1                                             JOY07290
      DUAL(J) = DUAL(I)-K                                               JOY07300
      NEWARC = IABS(L)                                                  JOY07310
      CAPAC(NEWARC) = -CAPAC(NEWARC)                                    JOY07320
      MTREE = MTREE+1                                                   JOY07330
      GO TO 220                                                         JOY07340
  285 CONTINUE                                                          JOY07350
C     CLEAR UPPER BOUND FLAGS ON BASIC ARCS                             JOY07360
      DO 290 I290=1,MNODE                                               JOY07370
      J290 = IABS(ARCID(I290))                                          JOY07380
      CAPAC(J290) = -CAPAC(J290)                                        JOY07390
  290 CONTINUE                                                          JOY07400
C     CLEAR OUT UPPER BOUND FLAGS ON DUMMY ARCS                         JOY07410
      DO 295 I295=1,MARC                                                JOY07420
      IF(CAPAC(I295)+BIG.EQ.0)                                          JOY07430
     *CAPAC(I295) = 0                                                   JOY07440
  295 CONTINUE                                                          JOY07450
C     SET UPPER BOUND FOR ARTIFICIAL AND EXCESS                         JOY07460
      CAPAC(ARTY) = BIG                                                 JOY07470
      CAPAC(MARC) = BIG                                                 JOY07480
C                                                                       JOY07680
C     INITIALIZE PRICING                                                JOY07690
C                                                                       JOY07740
      TO = 1                                                            JOY07750
      TRY = ARTYP1                                                      JOY07760
      FRM = FROM(TRY)                                                   JOY07770
      ITER = 0                                                          JOY07780
      OPTIM = .FALSE.                                                   JOY07790
      DMP = .TRUE.                                                      JOY07800
C                                                                       JOY07840
C     NEW ITERATION                                                     JOY07850
C                                                                       JOY07860
  300 CONTINUE                                                          JOY07880
      ITER = ITER+1                                                     JOY07900
  305 CONTINUE                                                          JOY08140
C                                                                       JOY08150
C     PRICING                                                           JOY08160
C                                                                       JOY08170
C     NOTE THAT WE ARE PRICING OUT BASIC ARCS                           JOY08180
      TOO = TO                                                          JOY08210
      NEWPR = 0                                                         JOY08220
  310 CONTINUE                                                          JOY08230
      PRICE0 = -DUAL(TO)                                                JOY08240
      LST = ISIGN(LNODP1,FRM)                                           JOY08250
  320 CONTINUE                                                          JOY08260
      FM = IABS(FRM)                                                    JOY08270
      PRICE = DUAL(FM)+PRICE0-COST(TRY)                                 JOY08280
      IF(CAPAC(TRY)) 325,330,326                                        JOY08290
  325 CONTINUE                                                          JOY08300
      PRICE = -PRICE                                                    JOY08310
  326 CONTINUE                                                          JOY08320
      IF(PRICE.LE.NEWPR) GO TO 330                                      JOY08330
      NEWARC = TRY                                                      JOY08340
      NEWPR = PRICE                                                     JOY08350
      NEWTO = TO                                                        JOY08360
  330 CONTINUE                                                          JOY08370
      TRY = TRY+1                                                       JOY08380
      FRM = FROM(TRY)                                                   JOY08390
      IF( XOR(FRM,LST).GT.0) GO TO 320                                  JOY08400
      TO = TO+1                                                         JOY08410
      IF(TO.NE.MNODP2) GO TO 350                                        JOY08420
      TO = 1                                                            JOY08430
      TRY = ARTYP1                                                      JOY08440
      FRM = FROM(TRY)                                                   JOY08450
  350 CONTINUE                                                          JOY08460
      IF(NEWPR.NE.0) GO TO 360                                          JOY08470
      IF(TO.NE.TOO) GO TO 310                                           JOY08480
C                                                                       JOY08490
C     OPTIMAL INDICATION                                                JOY08500
C                                                                       JOY08510
      IF(ITDOPT.NE.0) DMP = .TRUE.                                      JOY08570
      OPTIM = .TRUE.                                                    JOY08580
      GO TO 795                                                         JOY08590
  360 CONTINUE                                                          JOY08600
      NEWFRM = IABS(FROM(NEWARC))                                       JOY08610
C                                                                       JOY08620
C     RATIO TEST                                                        JOY08630
C                                                                       JOY08640
      THETA = IABS(CAPAC(NEWARC))                                       JOY08680
      JTHETA = 0                                                        JOY08690
C     SET FOR CYCLE SEARCH                                              JOY08700
      CH(2) = ISIGN(LARCP1,CAPAC(NEWARC))                               JOY08710
      CH(1) = -CH(2)                                                    JOY08720
      DW(1) = NEWFRM                                                    JOY08730
      DW(2) = NEWTO                                                     JOY08740
      LDIFF = LEVEL(NEWFRM)-LEVEL(NEWTO)                                JOY08750
      KTHETA = 1                                                        JOY08760
      IF(LDIFF) 380,450,390                                             JOY08770
  380 KTHETA = 2                                                        JOY08780
  390 CONTINUE                                                          JOY08790
      DWN = DW(KTHETA)                                                  JOY08800
      CHG = CH(KTHETA)                                                  JOY08810
      K440 = IABS(LDIFF)                                                JOY08820
      DO 440 I440=1,K440                                                JOY08830
      IF( XOR(CHG,ARCID(DWN)).GT.0) GO TO 410                           JOY08840
C     INCREASING FLOW                                                   JOY08850
      I = IABS(ARCID(DWN))                                              JOY08860
      POSS = CAPAC(I)-FLOW(DWN)                                         JOY08870
      JPOSS = -DWN                                                      JOY08880
      GO TO 420                                                         JOY08890
C     DECREASING FLOW                                                   JOY08900
  410 POSS = FLOW(DWN)                                                  JOY08910
      JPOSS = DWN                                                       JOY08920
C     FIND MIN                                                          JOY08930
  420 CONTINUE                                                          JOY08940
      IF(THETA.LE.POSS) GO TO 430                                       JOY08950
      THETA = POSS                                                      JOY08960
      JTHETA = JPOSS                                                    JOY08970
      IF(THETA.EQ.0) GO TO 530                                          JOY08980
  430 CONTINUE                                                          JOY08990
      DWN = DOWN(DWN)                                                   JOY09000
  440 CONTINUE                                                          JOY09010
      DW(KTHETA) = DWN                                                  JOY09020
C     AT COMMON LEVEL                                                   JOY09030
  450 CONTINUE                                                          JOY09040
C     SEARCH FOR CYCLE END                                              JOY09050
  460 CONTINUE                                                          JOY09060
      IF(DW(1).EQ.DW(2)) GO TO 520                                      JOY09070
      DO 510 L510=1,2                                                   JOY09080
      DWN = DW(L510)                                                    JOY09090
      IF( XOR(CH(L510),ARCID(DWN)).GT.0) GO TO 480                      JOY09100
C     INCREASING FLOW                                                   JOY09110
      I = IABS(ARCID(DWN))                                              JOY09120
      POSS = CAPAC(I)-FLOW(DWN)                                         JOY09130
      JPOSS = -DWN                                                      JOY09140
      GO TO 490                                                         JOY09150
C     DECREASING FLOW                                                   JOY09160
  480 POSS = FLOW(DWN)                                                  JOY09170
      JPOSS = DWN                                                       JOY09180
C     FIND MIN                                                          JOY09190
  490 CONTINUE                                                          JOY09200
      IF(THETA.LE.POSS) GO TO 500                                       JOY09210
      THETA = POSS                                                      JOY09220
      JTHETA = JPOSS                                                    JOY09230
      KTHETA = L510                                                     JOY09240
      IF(THETA.EQ.0) GO TO 530                                          JOY09250
  500 CONTINUE                                                          JOY09260
      DW(L510) = DOWN(DWN)                                              JOY09270
  510 CONTINUE                                                          JOY09280
      GO TO 460                                                         JOY09290
  520 DWE = DW(1)                                                       JOY09300
C     RATIO TEST COMPLETE                                               JOY09310
  530 CONTINUE                                                          JOY09320
      IF(THETA.EQ.0) GO TO 560                                          JOY09460
C     UPDATE FLOWS ON INTACT PORTION OF CYCLE                           JOY09490
      DW(1) = NEWFRM                                                    JOY09500
      DW(2) = NEWTO                                                     JOY09510
      IF(JTHETA.NE.0)                                                   JOY09520
     *DW(KTHETA) = IABS(JTHETA)                                         JOY09530
      DO 550 I550=1,2                                                   JOY09540
      DWN = DW(I550)                                                    JOY09550
      CHG = ISIGN(THETA,CH(I550))                                       JOY09560
  540 CONTINUE                                                          JOY09570
      IF(DWN.EQ.DWE) GO TO 550                                          JOY09580
      FLOW(DWN) = FLOW(DWN)-CHG*ISIGN(1,ARCID(DWN))                     JOY09590
      DWN = DOWN(DWN)                                                   JOY09600
      GO TO 540                                                         JOY09610
  550 CONTINUE                                                          JOY09620
  560 CONTINUE                                                          JOY09630
      IF(JTHETA.NE.0) GO TO 570                                         JOY09640
C     CHANGE OF BOUNDS ONLY                                             JOY09650
      CAPAC(NEWARC) = -CAPAC(NEWARC)                                    JOY09660
      GO TO 300                                                         JOY09690
  570 CONTINUE                                                          JOY09700
      ITHETA = IABS(JTHETA)                                             JOY09710
      IF(JTHETA.GT.0) GO TO 590                                         JOY09720
      J = IABS(ARCID(ITHETA))                                           JOY09730
C     SET OLD ARC TO UPPER BOUND                                        JOY09740
      CAPAC(J) = -CAPAC(J)                                              JOY09750
  590 CONTINUE                                                          JOY09780
C     FLOW ON NEWARC                                                    JOY09790
      FLW = THETA                                                       JOY09800
      IF(CAPAC(NEWARC).GT.0) GO TO 600                                  JOY09810
      CAPAC(NEWARC) = -CAPAC(NEWARC)                                    JOY09820
      FLW = CAPAC(NEWARC)-FLW                                           JOY09830
      NEWPR = -NEWPR                                                    JOY09840
  600 CONTINUE                                                          JOY09850
      IF(KTHETA.EQ.2) GO TO 610                                         JOY09860
      Q1 = NEWFRM                                                       JOY09870
      Q2 = NEWTO                                                        JOY09880
      AID = -NEWARC                                                     JOY09890
      NEWPR = -NEWPR                                                    JOY09900
      GO TO 620                                                         JOY09910
  610 CONTINUE                                                          JOY09920
      Q1 = NEWTO                                                        JOY09930
      Q2 = NEWFRM                                                       JOY09940
      AID = NEWARC                                                      JOY09950
  620 CONTINUE                                                          JOY09960
C                                                                       JOY09970
C     UPDATE TREE                                                       JOY09980
C                                                                       JOY09990
      I = Q1                                                            JOY10000
      J = DOWN(I)                                                       JOY10010
      LSTAR = LEVEL(Q2)+1                                               JOY10020
      IF(THETA.EQ.0) GO TO 730                                          JOY10030
C     FLOWS NEED TO BE UPDATED                                          JOY10040
      CHG = ISIGN(THETA,CH(KTHETA))                                     JOY10050
  680 CONTINUE                                                          JOY10060
C     UPDATE DUAL VARIABLE ON STEM                                      JOY10070
      DUAL(I) = DUAL(I)+NEWPR                                           JOY10080
C     UPDATE FLOW ON STEM                                               JOY10090
      N = FLOW(I)                                                       JOY10100
      FLOW(I) = FLW                                                     JOY10110
C     UPDATE ARC ID ON STEM                                             JOY10120
      DIR = ISIGN(1,ARCID(I))                                           JOY10130
      REF = IABS(ARCID(I))                                              JOY10140
      ARCID(I) = AID                                                    JOY10150
C     PREPARE FOR LEVEL UPDATES                                         JOY10160
      LSAVE = LEVEL(I)                                                  JOY10170
      LDIFF = LSTAR-LSAVE                                               JOY10180
      LEVEL(I) = LSTAR                                                  JOY10190
      THD = I                                                           JOY10200
  690 CONTINUE                                                          JOY10210
      NXT = NEXT(THD)                                                   JOY10220
      IF(LEVEL(NXT).LE.LSAVE) GO TO 700                                 JOY10230
C     UPDATE LEVEL                                                      JOY10240
      LEVEL(NXT) = LEVEL(NXT)+LDIFF                                     JOY10250
C     UPDATE DUAL VARIABLE                                              JOY10260
      DUAL(NXT) = DUAL(NXT)+NEWPR                                       JOY10270
      THD = NXT                                                         JOY10280
      GO TO 690                                                         JOY10290
  700 CONTINUE                                                          JOY10300
      K = J                                                             JOY10310
  710 CONTINUE                                                          JOY10320
      L = NEXT(K)                                                       JOY10330
      IF(L.EQ.I) GO TO 720                                              JOY10340
      K = L                                                             JOY10350
      GO TO 710                                                         JOY10360
  720 CONTINUE                                                          JOY10370
C     TEST FOR LEAVING ARC                                              JOY10380
      IF(I.EQ.ITHETA) GO TO 790                                         JOY10390
C     PREPARE FOR NEXT UPDATE ON STEM                                   JOY10400
      FLW = N-CHG*DIR                                                   JOY10410
      AID = -ISIGN(REF,DIR)                                             JOY10420
C     MOVE DOWN STEM                                                    JOY10430
      NEXT(K) = NXT                                                     JOY10440
      NEXT(THD) = J                                                     JOY10450
      K = I                                                             JOY10460
      I = J                                                             JOY10470
      J = DOWN(J)                                                       JOY10480
      DOWN(I) = K                                                       JOY10490
      LSTAR = LSTAR+1                                                   JOY10500
      GO TO 680                                                         JOY10510
  730 CONTINUE                                                          JOY10520
C     ONLY FLOW ON NEW ARC CHANGES                                      JOY10530
  740 CONTINUE                                                          JOY10540
C     UPDATE DUAL VARIABLE ON STEM                                      JOY10550
      DUAL(I) = DUAL(I)+NEWPR                                           JOY10560
C     UPDATE FLOW ON STEM                                               JOY10570
      N = FLOW(I)                                                       JOY10580
      FLOW(I) = FLW                                                     JOY10590
C     UPDATE ARC ID ON STEM                                             JOY10600
      DIR = ISIGN(1,ARCID(I))                                           JOY10610
      REF = IABS(ARCID(I))                                              JOY10620
      ARCID(I) = AID                                                    JOY10630
C     PREPARE FOR LEVEL UPDATES                                         JOY10640
      LSAVE = LEVEL(I)                                                  JOY10650
      LDIFF = LSTAR-LSAVE                                               JOY10660
      LEVEL(I) = LSTAR                                                  JOY10670
      THD = I                                                           JOY10680
  750 CONTINUE                                                          JOY10690
      NXT = NEXT(THD)                                                   JOY10700
      IF(LEVEL(NXT).LE.LSAVE) GO TO 760                                 JOY10710
C     UPDATE LEVEL                                                      JOY10720
      LEVEL(NXT) = LEVEL(NXT)+LDIFF                                     JOY10730
C     UPDATE DUAL VARIABLE                                              JOY10740
      DUAL(NXT) = DUAL(NXT)+NEWPR                                       JOY10750
      THD = NXT                                                         JOY10760
      GO TO 750                                                         JOY10770
  760 CONTINUE                                                          JOY10780
      K = J                                                             JOY10790
  770 CONTINUE                                                          JOY10800
      L = NEXT(K)                                                       JOY10810
      IF(L.EQ.I) GO TO 780                                              JOY10820
      K = L                                                             JOY10830
      GO TO 770                                                         JOY10840
  780 CONTINUE                                                          JOY10850
C     TEST FOR LEAVING ARC                                              JOY10860
      IF(I.EQ.ITHETA) GO TO 790                                         JOY10870
C     PREPARE FOR NEXT UPDATE ON STEM                                   JOY10880
      FLW = N                                                           JOY10890
      AID = -ISIGN(REF,DIR)                                             JOY10900
C     MOVE DOWN STEM                                                    JOY10910
      NEXT(K) = NXT                                                     JOY10920
      NEXT(THD) = J                                                     JOY10930
      K = I                                                             JOY10940
      I = J                                                             JOY10950
      J = DOWN(J)                                                       JOY10960
      DOWN(I) = K                                                       JOY10970
      LSTAR = LSTAR+1                                                   JOY10980
      GO TO 740                                                         JOY10990
  790 CONTINUE                                                          JOY11000
      NEXT(K) = NXT                                                     JOY11010
      NEXT(THD) = NEXT(Q2)                                              JOY11020
      NEXT(Q2) = Q1                                                     JOY11030
      DOWN(Q1) = Q2                                                     JOY11040
      GO TO 300                                                         JOY11050
C                                                                       JOY11060
C     SUMMARY                                                           JOY11070
C                                                                       JOY11080
  795 CONTINUE                                                          JOY11090
      INFEAS = .FALSE.                                                  JOY11140
      KOST = KOST0                                                      JOY11150
      DO 830 I830=1,MNODE                                               JOY11160
      I = IABS(ARCID(I830))                                             JOY11170
      IF((FLOW(I830).NE.0).AND.(COST(I).EQ.BIG)) INFEAS = .TRUE.        JOY11240
      KOST = KOST+COST(I)*FLOW(I830)                                    JOY11250
  830 CONTINUE                                                          JOY11260
      DO 840 I840=1,MSLK                                                JOY11270
      IF(CAPAC(I840).GE.0) GO TO 840                                    JOY11280
      J840 = -CAPAC(I840)                                               JOY11290
      KOST = KOST+COST(I840)*J840                                       JOY11300
  840 CONTINUE                                                          JOY11360
      IF(OPTIM) WRITE(6,8009) ITER                                      JOY11380
      IF(INFEAS) GO TO 850                                              JOY11390
      IF(OPTIM) WRITE(6,8005) KOST                                      JOY11400
 8005 FORMAT(1H+,21X,4HCOST,I16)                                        JOY11420
      GO TO 860                                                         JOY11430
  850 CONTINUE                                                          JOY11440
      IF(OPTIM) WRITE(6,8008)                                           JOY11450
 8008 FORMAT(1H+,21X,10HINFEASIBLE)                                     JOY11470
  860 CONTINUE                                                          JOY11480
      IF(.NOT.OPTIM) GO TO 305                                          JOY11530
CCCC  IF(INFEAS) GO TO 892                                              JOY11540
      WRITE(6,8009) ITER                                                JOY11560
 8009 FORMAT(10H ITERATION,I10)                                         JOY11570
      WRITE(6,8000)                                                     JOY11580
 8000 FORMAT(1H+,20X,19H OPTIMAL INDICATION)                            JOY11590
C                                                                       JOY11600
      PRES = .TRUE.                                                     JOY11780
      IF(.NOT.PRES) GO TO 885                                           JOY11790
C     MOVE BASIC FLOWS TO CAPACITY CELLS FOR BASIC ARCS                 JOY11800
      DO 884 I884=1,MNODE                                               JOY11810
      J884 = IABS(ARCID(I884))                                          JOY11820
      CAPAC(J884) = -FLOW(I884)                                         JOY11830
  884 CONTINUE                                                          JOY11840
  885 CONTINUE                                                          JOY11850
      WRITE(6,8023)                                                     JOY12230
 8023 FORMAT(10H0ARC FLOWS/                                             JOY12240
     *52H0ARC         FROM   TO           FLOW           COST   )       JOY12250
      TO = 1                                                            JOY12260
      TRY = ARTYP1                                                      JOY12270
      FRM = FROM(TRY)                                                   JOY12280
 8886 CONTINUE                                                          JOY12290
      LST = ISIGN(LNODP1,FRM)                                           JOY12300
 8888 CONTINUE                                                          JOY12310
      FLW = MAX0(0,-CAPAC(TRY))+FLOOR(TRY)                              JOY12320
      IF(FLW.EQ.0) GO TO 8889                                           JOY12330
      FM = IABS(FRM)                                                    JOY12340
      WRITE(6,8024) NAME(TRY),FM,TO,FLW,COST(TRY)                       JOY12350
 8024 FORMAT(1X,I10,1X,2I5,2I15)
*     IF(NAME(TRY).LT.20000) WRITE(7,8025) NAME(TRY)
 8025 FORMAT(I6)
 8889 CONTINUE                                                          JOY12370
      TRY = TRY+1                                                       JOY12380
      FRM = FROM(TRY)                                                   JOY12390
      IF( XOR(FRM,LST).GT.0) GO TO 8888                                 JOY12400
      TO = TO+1                                                         JOY12410
      IF(TO.NE.MNODP1) GO TO 8886                                       JOY12420
      CLOSE(5)
      CLOSE(6)
      STOP 12345                                                        JOY12430
  892 CONTINUE                                                          JOY12450
C                                                                       JOY12460
C     END OF PROBLEM                                                    JOY12470
C                                                                       JOY12480
      GO TO 5                                                           JOY12870
C                                                                       JOY12880
C     EXIT                                                              JOY12890
C                                                                       JOY12900
  999 CONTINUE                                                          JOY12910
      WRITE(6,1999) NSTOP,KARD                                          JOY12920
 1999 FORMAT(5H STOP,I5/1X,I10,16H DATA CARDS READ)                     JOY12930
      STOP 77777
      END
