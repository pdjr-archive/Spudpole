/**
 * Spudpole.cpp 20 July 2020 <preeve@pdjr.eu>
 *
 * Abstract data type modelling spudpoles from the manufacturer Ankreo.
 */

#include <cstddef>
#include "spudpole.h"

Spudpole::Spudpole(char* manufacturerName, char* modelCode, char* serialCode) {
  strcpy(this->manufacturerName, manufacturerName);
  strcpy(this->modelCode, modelCode);
  strcpy(this->serialCode, serialCode);
  this->state = UNKNOWN;
  this->counter = 0;
  this->controlCallback = 0;
  this->spoolDiameter = 0.0;
  this->lineDiameter = 0.0;
  this->spoolWidth = 0;
  this->lineTurnsWhenDocked = 0;
  this->motorRunTime = 0L;
  this->timerCallback = 0;
}

void Spudpole::setControlCallback(void (*controlCallback)(int)) {
  this->controlCallback = controlCallback;
}

void Spudpole::configureLineMeasurement(double spoolDiameter, double lineDiameter, unsigned int spoolWidth, unsigned int lineTurnsWhenDocked) {
  this->spoolDiameter = spoolDiameter;
  this->lineDiameter = lineDiameter;
  this->spoolWidth = spoolWidth;
  this->lineTurnsWhenDocked = lineTurnsWhenDocked;
}

void spudpole::configureRunTimeAccounting(unsigned long motorRunTime, unsigned long (*timerCallback)(SpudpoleTimer, unsigned long)) {
  this->motorRunTime = motorRunTime;
  this->timerCallback = timerCallback;
}

void Spudpole::setDocked() {
  this->state = DOCKED;
  this->counter = 0;
}

void Spudpole::setStopped() {
  this->state = STOPPED;
  if (this->timerCallback) this->motorRunTime = this->timerCallback(STOP, this->motorRunTime);
}

void Spudpole::deploy() {
  this->state = DEPLOYING;
  f (this->controlCallback) this->controlCallback(DEPLOY);
  if (this->timerCallback) this->timerCallback(START, this->motorRunTime);
}

void Spudpole::retrieve() {
  this->state = Retrieving;
  if (this->controlCallback) this->controlCallback(RETRIEVE);
  if (this->timerCallback) this->timerCallback(START, this->motorRunTime);
}

void Spudpole::stop() {
  this->setStopped();
}

SpudpoleState Spudpole::getState() {
  return(this->state);
}

bool Spudpole::isWorking() {
  return((this->state == DEPLOYING) || (this->state == RETRIEVING));
}

bool Spudpole::isDocked() {
  return(this->state == DOCKED);
}

int Spudpole::isDeployed() {
  return(this->state == STOPPED);
}

unsigned int Spudpole::getCounter() {
  return(this->counter);
}

unsigned int Spudpole::incrCounter() {
  if (this->state == DEPLOYING) this->counter++;
  return(this->counter);
}

unsigned int Spudpole::decrCounter() {
  if (this->state == RETRIEVING) && (this->counter > 0)) this->counter--;
  return(this->counter);
}

unsigned int Spudpole::bumpCounter() {
  switch (this->state) {
    case Unknown: break;
    case Docked: break;
    case Deploying: this->incrCounter(); break;
    case Retrieving: this->decrCounter(); break;
    case Stopped: break;
  }
  return(this->counter);
}

double Spudpole::getDeployedLineLength() {
  double retval = -1.0;
  if ((this->state != UNKNOWN) && (this->spoolDiameter > 0.0)) {
    retval = 0.0;
    if (this->counter >= 0) {
        retval = this->lineLengthFromCounter(this->lineTurnsWhenDocked) - this->lineLengthFromCounter(this->config.lineTurnsWhenDocked - this->counter);
    }
  }
  return(retval);
}

unsigned long Spudpole::getMotorRunTime() {
  return((this->timerCallback)?this->motorRunTime:0L);
}

//*****************************************************************************
// Private methods
//*****************************************************************************

double Spudpole::lineLengthFromCounter(int counter) {
  double retval = 0.0;
  double rlol;
  int layersUsed = (counter / this->spoolWidth);
  for (int layer = 0; layer <= layersUsed; layer++) {
    int turnsOnLayer = (layer < layersUsed)?this->spoolWidth:(counter % this->spoolWidth); 
    rlol = this->rodeLengthOnLayer(layer, turnsOnLayer);
    retval += rlol;
  }
  return(retval);
}

double Spudpole::lineLengthOnLayer(int layer, int turnsOnLayer) {
  double retval = turnsOnLayer * (3.1416 * (this->spoolDiameter + this->lineDiameter + (layer * 2 * this->lineDiameter)));
  return(retval);
}

