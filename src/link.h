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

#ifndef link_h
#define link_h
/*---------------------------------------------------------------------------*\
A street.
\*---------------------------------------------------------------------------*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// general headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <fstream>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "iomanip"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// own headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "subLink.h"
#include "vector2D.h"
#include "vector"
#include "nodes.h"
#include "dictionary.h"
#include "pedestrian.h"


class link{
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // idLink            |-->| ID OF THE STREET
    // idNode1           |-->| INTERSECTION OF THE STREET  
    // idNode2           |-->| THE OTHER INTERSECTION OF THE STREET 
    // length            |-->| LENGTH OF THE STREET
    // width             |-->| WIDTH OF THE STREET
    // linkOrientation   |-->| ANGLE THAT THE HORIZONTAL FORMS WITH THE STREET
    // subdivisionWidth  |-->| WIDTH OF A SUBDIVISION OF THE STREET
    // densityLevel      |-->| DENSITY LEVEL OF THE STREET
    // pedestriansLinkPtr|-->| PEOPLE IN THE STREET
    // subdivisions     |-->| SUBDIVISIONS OF THE STREET
    // numberLinkDivision|-->| NUMBER OF DIVISIONS OR SUBLINKS OF A STREET
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int idLink;
    const node* const node1Ptr;
    const node* const node2Ptr;
    const int length;
    const int width;
    const vector2D linkOrientation;
    const double subdivisionWidth;
    const int subdivisionCount;
    int densityLevel;
    std::vector<subLink> subdivisions;
    
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static member
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // constructor
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    link(const int idLink, const node* const node1Ptr, const node* const node2Ptr, const int length, const int width);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setDensityLevel(int densityLevel);
    void setPedestriansLink(std::vector<pedestrian*> pedestriansLink);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int getIdLink() const;
    const node* const getNode1Ptr() const;
    const node* const getNode2Ptr() const;
    const int getLength() const;
    const int getWidth() const;
    const vector2D getLinkOrientation() const;
    const double getSubdivisionWidth() const;
    const int getSubdivisionCount() const;
    int getDensityLevel();
    std::vector<subLink>& getSubdivisions();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const vector2D calculateLinkOrientation() const;
    const double calculateSubdivisionWidth(const std::string &subdivisionOption) const;
    const int calculateSubdivisionCount(const std::string &subdivisionOption);
    void calculateDensityGeneral();
    int calculatePedestriansLink() const;
    int calculateDensityLink() const;
    int calculateDensityLevelLink(const double linkDensity) const;
    void addPedestrianSublink(pedestrian* const person, const int idSublink);
    void removePedestrianSublink(pedestrian* const person, const int idSublink);
    subLink* calculateSublink(const vector2D &position) const;
    void resetSubdivisions();
    void showSubdivisions() const;
    void showLink() const;
    void printLink(std::fstream& file);
};
#endif
