# Note: If any of the below commands fail, check if the VMs are running.

# Steps to run the code:
# 1. make
# Running this command will copy the source files
# into the VMs and compile the inside the VMs using expect.
# 2. ./connect.sh <machine>
# Replace the machine with the machine to ssh into. This will work
# only if the machine is running.
# 3. Run the compiled code in the machine.

SRC=$(wildcard *.cpp)
BINS=$(SRC:.cpp=)
host=localhost
h1_port:=14501
h2_port:=14502
h3_port:=14503
machines=h1 h2 h3
h1=compute.cpp common.h Makefile
h2=compute.cpp inetd.cpp add.cpp sub.cpp mul.cpp idiv.cpp inetd_add.cpp inetd_sub.cpp inetd_mul.cpp inetd_idiv.cpp common.h Makefile
h3=compute.cpp square.cpp common.h Makefile
submission=compute.cpp common.h Makefile inetd.cpp add.cpp sub.cpp mul.cpp square.cpp \
			idiv.cpp inetd_add.cpp inetd_sub.cpp inetd_mul.cpp inetd_idiv.cpp \
			111801042-lab8.pdf connect.sh


# Copy the files to the machines and compile them in the machines.
.PHONY: all
all: copy compile

# Compile all cpp files in the current directory
# and create executables in the current directory	
.PHONY: local
local: $(BINS) 

%: %.cpp
	g++ -o $@ $<

.PHONY: compress
compress:
	zip -r 111801042.zip ${submission}

# Compile all the files in the VMs
.PHONY: compile
compile: $(addprefix compile_, ${machines})


compile_%:
	@/usr/bin/expect -c \
	'spawn ssh -p $($*_port) -o StrictHostKeyChecking=no tc@${host}; \
	set timeout -1; \
	expect "password:"; \
	send "user@123\n"; \
	expect "~"; \
	send "make clean_local\n"; \
	expect "~"; \
	send "make local\n"; \
	expect "~"; \
	send "exit\n"; \
	interact'; \

# Copy all the required files to each of the three machines.
.PHONY: copy
copy: $(addprefix copy_, ${machines})

# Copy the required files to the machine
copy_%:
	@/usr/bin/expect -c \
	'spawn scp -P $($*_port) ${$*} tc@${host}:~/; \
	expect "password:"; \
	send "user@123\n"; \
	interact';

# Remove all the compiled binaries from VMs.
.PHONY: clean
clean: $(addprefix clean_, ${machines})
	
.PHONY: clean_local
clean_local:
	rm -f $(BINS)

clean_%: 
	@/usr/bin/expect -c \
	'spawn ssh -p $($*_port) -o StrictHostKeyChecking=no tc@${host}; \
	set timeout -1; \
	expect "password:"; \
	send "user@123\n"; \
	expect "~"; \
	send "make clean_local\n"; \
	expect "~"; \
	send "exit\n"; \
	interact'; \