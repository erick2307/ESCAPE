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

#include <cstdlib>
#include <functional>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <variant>
#include "dictionary.h"
#include "pedestrian.h"
#include "simulationTime.h"
#include "nodes.h"
#include "links.h"
#include "stateMatrixs.h"
#include "pedestrians.h"

int main() {
    // print street mesh.
    links::get()->printMeshLinks();
    // Reading of past simulations.
    stateMatrixs::get()->readDbStateMatrixs();
    // according to the number of simulations
    while (simulationTime::get()->getINumberSimulation() <= simulationTime::get()->getEndNumberSimulation()) {
        // computes the value of the randomChoiceRate
        simulationTime::get()->calculateRandomChoiceRate();
        // loop for one evacuation
        while (simulationTime::get()->running()) {
            simulationTime::get()->increaseTime();
           // pedestrian modeling.
            pedestrians::get()->modelPedestrians();
            // pedestrian counter one time step behind the modeling function
            links::get()->countPedestrians();
            io::get()->printOutput();
            // sets the sublink value elements to 0            
            links::get()->resetSublinks();
        }
        // show simulation results
        simulationTime::get()->showSimulationResults();
        // increase the simulation number and reset values
        simulationTime::get()->increaseINumberSimulation();
    }
}
