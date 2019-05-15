VERSION=0.5.0
SRC_ROOT     	= ../../
include $(SRC_ROOT)/Makefile.base
#point to project root path ,there is ng-engine
include $(SRC_ROOT)/src/Makefile.inc

TARGET 		= rsatest

#must inlcude prefix $(SRC_ROOT)
BUILD_DIR 	= $(SRC_ROOT)/build/$(TARGET)

#include all cpp files,must inlcude prefix $(SRC_ROOT)
SOURCES_DIR	+= $(SRC_ROOT)/src/rsatest 

#please include the root of include path,there is .
#INCLUDES    += 	. \
			$(SRC_ROOT)/src/output/include/

INCLUDES    += . \
			$(SRC_ROOT)/src/

#LIB_PATHS	+= $(SRC_ROOT)/src/output/lib

LIB_PATHS += 

#STATIC_LINKINGS  += -lbigercli -ltcmalloc_minimal -lcurl -lssl -lcrypto
STATIC_LINKINGS  += -ltcmalloc_minimal -lssl -lcrypto -lcurl
#STATIC_LINKINGS  += -ltcmalloc_minimal -lcurl
DYNAMIC_LINKINGS += -lpthread -lz -ldl -licuuc 
#DYNAMIC_LINKINGS += -lpthread -lz -ldl -lssl -lcrypto


##----------------------------------------------------------
.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	$(FIND) $(BUILD_DIR) -name "*.o" -o -name "*.d" -o -name "*~"|$(XARGS) $(RM) -f
	$(RM) -f $(TARGET) 

.PHONY: install
install: $(TARGET)
	if [ -f $(TARGET) ]; then $(CP) -f $(TARGET) ../bin; fi

##----------------------------------------------------------
SOURCES = $(foreach d,$(SOURCES_DIR),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
ABSPATH_OBJS = $(addsuffix .o,$(basename $(SOURCES)))
OBJS = $(patsubst $(SRC_ROOT)/%.o,$(BUILD_DIR)/%.o,$(ABSPATH_OBJS))

DEPS = $(OBJS:.o=.d)

OBJDIRS  = $(sort $(dir $(OBJS) $(DEBUG_OBJS)))
__dummy := $(shell mkdir -p $(OBJDIRS))

LIBPATHS  += $(addprefix -L, $(LIB_PATHS))
INCPATHS  += $(addprefix -I, $(INCLUDES))
INCLUDES  += $(SYSINCLUDES)
LIB_PATHS += $(SYSLIB_PATHS)

-include $(DEPS)
-include $(DEBUG_DEPS)

$(BUILD_DIR)/%.o:$(SRC_ROOT)/%.cpp
	$(A)$(ECHO) "Compiling [cpp] file:[$@] ..."
	$(CXX) $(CXXFLAGS) $(INCPATHS) -MD -MF $(BUILD_DIR)/$*.d -c $< -o $@

$(BUILD_DIR)/%.o:$(SRC_ROOT)/%.cc
	$(A)$(ECHO) "Compiling [cc] file:[$@] ..."
	$(A)$(CXX) $(CXXFLAGS) $(INCPATHS) -MD -MF $(BUILD_DIR)/$*.d -c $< -o $@

$(BUILD_DIR)/%.o:$(SRC_ROOT)/%.c
	$(A)$(ECHO) "Compiling [c] file:[$@] ..."
	$(A)$(CXX) $(CXXFLAGS) $(INCPATHS) -MD -MF $(BUILD_DIR)/$*.d -c $< -o $@

%.pb.cc %.pb.h:%.proto
	@echo "Generating $@"
	@$(PROTOC) --cpp_out=./proto --proto_path=./proto --proto_path=$(PROTOBUF_HDR) $<

##----------------------------------------------------------
$(TARGET): $(OBJS)
	$(ECHO) "Linking   [bin] file:[$@] ..."
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBPATHS) -o $@ -Xlinker "-(" -Wl,-Bstatic $(STATIC_LINKINGS) -Wl,-Bdynamic -Xlinker "-)" $(DYNAMIC_LINKINGS)
	#$(STRIP) --strip-all $@

