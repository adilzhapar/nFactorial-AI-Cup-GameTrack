// A sample player implemented in C++.  The sled just marches across
// the middle trying to capture pucks.  The bumpers try to push the
// pucks toward the center so they can be captured.
//
// ICPC Challenge
// Sturgill, Baylor University

#include "Util.h"
#include <vector>
#include <iostream>
#include <list>

using namespace std;

/** Simple representation for a puck. */
struct Puck {
  // Position of the puck.
  Vector2D pos;

  // Puck velocity
  Vector2D vel;

  // Puck color
  int color;
};

/** Simple representation for a bumper. */
struct Bumper {
  // Position of the bumper.
  Vector2D pos;

  // Bumper velocity
  Vector2D vel;
};

/** Simple representation for a sled. */
struct Sled {
  // Position of the sled.
  Vector2D pos;

  // Sled direction.
  double dir;
};

/** Return a copy of x that's constrained to be between low and high. */
double clamp( double x, double low, double high ) {
  if ( x < low )
    x = low;
  if ( x > high )
    x = high;
  return x;
}

/** Compute a force vector that can be applied to a bumper to get it
    to run through the given target location.  Pos and vel are the
    bumper's current position and velocity.  Target is the position we
    want to run through and force is a returned vector that will move
    the bumper toward the target.  The function returns true until it
    looks like the next move will take us through the target
    location.  */
bool runTo( Vector2D const &pos,
            Vector2D const &vel, 
            Vector2D const &target, 
            Vector2D &force,
            double epsilon = 0.1 ) {
  // Compute a vector that will move self toward the target point.
  Vector2D direction = target - pos;
  double dist = direction.mag();

  // How much force do we have left to use in the force vector.
  double resForce = BUMPER_FORCE_LIMIT - force.mag();

  // Normalize the direction we need to move.
  direction = direction.norm();

  // First, cancel out any movement that is perpendicular to the desired
  // movement direction.
  Vector2D perp = Vector2D( -direction.y, direction.x );
  force = -(perp * vel ) * perp;

  // Use all the residual force to move toward the target.
  resForce = BUMPER_FORCE_LIMIT - force.mag();
  force = force + direction.limit( resForce );

  // See if this move will cross close enough to the target location.
  Vector2D nvel = ( vel + force ).limit( BUMPER_SPEED_LIMIT );
  double t = clamp( ( target - pos ) * nvel / (nvel * nvel), 0, 1 );
  if ( ( pos + t * nvel - target ).mag() < epsilon )
    return true;

  return false;
}

/** Return true if a puck at position pos is the responsibility of the
    bumper with index bdex. */
bool mySide( int bdex, Vector2D pos ) {
  if ( bdex == 0 && pos.y < 330 )
    return true;
  if ( bdex == 1 && pos.y > 470 )
    return true;
  return false;
}

