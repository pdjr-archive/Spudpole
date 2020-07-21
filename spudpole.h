/**
 * Spudpole.h 20 July 2020 <preeve@pdjr.eu>
 *
 * Abstract data type modelling spudpoles from the manufacturer Ankreo.
 */
 
enum SpudpoleControl { STOP=0, DEPLOY=1, RETRIEVE=2 };
enum SpudpoleTimer { STOP=0, START=1 };
enum SpudpoleState { UNKNOWN=0, DOCKED=1, DEPLOYING=2, RETRIEVING=3, STOPPED=4 };

class Spudpole {
  public:
    // Instantiation
    Spudpole(char* manufacturerName, char* modelCode, char* serialCode);
    // Configuration
    void setControlCallback(void (*controlCallback)(SpudpoleControl));
    void configureLineMeasurement(double spoolDiameter, double lineDiameter, unsigned int spoolWidth, unsigned int workingCapacity);
    void configureRuntimeAccounting(unsigned long runtimeStartTotal, unsigned long (*timerCallback)(SpudpoleTimer, unsigned long));
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
    char manufacturerName[20];
    char modelCode[20];
    char serialCode[20];
    SpudpoleState state;
    unsigned int counter;
    void (*controlCallback)(SpudpoleControl);
    double spoolDiameter;
    double lineDiameter;
    unsigned int spoolWidth;
    unsigned int lineTurnsWhenDocked;
    unsigned long (*timerCallback)(SpudpoleTimer, unsigned long);
    unsigned long motorRunTime;
    double lineLengthFromCounter(int counter);
    double lineLengthOnLayer(int layer, int turnsOnLayer);
};
