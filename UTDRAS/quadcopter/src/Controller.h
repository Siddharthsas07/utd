#ifndef CONTROLLER_H
#define CONTROLLER_H

/* @file Controller.h
 * @auth Gary Steelman
 * @edit 10/26/2012
 */
 
/*
  @fun Controller
  @des Constructor for the controller class
   
  @fun ~Controller
  @des Destructor for the controller class
  
  @fun error
  @des Checks the controller's running state and performance for errors
  @ret TRUE if an error in the controller
  @ret FALSE if no error in the controller
  
  @fun start
  @des Sets the controller's running state to CONT_RUN
  
  @fun stop
  @des Sets the controller's running state to CONT_STOP
  
  @fun read_sensors
  @des Reads the sensor values and stores them
  @ret TRUE if the read was OK
  @ret FALSE if the read errored
  
  @fun update_current_state
  @des Updates the controller's current state based on the stored readings
  @note You should call read_sensors and ensure it returns true before calling 
      this function. Otherwise the current state will be out of date or corrupt.
      This function also estimates the 8 states which cannot be directly computed
  @ret TRUE if the update went OK
  @ret FALSE if the udpate errored
  
  @fun apply
  @des Makes the controller try to transition to the reference state
  @note This function BLOCKS until the transition is completed or until there
      is an error trying to transition
  @param const float** reference -- a pointer to an array of NUM_STATES floats whose values
      represent the desired future state of the copter
  @ret TRUE if the transition went OK
  @ret FALSE if the transition errored
  
  @fun calculate_current_state
  @des Calculates the states which can be directly calculated given sensor readings
  @ret TRUE if the calculation went OK
  @ret FALSE if the calculation errored
  
  @fun estimate_current_state
  @des Estimates the states which cannot be directly calculated given sensor readings
  @note Call calculate_current_state before calling this otherwise your estimations
      will be incorrect
  @ret TRUE if the estimation went OK
  @ret FALSE if the estimation errored
  
  @fun relative_error 
  @des Calculates the relative error between two states 
  @note Essentially calculates how close two states are and returns a numeric indicator
  @ret Number representing how close the two states are (ie returns .05 means 5% apart)
  
  @fun arrived
  @des Checks if the controller's current state has transitioned to the reference state
  @ret TRUE if the controller has transitioned to the reference state
  @ret FALSE if the controller has not transitioned to the reference state
  
  Note: In C/C++ all arrays (which I'm using for matrices) are row major. The
    design document lists the matrices in column major format, so we must convert
    appropriately.
 */

// Running state values
typedef enum {
  CONT_INIT,      // Initialization
  CONT_RUN,       // Running
  CONT_STOP,      // Stopped  
  CONT_KILL,      // Killed
  CONT_ERROR,     // Error in execution
  
  CONT_FINAL      // Used only for looping
} state_t;

// Matrix accessor constants for the states
enum {
  C_POS_X,
  C_VEL_X,
  C_POS_Y,
  C_VEL_Y,
  C_POS_Z,
  C_VEL_Z,
  C_ANG_YAW,
  C_SPD_YAW,
  C_ANG_PITCH,
  C_SPD_PITCH,
  C_ANG_ROLL,
  C_SPD_ROLL,
  C_SPD_MOT_1,
  C_SPD_MOT_2,
  C_SPD_MOT_3,
  C_SPD_MOT_4,
  C_CUR_MOT_1,
  C_CUR_MOT_2,
  C_CUR_MOT_3,
  C_CUR_MOT_4
};

enum {
  C_SENSOR_ACC_X,
  C_SENSOR_ACC_Y,
  C_SENSOR_ACC_Z,
  C_SENSOR_SPD_YAW,
  C_SENSOR_SPD_PITCH,
  C_SENSOR_SPD_ROLL
}

const unsigned int NUM_STATES = 20;
const unsigned int NUM_SENSORS_READINGS = 6;

class Controller {
public:
  Controller();
  ~Controller();
  
  bool error();
  
  void    start();
  void    stop();
  state_t get_state() const;
  
  bool read_sensors();

  bool update_current_state();
  
  bool apply( const float** reference );
  
private:
  bool  calculate_current_state();
  bool  estimate_current_state();
  
  float relative_error( const float** s1, const float** s2, float t );
  bool  arrived( const float** reference );

  // Min and max the copter's state must change to be considered safe for some delT
  const static float m_eps = 0.01; 
  const static float m_EPS = 5.0;
  
  // Difference between to state vectors to be considered "close enough" to equal
  static float m_equivalent = 0.05;
  
  // The running state of the copter
  state_t m_run_state;

  // Timestamps for sensor readings, calculations, and updates. Necessary for
  // derivation and integration later
  unsigned long m_ts_last_sensor_read;
  unsigned long m_ts_last_state_calc;
  unsigned long m_ts_last_state_est;
  
  // Stores readings from the sensors and time of last read
  // Refer to the Matrix identifier enum above for entry meanings
  float m_sensor_readings[NUM_SENSOR_READINGS] = { 
    0,
    0, 
    0, 
    0, 
    0, 
    0  
  };

  // The current state of the copter
  // Refer to the Matrix identifier enum above for entry meanings
  static float m_current_state[20][1] = {
    { 0 },
    { 0 },
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
  
  // The previous state of the copter
  // Refer to the Matrix identifier enum above for entry meanings
  static float m_previous_state[20][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
  
  // The desired future state of the copter
  // Refer to the Matrix identifier enum above for entry meanings
  static float m_reference[20][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };

  // The matrices used for estimation and state transition computations
  static float m_U_internal[4][1] = {
    { 0 }, // V_1
    { 0 }, // V_2
    { 0 }, // V_3
    { 0 }  // V_4
  }
  
  static float m_U_external[4][1] = {
    { 0 }, // V_1
    { 0 }, // V_2
    { 0 }, // V_3
    { 0 }  // V_4
  }
  
  const static float m_K[4][20] = {
    { -1.60092380876712e000, -347.178413961401e-003, 556.742661449709e-003, 139.280368457826e-003, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };
  
  const static float m_A11[12][12] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  
  const static float m_A12[12][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  
  const static float m_A22[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  
  const static float m_B2[8][4] = {
    {       0,       0,       0, 0 }      ,
    {       0,       0,       0, 0 }      ,
    {       0,       0,       0, 0 }      ,
    {       0,       0,       0, 0 }      ,
    { 100e003,       0,       0, 0 }      ,
    {       0, 100e003,       0, 0 }      ,
    {       0,       0, 100e003, 0 }      ,
    {       0,       0,       0, 100e003 },
  };
  
  const static float m_L[8][12] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  
  // Refer to the Matrix identifier enum above for entry meanings
  // Lower 8 entries
  static float m_X_est[8][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
  
  const static float m_X_hover = { 
    { 0 }, // w_1_hover
    { 0 }, // w_2_hover
    { 0 }, // w_3_hover
    { 0 }, // w_4_hover
    { 0 }, // I_1_hover
    { 0 }, // I_2_hover
    { 0 }, // I_3_hover
    { 0 }  // I_4_hover
  };
  
  // Refer to the Matrix identifier enum above for entry meanings
  // Upper 12 etnries
  static float m_Y[12][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
  
  // Derivatives of Y
  static float m_Ydot[12][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
  
  const static float m_G = 0;
  
  // I do believe this is the lower half of m_X_hover
  const static float m_C[4][1] = {
    { 0 }, 
    { 0 }, 
    { 0 }, 
    { 0 }
  };
}

#endif