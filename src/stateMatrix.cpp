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

#include "stateMatrix.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// own headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Q.h"
#include "io.h"
#include "nodeEvacuation.h"
#include "pedestrian.h"
#include "stateMatrixs.h"
#include <vector>
#include "link.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// const int stateMatrix::tamanoVectorIO = 10;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
stateMatrix::stateMatrix()
    :
    nodoPtr(nullptr)
{
  
}
stateMatrix::stateMatrix(const nodeDestino* const nodeDestinoPtr, const std::vector<int> state)
    :
    nodoPtr(nodeDestinoPtr),
    state(state),
    Qs(1, Q(pedestrian::surviveReward))
{
}

stateMatrix::stateMatrix(const node* const node, const std::vector<int> state)
    :
    nodoPtr(node),
    state(state)
{
    const std::vector<link*> linkConnectionsPtr = nodoPtr->getLinkConnectionsPtr();
    for (link* linkConnection : linkConnectionsPtr) {
        Qs.emplace_back(linkConnection);
    } 
}
stateMatrix::stateMatrix(const node *const nodePtr, const std::vector<int> state, std::vector<Q> Qs)
    : nodoPtr(nodePtr), state(state), Qs(Qs)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const node* const stateMatrix::getNodePtr() const {
    return nodoPtr;
}
const std::vector<int> stateMatrix::getState() const {
    return state;    
}
std::vector<Q> &stateMatrix::getQs() {
    return Qs;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// int stateMatrix::getTamanoVector() {
//     return tamanoVectorIO;
// }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// method 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// void stateMatrix::mostrarStateMatrix() {
//     /* shows in the terminal each line of the stateMatrix. */
//     // state value
//     stateValue.mostrarState();
//     /* prints 0 where there is no state because it always prints 10 state elements. */
//     // for (int i = 0; i < tamanoVector - getStateValue().getDensityLinks().size(); i++) {
//     //     std::cout << "0,";
//     // }
Q* stateMatrix::buscarQ(const link* const callePtr) {
    // when it is nodeEvacuacion the street points to a nullptr
    // it will only have one Q
    if(callePtr == nullptr)
    {
        return &(Qs.at(0));
    }
    else {
    /* Get Q according to the executed street*/
        for (Q& q : Qs) {
            // Compare the street pointer of the Q object with callePtr
            if (q.getCallePtr() == callePtr) {
                // Return a pointer to the found Q object
                return &q;
            }
        }
    }
    return nullptr;
}
const Q* stateMatrix::buscarQMax() {
    // Find the maximum value
    auto maxElementIt = std::max_element(Qs.begin(), Qs.end(),
                                         [](const Q& a, const Q& b) {
                                             return a.getValor() < b.getValor();
                                         });
    double maxValue = *(maxElementIt->getValor());
    // Collect all the elements that have the maximum value
    std::vector<const Q*> maxElements;
    for (const auto& q : Qs) {
        if (q.getValor() == maxValue) {
            maxElements.push_back(&q);
        }
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, maxElements.size() - 1);
    // Generate a random index
    int randomIndex = dis(gen);
    // Get the random element
    const Q* randomElement = maxElements[randomIndex];
    return randomElement; // Return one of the random maxima
}
void stateMatrix::mostrarStateMatrix() const {
    /* shows in the terminal each line of the stateMatrix. */
    // node id
    std::cout << "idN:" << nodoPtr->getIdNode() << " ";
    // state 
    std::cout << "s:";
    for (const int &value : state) {
        std::cout << value << " ";
    }
    // Q
    std::cout << "Qs: ";
    for(const Q &q : Qs){
        q.mostrarQs(); 
    }
    // line break
    std::cout << std::endl;
}
void stateMatrix::imprimirState(fileIO* const file) const {
    /* printing of states in an array of 10 columns*/
    // printing of state data as far as it has, it may vary
    for(auto it = state.begin(); it != state.end(); ++it){
        file->getFileFstream() << (*it) << ',';
    }
    // fill with 0 up to 10 elements
    size_t faltante = io::tamanoElementosIO - state.size();
    for (size_t i = 0; i < faltante; i++) {
        file->getFileFstream() << "0,";
    }
 
    // for (int i = 0; i < stateMatrix::tamanoVectorIO; i++) {
    //     if (i < state.size()) {
    //         file->getFileFstream() << state.at(i) << ',';
    //     } else {
    //         file->getFileFstream() << "0,";
    //     }
    // }
}
void stateMatrix::imprimirQs(fileIO* const file) const {
    /* printing of Q in an array of 10 columns*/
    for (int i = 0; i < io::tamanoElementosIO; i++) {
        if (i < Qs.size()) {
            file->getFileFstream() << Qs.at(i).getValor() << ',';
        } else {
            file->getFileFstream() << "0,";
        }
    }  
    for (int i = 0; i < io::tamanoElementosIO; i++) {
        if (i < Qs.size()) {
            file->getFileFstream() << Qs.at(i).getObservaciones() << ',';
        } else {
            file->getFileFstream() << "0,";
        }
    }  

}
void stateMatrix::imprimirStateMatrix(fileIO* const file) const {
   // Print a row of the stateMatrix element.
    file->getFileFstream() << nodoPtr->getIdNode() << ",";
    // !-----------------------------------------------------------------------
    // Print all the elements of state and fill with 0 to reach
    // 10 elements.
    imprimirState(file);
    // !-----------------------------------------------------------------------
    // Print all the elements of Q and fill with 0 to reach
    // 10 elements.
    imprimirQs(file);
    // Print all the elements of pedestrianMassState and fill with 0 to reach
    // 10 elements.
    // pedestrianMassStateValue.imprimirPedestrianMassStateVector(file);
    file->getFileFstream() << std::endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
stateMatrix* stateMatrix::creacionObtencionStateMatrix(
    node* const nodo,
    const std::vector<int> stateObservado)
{
    /* when a person arrives at a node, with what was observed it looks in the node
        where it is whether the stateMatrixExperimentado is found, otherwise it creates it
        */
    std::vector<stateMatrix*>* stateMatrixExperimentados = nodo->getStateMatrixExperimentadosPtr();
    // loop to search each list of stateMatrix
    for (stateMatrix* stateMatrixExperimentado : *stateMatrixExperimentados) {
       // if stateMatrixExperiemntado is equal to stateObservado then that stateMatrix exists
        if (stateMatrixExperimentado->getState() == stateObservado) {
            return stateMatrixExperimentado; 
        }
    }
    // creation of the experienced stateMatrix
    stateMatrix* nuevoStateMatrix = (dynamic_cast<nodeDestino*>(nodo)) 
        ? new stateMatrix(static_cast<nodeDestino*>(nodo), stateObservado)
        : new stateMatrix(nodo, stateObservado);
    std::vector<stateMatrix*>& dbStateMatrix = stateMatrixs::get()->getDbStateMatrixs();
    dbStateMatrix.emplace_back(nuevoStateMatrix);
    nodo->addStateMatrixExperimentadosPtr(nuevoStateMatrix);
    return nuevoStateMatrix;
}
