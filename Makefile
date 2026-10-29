CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
CPPFLAGS =  -DGLOG_USE_GLOG_EXPORT
LDFLAGS = -lX11 -pthread
LDLIBS = -lglog

TARGET = hero

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
