
LIB = $(shell pwd)
INCLUDE = $(shell pwd)/include

ROOT_LIBS = $(shell root-config --glibs)
ROOT_GCC_FLAGS = $(shell root-config --cflags)
#ROOT_LIBSEXTRA =  -lTreePlayer -lMathMore -lSpectrum -lMinuit -lPyROOT

GRSI_LIBS = -L$(GRSISYS)/lib -lTGRSIDetector -lTDetector -lTGRSIint -lTGRSIFormat -lTGRSIint -lTGRSIFormat -lTLoops -lTKinematics -lTSRIM -lTBetaDecay -lTCal -lTReaction -lTAngularCorrelation -lTFipps -lTTigress -lTGriffin -lTBgo -lTSharc -lTTriFoil -lTSceptar -lTPaces -lTDescant -lTTip -lTCSM -lTS3 -lTGenericDetector -lTSiLi -lTRF -lTPulseAnalyzer -lTTAC -lTLaBr -lTZeroDegree -lGROOT -lTGRSIFit -lTNucleus -lTMidas -lTDataParser -lTHistogramming -lMakeAnalysisHistograms -lMakeGriffinFragmentHistograms -lMakeTigressHistograms -lXMLParser -lXMLIO -lX11 -lXpm -lProof -lGuiHtml -lMinuit -lPyROOT -lSpectrum -lMathMore $(shell grsi-config --root)
GRSI_GCC_FLAGS = $(shell grsi-config --cflags)

EXTERNAL_LIBS = -lJanalysistools

CC = g++
CFLAGS = -std=c++11 -g -fPIC $(ROOT_GCC_FLAGS) $(GRSI_GCC_FLAGS) -I$(INCLUDE) 
LIBRS = -L$(INCLUDE) $(EXTERNAL_LIBS) $(ROOT_LIBS) $(ROOT_LIBSEXTRA) $(GRSI_LIBS) -L$(LIB)/bin

SYSHEAD = $(wildcard include/spice_*.h)
OBJECTS = $(patsubst include/%.h,bin/build/%.o,$(SYSHEAD))
HEAD = $(patsubst %.h,$(shell pwd)/%.h,$(SYSHEAD))

PROGRAMS = $(patsubst programs/%.cpp,bin/%,$(wildcard programs/*.cpp))

TARG = bin/libspice_comp.so

DUMMY: $(PROGRAMS)

$(TARG): $(OBJECTS) bin/DictOutput.cxx $(GRSISYS)/bin/grsi-config $(JATOOLLIB)/bin/libJanalysistools.so
	$(CC) $(CFLAGS) -o $@ -shared bin/DictOutput.cxx $(OBJECTS) -I. $(LIBRS) # -Wl,--verbose
	bash bin/build/header.sh
	bash bin/build/make_export.sh

bin/DictOutput.cxx: $(HEAD)
	bash bin/build/link.sh $(HEAD)
	rootcint -f $@ -c -I$(INCLUDE) $(HEAD) bin/build/Linkdef.h
		
bin/build/%.o: src/%.cpp include/%.h
	$(CC) $(CFLAGS) -o $@ -c $< $(LIBRS)
	
bin/%: programs/%.cpp $(TARG) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $< bin/DictOutput.cxx -I. $(OBJECTS) $(LIBRS)
	chmod +x $@	

# 	$(CC) $(CFLAGS) -o $@ $< $(LIBRS) -lspice_comp #-Wl,--verbose
# 	chmod +x $@

clean:
	rm -f $(LIB)/bin/build/*.o
	rm -f $(LIB)/bin/build/Linkdef.h
	rm -f $(LIB)/bin/DictOutput*
	rm -f $(LIB)/bin/*.sh
	rm -f $(PROGRAMS)
	rm -f $(TARG)
