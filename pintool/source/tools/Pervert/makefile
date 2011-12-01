##
## PIN tools
##

##############################################################
#
# Here are some things you might want to configure
#
##############################################################

TARGET_COMPILER?=gnu
ifdef OS
    ifeq (${OS},Windows_NT)
        TARGET_COMPILER=ms
    endif
endif

##############################################################
#
# include *.config files
#
##############################################################

ifeq ($(TARGET_COMPILER),gnu)
    include ../makefile.gnu.config
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT)
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif

##############################################################
#
# build rules
#
##############################################################

OBJS = $(OBJDIR)CallStack.o $(OBJDIR)pervert.o
TOOL_ROOTS = pervert

all: tools

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

tools: $(OBJDIR) $(TOOLS)
test: $(OBJDIR) $(TOOL_ROOTS:%=%.test)

## build rules

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : $(OBJS)
	${PIN_LD} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(OBJDIR)pervert.o

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.log *.tested *.failed
