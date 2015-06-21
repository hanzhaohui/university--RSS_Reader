cd spiderbot/
rm *.o 
g++ -c Queue.cpp
g++ -c Node.cpp
g++ -c MyDB.cpp
g++ -g spider.cpp Queue.o Node.o MyDB.o -o spidercpp.o -L/usr/lib/i386-linux-gnu -lmysqlclient
##################
./spidercpp.o

