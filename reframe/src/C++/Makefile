RM := rm -fr
CXX=CC
OBJS := GSimulation.o main.o
EXECUTABLE := exe

ifeq ($(NOCPU),1)
unexport CRAY_CPU_TARGET
endif

ifeq ($(VER),)
$(info # VER=$(VER) ---> setting it to default (VER=0))
VER += 0
endif

all: $(EXECUTABLE)

ifeq ($(PE_ENV),INTEL)
COMPILER_VERSION = $(INTEL_VERSION)
#SIMDFLAGS = -xCORE-AVX2
ifeq ($(CXXFLAGS),)
CXXFLAGS = -std=c++14 -g -O2 
endif
CXXFLAGS += $(SIMDFLAGS)

ifeq ($(REPORT), yes)
CXXFLAGS+=-qopt-report=5
ifeq ($(FILTER), yes)
CXXFLAGS += -qopt-report-phase=vec -qopt-report-filter="GSimulation.cpp,125-175"
endif
endif

ifeq ($(PE_ENV),GNU)
	COMPILER_VERSION = $(GNU_VERSION)
	CXXFLAGS = -std=c++14 -O2 -march=core-avx2
endif

ifeq ($(PE_ENV),PGI)
	COMPILER_VERSION = $(PGI_VERSION)
	CXXFLAGS = -std=c++11 -O2 -tp=haswell
	# -std=c++14: undefined reference to operator delete
endif

ifeq ($(PE_ENV),CRAY)
	COMPILER_VERSION = $(CRAY_CC_VERSION)
	CXXFLAGS = -hstd=c++14 -O2 -hcpu=haswell
endif

endif

version:
	@echo "# compiling version ver$(VER) with $(PE_ENV)/$(COMPILER_VERSION)"
	@echo "# CRAY_CPU_TARGET=$(CRAY_CPU_TARGET)"
	@cp versions/$(VER)/GSimulation.cpp	GSimulation.cpp
	@cp versions/$(VER)/GSimulation.hpp	GSimulation.hpp
	@cp versions/$(VER)/Particle.hpp    Particle.hpp

$(OBJS): version
	$(PREP) $(CXX) $(CXXFLAGS) -c $(@:.o=.cpp)

$(EXECUTABLE): $(OBJS)
	$(PREP) $(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(@) 

clean:
	-$(RM) $(OBJS) *exe GSimulation.cpp GSimulation.hpp Particle.hpp

distclean:
	-$(RM) $(OBJS) $(EXECUTABLE)
