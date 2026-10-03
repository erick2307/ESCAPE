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

#include "subLink.h"
#include "dictionary.h"
#include "link.h"
#include "pedestrian.h"
#include <iomanip>
// #include "pedestrian.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
subLink::subLink(const link *street)
    : street(street),
      sublinkDensity(0)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void subLink::setPedestriansInSublink(std::vector<pedestrian*> pedestriansInSublink) {
    (*this).pedestriansInSublink = pedestriansInSublink;
}
void subLink::setSublinkDensity(double sublinkDensity) {
    (*this).sublinkDensity = sublinkDensity;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<pedestrian*>& subLink::getPedestriansInSublink() {
    return pedestriansInSublink;
}
double subLink::getSublinkDensity() const {
    return sublinkDensity;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void subLink::updatePedestrianVelocityInSublink(double &velocity) {
    for (pedestrian* person: pedestriansInSublink) {
        person->setPedestrianVelocity(velocity);
    }
}
double subLink::calculateSubdivisionDensity() const{
    /* calculates the density of the subdivision*/
    return pedestriansInSublink.size() / (street->getSubdivisionWidth() * street->getWidth()); 
}
void subLink::addPedestrian(pedestrian* const person) {
//     /* add person to the sublink*/
    pedestriansInSublink.push_back(person);
}
void subLink::removePedestrian(pedestrian *const person) {
    /* remove person from the sublink because they move to another sublink*/
    pedestriansInSublink.erase(std::remove(pedestriansInSublink.begin(), pedestriansInSublink.end(), person), pedestriansInSublink.end());
}
double subLink::calculatePedestrianCount() const {
    return pedestriansInSublink.size();
}
void subLink::reset() {
    pedestriansInSublink.clear();
    sublinkDensity = 0;
}
void subLink::showSubdivision() const {
    std::cout << std::setw(2) << pedestriansInSublink.size() << " ";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << sublinkDensity;
    std::cout << "|";
}
