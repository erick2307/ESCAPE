// MIT License
// Copyright (c) 2025 
// Luis Angel Moya Huallpa, Julio Cesar Ramirez Paredes, Erick Mas, Shunichi Koshimura
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "node.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// extras
#include "link.h"
#include "pedestrian.h"
#include "stateMatrix.h"
#include "vector2D.h"
#include <cstddef>
#include <vector>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
node::node(const int idNode, const vector2D coordinate) :
    idNode(idNode), coordinate(coordinate){
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// destructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// node::~node(){}

std::string node::getNodeType() {
    return "node";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void node::setLinkConnectionsPtr(std::vector<link *> linkConnectionsPtr) {
    (*this).linkConnectionsPtr =  linkConnectionsPtr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int node::getIdNode() const{
    return idNode;
}
const vector2D node::getCoordinate() const{
    return coordinate;
}
const std::vector<link*> node::getLinkConnectionsPtr() const {
    return linkConnectionsPtr;  
}
std::vector<stateMatrix*>* node::getExperiencedStateMatrixsPtr() {
    return &experiencedStateMatrixsPtr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const node* node::findEndNode(link *streetPtr) const {
    /* search for the final node according to the street found
        each street has a start and an end node depending on
        whether it is outbound or inbound it will return the final node, this would be outbound */
    // If the node where I am is node 1 of the street then the final node is node 2
    if(streetPtr->getNode1Ptr() == this){
        return streetPtr->getNode2Ptr();
    }
    // If the node where I am is node 2 of the street then the final node is node 1
    else {
        return streetPtr->getNode1Ptr();    
    }
}
// void node::findStateMatrix(stateMatrix searchedStateMatrix, bool& checkStateMatrix, int& iStateMatrixTable) {
//     /* iterates over the stateMatrix table searching for the element searchedStateMatrix  */
//     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     // searchedStateMatrix   |-->| ELEMENT stateMatrix to search for 
//     // checkStateMatrix  |-->| IF THE ELEMENT IS FOUND, IT IS TRUE 
//     for (int i = 0; i < stateMatrixTable.size(); i++) {
//         if (stateMatrixTable.at(i) == searchedStateMatrix) {
//             checkStateMatrix = true;
//             iStateMatrixTable = i;
//             return;
//         }
//     } 
//     // Set checkQ based on foundQ
//     checkStateMatrix = false;
// }
// void node::addqQTable(stateActionQ qElement) {
//     qTable.push_back(qElement); 
// }
// void node::sortQTable() {
//     std::sort(qTable.begin(), qTable.end(), stateActionQ::compareId);
// }
// void node::findQ(stateActionQ searchedQ, bool* checkQ) {
//     std::cout << "searching.." << std::endl;
//     checkQ = false;
//     for (int i = 0; i < getQTable().size(); i++) {
//         if (getQTable().at(i) == searchedQ) {
//             std::cout << "hello";
//             // foundQ = &getQTable().at(i);
//             *checkQ = true;
//             std::cout << *checkQ << std::endl;
//             break;
//         }
//     } 
//     *checkQ = false;
// }
// void node::createStateMatrix() {
//     stateMatrix stateMatrixElement;
//     for (int i = 0; i < qTable.size(); i++) {
//         qTable[i].getQ();
//     }
// }
pedestrianStatus node::pedestrianStateAtNode() const {
    return evacuating;    
}
bool node::checkNodeEvacuation() const {
    return false;    
}
std::vector<int> node::observedState() const {
    // state vector
    std::vector<int> observedState;
    // Reserve space in the vector
    observedState.reserve(linkConnectionsPtr.size());
    // assignment of states
    for (link* street : linkConnectionsPtr) {
        observedState.push_back(street->getDensityLevel());
    }
    return observedState;
}
double node::calculateDistanceTo(const node* node2) const {
    /* calculates the distance between two nodes*/
    return coordinate.distanceTo(node2->getCoordinate());
}
double node::calculateDistanceTo(const vector2D &position) const {
    /* calculates the distance between two nodes*/
    return coordinate.distanceTo(position);
}
void node::addLink(link *street) {
    /* adds a street to the linkConnections vector*/
    linkConnectionsPtr.push_back(street);
}
void node::addExperiencedStateMatrixPtr(stateMatrix *experiencedStateMatrix){
    /* adds a experiencedStateMatrix to the experiencedStateMatrixs vector*/
    experiencedStateMatrixsPtr.push_back(experiencedStateMatrix);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// show
void node::showNode() const {
    /* Shows node data in the terminal:
    // IdNode
    // x y
    // idLinkConnections*/
    std::cout << "Node: " << idNode << std::endl;
    std::cout << "x: " << coordinate.getX() << " ";
    std::cout << "y: " << coordinate.getY() << std::endl;
    std::cout << "linkConnections: ";
    for (int i = 0; i < linkConnectionsPtr.size(); i++) {
        std::cout << linkConnectionsPtr.at(i)->getIdLink() << " "; 
        std::cout << "p: "<< linkConnectionsPtr.at(i)->calculatePedestriansLink() << " "; 
        std::cout << "d: " <<linkConnectionsPtr.at(i)->getDensityLevel() << "|"; 
    }
    std::cout << std::endl;
}
void node::showStateMatrixTable() const {
    /* shows the table of experienced stateMatrix*/
    std::cout << "Qtable: " << std::endl;
    for (const stateMatrix* const entry : experiencedStateMatrixsPtr) {
        entry->showStateMatrix();
    } 
}
void node::printAction(std::fstream& file) const {
    /* printing of one line of actionDb*/ 
    // id, streetCount, streetId....
    file << idNode << ",";
    file << linkConnectionsPtr.size() << ",";
    // prints the street id
    for (auto it = linkConnectionsPtr.begin(); it != linkConnectionsPtr.end(); ++it) {
        file << (*it)->getIdLink();
        file << ",";
    }
    // iterates over the elements to print in a row
    // by default it is 10
    size_t missing = io::ioElementSize - linkConnectionsPtr.size();
    for (size_t i = 0; i < missing; i++) {
        // only the last one without a comma
        if (i == missing - 1) {
            file << "0";
        }
        else {
            file << "0,";
        }
    }
        
    file << std::endl;
}
void node::printTransition(std::fstream& file) const {
    /* printing of one line of transition*/ 
    // id, streetCount, idNode...
    file << idNode << ",";
    file << linkConnectionsPtr.size() << ",";
    // prints the street id
    for (auto it = linkConnectionsPtr.begin(); it != linkConnectionsPtr.end(); ++it) {
        // gives the other node of the street, different from idNode
        if ((*it)->getNode1Ptr() == this) {
            file << (*it)->getNode2Ptr()->idNode;
        }
        else {
            file << (*it)->getNode1Ptr()->idNode;
        }
        if (std::next(it) != linkConnectionsPtr.end()) {
            file << ",";
        }
    }
    // iterates over the elements to print in a row
    // by default it is 10
    size_t missing = io::ioElementSize - linkConnectionsPtr.size();
    for (size_t i = 0; i < missing; i++) {
        // only the last one without a comma
        if (i == missing - 1) {
            file << "0";
        }
        else {
            file << "0,";
        }
    }
 
    file << std::endl;
}


