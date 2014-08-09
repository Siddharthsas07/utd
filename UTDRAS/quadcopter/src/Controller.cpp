#include "Controller.h"
#include "MatrixMath.h"
#include <Arduino.h>

Controller::Controller() :
  m_sensor_last_read(0),
  m_current_last_update(0)
{
  // Nothing on constsruction
}

Controller::~Controller() {
  // Nothing on destsruction
}

void Controller::start() {
  m_state = CONT_RUN;
}

void Controller::stop() {
  m_state = CONT_STOP;
}

state_t Controller::get_run_state() const {
  return m_run_state;
} 

void Controller::set_run_state( state_t new_state ) {
  m_run_state = new_state;
}

bool Controller::error() {
  // When checking for error, always assume there IS an error and prove otherwise. 
  bool error = true;
  
  float rel_err = relative_error( m_current_state, m_previous_state );
  
  if ( rel_err <= m_EPS && rel_err >= m_eps ) {
    error = false;
  }
  
  return error;
}

bool Controller::read_sensors() {
  bool error = true;
  
  // Get the current time
  m_ts_last_sensor_read = 0;
  
  // Read sensor values and store into m_sensor_readings
  m_sensor_readings[0] = 0;
  m_sensor_readings[1] = 0;
  m_sensor_readings[2] = 0;
  m_sensor_readings[3] = 0;
  m_sensor_readings[4] = 0;
  m_sensor_readings[5] = 0;
  
  // if the reading is successful, flip error to false and return  
  error = false;
  return error;
}

bool Controller::update_current_state() {
  // Yes this is formatted in a weird way, but will allow us to expand it later
  // to log errors

  bool error = true;
  
  // Copy the current state to the previous state for mathing later
  //MatrixMath::MatrixCopy
  
  // Fill upper 12 entries of new current state
  error = calculate_current_state();
  
  if ( error ) {
    return true;
  }
  
  // Fill loer 8 entries of new current state
  error = estimate_current_state();
  
  if ( error ) {
    return true;
  }
  
  return error;
}

bool Controller::apply( const float* reference ) {
  // Store the reference so we can... 8D refer to it later
  memcpy( m_reference, reference, NUM_STATES*sizeof(float) )
  
  // difference between current state and reference (amount of change
  // that needs to happen yet!)
  float error[NUM_STATES][1];
  
  // Keep sending control signal changes until we reach the reference state
  while ( true ) {
  
    // Check if arrived at reference state
    if ( arrived( m_reference ) ) {
      return true;
    }
    
    // Or if there's an error
    else if ( error() ) {
      return false;
    }
    
    // Calculate how far off the controller is from the reference state
    //MatrixMath::MatrixSub error = m_reference - m_current_state;
    
    // Calculate the control signal needed to move toward reference state
    // m_U_internal = ((K * error) * G) + C;
    
    // TODO send signals in m_U_internal to the four motors
    
    // Check the sensors and update the current state
    read_sensors();
    update_current_state();
    
    // And loop again to see if the new state says arrived at the reference state
  }
}

bool Controller::calculate_current_state() {
  bool error = true;
  
  m_current_state[C_VEL_X] = integrate( m_sensor_readings[C_SENSOR_ACC_X], 
  m_current_state[C_POS_X] = integrate( integrate
  
  // Do some integration over some delT for each of the elements in
  // m_current_state which can be derived from the sensor readings
  
  error = false;
  return error;
}

bool Controller::estimate_current_state() {
  bool error = true;
  
  // Do some estimation for each of the elements in m_current_state 
  // that we can't directly compute
  
  error = false;
  return error;
}

float Controller::relative_error( const Matrix& s1, const Matrix& s1, unsigned long t ) {
  float rel_error = 0.0f;
  // Check the percent differences between s1 and s2 over some time t and return the 
  // value
  
  return rel_error;
}

bool Controller::arrived( const Matrix& reference ) {
  if ( relative_error( reference, m_current_state, 0 ) < m_CLOSE_ENOUGH ) {
    return true;
  }
  
  return false;
}
