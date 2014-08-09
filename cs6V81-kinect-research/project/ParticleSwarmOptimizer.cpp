#include "ParticleSwarmOptimizer.h"
#include "RNG.h"
#include <cmath> 

Particle::Particle() {
  best_error = FLT_MAX;
  curr_error = FLT_MAX;
}

Particle::Particle( unsigned int num_params ) {
  best_error = FLT_MAX;
  curr_error = FLT_MAX;

  curr_sln.resize( num_params );
  best_sln.resize( num_params );
  velocity.resize( num_params );
}

Particle::~Particle() {}

ParticleSwarmOptimizer::ParticleSwarmOptimizer() {
  kinetic_penalty_normalization = 0.0f;
  skin_difference_normalization = 0.0f;
  max_OK_depth_difference       = 0.0f;
  min_OK_depth_difference       = 0.0f; 
  epsilon                       = 0.0f;
  num_particles                 = 0; 
  num_generations               = 0; 
  cognitive_weight              = 0.0f; 
  social_weight                 = 0.0f; 
  cognitive_random_factor       = 0.0f;
  social_random_factor          = 0.0f;
  constriction_factor           = 0.0f;
  min_error                     = 0.0f;
  num_generations_to_perturb    = 0;
  generation_best_particle      = 0;
  generation_min_error          = 0.0f;
}

ParticleSwarmOptimizer::~ParticleSwarmOptimizer() {}

HRESULT ParticleSwarmOptimizer::run() {
  // 1 - initialize member values
  kinetic_penalty_normalization = 10.0f;
  skin_difference_normalization = 20.0f;
  max_OK_depth_difference       = 4.0f;
  min_OK_depth_difference       = 1.0f; 
  epsilon                       = 1e-6f;
  num_particles                 = 64; 
  num_generations               = 20; 

  cognitive_weight              = 2.8f; 
  social_weight                 = 1.3f; 
  RNG<float> rng;
  cognitive_random_factor       = rng.randRange(0.0f, 1.0f);
  social_random_factor          = rng.randRange(0.0f, 1.0f);
  float psi                     = cognitive_weight + social_weight;
  constriction_factor           = 2.0f / static_cast<float>(fabs(2 - psi - sqrt((psi*psi) - (4*psi))));

  // 2 - initialize particles
  //   first particle curr_sln = previous generation model's ansers
  //   everyone else = perturbations of previous generation's model
  particles.resize( num_particles );
  particles[0].curr_sln = initial_sln.get_params_without_bounds();
  for ( unsigned int i = 1; i < particles.size(); i++ ) {
    particles[i].curr_sln = particles[0].curr_sln;
    perturb_particle( particles[i] );
  }

  best_sln = initial_sln.get_params_without_bounds();

  // 3 - initialize the particle ID list for perturbation generation later
  num_generations_to_perturb = 3;
  for ( unsigned int i = 0; i < num_particles; i++ ) {
    particles_to_perturb.push_back( i );
  }

  generation_best_particle = 0;
  generation_min_error     = 0.0f;
  min_error                = 0.0f;

  // 4 - set up the matched depths map to work with
  matched_depths.set_size( observed_depth.width, observed_depth.height );
  
  // 2 - execute
  // for each generation
  //   for each particle
  //     update velocty 
  //     update position
  //     calculate error for this gen's solution
  //       render model using new solution
  //       calculate error between new render and o_d and o_s
  //     update best_sln for particle 
  //       if error for this sln < best particle sln so far
  //       store this sln 
  //     update best_sln for all particles
  //       find particle with min error this generation
  //       if that particle's error < global min error so far...
  //   if this gen % gens to perturb == 0
  //     randomly choose half of the particles
  //     for each randomly chosen particle 
  //       randomly choose a finger joint
  //       set value for that parameter to values from valid range for that joint
  //     rof
  //   fi
  // rof
  // return solution stored in best_sln

  // Each generation
  for ( unsigned int i = 0; i < num_generations; i++ ) {
    generation_best_particle = 0;
    generation_min_error = FLT_MAX;

    // Update each particle
    for ( unsigned int curr_particle = 0; curr_particle < num_particles; curr_particle++ ) {
      // Update particle velocity
      if ( FAILED(update_velocity( particles[curr_particle] ) ) ) {
        continue;
      }

      // Update particle position
      if ( FAILED(update_position( particles[curr_particle] ) ) ) {
        continue;
      }

      // Update particle error -- this renders the particle's 
      // parameters and compares the depth maps
      if ( FAILED( update_particle_error( particles[curr_particle] ) ) ) {
        continue; 
      }

      // Update this generation's best particle
      if ( particles[curr_particle].curr_error < generation_min_error ) {
        generation_best_particle = curr_particle;
      }
    }

    // update global best if pointer from this generation's updates is
    // better on the errors
    if ( FAILED( update_global_error() ) ) {
      continue;
    }

    // Perturb some random particles to prevent swarm collapse
    if ( num_generations_to_perturb % i == 0 ) {
      if ( FAILED( pick_particles_to_perturb() ) ) {
        continue;
      }

      for ( unsigned int curr_particle = 0; curr_particle < (num_particles / 2); curr_particle++ ) {
        perturb_particle( particles[curr_particle] );
      }
    }
  }

  // The global best solution after all iterations is now stored in 
  // best_sln

  return S_OK;
}

