include /usr/local/etc/PcapPlusPlus.mk

all:
	g++ $(PCAPPP_INCLUDES) -c -o net.o net.cpp
	g++ $(PCAPPP_LIBS_DIR) -static-libstdc++ -o net net.o $(PCAPPP_LIBS)
clean: 
	rm net.o
	rm net

