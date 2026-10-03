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

#include "simulationTime.h"
#include "dictionary.h"
#include "nodes.h"
#include "links.h"
#include "pedestrians.h"
#include <chrono>

int simulationTime::deltaTime = 1;
std::string simulationTime::filenameData = "data/";


simulationTime* simulationTime::simulationTimeInstance = nullptr;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
simulationTime::simulationTime()
    : deltaT(1),
      timeValue(0),
      endTime(std::get<int>(dictionary::get()->lookup("endTime"))),
      graphicPrintoutPeriod(std::get<int>(dictionary::get()->lookupDefault("graphicPrintoutPeriod"))),
      pedestrianCountPeriod(std::get<int>(dictionary::get()->lookupDefault("pedestrianCountPeriod")))
{
    initializeNumberSimulation();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void simulationTime::setTimeValue(int timeValue) {
    (*this).timeValue = timeValue;
}
void simulationTime::setStartNumberSimulation(int startNumberSimulation) {
    (*this).startNumberSimulation = startNumberSimulation;
}
void simulationTime::setINumberSimulation(int iNumberSimulation) {
    (*this).iNumberSimulation = iNumberSimulation;
}
void simulationTime::setEndNumberSimulation(int endNumberSimulation) {
    (*this).endNumberSimulation = endNumberSimulation;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int simulationTime::getTimeValue() const {
    return timeValue;
}
const int simulationTime::getEndTime() const {
    return endTime;
}
const int simulationTime::getDeltaT() const {
    return deltaT;  
}
const int simulationTime::getGraphicPrintoutPeriod() const {
    return graphicPrintoutPeriod;
}
int simulationTime::getStartNumberSimulation() const {
    return startNumberSimulation;
}
int simulationTime::getINumberSimulation() const {
    return iNumberSimulation;
}
int simulationTime::getEndNumberSimulation() const {
    return endNumberSimulation;
}
const int simulationTime::getPedestrianCountPeriod() const {
    return pedestrianCountPeriod;
}
double simulationTime::getRandomChoiceRate() const {
    return randomChoiceRate;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
simulationTime* simulationTime::get() {
    if (!simulationTimeInstance) {
        simulationTimeInstance =  new simulationTime();
    }
    return simulationTimeInstance;
}

std::string simulationTime::getFilenameData() {
    return filenameData;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
simulationTime& simulationTime::operator++(int) {
    setTimeValue(timeValue + deltaT); 
    return *this;
}
void simulationTime::increaseTime() {
    // increases the evacuation time
    timeValue += deltaT;
}
void simulationTime::increaseINumberSimulation() {
    // increase the simulation number
    // setINumberSimulation(getINumberSimulation()+1);
    iNumberSimulation += 1;
    // reset the time
    timeValue = 0;
    // reset the count of evacuated people
    nodeEvacuation::totalEvacuatedPeople = 0;
    // return the people to the start node
    pedestrians::get()->resetPedestrians();
    nodes::get()->resetNodesEvacuations();
    // resets the list of people on streets
    links::get()->resetLinks();
    // restart the timer of a simulation 
    startTimeSimulation = std::chrono::high_resolution_clock::now();
}
void simulationTime::initializeNumberSimulation() {
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
int simulationTime::extractINumberSimulation() const {
    /* Extract the current simulation number according to the sim file of previous
        states from the control of the variable previousComputation*/
    const std::string lastFile_str = std::get<std::string>(dictionary::get()->lookup("previousComputationFile"));
    // finds the first number from 1-9 in the name of the states file
    const size_t position = lastFile_str.find_first_of("123456789");
    return std::stoi(lastFile_str.substr(position));
}
void simulationTime::calculateRandomChoiceRate() {
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
            temp = calculateTemp(std::get<double>(it->second));
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
const double simulationTime::calculateTemp(const double r) const {
    const int N = endNumberSimulation;
    const double factor = N - 1;
    return (N - r * factor) / (r * factor);
}
bool simulationTime::running() const {
    /* controls the evacuation time*/
    // Check whether the current time is less than the adjusted total evacuation time
    if (timeValue >= (endTime - 0.5 * deltaT)) {
        return false;
    }
    // Check whether all people have been evacuated
    return !nodeEvacuation::checkTotalEvacuation();
}
void simulationTime::showSimulationResults() {
    // show 
    std::cout << "***** Simu: " << iNumberSimulation << " *****" << std::endl;
    std::cout << "epsilon greedy - exploration: " << randomChoiceRate << std::endl;
    std::cout << "survived pedestrian: " << nodeEvacuation::getTotalEvacuatedPeople() << std::endl;
    // end of the simulation
    endTimeSimulation = std::chrono::high_resolution_clock::now();
    // simulation time
    const auto duration = endTimeSimulation - startTimeSimulation;
    const auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    const auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    const auto durationMinutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    std::cout << "Duration: " << durationMinutes.count() << " min";
    std::cout << " / " << durationSeconds.count() << " s";
    std::cout << " / " << milliSeconds.count() << " ms" << std::endl;
    std::cout << std::endl;
}
void simulationTime::showTime() const {
    // Show current time in the terminal.
    std::cout << "Time = " << timeValue << std::endl;
}
bool simulationTime::checkGraphicPrintoutPeriod() const {
    /* how often to print variables*/
    return (getTimeValue() % graphicPrintoutPeriod == 0);
}
bool simulationTime::checkPedestrianCountPeriod() const {
    /* how often to count the people*/
    return getTimeValue() % pedestrianCountPeriod == 0;
}
