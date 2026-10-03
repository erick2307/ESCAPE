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

#include "pedestrian.h"

#include "io.h"
#include "node.h"
#include "link.h"
#include "nodeEvacuation.h"
#include "sarsa.h"
#include "stateMatrix.h"
#include "stateMatrixs.h"
#include "simulationTime.h"
#include "vector2D.h"
#include "velocity.h"
#include <bits/types/FILE.h>
#include <iostream>
#include <vector>
#include "pedestrians.h"
#include <limits> // For std::numeric_limits

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int pedestrian::contador = 1;
const int pedestrian::meanRayleigh = std::get<int>(dictionary::get()->lookup("meanRayleigh"));
const int pedestrian::surviveReward = 100000;
const int pedestrian::deadReward = -1000; 
const int pedestrian::stepReward = -1;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pedestrian::pedestrian(const int edad, const int gender, const int hhType, const int hhId, node* nodeArranque)
    : idPedestrian(contador++),
      edad(edad),
      gender(gender),
      hhType(hhType),
      hhId(hhId),
      nodeArranque(nodeArranque),
      tiempoInicial(calcularRayleighDistribution(calcularScaleRayleigh())),
      position(nodeArranque->getCoordenada()),
      nodeInicioPtr(nodeArranque),
      nodeFinalPtr(nullptr),
      direccionPedestrian(),
      velocidadPedestrian(),
      estadoPedestrian(pasivo),
      reward(0),
      tiempoAnteriorInterseccion(0),
      interseccion(true),
      stateMatrixCurrentPtr(nullptr),
      QCurrentPtr(nullptr),
      QPreviousPtr(nullptr),
      linkCurrentPtr(nullptr)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void pedestrian::setPosition(vector2D position) {
//     (*this).position = position;
// }
void pedestrian::setNodeInicio(node* nodeInicio){
    (*this).nodeInicioPtr = nodeInicio;
}
// void pedestrian::setNodeFinal(node* nodeFinal) {
//     (*this).nodeFinal = nodeFinal;
// }
// void pedestrian::setNodeInicioAnterior(node* nodeInicioAnterior) {
//     (*this).nodeInicioAnterior = nodeInicioAnterior;
// }
// void pedestrian::setLinkActual(link* linkActual) {
//     (*this).linkActual = linkActual;
// }
// void pedestrian::setLinkPasado(link *linkPasado) {
//     (*this).linkPasado = linkPasado;
// }
void pedestrian::setDireccionPedestrian(vector2D direccionPedestrian) {
    (*this).direccionPedestrian = direccionPedestrian;
}
void pedestrian::setVelocidadPedestrian(double velocidadPedestrian) {
    (*this).velocidadPedestrian.setMagnitud(velocidadPedestrian);
}
void pedestrian::setEstadoPedestrian(estado estadoPedestrian) {
    (*this).estadoPedestrian = estadoPedestrian;
}
// void pedestrian::setOrientacionLinkPasado(vector2D orientacionLinkPasado) {
//     (*this).orientacionLinkPasado = orientacionLinkPasado;
// }
// void pedestrian::setTiempoFinal(int tiempoFinal) {
//     (*this).tiempoFinal = tiempoFinal;
// }
// void pedestrian::setEmpezoCaminar(bool empezoCaminar) {
//     (*this).empezoCaminar = empezoCaminar;
// }
// void pedestrian::setPrimerTiempo(bool primerTiempo) {
//     (*this).primerTiempo = primerTiempo;
// }
// void pedestrian::setSaltoLink(bool saltoLink) {
//     (*this).saltoLink = saltoLink;
// }
void pedestrian::setReward(int reward) {
    (*this).reward = reward;
}
void pedestrian::setInterseccion(bool interseccion) {
    (*this).interseccion = interseccion;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int pedestrian::getIdPedestrian() const {
    return idPedestrian;
}
const int pedestrian::getEdad() const{
    return edad;
}
const int pedestrian::getGender() const {
    return gender;
}
const int pedestrian::getHHType() const{
    return hhType;
}
const int pedestrian::getHHId() const{
    return hhId;
}
const node* pedestrian::getNodeArranque() const {
    return nodeArranque;
}
const int pedestrian::getTiempoInicial() const {
    return tiempoInicial;
}
vector2D pedestrian::getPosition() const{
    return position;
}
node* pedestrian::getNodeInicio() const{
    return nodeInicioPtr;
}
node* pedestrian::getNodeFinal() const {
    return nodeFinalPtr;  
}
vector2D pedestrian::getDireccionPedestrian() const {
    return direccionPedestrian;
}
velocidad& pedestrian::getVelocidadPedestrian() {
    return velocidadPedestrian;
}
estado& pedestrian::getEstadoPedestrian() {
    return estadoPedestrian;
}
int pedestrian::getReward() const {
    return reward;  
}
int pedestrian::getTiempoAnteriorInterseccion() const{
    return tiempoAnteriorInterseccion;
}
bool pedestrian::getInterseccion() const {
    return interseccion;    
}
link* pedestrian::getLinkCurrent() const {
    return linkCurrentPtr;
}
stateMatrix *pedestrian::getStateMatrixCurrent() const {
    return stateMatrixCurrentPtr;    
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool pedestrian::operator==(const pedestrian& pedestrian2) const{
    /* compares whether two pedestrians are equal*/
    // the comparison is by id
    return idPedestrian == pedestrian2.idPedestrian;
}
void pedestrian::caminar() {
    /* displacement formula*/
    const vector2D velocidad = direccionPedestrian * velocidadPedestrian.getMagnitud();
    position += velocidad * tiempo::get()->getDeltaT();
}
double pedestrian::calcularIdSublink() {
    /* Calculates the location of the person in the subLink array*/
    // distance from the person to node 1 of the street
    const double anchoSubdivision = linkCurrentPtr->getAnchoSubdivisiones();
    const double index_x = position.getX() - linkCurrentPtr->getNode1Ptr()->getCoordenada().getX();
    const double index_y = position.getY() - linkCurrentPtr->getNode1Ptr()->getCoordenada().getY();
    int index_hipo = std::sqrt(std::pow(index_x,2) + pow(index_y, 2)) / anchoSubdivision;
    // if it is in a subdivision beyond the ones that exist, it is about to enter an intersection
    if (index_hipo >= linkCurrentPtr->getCantidadSubdivisiones()) {
        interseccion = true;
        // index_hipo = linkCurrentPtr->getCantidadSubdivisiones() - 1;    
    }
    return index_hipo;
}
bool pedestrian::verificarEndLink() const {
    // Calculates the Euclidean distance between the current coordinates and the target point
    const double umbral = velocidadPedestrian.getMagnitud();
    const double distancia = std::sqrt(std::pow(position.getX() - nodeFinalPtr->getCoordenada().getX(), 2) + std::pow(position.getY() - nodeFinalPtr->getCoordenada().getY(), 2));
    // Checks whether the distance is less than or equal to the threshold
    return distancia <= umbral;
}
int pedestrian::calcularIdEndSublink() const {
    /* Determines which is the last sublink*/
    // find out whether I am at the end or at the start
    if (nodeInicioPtr == linkCurrentPtr->getNode1Ptr()) {
        // if at the start, it is the last subdivision
        return linkCurrentPtr->getSubdiviones().size() - 1; 
    } 
    // it is at the start
    else {
        return 0;
    }
}
link* pedestrian::eleccionGeneralLink() const {
    // the first choice must be random
    if (tiempo::get()->getValorTiempo() == tiempoInicial and std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration") {
        return eleccionRandomLink();
    }
    if (estadoPedestrian == evacuado) {
        return nullptr;
    }
    // Configure the random number generator
    else {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<double> dis(0.0, 1.0);
        // Generate a random number in the range [0.0, 1.0)
        const double randomNumber = dis(gen);
        // compares the random number with the optimal choice rate
        // the latter must decrease as there are more simulations
        // the more simulations, the more use of the sarsa choice
        switch (randomNumber <= tiempo::get()->getRandomChoiceRate() ? 1 : 2) {
            case 1:
                return eleccionRandomLink();
            case 2:
                return eleccionSarsaLink();
        }
    }
    return nullptr;
}
link* pedestrian::eleccionRandomLink() const {
    /* The person is at an intersection and has multiple options for choosing a street.
        the street to take is decided randomly and will be stored in linkActual.*/
    // Mersenne Twister engine algorithm
    static std::random_device rd;
    static std::mt19937 generador(rd());
    // linkConnection of the start node 
    const std::vector<link*> linkConnection = nodeInicioPtr->getLinkConnectionsPtr();
    const int limite_max = linkConnection.size() - 1 ;
    // Create a uniform distribution using the specified range
    std::uniform_int_distribution<size_t> distribucion(0, limite_max);
    // choice of a street at random
    const size_t numero_aleatorio = distribucion(generador);
    // choice of the new street, main part of the function
    return linkConnection.at(numero_aleatorio);
}
link* pedestrian::eleccionSarsaLink() const {
    // finds the largest Q element of the experienced stateMatrix
    const Q* Qmax = stateMatrixCurrentPtr->buscarQMax();
    return const_cast<link*>(Qmax->getCallePtr());
}
// void pedestrian::eleccionDosCallesContinuas() {
//     // linkActual is the street about to change
//     if (!(nodeInicio->getLinkConnection().at(0)->getIdLink() == linkActual->getIdLink())) {
//         // setLinkActual(&dbLinkTotal.at(getNodeInicio()->getIdLinkConnection().at(0)));
//         // setLinkActual(links::get()->getDbLinkTotal().at(getNodeInicio()->getIdLinkConnection().at(0)).get());
//         setLinkActual(nodeInicio->getLinkConnection().at(0));
//         // sending action information to the stateMatrix
//         stateMatrixPedestrian.getActionValue().setILinkConnection(0);
//         stateMatrixPedestrian.getActionValue().setIdLink(linkActual->getIdLink());
//         // knowing the street, define the final node.
//         calcularNodeFinal();
//         // check whether the final node is an evacuation node.
//         verificarPedestrianEvacuation();
//     }
//     else {
//         // setLinkActual(&dbLinkTotal.at(getNodeInicio()->getIdLinkConnection().at(1)));
//         // setLinkActual(links::get()->getDbLinkTotal().at(getNodeInicio()->getIdLinkConnection().at(1)).get());
//         setLinkActual(nodeInicio->getLinkConnection().at(0));
//         // sending action information to the stateMatrix
//         stateMatrixPedestrian.getActionValue().setILinkConnection(1);
//         stateMatrixPedestrian.getActionValue().setIdLink(linkActual->getIdLink());
//         // knowing the street, define the final node.
//         calcularNodeFinal();
//         // check whether the final node is an evacuation node.
//         verificarPedestrianEvacuation();
//     }
// }
int pedestrian::calcularSignoNumero(double numero) {
    if (numero >= 0) {
        return 1;
    }
    else {
        return -1;
    }
}
void pedestrian::calcularDireccionPedestrian() {
    direccionPedestrian = linkCurrentPtr->getOrientacionLink() * calcularSignoDireccion();
}
vector2D pedestrian::calcularSignoDireccion() {
    double x = calcularSignoNumero(nodeFinalPtr->getCoordenada().getX() - nodeInicioPtr->getCoordenada().getX());
    double y = calcularSignoNumero(nodeFinalPtr->getCoordenada().getY() - nodeInicioPtr->getCoordenada().getY());
    return vector2D(x,y);
}
int pedestrian::calcularReward() const {
    /* calculation of the reward per step*/
    const int tiempoDesplazamiento = calcularTiempoDesplazamiento();
    const int pasos = tiempoDesplazamiento / tiempo::get()->getDeltaT();
    return pasos * stepReward;
}
int pedestrian::calcularTiempoDesplazamiento() const {
   /* calculates the next time at which the pedestrian will be at an intersection*/
    const int tiempoDesplazado = tiempo::get()->getValorTiempo() - tiempoAnteriorInterseccion;
    return  tiempoDesplazado;
}
void pedestrian::modelamientoPedestrian() {
    if(!(estadoPedestrian == evacuado)){
        const int tiempoActual = tiempo::get()->getValorTiempo();
        // when the person is passive, the state changes to evacuated when their departure time arrives
        if (estadoPedestrian == pasivo && tiempoInicial == tiempoActual) {
            estadoPedestrian = evacuando;
        }
        // performs the movement only when evacuating
        if (estadoPedestrian == evacuando or estadoPedestrian==evacuado) {
                // modeling when the person is at an intersection
            if (interseccion) {
                // if not yet evacuated, do the following
                // if it is a time different from the initial one, save the present into the past
                if(!(tiempoInicial == tiempoActual)){
                    // resets the reward value
                    reward = 0;
                    // saves the QCurrent before it is changed
                    // stateMatrixPreviousPtr = stateMatrixCurrentPtr;
                    QPreviousPtr = QCurrentPtr;
                    // now the final intersection is the initial intersection.
                    nodeInicioPtr = nodeFinalPtr;
                    // position correction when arriving close to the node.
                    position = {nodeInicioPtr->getCoordenada().getX(), nodeInicioPtr->getCoordenada().getY()};
                    // check whether I am at an evacuation point
                }
                // nodeInicioPtr->mostrarNode();
                estadoPedestrian = nodeInicioPtr->estadoPedestrianEnNodo();
                // observes the state of the node or nodeEvacuation
                const std::vector<int> stateObservado = nodeInicioPtr->stateObservado();
                // get stateMatrix
                stateMatrixCurrentPtr = stateMatrix::creacionObtencionStateMatrix(nodeInicioPtr, stateObservado);
               // stateMatrixCurrentPtr->mostrarStateMatrix();
                // choice of the street
                linkCurrentPtr = eleccionGeneralLink();
                if (estadoPedestrian == evacuando) {
                    // // add the people on the street
                    // linkCurrentPtr->agregarPedestrian(this);
                    // get final node
                    nodeFinalPtr = const_cast<node*>(nodeInicioPtr->buscarNodoFinal(linkCurrentPtr));
                    // direction of the person on the street.
                    calcularDireccionPedestrian();
                    // calculate idEndSublink
                    idEndSublink = calcularIdEndSublink();
                }
                // get Qcurrent
                QCurrentPtr = stateMatrixCurrentPtr->buscarQ(linkCurrentPtr);
                // increase observation
                QCurrentPtr->aumentar1Observacion();
                // except at the start
                if (estadoPedestrian == evacuado) {
                    dynamic_cast<nodeDestino*>(nodeInicioPtr)->contabilizarPersona(this);
                }
                if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration"){
                    if(!(tiempoInicial == tiempoActual)){
                        reward = calcularReward();
                        sarsa::sarsaActualizarQ(QPreviousPtr->getValor(), QCurrentPtr->getValor(), reward);
                    }
                }
                // saves the previous intersection
                tiempoAnteriorInterseccion = tiempoActual;
                // move onto the street
                interseccion=false;
            }
            else {
                // modeling when the person is inside the street
                if (estadoPedestrian == evacuando) {
                    // only adds when not at the intersection
                    if (tiempo::get()->getPedestrianCountPeriod()) {
                        // speed with random
                        velocidadPedestrian.calcularAjusteRandom();
                    }
                    // the person walks
                    caminar();    
                   // calculates position in subdivision 
                    const int idSublink = calcularIdSublink();
                    // when in a sublink close to nodoFinal
                    if (idSublink == idEndSublink and interseccion == false) {
                        // checks when it is close to an intersection
                        interseccion = verificarEndLink();
                    }
                    // checks how often it must count
                    if (tiempo::get()->getPedestrianCountPeriod()) {
                        // only adds when not at the intersection
                        if (interseccion==false) {
                            // adds person in sublink
                            linkCurrentPtr->agregarPedestrianSublink(this, idSublink);
                        }
                    }
                }
            }
        }
    }
}
void pedestrian::reiniciar() {
    /* reset values for next simulation*/
    nodeInicioPtr = const_cast<node*>(nodeArranque);
    position = nodeInicioPtr->getCoordenada();
    estadoPedestrian = pasivo;
    interseccion = true;
    // reward = 0;
}
void pedestrian::mostrarMovimientoPedestrian() const {
    /* shows the start and end intersection of a street, when
        the person.*/
    if (estadoPedestrian == evacuando or estadoPedestrian== evacuado) {
        std::cout << idPedestrian << ' ';
        std::cout << std::setw(6) << nodeInicioPtr->getIdNode() << ' ';
        std::cout << "start: ";
        // decimals for printing
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(5) << nodeInicioPtr->getCoordenada().getX() << ' ';
        std::cout << std::setw(5) << nodeInicioPtr->getCoordenada().getY() << ' ';
        std::cout << "now: ";
        std::cout << std::setw(5) << position.getX() << " ";
        std::cout << std::setw(5) << position.getY() << " ";
        std::cout << "end: ";
        
        std::cout << std::setw(5) << nodeFinalPtr->getCoordenada().getX() << ' ';
        std::cout << std::setw(5) << nodeFinalPtr->getCoordenada().getY() << ' ';
        std::cout << std::setw(5) << getReward() << ' ';
        std::cout << std::endl;
    }
}
void pedestrian::mostrarPedestrian() const {
    std::cout << idPedestrian << ' ';
    std::cout << nodeInicioPtr->getIdNode() << ' ';
    std::cout << "ti: " << tiempoInicial;
}
void pedestrian::imprimirPedestrianPosition(fileIO* file) const {
    // if (getEvacuado()) {
    // decimals for saving to files.
    file->getFileFstream() << std::fixed << std::setprecision(2);
    file->getFileFstream() << position.getX() << " ";
    file->getFileFstream() << position.getY() << " ";
    file->getFileFstream() << std::endl;
    // }
}
void pedestrian::imprimirPedestrianVelocity(fileIO* file) const{
    file->getFileFstream() << velocidadPedestrian.getMagnitud() << " ";
    file->getFileFstream() << std::endl;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const double pedestrian::calcularScaleRayleigh() {
    return static_cast<double>(meanRayleigh) * std::pow((2.0/M_PI), 0.5);
}
double generate_uniform_random(std::mt19937& gen) {
    // Generate a uniform random number in the range (0, 1)
    return std::generate_canonical<double, std::numeric_limits<double>::digits>(gen);
}
double pedestrian::calcularRayleighDistribution(const double sigma) {
    /* calculates a number according to the Rayleigh distribution, as a parameter it needs
        the variable sigma, which is the scaleRayleigh */
    std::random_device rd;
    std::mt19937 gen(rd());
    // Generate a uniform random number
    double u = generate_uniform_random(gen);
    // Calculate the random number according to the Rayleigh distribution
    return sigma * std::sqrt(-2.0 * std::log(1.0 - u));
}
void pedestrian::plotearPedestrians(fileIO* const file) {
    
    // Initialize maximum and minimum values
    static double minX = std::numeric_limits<double>::max();
    static double maxX = std::numeric_limits<double>::lowest();
    static double minY = std::numeric_limits<double>::max();
    static double maxY = std::numeric_limits<double>::lowest();
    // read it only at the start 
    if (tiempo::get()->getValorTiempo() == 1) {
        const auto& lineasCalles = links::get()->getDbLinkTotal();
        for (const auto lc : lineasCalles) {
            // Get start and end points for the line
            const auto puntoInicial = lc->getNode1Ptr();
            const auto puntoFinal = lc->getNode2Ptr();
            
            // Get the coordinates
            double x1 = puntoInicial->getCoordenada().getX();
            double y1 = puntoInicial->getCoordenada().getY();
            double x2 = puntoFinal->getCoordenada().getX();
            double y2 = puntoFinal->getCoordenada().getY();
            
            // Update the maximum and minimum values
            minX = std::min(minX, std::min(x1, x2));
            maxX = std::max(maxX, std::max(x1, x2));
            minY = std::min(minY, std::min(y1, y2));
            maxY = std::max(maxY, std::max(y1, y2));
        }
    }
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        // Configure Gnuplot
        fprintf(gnuplotPipe, "set output '%s'\n", file->getFullPath().c_str());
        fprintf(gnuplotPipe, "set terminal png size 1920,1080\n");
        fprintf(gnuplotPipe, "set yrange [%lf:%lf]\n", minY, maxY);
        fprintf(gnuplotPipe, "set xrange [%lf:%lf]\n", minX, maxX);
        fprintf(gnuplotPipe, "unset border\n");
        fprintf(gnuplotPipe, "set palette rgbformulae 10,13,22\n");
        fprintf(gnuplotPipe, "set colorbox\n");
        fprintf(gnuplotPipe, "set cbrange [0.2:1.2]\n"); // Replace min and max with your fixed values
        fprintf(gnuplotPipe, "set grid\n");
        fprintf(gnuplotPipe, "set bmargin 3\n"); // Increased bottom margin
        int minutos =  tiempo::get()->getValorTiempo() / 60; // Divide to get whole minutes
        int segundos =  tiempo::get()->getValorTiempo() % 60; // Remainder for the leftover seconds
        fprintf(gnuplotPipe, "set label 't = %d.%d min, evacuated: %d' at screen 0.5, 0.02 center\n", minutos, segundos, nodeDestino::totalPersonasEvacuadas);

        // plot creation
        std::string plotCommand = "plot";
        plotCommand += " '-' with lines lc 'black' notitle,";
        // check whether there are pedestrians evacuating
        bool peatonesEvacuado = false;
        for (const pedestrian& ped : pedestrians::get()->getDbPedestrianTotal()) {
            if (ped.estadoPedestrian == evacuando) {
                peatonesEvacuado = true;
                break;
            }
        } 
        if (peatonesEvacuado) {
            plotCommand += " '-' with points pt 7 palette notitle,";
        }
        // always add the evacuation points
        plotCommand += " '-' with points pt 12 ps 3.0 lc 'red' notitle";
        fprintf(gnuplotPipe, "%s\n", plotCommand.c_str());
        const auto& dbPedestrianTotal = pedestrians::get()->getDbPedestrianTotal();
        const auto& puntosEvacuacion = nodes::get()->getDbNodeEvacuation();
        const auto& lineasCalles = links::get()->getDbLinkTotal();
        // plotting of street lines
        for (const auto lc : lineasCalles) {
            // Get start and end points for the line
            const auto puntoInicial = lc->getNode1Ptr();
            const auto puntoFinal = lc->getNode2Ptr();
            fprintf(gnuplotPipe, "%lf %lf\n", puntoInicial->getCoordenada().getX(), puntoInicial->getCoordenada().getY());
            fprintf(gnuplotPipe, "%lf %lf\n", puntoFinal->getCoordenada().getX(), puntoFinal->getCoordenada().getY());
            fprintf(gnuplotPipe, "\n");  // Space between the lines
            // std::cout << puntoInicial->getCoordenada().getX() << " " << puntoInicial->getCoordenada().getY() << " ";
            // std::cout << puntoFinal->getCoordenada().getX() << " " << puntoFinal->getCoordenada().getY() << std::endl;
        }
        fprintf(gnuplotPipe, "e\n");
        // fprintf(gnuplotPipe, "e\n");
        // Iterate over the pedestrian vector using iterators
        if (peatonesEvacuado) {
            for (const pedestrian& ped : dbPedestrianTotal) {
                if(ped.estadoPedestrian == evacuando){
                    fprintf(gnuplotPipe, "%lf %lf %lf\n", ped.position.getX(), ped.position.getY(), ped.velocidadPedestrian.getMagnitud());
                    // std::cout <<  ped.position.getX() << " " <<  ped.position.getY()<< " "<<ped.velocidadPedestrian.getMagnitud() << std::endl;
                }
            }
            fprintf(gnuplotPipe, "e\n");
        }
        // Second series of points (example: initial position)
        for (const nodeDestino* const pe : puntosEvacuacion) {
            fprintf(gnuplotPipe, "%lf %lf\n", pe->getCoordenada().getX() , pe->getCoordenada().getY());
        }
        fprintf(gnuplotPipe, "e\n");
        pclose(gnuplotPipe);
    }
    if (tiempo::get()->getValorTiempo() == tiempo::get()->getEndTime() or nodeDestino::verificarEvacuacionTotal()) {
        const std::string directorio = file->getDirectory()->getFullPath();
        const std::string comando = "ffmpeg -y -framerate 10 -i "+ directorio + "Figure-%d.png -c:v libx264 -pix_fmt yuv420p " + directorio + "animation.mp4 > /dev/null 2>&1";
        int resultado = system(comando.c_str());
        const std::string deleteCommand = "rm -f " + directorio + "Figure-*.png";
        int deleteResult = system(deleteCommand.c_str());
    }

}

