TARGET = ../lib/libusec.a

OBJS = usage_sec.o

ARFLAGS = rcs
CXXFLAGS = -O2 -Wall

.PHONY: all
all: $(TARGET)

$(TARGET): $(TARGET)($(OBJS))

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) *.a *.o $(TARGET)
