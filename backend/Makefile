SRCDIR      := src
INCLUDEDIR  := include

CXX := g++
LINKER := g++

# All CXX files grouped by sub-namespace
SERVER_CXXFILES := $(wildcard $(SRCDIR)/server/*.cpp)
CXXFILES           := $(SRCDIR)/backend.cpp $(SERVER_CXXFILES)

# All Header files grouped by sub-namespace
SERVER_HEADERS     := $(wildcard $(INCLUDEDIR)/server/*.h)
HEADERS            := $(INCLUDEDIR)/backend.h $(SERVER_HEADERS)

CFLAGS      := -W -Wall -I -pthread -g
INCLUDE := -I$(INCLUDEDIR)/ -Ilib/
CXXFLAGS := -g -Wall -O3 -fmessage-length=0 $(INCLUDE)
LDFLAGS := 
LIBS := lib/mongoose.o

#Rules -----------------------------------------------------------------

CXXOBJS     := $(CXXFILES:.cpp=.o)

%.o : %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

server: $(CXXOBJS) $(HEADERS) $(LIBS)
	$(LINKER) -o server $(CXXOBJS) $(LDFLAGS) $(LIBS)

lint:
	$(foreach myfile,$(SERVER_HEADERS),python cpplint.py --filter=-whitespace $(myfile);)
	$(foreach myfile,$(SERVER_CXXFILES),python cpplint.py --filter=-whitespace $(myfile);)

clean:
	rm -f server $(CXXOBJS)

.DEFAULT_GOAL := server

#Libraries -----------------------------------------------------------------

lib/mongoose.o: 
	$(CC) $(CFLAGS) -flat_namespace -bundle -undefined suppress -pthreads -c lib/mongoose/mongoose.c -o lib/mongoose.o
