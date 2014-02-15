
BUILDDIR := out
SRCDIR   := src 

CFLAGS := $(shell pkg-config --cflags opencv) -Wno-write-strings
LIBS   := $(shell pkg-config --libs opencv)

TARGET  := $(BUILDDIR)/SetSolver
SRCEXT  := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT) )
OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))

$(TARGET): $(OBJECTS)
	echo $(OBJECTS)
	@echo " Linking..."; $(CXX) $^ -o $(TARGET) $(LIBS)
		 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CXX) $<"; $(CXX) $(CFLAGS) c -o $@ $<

clean:
	rm -rf $(OBJECTS) $(TARGET)
