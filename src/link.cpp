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
      orientacionLink(calcularOrientacionLink()),
      anchoSubdivision(calcularAnchoSubdivision(std::get<std::string>(dictionary::get()->lookupDefault("opcionSubdivision")))),
      cantidadSubdivisiones(calcularCantidadSubdivisiones(std::get<std::string>(dictionary::get()->lookupDefault("opcionSubdivision")))),
      subdivisiones(cantidadSubdivisiones, subLink(this)),
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
const vector2D link::getOrientacionLink() const {
    return orientacionLink;
}
int link::getDensityLevel() {
    return densityLevel;
}
std::vector<subLink>& link::getSubdiviones() {
    return subdivisiones;  
}
const double link::getAnchoSubdivisiones() const {
    return anchoSubdivision;
}
const int link::getCantidadSubdivisiones() const {
    return cantidadSubdivisiones;    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const vector2D link::calcularOrientacionLink() const{
    /* this should calculate the orientation but it needs access to dbLink
        so the direction will be calculated in pedestrian*/
    /* calculates the direction of the street with the start and end nodes*/
    const double x = node2Ptr->getCoordenada().getX() - node1Ptr->getCoordenada().getX();
    const double y = node2Ptr->getCoordenada().getY() - node1Ptr->getCoordenada().getY();
    // Calculates the magnitude of the direction vector
    const double magnitud = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
    // Normalizes the direction vector (divides each element by the magnitude)
    return {std::abs(x / magnitud), std::abs(y / magnitud)};
}
const double link::calcularAnchoSubdivision(const std::string &opcionSubdivision) const{
    // the opcionSubdivision is cantidadSubvisiones,
    // that is, I will give it the number of subdivisions it must have
    // in a link so I calculate the width
    if (opcionSubdivision == "cantidadSubdivisiones") {
        /* Calculates the width of the street divisions according to the preset number of divisions*/
        const vector2D nodo1Coordenada = node1Ptr->getCoordenada();
        const vector2D nodo2Coordenada = node2Ptr->getCoordenada();
        const double ancho_x = nodo1Coordenada.getX() - nodo2Coordenada.getX();
        const double ancho_y = nodo1Coordenada.getY() - nodo2Coordenada.getY();
        const double ancho = std::sqrt(ancho_x * ancho_x + ancho_y * ancho_y) / static_cast<double>(std::get<int>(dictionary::get()->lookupDefault("cantidadSubdivisiones")));
        return ancho;
    }
    // the width is given, the default is 2 meters
    else {
        // calculates the number of subdivisions according to the given width, then calculates the width again
        const int cantSec = std::round(length / std::get<double>(dictionary::get()->lookupDefault("anchoSubdivision")));
        return length / static_cast<double>(cantSec);
    }
}
const int link::calcularCantidadSubdivisiones(const std::string &opcionSubdivision) {
    // if the opcionSubdivision is cantidadSubvisiones, that is, I will give it the number of subdivisions there must be
    // in a link no calculation is needed because the number of subdivisions is already known
    if (opcionSubdivision == "cantidadSubdivisiones") {
        return std::get<int>(dictionary::get()->lookupDefault("cantidadSubdivisiones"));
    }
    // the subdivision width is given, I must calculate the number of subdivisions.
    else {
        // the number of sections is an average of the length divided by the subsection width
        return std::round(length / anchoSubdivision);
    }
}
void link::calcularDensityGeneral() {
    /* calculation of the density in each sublink of the street*/
    // subdivision is it
    double densidadMaxima = 0;
    for (auto it = subdivisiones.begin(); it != subdivisiones.end(); ++it) {
        // calculates the density according to the number of people
        double densidadSublink = it->calcularDensidadSubdivision();
        // store densidadMaxima
        if (densidadSublink > densidadMaxima) {
            densidadMaxima = densidadSublink;
        }
        // checks whether there is a change in the sublink density
        if (it->getDensidadSublink() != densidadSublink) {
            double velocidadEnSublink = velocidad::actualizarVelocidad(densidadSublink);
            // checks whether there is a change in the speed of the people within the sublink
            if (!(it->getPedestriansInSublink().empty())) {
                if (it->getPedestriansInSublink().at(0)->getVelocidadPedestrian().getMagnitud() != velocidadEnSublink) {
                    it->actualizarVelocidadPedestrianInSublink(velocidadEnSublink);
                }
            }
        }
        // stores the densities for the next calculation
        it->setDensidadSublink(densidadSublink);
    }
    densityLevel = calcularDensityLevelLink(densidadMaxima);
}
int link::calcularDensityLink() const{
    /* calculates the density of the street according to the number of people inside*/
    return calcularPedestriansLink() / (anchoSubdivision * width);
}
int link::calcularDensityLevelLink(const double densidadLink) const {
    /* calculates the density level according to the density of the street*/
    // choice of the density level according to preset ranges
    if(densidadLink <= 0.5){
        return 0;
    }  
    else if (densidadLink <= 3.0) {
        return 1;
    }
    else {
        return 2;
    }
}
void link::agregarPedestrianSublink(pedestrian* const persona, const int idSublink) {
    subdivisiones.at(idSublink).agregarPedestrian(persona);
}
void link::quitarPedestrianSublink(pedestrian* const persona, const int idSublink) {
    subdivisiones.at(idSublink).agregarPedestrian(persona);
}
// subLink* link::calcularSublink(const vector2D &position) const {
//      /* Calculates the location of the person in the subLink array*/
//     // distance from the person to the nodeInicio of the person
//     // double index_x = position.getX() - nodeInicioPtr->getCoordenada().getX();
//     const double index_x = position.getX() - node1Ptr->getCoordenada().getX();
//     // double index_y = position.getY() - nodeInicioPtr->getCoordenada().getY();
//     const double index_y = position.getY() - node1Ptr->getCoordenada().getY();
//     int index_hipo = std::sqrt(std::pow(index_x,2) + pow(index_y, 2)) / anchoSubdivision;
//     if (index_hipo>= 10) {
//         index_hipo=9;    
//     }
//     return &subdivisiones.at(index_hipo);
   
// }
int link::calcularPedestriansLink() const {
    int personasEnCalle = 0 ;
    for (auto& subdivion : subdivisiones) {
        personasEnCalle += subdivion.calcularCantidadPedestrians();
    }
  return personasEnCalle; 
}
void link::reiniciarSubdivisiones() {
    // reset the subdivisions vector
    for (auto& sublink : subdivisiones) {
        // We call the reiniciar() method of each subLink
        sublink.reiniciar();
    }
}
// void link::mostrarPedestriansLink() const {
//     std::cout << "pedestrianLink: ";
//     for (int i = 0; i < pedestriansLinkPtr.size(); i++) {
//         std::cout << pedestriansLinkPtr.at(i)->getIdPedestrian() << " ";
//     }
// }
void link::mostrarSubdivisiones() const {
    std::cout << "l: ";
    std::cout << std::setw(2) << idLink << " ";
    for (int i = 0; i < subdivisiones.size(); i++) {
        subdivisiones.at(i).mostrarsubdivision();
    }
    std::cout << "dl: " << densityLevel;
    std::cout << "p: " << calcularPedestriansLink();
    // std::cout << "p: " << pedestriansLinkPtr.size() << std::endl;
}
void link::mostrarLink() const {
    std::cout << "link: ";
    std::cout << idLink << " ";
    std::cout << "nodes: ";
    std::cout << node1Ptr->getIdNode() << " ";
    std::cout << node2Ptr->getIdNode() << " ";
    // mostrarPedestriansLink();
    mostrarSubdivisiones();
    std::cout << std::endl;
}
void link::imprimirLink(std::fstream& file) {
    file << std::fixed << std::setprecision(2);
    file << node1Ptr->getCoordenada().getX() << " ";
    file << node1Ptr->getCoordenada().getY() << " ";
    file << node2Ptr->getCoordenada().getX() << " ";
    file << node2Ptr->getCoordenada().getY() << " ";
    file << std::endl;
}
