CC = gcc -g
DEBUG=-D_vince_
CFLAGS =  -I. -I/usr/include/mysql/ -I/sbilling/charging/src/libnew/
LZIBS = -L. -lm -lnsl -L/usr/lib64/mysql/ -lmysqlclient
COMPILE = ${CC} ${DEBUG} ${CFLAGS} 
MAKEEXE = ${CC}


BILLINGOBJ = sync.o syncutil.o syncdb.o syncdeal.o public.o dealFunc.o dbprepare.o

all:	csvToMydb
csvToMydb:	${BILLINGOBJ}
	 	${MAKEEXE} -o $@ $^ ${CFLAGS} ${LZIBS} 

.c.o:
	${COMPILE} -c $<

clean:
		rm -f *.o core csvToMydb

move:
		cp csvToMydb ../bin/.
