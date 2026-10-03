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

#include "stateMatrixs.h"
#include "Q.h"
#include "io.h"
#include "stateMatrix.h"
#include <vector>
#include "links.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
stateMatrixs* stateMatrixs::stateMatrixsInstance = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
stateMatrixs::stateMatrixs() {
    // setINumeroSimulacion(1);
    // leerDbStateMatrixs();
    // Read the last simulation.
    // leerDbStateMatrixs(simulationFile + dictionary::controlDict["previousComputationFile"]); 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name of the folder where the simulations are.
const std::string stateMatrixs::simulationFile = "stateMatrices/";

// stateMatrixs::stateMatrixs(nodes* dbNode) {
//     // dbNode contains all the nodes of the simulation
//     (*this).dbNode = dbNode;
//     // Read the last simulation.
//     leerDbStateMatrixs(encontrarUltimoFile()); 
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<stateMatrix*> &stateMatrixs::getDbStateMatrixs() {
    return dbStateMatrixs;    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
stateMatrixs* stateMatrixs::get() {
    /* if it does not exist yet, create the unique instance of nodes*/
    if (!stateMatrixsInstance) {
        stateMatrixsInstance =  new stateMatrixs();
    }
    return stateMatrixsInstance;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string stateMatrixs::creacionFileStateMatrix() const {
    /* Create the name of the export file.*/
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // iFileInicio     |-->| ID OF THE START FILE, 1
    // preName         |-->| EXPORT EXTENSION
    // typeFile        |-->| EXPORT EXTENSION
    // filenameStream  |-->| FAKE FILE, ALLOWS CONTROLLING THE CHARACTERS
    // OF A VARIABLE TO THEN STORE IT IN A STRING
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // increase of the simulation number
    const std::string preName = "sim_";
    const std::string typeFile = ".csv";
    std::ostringstream filenameStream;
    // Create the initial file with the following format sim_000000001.csv
    filenameStream << std::setw(9) << std::setfill('0') << tiempo::get()->getINumberSimulation() ;
    // Final export name 
    return preName + filenameStream.str() + typeFile;
}
std::string stateMatrixs::encontrarUltimoFile() {
    // Find the last simulation to read it
    std::string ultimoFile;
    ultimoFile = "stateMatrices/sim_000000006.csv";
    return ultimoFile;
}
std::string stateMatrixs::crearFilenameSalida(int numeroSimulacion) {
    /* Create the name of the export file.*/
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // iFileInicio     |-->| ID OF THE START FILE, 1
    // preName         |-->| EXPORT EXTENSION
    // typeFile        |-->| EXPORT EXTENSION
    // filenameStream  |-->| FAKE FILE, ALLOWS CONTROLLING THE CHARACTERS
    // OF A VARIABLE TO THEN STORE IT IN A STRING
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // If it reads a state file
    // if (dictionary::controlDict["computationContinued"] == "yes" ) {
        
    // }
    // else {
        
    // }
    int iFileInicio = 1;
    std::string preName = "sim_";
    std::string typeFile = ".csv";
    std::ostringstream filenameStream;
    // Create the initial file with the following format sim_000000001.csv
    filenameStream << std::setw(9) << std::setfill('0') << iFileInicio ;
    // Final export name 
    return simulationFile +preName + filenameStream.str() + typeFile;
}
std::string stateMatrixs::fileNameSalida() {
    std::string lastFile_str = std::get<std::string>(dictionary::get()->lookup("previousComputationFile"));
    size_t posicion = lastFile_str.find_first_of("123456789");
    int iLastFile = std::stoi(lastFile_str.substr(posicion));
    std::string preName = "sim_";
    std::string typeFile = ".csv";
    std::ostringstream filenameStream;
    // Create the initial file with the following format sim_000000001.csv
    filenameStream << std::setw(9) << std::setfill('0') << iLastFile ;
    return simulationFile + preName + filenameStream.str() + typeFile;
}
// void stateMatrixs::agregarStateMatrix(stateMatrix stateMatrixElement) {
//     dbStateMatrixs.push_back(stateMatrixElement);
// }
void stateMatrixs::leerActionsDb(std::fstream& file) {
    /* Allows reading python files, with this actiondb file I can know
        the order of the streets at each node*/
    std::string line;
    char comma;
    int idNode, cantidadLinks;
    int idLink;
    actiondb.resize(nodes::get()->getDbNodeTotal().size()); 
    // if the file does not exist
    if (file.fail()) {
        std::cout << "Error opening the file " << std::endl;
        exit(1);
    }
    // reading of each line
    while (std::getline(file, line)) {
        // If the file has comments with #, do not read them.
        if (line[0] == '#') {
            continue;
        }
        // reading of line
        std::istringstream iss(line);
        if (!(iss >> idNode >> comma >> cantidadLinks >> comma)) {
            std::cerr << "Error reading ID or count." << std::endl;
        }
        // check that it is not an evacuation node
        // because it has no connections
        if (nodes::get()->getDbNodeTotal().at(idNode).get()->verificarNodoEvacuation() == false) {
            // create the size of the vector
            actiondb.at(idNode).resize(cantidadLinks);
            for (int i = 0; i < cantidadLinks; ++i) {
                if (!(iss >> idLink)) {
                    std::cerr << "Error reading value for conectionCalles at position " << i << std::endl;
                    return; // Exit the function if there is an error
                }
                // add to action each street connection according to the node 
                actiondb.at(idNode).at(i) = links::get()->getDbLinkTotal().at(idLink).get();
                // std::cout << actiondb.at(idNode).at(i)->getIdLink() << " ";
                // Ignore the comma between values, if it is not the last value
                if (i < cantidadLinks - 1) {
                    iss >> comma;  // Read and discard the comma
                    if (iss.fail()) {
                        std::cerr << "Error reading the comma after the value at position " << i << std::endl;
                        return; // Exit the function if there is an error
                    }
                }
            }
        }
    } 
}
void stateMatrixs::leerDbStateMatrixs() {
    if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "trained") {
        dictionary::get()->getControlDict()["computationContinued"] = "yes";
    }
    // if the option to read previous stateMatrixs data is active
    if (std::get<bool>(dictionary::get()->lookupDefault("computationContinued")) == true) {
        // reading of stateMatrixs data from the python version,
        // it must be read according to the order of the street connections
        if (std::get<bool>(dictionary::get()->lookupDefault("pythonVersion")) == true
        and std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")) == "in") {
            leerActionsDb(io::fileActionsDb.getFileFstream());
        }
        /* Reading of data from a past simulation.*/
        std::fstream file;
        file.open(simulationFile + std::get<std::string>(dictionary::get()->lookup("previousComputationFile")), std::ios::in);
        // If the file does not exist
        if (file.fail()) {
            std::cout << "Error opening the file: "<< std::get<std::string>(dictionary::get()->lookup("previousComputationFile")) << std::endl;
            exit(1);
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // iLinkConnection          |-->| POSITION IN THE ARRAY linkConection
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Update the street or linkActual, which is the street the person will go along. 

        // Store each line of the file filname in the variable line.
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // id     |-->| ID OF THE NODE OR OF THE INTERSECTION
        // s      |-->| STATE OF ONE OF THE STREETS IN linksConnection OF A NODE
        // Q      |-->| Q OF STREET 1 OF THE linksConnection OF THE NODE
        // o      |-->| OTHER VARIABLES TO BE DEFINED
        // p0     |-->| WORD IN 0, READ BUT STORED 
        // stateMatrixLeido |-->| CLASS IS CREATED AND THEN DESTROYED WITHIN THIS
        // SCOPE
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // id could become unsigned short int
        std::string line;
        int idNode;
        int s;
        double Qa;
        int O;
        std::string p0;
        std::string idNode_str;
        std::string s_str;
        std::string Q_str;
        std::string O_str;
        std::vector<int> stateLeido;
        std::vector<Q> QsLeido;
        // Iterate over all the lines of the file.
        while (std::getline(file, line)) {
            // If the file has comments with #, do not read them.
            if (line[0] == '#') {
                continue;
            }
            // !-----------------------------------------------------------------------
            // Store each line in the variable line  
            std::istringstream iss(line);
            // Store the variable idNode
            std::getline(iss, idNode_str, ',');
            idNode = std::stoi(idNode_str);
            node* const nodeLeido = nodes::get()->getDbNodeTotal().at(idNode).get();
            // !-----------------------------------------------------------------------
            // Store the elements of state
            stateLeido.clear();
            for (int i = 0; i < io::tamanoElementosIO; ++i) {
                if (i < nodeLeido->getLinkConnectionsPtr().size()) {
                    std::getline(iss, s_str, ',');
                    s = std::stoi(s_str);
                    stateLeido.push_back(s);
                } 
                else {
                    std::getline(iss, p0, ',');
                }
            }
            // !-----------------------------------------------------------------------
            // Elements of Q
            // when reading stateMatrix files from python
            if (std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")) == "in" and
            std::get<bool>(dictionary::get()->lookupDefault("pythonVersion")) == true) {
                QsLeido.clear();
                QsLeido.resize(nodeLeido->getLinkConnectionsPtr().size());
                for (int i = 0; i < io::tamanoElementosIO; ++i) {
                    // check whether it is an evacuation node
                    if(nodeLeido->verificarNodoEvacuation()){
                        if (i == 0) {
                            std::getline(iss, Q_str, ',');
                            Qa = std::stod(Q_str);
                            QsLeido[i] = Q(Qa);
                        }
                        else {
                            std::getline(iss, p0, ',');
                        }
                    }
                    // when it is not an evacuation node
                    else {
                        if (i < nodeLeido->getLinkConnectionsPtr().size()) {
                            std::getline(iss, Q_str, ',');
                            Qa = std::stod(Q_str);
                            QsLeido[i] = Q(Qa, actiondb.at(idNode).at(i));
                        } 
                        else {
                            std::getline(iss, p0, ',');
                        }
                    }

                }
            }
            // when reading a stateMatrix file from the same program
            else {
                QsLeido.clear();
                // iterate over the size of elements, generally it is 10
                for (int i = 0; i < io::tamanoElementosIO; ++i) {
                    // check whether it is an evacuation node
                    if(nodeLeido->verificarNodoEvacuation()){
                        if (i == 0) {
                            std::getline(iss, Q_str, ',');
                            Qa = std::stod(Q_str);
                            QsLeido.push_back(Q(Qa));
                        }
                        else {
                            std::getline(iss, p0, ',');
                        }
                    }
                    // when it is not an evacuation node
                    else {
                        if (i < nodeLeido->getLinkConnectionsPtr().size()) {
                            std::getline(iss, Q_str, ',');
                            Qa = std::stod(Q_str);
                            QsLeido.push_back(Q(Qa, nodeLeido->getLinkConnectionsPtr().at(i)));
                        } 
                        else {
                            std::getline(iss, p0, ',');
                        }
                    }
                }
            }
            // !-----------------------------------------------------------------------
            // Observation elements
            for (int i = 0; i < io::tamanoElementosIO; ++i) {
                // check whether it is an evacuation node
                if(nodeLeido->verificarNodoEvacuation()){
                    if (i == 0) {
                        // always go through the observations
                        std::getline(iss, O_str, ',');
                        // if I want to read past observations
                        if (std::get<bool>(dictionary::get()->lookupDefault("observationStatePedestrian")) == true) {
                            O = std::stod(O_str);
                            QsLeido.at(i).setObservaciones(O);
                        }
                    } 
                    else {
                        std::getline(iss, p0, ',');
                    }
                }
                // if it is not an evacuation node
                else {
                    if (i < nodeLeido->getLinkConnectionsPtr().size()) {
                        // always go through the observations
                        std::getline(iss, O_str, ',');
                        // if I want to read past observations
                        if (std::get<bool>(dictionary::get()->lookupDefault("observationStatePedestrian")) == true) {
                            O = std::stod(O_str);
                            QsLeido.at(i).setObservaciones(O);
                        }
                    } 
                    else {
                        std::getline(iss, p0, ',');
                    }
                 
                }
            }
            // creation of the stateMatrix
            stateMatrix* nuevoStateMatrix = new stateMatrix(nodeLeido, stateLeido, QsLeido);
            dbStateMatrixs.emplace_back(nuevoStateMatrix);
            nodeLeido->addStateMatrixExperimentadosPtr(nuevoStateMatrix);
        }
        file.close(); 
        // // End the timing
        // auto stop = std::chrono::high_resolution_clock::now();
        // auto duration = stop - start;
        // auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        // auto durationMinutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        // std::cout << "Reading duration: " << durationMinutes.count() << " min";
        // std::cout << " / " << durationSeconds.count() << " s" << std::endl;
    }

}
void stateMatrixs::mostrarDbStateMatrixs() const {
    // Show all the stateMatrix inside dbStateMatrixs.
   for (int i = 0; i < dbStateMatrixs.size(); i++) {
        dbStateMatrixs[i]->mostrarStateMatrix();
    }
}
void stateMatrixs::imprimirDbStateMatrixs(fileIO* const file) const {
    // Create the name of the export file.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // file    |-->| OUTPUT FILE, THE NAME IS CREATED WITH crearFilenameSalida()
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // std::fstream file;
    // file.open(creacionArchivoSalida(), std::ios::out);
    // Iterate over all the nodes
    for (const auto& it : dbStateMatrixs) {
        // print the statesMatrix of the tables of each node 
        it->imprimirStateMatrix(file);
    }
}
