/**
 * Spudpole.h 20 July 2020 <preeve@pdjr.eu>
 *
 * Class modelling an Ankreo spud pole in an NMEA 2000 context.  The class
 * provides methods which support interfacing to spudpole hardware and methods
 * which support interfacing to an NMEA 2000 client.
 *
 * A Spudpole instance is essentially a state machine representing associated
 * hardware states as:
 * 
 * UNKNOWN      The state of the hardware is unknown. This is generally a
 *              temporary and transient condition which will be updated as soon
 *              as an input is received from the hardware interface.
 * DOCKED       The spudpole is docked. This state is forced when a call is made
 *              to setDocked() - typically this will be triggered when the
 *              external pole sensor is activated by a retrieved pole. 
 * DEPLOYING    The spudpole is being deployed. This state is forced when a
 *              call is made to the deploy() function.
 * RETRIEVING   The spudpole is being retrieved. This state is forced when a
 *              call is made to the retrieve() function. The transition to
 *              RETRIEVING cannot be made from DOCKED. 
 * STOPPED
 * created in the usual C++ way by simply declaring a
 * variable of the Spudpole type.
 *
 * Spudpole mySpudpole;
 * 
 * Before use the new instance must be initialised by calling the initialise()
 * method and supplying some critical configuration data.
 *
 * mySpudpole.initialise(instance, configuration, action);
 *
 * The <instance> value should be an integer in the range 0..252 which gives
 * the unique N2K identifier for <mySpudpole>.
 *
 * The <configuration> value must be a structure of type <SpudpoleConfiguration>
 * whose properties define the unique characteristics of <mySpudpole>, including
 * values which characterise associated hardware.
 *
 * Finally, the <action> value should be a pointer to a callback function with
 * the signature void action(int doThis) which will be called by a 
 *POSITION represents the length of anchor rode deployed from the windlass
 * drum in metres. The setDocked() function sets position to 0.0m which
 * indicates that the spudpole is fully retrieved and docked and 
*/

#include <N2kTypes.h>

#define STOP 0
#define RETRIEVE 1
#define DEPLOY 2

enum SpudpoleState { Unknown=0, Docked=1, Deploying=2, Retrieving=3, Stopped=4 };


struct SpudpoleConfiguration {
  int PRODUCT_CODE;
  const char* MODEL_CODE;
  const char* SERIAL_CODE;
  float SPOOL_DIAMETER;                 // In metres, e.g. 0.05
  float RODE_DIAMETER;                  // In metres, e.g. 0.008
  int RODE_TURNS_PER_LAYER;             // On the windlass drum, e.g. 10
  double RODE_TURNS_WHEN_DOCKED;        // In revolutions, e.g. 51
  unsigned char CONTROLLER_VOLTAGE;     // Nominal in volts, e.g. 24
  unsigned char MOTOR_CURRENT;          // Maximum in amps, e.g. 85
};
typedef struct SpudpoleConfiguration SpudpoleConfiguration;

class Spudpole {
  public:
    // Instantiation and initialisation
    Spudpole();
    void initialise(int instance, struct SpudpoleConfiguration, void (*action)(int));
    // Get methods
    unsigned char getInstance();
    SpudpoleConfiguration getConfiguration();
    SpudpoleState getState();
    unsigned int getCounter();
    // Methods updating state
    void incrCounter();
    void decrCounter();
    void bumpCounter();
    void setDocked();
    void setStopped();
    void deploy();
    void retrieve();
    void stop();
    
    void initialiseMotorTiming(unsigned long totalMotorTime, unsigned long (*timerFunction)(int));
    unsigned long getTotalMotorTime();
    // Convenience methods
    int isDocked();
    int isStopped();
    int isWorking();
    double rodeLengthDeployed();
    //************************************************************************
    // N2K interface methods
    tN2kDD477 getWindlassMonitoringEvents();
    tN2kDD480 getWindlassMotionStatus();
    tN2kDD481 getRodeTypeStatus();
    tN2kDD482 getAnchorDockingStatus();
    tN2kDD483 getWindlassOperatingEvents();
    tN2kDD484 getWindlassDirectionControl();
    double getRodeCounterValue();
    double getWindlassLineSpeed();
    unsigned char getControllerVoltage();
    unsigned char getMotorCurrent();
  private:
    struct SpudpoleConfiguration config;
    unsigned char instance; 
    SpudpoleState state;
    unsigned int counter;
    unsigned long totalMotorTime;
    void (*action)(int);
    unsigned long (*timerFunction)(int mode);
    double rodeLengthFromCounter(int counter);
    double rodeLengthOnLayer(int layer, int turnsOnLayer);
};
