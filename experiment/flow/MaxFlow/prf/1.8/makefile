# makefile for programs and generator

CCOMP = gcc
CFLAGS = -O4

all:  h_prf q_prf m_prf f_prf df ak

h_prf: h_prf_t.c h_prf.c parser_fl.c types_pr.h phase2.c
	$(CCOMP) $(CFLAGS) -o h_prf h_prf_t.c

m_prf: m_prf_t.c m_prf.c parser_fl.c types_pr.h phase2.c
	$(CCOMP) $(CFLAGS) -o m_prf m_prf_t.c

q_prf: q_prf_t.c q_prf.c parser_fl.c types_qpr.h phase2.c
	$(CCOMP) $(CFLAGS) -o q_prf q_prf_t.c

f_prf: f_prf_t.c f_prf.c parser_fl.c types_fpr.h phase2.c
	$(CCOMP) $(CFLAGS) -o f_prf f_prf_t.c

df:    df_t.c    df.c    parser_fl.c types_df.h phase2.c
	$(CCOMP) $(CFLAGS) -o df    df_t.c

ak:    ak.c
	$(CCOMP) $(CFLAGS) -o ak    ak.c

