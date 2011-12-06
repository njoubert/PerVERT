SRCDIR      := src
INCLUDEDIR  := include

CXX := g++
LINKER := g++

# All CXX files grouped by sub-namespace
UTILS_CXXFILES := $(shell find $(SRCDIR)/utils -type f -name '*.cpp')
PERVERT_CXXFILES := $(shell find $(SRCDIR)/pervert -type f -name '*.cpp')
CXXFILES           := $(UTILS_CXXFILES) $(PERVERT_CXXFILES)

# All Header files grouped by sub-namespace
UTILS_HEADERS     := $(shell find $(INCLUDEDIR)/utils -type f -name '*.h')
PERVERT_HEADERS   := $(shell find $(INCLUDEDIR)/pervert -type f -name '*.h')
HEADERS            := $(UTILS_HEADERS) $(SERVER_HEADERS)

CFLAGS      := -W -Wall -I -pthread -g
INCLUDE := -I$(INCLUDEDIR)/ -Ilib/
CXXFLAGS := -g -Wall -O3 -fmessage-length=0 $(INCLUDE)
LDFLAGS := 
LIBS := lib/mongoose.o

ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	PLATFORM := OSX
	MONGOOSE_FLAGS := -flat_namespace -bundle -undefined suppress -pthreads -c lib/mongoose/mongoose.c -o lib/mongoose.o
else
	PLATFORM := _POSIX
	MONGOOSE_FLAGS := -shared -fPIC -fpic -c lib/mongoose/mongoose.c -o lib/mongoose.o
	LDFLAGS += -lpthread -ldl
endif

lib/mongoose.o: 
	$(CC) $(CFLAGS) $(MONGOOSE_FLAGS)

#Rules -----------------------------------------------------------------

CXXOBJS     := $(CXXFILES:.cpp=.o)

%.o : %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

server: $(CXXOBJS) $(HEADERS) $(LIBS)
	echo $(CXXFILES)
	echo $(HEADERS)
	$(LINKER) -o server $(CXXOBJS) $(LIBS) $(LDFLAGS) 

lint:
	$(foreach myfile,$(SERVER_HEADERS),python cpplint.py --filter=-whitespace $(myfile);)
	$(foreach myfile,$(SERVER_CXXFILES),python cpplint.py --filter=-whitespace $(myfile);)

clean:
	rm -f server $(CXXOBJS)

.DEFAULT_GOAL := server

#Libraries -----------------------------------------------------------------