int main() {
  // List of current sled, bumper and puck locations.  These are
  // updated on every turn snapshot from the server.
  vector< Puck > plist;
  vector< Bumper > blist;
  vector< Sled > slist;

  // State of each bumper.
  enum BumperState {
    SeekLeft,
    SeekRight,
    MoveUp,
  };

  BumperState bstate[ 2 ] = { MoveUp, MoveUp };
  
  // Latest target for each bumper, the bumper will try to move through
  // this position to hit a puck toward the horizontal center.
  Vector2D target[ 2 ];

  // How much longer the bumper has to pursue its target.  This keeps
  // the bumper from chasing after a bad target forever.
  int targetTime[ 2 ] = { 0, 0 };
  
  int n, turnNum;
  cin >> turnNum;
  while ( turnNum >= 0 ) {
    // Read all the puck locations.
    cin >> n;
    plist.resize( n );
    for ( int i = 0; i < n; i++ ) {
      Puck &puck = plist[ i ];
      cin >> puck.pos.x >> puck.pos.y >> puck.vel.x >> puck.vel.y
          >> puck.color;
    }

    // Read all the bumper locations.
    cin >> n;
    blist.resize( n );
    for ( int i = 0; i < n; i++ ) {
      Bumper &bumper = blist[ i ];
      cin >> bumper.pos.x >> bumper.pos.y >> bumper.vel.x >> bumper.vel.y;
    }

    // Read all the sled locations.
    cin >> n;
    slist.resize( n );
    for ( int i = 0; i < n; i++ ) {
      Sled &sled = slist[ i ];
      cin >> sled.pos.x >> sled.pos.y >> sled.dir;

      // Just ignore the path history for this sled.
      int m;
      cin >> m;
      double dummy;
      for ( int j = 0; j < m; j++ ) {
        cin >> dummy;
        cin >> dummy;
      }
    }

    // Choose a move for each sled.
    for ( int i = 0; i < 2; i++ ) {
      Bumper &bumper = blist[ i ];
      Vector2D force( 0, 0 );
      
      // Choose an up direction for this bumper.
      Vector2D up( 0, i == 0 ? -1 : 1 );

      // Move away from the center line until we get near the top or bottom.
      if ( bstate[ i ] == MoveUp ) {
        if ( fabs( bumper.pos.y - 400 ) > 380 ) {
          bstate[ i ] = SeekRight;
          targetTime[ i ] = 0;
        } else
          // Move up and scoot a little to the side to prevent stupid behavior.
          force = force + BUMPER_FORCE_LIMIT * up + Vector2D( 0.01, 0 );
      }

      // Move right until we ge near the right edge.
      if ( bstate[ i ] == SeekRight && bumper.pos.x > 750 ) {
        bstate[ i ] = SeekLeft;
      }

      // Move left until we ge near the left edge.
      if ( bstate[ i ] == SeekLeft && bumper.pos.x < 50 )
        if ( fabs( bumper.pos.y - 400 ) < 50 )
          bstate[ i ] = MoveUp;
        else
          bstate[ i ] = SeekRight;

      if ( bstate[ i ] != MoveUp ) {
        // Go across the field and angle back toward the horizontal center
        // line.
        Vector2D across( bstate[ i ] == SeekRight ? 1 : -1, 0 );

        // If we don't have a target, find one.
        if ( targetTime[ i ] <= 0 ) {
          for ( int j = 0; j < plist.size(); j++ )
            // Find a grey target that on my side of the center line, 
            // not moving too fast, close to me and kind of in the direction
            // i'm moving.  Also, make sure it's not too close to the top or
            // bottom.  Among these pucks, pick the one that's closest to
            // the direction I should be heading.
            if ( plist[ j ].color == GREY &&
                 mySide( i, plist[ j ].pos ) &&
                 plist[ j ].vel.mag() < 1 &&
                 ( plist[ j ].pos - bumper.pos ).mag() < 150 &&
                 ( plist[ j ].pos - bumper.pos ).norm() * across > 0.6 &&
                 fabs( plist[ j ].pos.y - 400 ) < 360 &&
                 ( targetTime[ i ] <= 0 || 
                   ( plist[ j ].pos - bumper.pos ).norm() * across >
                   ( target[ i ] - bumper.pos ).norm() * across ) ) {
              // Give 20 turns to hit the puck.
              targetTime[ i ] = 20;
              // Hit it on its outside edge to bump it closer to the
              // center.
              target[ i ] = plist[ j ].pos - across * 8 + up * 11;
            }

          if ( targetTime[ i ] > 0 ) {
            // See what this player is doing
            cerr << "New Target: " << i << " " 
                 << target[ i ].x << " " << target[ i ].y << endl;
          }
        }

        // If w have a target, try to hit it.
        if ( targetTime[ i ] >= 0 ) {
          if ( runTo( bumper.pos, bumper.vel, target[ i ], force ) ) {
            targetTime[ i ] = 0;
          } else
            targetTime[ i ] -= 1;
        } else {
          // Otherwise, just move across the field and angle toward
          // the center line a little bit.
          force = force + ( across - up * 0.05 ).norm() * BUMPER_FORCE_LIMIT;
        }
      }

      // Output the bumper's move.
      cout << force.x << " " << force.y << " ";
    }

    // Just keep making circles and march across the playing field.
    double sledDir = 0;
    int loopSize = 40;
    int loopGap = 5;
    int startup = 12;

    // Make the sled drive around in circles.
    if ( turnNum < startup ) {
      // Initially, try to move off-center a little bit.
      if ( turnNum < 6 )
        sledDir = -0.2;
      else
        sledDir = 0.2;
    } else if ( ( turnNum - startup ) % ( loopSize + loopGap ) < loopSize ) {
      // Drive in a loop most of the time.
      sledDir = 2 * acos( -1.0 ) / loopSize;
    } else {
      // Move the loop ahead.
      sledDir = 0;
    }
    // Output the sled's move.
    cout << sledDir << endl;

    cin >> turnNum;
  }
}
