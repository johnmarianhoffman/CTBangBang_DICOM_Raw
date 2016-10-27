CC=g++

ODIR=obj

IDIR_RECON=../include/
INCLUDES=-I$(IDIR_RECON)

LIBS=-lm -ldcmdata -loflog -lofstd -lpthread -lz

CFLAGS=$(INCLUDES) -g -Wall -L/usr/local/lib -std=c++11

_OBJ=main.o dicom_v1_dcmtk.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ));

all: ../a.out

../a.out: $(OBJ)
	(cd obj && $(CC) -g -o ../../a.out *.o $(LIBS))

obj/main.o: main.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

obj/dicom_v1_dcmtk.o: dicom_v1_dcmtk.cpp
	python dcm_def_generator.py
	$(CC) $(CFLAGS) -c -o $@ $< 

.PHONY: all test clean

clean:
	rm obj/*.o; rm -rf ../*~

