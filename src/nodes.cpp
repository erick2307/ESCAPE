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

#include "nodes.h"
#include "nodeEvacuation.h"
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nodes* nodes::nodesInstance = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nodes::nodes() {
    (*this).fileName = std::get<std::string>(dictionary::get()->lookupDefault("nodesFile"));
    readNodes(fileName);
}
nodes::nodes(std::string fileName) {
   readNodes(fileName);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string nodes::getFileName() {
    return fileName;  
}
std::vector<std::shared_ptr<node>> nodes::getDbNodeTotal() {
    return dbNodeTotal;
}
std::vector<nodeEvacuation *> nodes::getDbNodeEvacuation() {
    return dbNodeEvacuation;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nodes* nodes::get() {
    /* if it does not exist yet, creates the single instance of nodes*/
    if (!nodesInstance) {
        nodesInstance =  new nodes();
    }
    return nodesInstance;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nodes::readNodes(std::string fileName) {
    // Stores all the information of a single line. 
    std::string line;
    std::string lineActions;
    std::fstream fileActions;
    int id, linkCount;
    std::string l_str;
    int l;
    char comma;
    std::vector<int> streetConnections;
    // Reading of nodes file
    std::fstream file;
    file.open(fileName, std::ios::in);

    if (file.fail()) {
        // std::cout << "Error opening the file nodes.csv" << std::endl;
        std::cout << "Error opening the file: " << getFileName() << std::endl;
        exit(1);
    }
    // Variables of one row of the nodes file, which would be a single node
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // idNode          |-->| IDNODE
    // x               |-->| X POSITION
    // y               |-->| Y POSITION
    // e               |-->| EVACUATION NODE, IF IT IS 1, IF IT IS 2 LIMITED
    // r               |-->| 
    // m               |-->| MAXIMUM NUMBER OF PEOPLE EVACUATED
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int n, x, y, e, r, m;
    std::string n_str, y_str, x_str, e_str, r_str, m_str;
    // Goes through all the lines of the file.
    while (std::getline(file, line)) {
        // If the file has comments with #, do not read them.
        if (line[0] == '#') {
            continue;
        }
        // Save each line in the variable line  
        std::istringstream iss(line);
        // Save each value in the variables.

        std::getline(iss, n_str, ',');
        n = std::stoi(n_str);
        std::getline(iss, x_str, ',');
        x = std::stoi(x_str);
        std::getline(iss, y_str, ',');
        y = std::stoi(y_str);
        std::getline(iss, e_str, ',');
        e = std::stoi(e_str);
        // if the stateMatrix comes from the python version
        // there is a different order of the linksConnecton
        if (e==0) {
            std::unique_ptr<node> newNode = std::make_unique<node>(n, vector2D(x, y));
            // node newNode1 = node(n, x, y);
            nodes::dbNodeTotal.push_back(std::move(newNode));
        }
        else if (e==1) {
            std::unique_ptr<nodeEvacuation> newEvacuationNode = std::make_unique<nodeEvacuation>(n, vector2D(x, y));
            // nodeEvacuation newEvacuationNode= nodeEvacuation(n, x, y);
            dbNodeTotal.push_back(std::move(newEvacuationNode));
            // create an array of evacuation nodes
            dbNodeEvacuation.push_back(dynamic_cast<nodeEvacuation*>(dbNodeTotal.back().get()));
        }
        // limited evacuation node
        else if (e==2) {
            // reading of the maximum number of people evacuated at that node
            std::getline(iss, m_str, ',');
            m = std::stoi(m_str);
            std::unique_ptr<nodeEvacuation> newEvacuationNode = std::make_unique<nodeEvacuation>(n, vector2D(x, y), m);
            dbNodeTotal.push_back(std::move(newEvacuationNode));
            // create an array of evacuation nodes
            dbNodeEvacuation.push_back(dynamic_cast<nodeEvacuation*>(dbNodeTotal.back().get()));
        }
        std::getline(iss, r_str, '\n');
        r = std::stoi(r_str);
        // Convert from str to int
        // Creation of each person in the data base.
    }
    file.close(); 
}


void nodes::resetNodesEvacuations() {
    for (int i = 0; i < dbNodeEvacuation.size(); i++) {
        dbNodeEvacuation.at(i)->reset();

    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// show
// void nodes::showNodes() const {
//     // Shows all the nodes and their data in the terminal.
//     for (int i = 0; i < dbNodeTotal.size(); i++) {
//         dbNodeTotal.at(i)->showNode();
//         dbNodeTotal.at(i)->showQTable();
//         // const node* baseNode = dbNode.at(i);
//         // const nodeEvacuation* evacuationNode = dynamic_cast<const nodeEvacuation*>(baseNode);
//         // if (evacuationNode) {
//         //     std::cout << "nodeEvacuation: " << evacuationNode->getIdNode() << ", X: " << evacuationNode->getCoordX() << ", Y: " << evacuationNode->getCoordY() << std::endl;
//         // }
//     }
// }
void nodes::showDbNodeTotal() const {
    for (int i = 0; i < dbNodeTotal.size(); i++) {
        dbNodeTotal.at(i)->showNode();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// print
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nodes::printActionsDb(std::fstream& file) const {
    /* print actionDb, compatibility file for the python version*/
    for (auto it = dbNodeTotal.begin(); it != dbNodeTotal.end(); ++it) {
        (*it)->printAction(file);
    }
    file.close();
}
void nodes::printTransitionsDb(std::fstream& file) const {
    /* print transitionDb, compatibility file for the python version*/
    for (auto it = dbNodeTotal.begin(); it != dbNodeTotal.end(); ++it) {
        (*it)->printTransition(file);
    }
    file.close();
}


// void nodes::printEvacuatedPedestrianCount(std::string folderName) {
//     std::fstream file3;
//     file3.open(folderName + "/evacuatedPedestrianCount",std::ios::out);
//     for (int i = 0; i < dbNode.size(); i++) {
//         // const node* baseNode = dbNode.at(i);

//         // if (evacuationNode) {
//         //     file3 << evacuationNode->getEvacuatedPeople();
//         // }
//     }

// }
