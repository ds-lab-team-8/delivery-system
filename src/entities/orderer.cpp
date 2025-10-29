#include "orderer.h"

// Constructor
Orderer::Orderer(int id_in, const string& name_in, const Location& location_in) 
    : id(id_in), name(name_in), location(location_in) {
    
}

// Destructor
Orderer::~Orderer() {

}

//Getters
int Orderer::getId() const{
    return id;
}

string Orderer::getName() const {
    return name;
}

Location Orderer::getLocation() const {
    return location;
}

void Orderer::setLocationNode(int node) {
    location.node = node;
}