// equation (3) from the paper
HRESULT ParticleSwarmOptimizer::update_velocity( Particle& p ) {
  HRESULT hr = S_OK;

  vector<float> cog_comp; // individual cognitive component
  vector<float> soc_comp; // social component
  
  cog_comp = vector_multiply(constriction_factor,
                             vector_addition(p.velocity,
                                             vector_multiply(cognitive_weight*cognitive_random_factor,
                                                             vector_difference(p.best_sln, 
                                                                               p.curr_sln
                                                                              ))));
  soc_comp = vector_multiply(social_weight*social_random_factor,
                             vector_difference(best_sln,
                                               p.curr_sln
                                              ));

  p.velocity = vector_addition(cog_comp, soc_comp);

  return hr;
}

// equation (4) from the paper
HRESULT ParticleSwarmOptimizer::update_position( Particle& p ) {
  HRESULT hr = S_OK;

  p.curr_sln = vector_addition(p.curr_sln, p.velocity);

  return hr;
}

// update the error for a particle based on the stored data
HRESULT ParticleSwarmOptimizer::update_particle_error( Particle& p ) {
  HRESULT hr = S_OK;

  HandModel hand( p.curr_sln );
  // render the model stored by the particle.
  hr = renderer.render( hand );
  if ( FAILED(hr) ) {
    return hr;
  }

  generate_matched_depths();

  p.curr_error = calculate_error_term(); 

  return hr;
}

// check the maps stored in renderer vs the passed in observations
// store matched into matched_depths
HRESULT ParticleSwarmOptimizer::generate_matched_depths() {
  HRESULT hr = S_OK;

  matched_depths.zero();
  char difference = 0;

  // Loop over the observed depth
  for ( unsigned int i = 0; i < observed_depth.height; i++ ) {
    for ( unsigned int j = 0; j < observed_depth.width; j++ ) {
      // Compare it to rendered depth and set matched depths to 1
      // if the difference is < d_m or 0 otherwise
      difference = abs(observed_depth.get(i, j) - renderer.depth_map.get(i, j));
      matched_depths.set( i, j, (difference < min_OK_depth_difference ? 1 : 0) );
    }
  }

  return hr;
}

// equation (1) from paper
float ParticleSwarmOptimizer::calculate_error_term() {
  return calculate_distance_term() + calculate_kinetic_penalty_term();
}

// equation (2) from paper
float ParticleSwarmOptimizer::calculate_distance_term() {
  
}

float ParticleSwarmOptimizer::calculate_kinetic_penalty_term() {
  return 0.0f;
}

// update the error for the global based on particle pointed to
// by generation_best_particle
HRESULT ParticleSwarmOptimizer::update_global_error() {
  HRESULT hr = S_OK;

  if ( generation_min_error < min_error ) {
    best_sln = particles[generation_best_particle].best_sln;
  }

  return hr;
}

HRESULT ParticleSwarmOptimizer::pick_particles_to_perturb() {
  HRESULT hr = S_OK;

  // http://stackoverflow.com/questions/9650991/pick-a-unique-random-subset-from-a-set-of-unique-values

  return hr;
}

HRESULT ParticleSwarmOptimizer::perturb_particle( Particle& p ) {
  HRESULT hr = S_OK;

  // pick a random parameter of p and change it to a random value in the range
  // bounded by the initial_sln;

  return hr;
}

vector<float> ParticleSwarmOptimizer::vector_difference( const vector<float>& v1, const vector<float>& v2 ) {
  vector<float> res( v1.size(), 0 );
  for ( unsigned int i = 0; i < res.size(); i++ ) {
    res[i] = v1[i] - v2[i];
  }
  return res;
}

vector<float> ParticleSwarmOptimizer::vector_addition( const vector<float>& v1, const vector<float>& v2 ) {
  vector<float> res( v1.size(), 0 );
  for ( unsigned int i = 0; i < res.size(); i++ ) {
    res[i] = v1[i] + v2[i];
  }
  return res;
}

vector<float> ParticleSwarmOptimizer::vector_multiply( float c, const vector<float>& v ) {
  vector<float> res( v.size(), 0 );
  for ( unsigned int i = 0; i < res.size(); i++ ) {
    res[i] = c*v[i];
  }
  return res;
}