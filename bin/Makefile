CXX = /usr/bin/clang++

CXXFLAGS = -std=c++17 -I/usr/local/include\
		   -pthread -g

LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -lssl -lcrypto -lz

INCLUDE = -I../include/protos

PROTOSRC = ../src/protos/*.cc

test: test_ability_lifecycle.cpp 
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) $(PROTOSRC) -o $@ $(INCLUDE)

test_client: test_client.cpp 
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) $(PROTOSRC) -o $@ $(INCLUDE)
