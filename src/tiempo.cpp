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

#include "tiempo.h"
#include "dictionary.h"
#include "nodes.h"
#include "links.h"
#include "pedestrians.h"
#include <chrono>

int tiempo::deltaTiempo = 1;
std::string tiempo::filenameData = "data/";


tiempo* tiempo::tiempoInstance = nullptr;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tiempo::tiempo()
    : deltaT(1),
      valorTiempo(0),
      endTime(std::get<int>(dictionary::get()->lookup("endTime"))),
      graphicPrintoutPeriod(std::get<int>(dictionary::get()->lookupDefault("graphicPrintoutPeriod"))),
      pedestrianCountPeriod(std::get<int>(dictionary::get()->lookupDefault("pedestrianCountPeriod")))
{
    inicializarNumberSimulation();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void tiempo::setValorTiempo(int valorTiempo) {
    (*this).valorTiempo = valorTiempo;
}
void tiempo::setStartNumberSimulation(int startNumberSimulation) {
    (*this).startNumberSimulation = startNumberSimulation;
}
void tiempo::setINumberSimulation(int iNumberSimulation) {
    (*this).iNumberSimulation = iNumberSimulation;
}
void tiempo::setEndNumberSimulation(int endNumberSimulation) {
    (*this).endNumberSimulation = endNumberSimulation;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int tiempo::getValorTiempo() const {
    return valorTiempo;
}
const int tiempo::getEndTime() const {
    return endTime;
}
const int tiempo::getDeltaT() const {
    return deltaT;  
}
const int tiempo::getGraphicPrintoutPeriod() const {
    return graphicPrintoutPeriod;
}
int tiempo::getStartNumberSimulation() const {
    return startNumberSimulation;
}
int tiempo::getINumberSimulation() const {
    return iNumberSimulation;
}
int tiempo::getEndNumberSimulation() const {
    return endNumberSimulation;
}
const int tiempo::getPedestrianCountPeriod() const {
    return pedestrianCountPeriod;
}
double tiempo::getRandomChoiceRate() const {
    return randomChoiceRate;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tiempo* tiempo::get() {
    if (!tiempoInstance) {
        tiempoInstance =  new tiempo();
    }
    return tiempoInstance;
}

std::string tiempo::getFilenameData() {
    return filenameData;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tiempo& tiempo::operator++(int) {
    setValorTiempo(valorTiempo + deltaT); 
    return *this;
}
void tiempo::aumentarTiempo() {
    // increases the evacuation time
    valorTiempo += deltaT;
}
void tiempo::aumentarINumberSimulation() {
    // increase the simulation number
    // setINumberSimulation(getINumberSimulation()+1);
    iNumberSimulation += 1;
    // reset the time
    valorTiempo = 0;
    // reset the count of evacuated people
    nodeDestino::totalPersonasEvacuadas = 0;
    // return the people to the start node
    pedestrians::get()->reiniciarPedestrians();
    nodes::get()->reiniciarNodesEvacuations();
    // resets the list of people on streets
    links::get()->resetLinks();
    // restart the timer of a simulation 
    startTimeSimulation = std::chrono::high_resolution_clock::now();
}
void tiempo::inicializarNumberSimulation() {
    /* Initialize the NumberSimulation variables*/
    // For calibration process
    if(std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration"){
        // if it reads statematrix
        if (std::get<bool>(dictionary::get()->lookupDefault("computationContinued")) == true) {
            startNumberSimulation = 0;
            endNumberSimulation = std::get<int>(dictionary::get()->lookup("endNumberSimulation"));
            startNumberSimulation = startNumberSimulation + 1;
            iNumberSimulation = startNumberSimulation;
        }
        // if it does not read statematrix
        else {
            // starts at simulation 1
            startNumberSimulation = 1;
            iNumberSimulation = 1;
            endNumberSimulation = std::get<int>(dictionary::get()->lookup("endNumberSimulation"));
        }
        // start the real-time timer of the simulation
        startTimeSimulation = std::chrono::high_resolution_clock::now();
    }
    // For already trained process
    else if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "trained"){
        // start the real-time timer of the simulation
        startTimeSimulation = std::chrono::high_resolution_clock::now();
        startNumberSimulation = 1;
        iNumberSimulation = 1;
        // only 1 simulation
        endNumberSimulation = 1;
    }
}
int tiempo::extractINumberSimulation() const {
    /* Extract the current simulation number according to the sim file of previous
        states from the control of the variable previousComputation*/
    const std::string lastFile_str = std::get<std::string>(dictionary::get()->lookup("previousComputationFile"));
    // finds the first number from 1-9 in the name of the states file
    const size_t posicion = lastFile_str.find_first_of("123456789");
    return std::stoi(lastFile_str.substr(posicion));
}
void tiempo::calcularRandomChoiceRate() {
    const int k = iNumberSimulation;
    const int N = endNumberSimulation;
    // default value, can be 4 or 9 
    double temp = 4.0;
    // find the correct name
    // in calibration process
    if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration") {
        // looks for the keyword exploration
        auto it = dictionary::get()->getControlDict().find("exploration");
        if (it != dictionary::get()->getControlDict().end()) {
            // Key found, proceed with the operation
            temp = calcularTemp(std::get<double>(it->second));
        }
        // formula for random choice
        const double gleeFactor = temp / double(N);
        // the -1 is to start the number of simulations at 0
        randomChoiceRate = 1.0 / (gleeFactor * double(k - 1) + 1.0);
    }
    // in trained process
    else if(std::get<std::string>(dictionary::get()->lookupDefault("process")) == "trained") {
        // so that it only chooses sarsa, nothing random
        randomChoiceRate = 0;
    }
}
const double tiempo::calcularTemp(const double r) const {
    const int N = endNumberSimulation;
    const double factor = N - 1;
    return (N - r * factor) / (r * factor);
}
bool tiempo::running() const {
    /* controls the evacuation time*/
    // Check whether the current time is less than the adjusted total evacuation time
    if (valorTiempo >= (endTime - 0.5 * deltaT)) {
        return false;
    }
    // Check whether all people have been evacuated
    return !nodeDestino::verificarEvacuacionTotal();
}
void tiempo::mostrarIResultadosSimulacion() {
    // show 
    std::cout << "***** Simu: " << iNumberSimulation << " *****" << std::endl;
    std::cout << "epsilon greedy - exploration: " << randomChoiceRate << std::endl;
    std::cout << "survived pedestrian: " << nodeDestino::getTotalPersonasEvacuadas() << std::endl;
    // end of the simulation
    endTimeSimulation = std::chrono::high_resolution_clock::now();
    // simulation time
    const auto duration = endTimeSimulation - startTimeSimulation;
    const auto miliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    const auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    const auto durationMinutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    std::cout << "Duration: " << durationMinutes.count() << " min";
    std::cout << " / " << durationSeconds.count() << " s";
    std::cout << " / " << miliSeconds.count() << " ms" << std::endl;
    std::cout << std::endl;
}
void tiempo::mostrarTiempo() const {
    // Show current time in the terminal.
    std::cout << "Time = " << valorTiempo << std::endl;
}
bool tiempo::verificarGraphicPrintoutPeriod() const {
    /* how often to print variables*/
    return (getValorTiempo() % graphicPrintoutPeriod == 0);
}
bool tiempo::verificarPedestrianCountPeriod() const {
    /* how often to count the people*/
    return getValorTiempo() % pedestrianCountPeriod == 0;
}
