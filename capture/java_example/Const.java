package java_example;

import java.awt.geom.Point2D;

/** Collection of constants from the game. */
public class Const {
  /** Width and height of the world in game units. */
  public static final double SIZE = 800;
  
  /** Maximum length of the trail behind a sled */
  public static final double TRAIL_LIMIT = 600;
  
  /** Maximum velocity for a bumper. */
  public static final double BUMPER_SPEED_LIMIT = 24.0;
  
  /** Maximum acceleration for a bumper. */
  public static final double BUMPER_ACCEL_LIMIT = 8.0;
  
  /** Maximum turn angle for a sled. */
  public static final double SLED_TURN_LIMIT = 0.5;

  /** Per-move speed of the sled. */
  public static final double SLED_SPEED = 15;

  /** Puck velocity lost per turn */
  public static final double PUCK_FRICTION = 1;
  
  /** Color value for the red player. */
  public static final int RED = 0;
  
  /** Color value for the blue player. */
  public static final int BLUE = 1;
  
  /** Color value for unclaimed pucks. */
  public static final int GREY = 2;
}
