// SpudPole.cpp

#include "spudpole.h"
#include "Arduino.h"
#include <limits.h>
#include <cstddef>

Spudpole::Spudpole() {
  this->instance = 0;
  this->action = 0;
  this->state = Unknown;
  this->counter = 0;
  this->totalMotorTime = 0L;
  this->timerFunction = 0;
}

void Spudpole::initialise(int instance, SpudpoleConfiguration config, void (*action)(int)) {
  this->instance = instance;
  this->config = config;
  this->action = action;
}

unsigned char Spudpole::getInstance() {
  return(this->instance);
}

SpudpoleConfiguration Spudpole::getConfiguration() {
  return(this->config);
}

SpudpoleState Spudpole::getState() {
  return(this->state);
}

unsigned int Spudpole::getCounter() {
  return(this->counter);
}

void Spudpole::initialiseMotorTiming(unsigned long totalMotorTime, unsigned long (*timerFunction)(int)) {
  this->totalMotorTime = totalMotorTime;
  this->timerFunction = timerFunction;
}

unsigned long Spudpole::getTotalMotorTime() {
  return(this->totalMotorTime);
}

void Spudpole::incrCounter() {
  if ((this->state == Deploying) || (this->state == Retrieving) || (this->state == Stopped)) this->counter++;
}

void Spudpole::decrCounter() {
  if (((this->state == Deploying) || (this->state == Retrieving) || (this->state == Stopped)) && (this->counter > 0)) this->counter--;
}

void Spudpole::bumpCounter() {
  switch (this->state) {
    case Unknown: break;
    case Docked: break;
    case Deploying: this->incrCounter(); break;
    case Retrieving: this->decrCounter(); break;
    case Stopped: break;
  }
}

void Spudpole::setDocked() {
  this->state = Docked;
  this->counter = 0;
}

void Spudpole::setStopped() {
  this->state = Stopped;
  if (this->timerFunction) this->totalMotorTime = this->timerFunction(2);
}

void Spudpole::deploy() {
  this->state = Deploying;
  this->action(DEPLOY);
  if (this->timerFunction) this->timerFunction(0);
}

void Spudpole::retrieve() {
  this->state = Retrieving;
  this->action(RETRIEVE);
  if (this->timerFunction) this->timerFunction(0);
}

void Spudpole::stop() {
  this->state = Stopped;
  this->action(STOP);
}

int Spudpole::isDocked() {
  return(this->state == Docked);
}

int Spudpole::isStopped() {
  return(this->state == Stopped);
}

int Spudpole::isWorking() {
  return((this->state == Deploying) || (this->state == Retrieving));
}

double Spudpole::rodeLengthDeployed() {
  double retval = -1.0;
  if ((this->state != Unknown) && (this->counter >= 0)) {
    retval = this->rodeLengthFromCounter(this->config.RODE_TURNS_WHEN_DOCKED) - this->rodeLengthFromCounter(this->config.RODE_TURNS_WHEN_DOCKED - this->counter);
  }
  return(retval);
}

//*****************************************************************************
// N2K interface methods...
//*****************************************************************************

tN2kDD477 Spudpole::getWindlassMonitoringEvents() {
  tN2kDD477 retval = N2kDD477_NoErrorsPresent;
  return(retval);
}

tN2kDD480 Spudpole::getWindlassMotionStatus() {
  tN2kDD480 retval = N2kDD480_Unavailable;
  switch (this->state) {
    case Unknown: retval = N2kDD480_Unavailable; break;
    case Docked: retval = N2kDD480_WindlassStopped; break;
    case Deploying: retval = N2kDD480_DeploymentOccurring; break;
    case Retrieving: retval = N2kDD480_RetrievalOccurring; break;
    case Stopped: retval = N2kDD480_WindlassStopped; break;
  }
  return(retval);
}

tN2kDD481 Spudpole::getRodeTypeStatus() {
  tN2kDD481 retval = N2kDD481_RopePresentlyDetected;
  return(retval);
}

tN2kDD482 Spudpole::getAnchorDockingStatus() {
  tN2kDD482 retval = N2kDD482_DataNotAvailable;
  switch (this->state) {
    case Unknown: retval = N2kDD482_DataNotAvailable; break;
    case Docked: retval = N2kDD482_FullyDocked; break;
    case Deploying: retval = N2kDD482_NotDocked; break;
    case Retrieving: retval = N2kDD482_NotDocked; break;
    case Stopped: retval = N2kDD482_NotDocked; break;
  }
  return(retval);
}

tN2kDD483 Spudpole::getWindlassOperatingEvents() {
  tN2kDD483 retval = N2kDD483_NoErrorsOrEventsPresent;
  return(retval);
}

tN2kDD484 Spudpole::getWindlassDirectionControl() {
  tN2kDD484 retval = N2kDD484_Off;
  switch (this->state) {
    case Unknown: retval = N2kDD484_Off; break;
    case Docked: retval = N2kDD484_Off; break;
    case Deploying: retval = N2kDD484_Down; break;
    case Retrieving: retval = N2kDD484_Up; break;
    case Stopped: retval = N2kDD484_Off; break;
  }
  return(retval);
}

double Spudpole::getRodeCounterValue() {
  return(this->rodeLengthDeployed());
}

double Spudpole::getWindlassLineSpeed() {
  return(1.0);
}

unsigned char Spudpole::getControllerVoltage() {
  return(this->config.CONTROLLER_VOLTAGE);
}

unsigned char Spudpole::getMotorCurrent() {
  return(this->config.MOTOR_CURRENT);
}

//*****************************************************************************
// Private methods
//*****************************************************************************


double Spudpole::rodeLengthFromCounter(int counter) {
  double retval = 0.0;
  double rlol;
  int layersUsed = (counter / this->config.RODE_TURNS_PER_LAYER);
  for (int layer = 0; layer <= layersUsed; layer++) {
    int turnsOnLayer = (layer < layersUsed)?this->config.RODE_TURNS_PER_LAYER:(counter % this->config.RODE_TURNS_PER_LAYER); 
    rlol = this->rodeLengthOnLayer(layer, turnsOnLayer);
    retval += rlol;
  }
  return(retval);
}

double Spudpole::rodeLengthOnLayer(int layer, int turnsOnLayer) {
  double retval = turnsOnLayer * (3.1416 * (this->config.SPOOL_DIAMETER + this->config.RODE_DIAMETER + (layer * 2 * this->config.RODE_DIAMETER)));
  return(retval);
}

