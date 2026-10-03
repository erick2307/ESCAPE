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

#include "link.h"
#include "pedestrian.h"
#include "subLink.h"
#include "simulationTime.h"
#include "vector2D.h"
#include "velocity.h"
#include <iomanip>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
link::link(const int idLink, const node* const node1, const node* const node2, const int length, const int width)
    : idLink(idLink), node1Ptr(node1), node2Ptr(node2), length(length), width(width),
      linkOrientation(calculateLinkOrientation()),
      subdivisionWidth(calculateSubdivisionWidth(std::get<std::string>(dictionary::get()->lookupDefault("subdivisionOption")))),
      subdivisionCount(calculateSubdivisionCount(std::get<std::string>(dictionary::get()->lookupDefault("subdivisionOption")))),
      subdivisions(subdivisionCount, subLink(this)),
      densityLevel(0)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void link::setDensityLevel(int densityLevel) {
    (*this).densityLevel = densityLevel;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int link::getIdLink() const {
    return idLink;  
}
const node* const link::getNode1Ptr() const {
    return node1Ptr;
}
const node* const link::getNode2Ptr() const {
    return node2Ptr;
}
const int link::getLength() const{
    return length;
}
const int link::getWidth() const{
    return width;
}
const vector2D link::getLinkOrientation() const {
    return linkOrientation;
}
int link::getDensityLevel() {
    return densityLevel;
}
std::vector<subLink>& link::getSubdivisions() {
    return subdivisions;  
}
const double link::getSubdivisionWidth() const {
    return subdivisionWidth;
}
const int link::getSubdivisionCount() const {
    return subdivisionCount;    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const vector2D link::calculateLinkOrientation() const{
    /* this should calculate the orientation but it needs access to dbLink
        so the direction will be calculated in pedestrian*/
    /* calculates the direction of the street with the start and end nodes*/
    const double x = node2Ptr->getCoordinate().getX() - node1Ptr->getCoordinate().getX();
    const double y = node2Ptr->getCoordinate().getY() - node1Ptr->getCoordinate().getY();
    // Calculates the magnitude of the direction vector
    const double magnitude = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
    // Normalizes the direction vector (divides each element by the magnitude)
    return {std::abs(x / magnitude), std::abs(y / magnitude)};
}
const double link::calculateSubdivisionWidth(const std::string &subdivisionOption) const{
    // the subdivisionOption is subdivisionCount,
    // that is, I will give it the number of subdivisions it must have
    // in a link so I calculate the width
    if (subdivisionOption == "subdivisionCount") {
        /* Calculates the width of the street divisions according to the preset number of divisions*/
        const vector2D node1Coordinate = node1Ptr->getCoordinate();
        const vector2D node2Coordinate = node2Ptr->getCoordinate();
        const double width_x = node1Coordinate.getX() - node2Coordinate.getX();
        const double width_y = node1Coordinate.getY() - node2Coordinate.getY();
        const double computedWidth = std::sqrt(width_x * width_x + width_y * width_y) / static_cast<double>(std::get<int>(dictionary::get()->lookupDefault("subdivisionCount")));
        return computedWidth;
    }
    // the width is given, the default is 2 meters
    else {
        // calculates the number of subdivisions according to the given width, then calculates the width again
        const int cantSec = std::round(length / std::get<double>(dictionary::get()->lookupDefault("subdivisionWidth")));
        return length / static_cast<double>(cantSec);
    }
}
const int link::calculateSubdivisionCount(const std::string &subdivisionOption) {
    // if the subdivisionOption is subdivisionCount, that is, I will give it the number of subdivisions there must be
    // in a link no calculation is needed because the number of subdivisions is already known
    if (subdivisionOption == "subdivisionCount") {
        return std::get<int>(dictionary::get()->lookupDefault("subdivisionCount"));
    }
    // the subdivision width is given, I must calculate the number of subdivisions.
    else {
        // the number of sections is an average of the length divided by the subsection width
        return std::round(length / subdivisionWidth);
    }
}
void link::calculateDensityGeneral() {
    /* calculation of the density in each sublink of the street*/
    // subdivision is it
    double maxDensity = 0;
    for (auto it = subdivisions.begin(); it != subdivisions.end(); ++it) {
        // calculates the density according to the number of people
        double sublinkDensity = it->calculateSubdivisionDensity();
        // store maxDensity
        if (sublinkDensity > maxDensity) {
            maxDensity = sublinkDensity;
        }
        // checks whether there is a change in the sublink density
        if (it->getSublinkDensity() != sublinkDensity) {
            double velocityInSublink = velocity::updateVelocity(sublinkDensity);
            // checks whether there is a change in the speed of the people within the sublink
            if (!(it->getPedestriansInSublink().empty())) {
                if (it->getPedestriansInSublink().at(0)->getPedestrianVelocity().getMagnitude() != velocityInSublink) {
                    it->updatePedestrianVelocityInSublink(velocityInSublink);
                }
            }
        }
        // stores the densities for the next calculation
        it->setSublinkDensity(sublinkDensity);
    }
    densityLevel = calculateDensityLevelLink(maxDensity);
}
int link::calculateDensityLink() const{
    /* calculates the density of the street according to the number of people inside*/
    return calculatePedestriansLink() / (subdivisionWidth * width);
}
int link::calculateDensityLevelLink(const double linkDensity) const {
    /* calculates the density level according to the density of the street*/
    // choice of the density level according to preset ranges
    if(linkDensity <= 0.5){
        return 0;
    }  
    else if (linkDensity <= 3.0) {
        return 1;
    }
    else {
        return 2;
    }
}
void link::addPedestrianSublink(pedestrian* const person, const int idSublink) {
    subdivisions.at(idSublink).addPedestrian(person);
}
void link::removePedestrianSublink(pedestrian* const person, const int idSublink) {
    subdivisions.at(idSublink).addPedestrian(person);
}
// subLink* link::calculateSublink(const vector2D &position) const {
//      /* Calculates the location of the person in the subLink array*/
//     // distance from the person to the startNode of the person
//     // double index_x = position.getX() - startNodePtr->getCoordinate().getX();
//     const double index_x = position.getX() - node1Ptr->getCoordinate().getX();
//     // double index_y = position.getY() - startNodePtr->getCoordinate().getY();
//     const double index_y = position.getY() - node1Ptr->getCoordinate().getY();
//     int index_hypotenuse = std::sqrt(std::pow(index_x,2) + pow(index_y, 2)) / subdivisionWidth;
//     if (index_hypotenuse>= 10) {
//         index_hypotenuse=9;    
//     }
//     return &subdivisions.at(index_hypotenuse);
   
// }
int link::calculatePedestriansLink() const {
    int peopleOnStreet = 0 ;
    for (auto& subdivision : subdivisions) {
        peopleOnStreet += subdivision.calculatePedestrianCount();
    }
  return peopleOnStreet; 
}
void link::resetSubdivisions() {
    // reset the subdivisions vector
    for (auto& sublink : subdivisions) {
        // We call the reset() method of each subLink
        sublink.reset();
    }
}
// void link::showPedestriansLink() const {
//     std::cout << "pedestrianLink: ";
//     for (int i = 0; i < pedestriansLinkPtr.size(); i++) {
//         std::cout << pedestriansLinkPtr.at(i)->getIdPedestrian() << " ";
//     }
// }
void link::showSubdivisions() const {
    std::cout << "l: ";
    std::cout << std::setw(2) << idLink << " ";
    for (int i = 0; i < subdivisions.size(); i++) {
        subdivisions.at(i).showSubdivision();
    }
    std::cout << "dl: " << densityLevel;
    std::cout << "p: " << calculatePedestriansLink();
    // std::cout << "p: " << pedestriansLinkPtr.size() << std::endl;
}
void link::showLink() const {
    std::cout << "link: ";
    std::cout << idLink << " ";
    std::cout << "nodes: ";
    std::cout << node1Ptr->getIdNode() << " ";
    std::cout << node2Ptr->getIdNode() << " ";
    // showPedestriansLink();
    showSubdivisions();
    std::cout << std::endl;
}
void link::printLink(std::fstream& file) {
    file << std::fixed << std::setprecision(2);
    file << node1Ptr->getCoordinate().getX() << " ";
    file << node1Ptr->getCoordinate().getY() << " ";
    file << node2Ptr->getCoordinate().getX() << " ";
    file << node2Ptr->getCoordinate().getY() << " ";
    file << std::endl;
}
