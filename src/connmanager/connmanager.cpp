#include "connmanager/conn_interface.h"
#include <iostream>

std::string ConnectionManager::getName() const{
    return "ConnectionManager";
}

std::string ConnectionManager::getDescription() const{
    return "ConnectionManager works for managing connections";
}

void ConnectionManager::execute(){
    std::cout << "ConnectionManager is running" << std::endl;
}

void ConnectionManager::registerObserver(Subject *subject){
    std::cout << "ConnectionManager is registering" << std::endl;
}