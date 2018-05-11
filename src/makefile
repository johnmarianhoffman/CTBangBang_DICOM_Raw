CC=g++

ODIR=obj

IDIR_RECON=../include/
INCLUDES=-I$(IDIR_RECON)

LIBS=-lm -ldcmdata -loflog -lofstd -lpthread  -lz -licui18n -licuuc 

#CFLAGS=$(INCLUDES) -g -Wall -L/usr/local/lib -std=c++11 
CFLAGS=$(INCLUDES) -g -Wall -L/usr/local/lib -std=c++11 

_OBJ=main.o dicom_v1_dcmtk.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ));

all: ../raw_test

../raw_test: $(OBJ)
	(cd obj && $(CC) -g -o ../../raw_test *.o $(LIBS))

obj/main.o: main.cpp dicom_raw_series.hpp dicom_raw_frame.hpp dicom_scanner.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

obj/dicom_v1_dcmtk.o: dicom_v1_dcmtk.cpp dicom_scanner.hpp 
	$(CC) $(CFLAGS) -c -o $@ $< 

.PHONY: all test clean

clean:
	rm obj/*.o; rm -rf ../*~

