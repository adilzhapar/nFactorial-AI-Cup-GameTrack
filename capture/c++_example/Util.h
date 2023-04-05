#ifndef __CHALLENGE_UTIL_H__
#define __CHALLENGE_UTIL_H__

// Supporting functions for a simple player.  Feel free to use this
// code or write better code for yourself.
//
// ICPC Challenge
// Sturgill, Baylor University

#include <cmath>

/** Width and height of the world in game units. */
const double FIELD_SIZE = 800;
  
/** Maximum length of the trail behind a sled */
const double TRAIL_LIMIT = 600;
  
/** Maximum velocity for a bumper. */
const double BUMPER_SPEED_LIMIT = 24.0;
  
/** Maximum acceleration for a bumper. */
const double BUMPER_FORCE_LIMIT = 8.0;
  
/** Maximum turn angle for a sled. */
const double SLED_TURN_LIMIT = 0.5;
  
/** Per-turn movement speed of the sled. */
const double SLED_SPEED = 15;
  
/** Puck velocity lost per turn */
const double PUCK_FRICTION = 1;

/** Color values for the two sides and for neutral.  The player can
    always think of itself as red. */
enum GameColor {
  RED = 0, BLUE = 1, GREY = 2
};

/** Simple 2D Point/Vector representation along with common utility
    functions. */
struct Vector2D {
  /** X coordinate of this point/vector. */
  double x;

  /** Y coordinate of this point/vector. */
  double y;

  /** Initialize with given coordinates. */
  Vector2D( double xv = 0, double yv = 0 ) {
    x = xv;
    y = yv;
  }

  /** Return the squared magnitude of this vector. */
  double squaredMag() const {
    return x * x + y * y;
  }

  /** Return the magnitude of this vector. */
  double mag() const {
    return std::sqrt( x * x + y * y );
  }

  /** Return a unit vector pointing in the same direction as this. */
  Vector2D norm() const {
    double m = mag();
    return Vector2D( x / m, y / m );
  }

  /** Return a vector pointing in the same direction as this, but with
      magnitude no greater than d. */
  Vector2D limit( double d ) const {
    double m = mag();
    if ( m > d )
      return Vector2D( d * x / m, d * y / m );
    else
      return *this;
  }
};

/** Return a vector that's the sum of a and b. */
Vector2D operator+( Vector2D const &a, Vector2D const &b ) {
  return Vector2D( a.x + b.x, a.y + b.y );
}

/** Return a vector that's a minus b. */
Vector2D operator-( Vector2D const &a, Vector2D const &b ) {
  return Vector2D( a.x - b.x, a.y - b.y );
}

/** Return a copy of a that's a scaled by b. */
Vector2D operator*( Vector2D const &a, double b ) {
  return Vector2D( a.x * b, a.y * b );
}

/** Return a copy of a that's a scaled by b. */
Vector2D operator*( double b, Vector2D const &a ) {
  return Vector2D( b * a.x, b * a.y );
}

/** Return the dot product of a and b. */
double operator*( Vector2D const &a, Vector2D const &b ) {
  return a.x * b.x + a.y * b.y;
}

#endif
