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

#ifndef nodeEvacuation_h
#define nodeEvacuation_h

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// own headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "pedestrian.h"
#include "vector2D.h"
#include "io.h"
#include <vector>
#include "node.h"

// forward declaration
class pedestrian;

class nodeEvacuation: public node {
    
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // maxEvacuatedPeople   |-->| ID OF THE INTERSECTION
    // evacuatedPeoplePtr   |-->| LIST OF POINTERS TO EVACUATED PEOPLE
    // full                  |-->| EVACUATION NODE AT MAXIMUM CAPACITY 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int maxEvacuatedPeople;
    std::vector<pedestrian*> evacuatedPeoplePtr;
    bool full;

public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static member
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static int totalEvacuatedPeople;
    static int maxEvacuatedPeopleGlobal;
    static bool totalEvacuation;

    std::string getNodeType() override;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // constructor
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    nodeEvacuation();
    nodeEvacuation(const int id, const vector2D coordinate);
    nodeEvacuation(const int id, const vector2D coordinate, const int maxEvacuatedPeople);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int getMaxEvacuatedPeople() const;
    std::vector<pedestrian*> getEvacuatedPeoplePtr() const;
    bool getFull() const;
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int static getTotalEvacuatedPeople();
    static bool checkTotalEvacuation();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void reset();
    bool checkFull() const;
    pedestrianStatus pedestrianStateAtNode() const override;
    bool checkNodeEvacuation() const override;
    std::vector<int> observedState() const override;
    void registerPerson(pedestrian* const person);
    void printEvacuatedPeople(std::fstream* file);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static metods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static void addTotalEvacuatedPeople();
    static void printNodeEvacuation(fileIO* const file);
    static void printTotalEvacuatedPeople(fileIO* const file);
    static void plotEvacuatedVsTime(fileIO* const file);
    static void printEvacuatedVsTime(fileIO* const file);
    static void plotTotalEvacuatedVsSimulation(fileIO* const file);
    static void printTotalEvacuatedVsSimulation(fileIO* const file);
    static void printTotalEvacuatedPeopleVariable(fileIO* const file);


};

#endif
