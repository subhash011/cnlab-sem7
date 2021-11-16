SRC=$(wildcard *.cpp)
BINS=$(SRC:.cpp=)

.PHONY: all
all: $(BINS)

%: %.cpp
	g++ -o $@ $<

.PHONY: clean
clean:
	rm -f $(BINS) valid_fragment_files.txt