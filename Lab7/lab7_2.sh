g++ -c lab7_2.cpp
g++ -o lab7_2 lab7_2.o -lrt -lpthread 
sudo setcap cap_sys_resource=eip lab7_2
./lab7_2
