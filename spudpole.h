/**
 * Spudpole.h 20 July 2020 <preeve@pdjr.eu>
 *
 * Abstract data type modelling spudpoles from the manufacturer Ankreo.
 */
 
#define SPUDPOLE_STOP 0
#define SPUDPOLE_DEPLOY 1
#define SPUDPOLE_RETRIEVE 2

enum SpudpoleState { Unknown=0, Docked=1, Deploying=2, Retrieving=3, Stopped=4 };

class Spudpole {
  public:
    // Instantiation
    Spudpole(char* manufacturerName, char* modelCode, char* serialCode);
    // Configuration
    void setControlCallback(void (*controlCallback)(int action);
    void configureLineMeasurement(double spoolDiameter, double lineDiameter, spoolWidth, operatingCapacity);
    void configureRuntimeAccounting(unsigned long runtimeStartTotal, timer);
    // Primitives
    char* getManufacturerName();
    char* getModelCode();
    char* getSerialCode();
    void setDocked();
    void deploy();
    void retrieve();
    void stop();
    void setStopped();
    SpudpoleState getState();
    bool isWorking();
    bool isDocked();
    bool isDeployed();
    unsigned int getCounter();
    unsigned int counterIncr();
    unsigned int counterDecr();
    unsigned int counterBump();
    // If line measurement is configured...
    double getDeployedLineLength();
    // If run time accounting is configured...
    unsigned long getMotorRunTime();
  private:
    SpudpoleState state;
    unsigned int counter;
    unsigned long motorRunTime;
    void (*controlCallback)(int);
    unsigned long (*timer)(int mode);
    double lineLengthFromCounter(int counter);
    double lineLengthOnLayer(int layer, int turnsOnLayer);
};
