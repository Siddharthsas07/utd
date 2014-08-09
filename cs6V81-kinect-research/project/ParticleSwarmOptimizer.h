#pragma once

#include "useful_things.h"
#include "HandModel.h"
#include "HandModelRenderer.h"
#include "ImageMap.h"
#include <Windows.h>
#include <vector>
using std::vector;

class Particle {
  public:
    Particle();
    Particle( unsigned int num_params );
    ~Particle();

    float best_error;        // e
    float curr_error;        // error on this generation's estimate
    vector<float> curr_sln;  // x_k
    vector<float> best_sln;  // P_k   
    vector<float> velocity;  // current velocity
};

class ParticleSwarmOptimizer {
  private:
    float kinetic_penalty_normalization; // lambda_k
    float skin_difference_normalization; // lambda
    float max_OK_depth_difference;       // d_M
    float min_OK_depth_difference;       // d_m
    float epsilon;                       // eps
    unsigned int num_particles;          // N
    unsigned int num_generations;        // k
    
    float cognitive_weight;              // c1
    float social_weight;                 // c2
    float cognitive_random_factor;       // r1
    float social_random_factor;          // r2
    float constriction_factor;           // w

    vector<Particle>     particles;      // P
    vector<float>        best_sln;       // G_k 

    unsigned int         num_generations_to_perturb; // i_r
    vector<unsigned int> particles_to_perturb;
    unsigned int         generation_best_particle;
    float                generation_min_error;
    float                min_error;

    HandModelRenderer   renderer;
    ImageMap<char>      matched_depths;

    HRESULT update_velocity( Particle& p );
    HRESULT update_position( Particle& p );
    HRESULT generate_matched_depths();

    HRESULT update_particle_error( Particle& p );
    float calculate_error_term();           // E()
    float calculate_distance_term(); // D()
    float calculate_kinetic_penalty_term(); // kc()

    HRESULT update_global_error();
    HRESULT pick_particles_to_perturb();
    HRESULT perturb_particle( Particle& p );

    vector<float> vector_difference( const vector<float>& v1, const vector<float>& v2 );
    vector<float> vector_addition( const vector<float>& v1, const vector<float>& v2 );
    vector<float> vector_multiply( float c, const vector<float>& v );

  public:
    ParticleSwarmOptimizer();
    ~ParticleSwarmOptimizer();

    HRESULT run();

    HandModel initial_sln;    // solution from previous frame
    ImageMap<unsigned __int16>  observed_depth; // o_d
    ImageMap<unsigned __int32>  observed_color; // o_s    
};