all : read manager write
read: read.cpp
	g++ -o read read.cpp
manager: manager.cpp
	g++ -o manager manager.cpp
write: write.cpp
	 g++ -o write write.cpp