CXX     := g++
CXXFLAGS:= -std=c++17 -Wall -O2


SRCS    := $(shell find . -name '*.cpp')
BUILDDIR:= build
OBJS    := $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRCS))
TARGET  := $(BUILDDIR)/compiler


DEPS    := token.h lexer.h


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^


$(BUILDDIR)/%.o: %.cpp $(DEPS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
