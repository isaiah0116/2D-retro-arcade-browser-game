#include "olcPixelGameEngine.h"
#include "vector"

using namespace std;

#define OK 1

// Radius of the biggest asteroid
// BIG_RADIUS = BIG_ASTEROID_RADIUS
// AVERAGE_RADIUS = BIG_RADIUS / 2
// SMOLL_RADIUS = AVERAGE_RADIUS / 4 ?
#define BIG_ROCK_RADIUS 16
// Amount of steps taken to draw the big asteroid 
#define BIG_ROCK_STEPS 9
// Max speed coefficient to be applied upon destruction
#define ROCK_SPEED_COEFFICIENT 2.5
// The least size of rocks radius de-facto to avoid collision confusion
#define ROCK_LEAST_RADIUS_COEFFICIENT 0.8
// The angle at which smaller asteroids spawn (rad)
#define ROCK_TILT_ANGLE 2
// Maximum amount of projectiles to be alive
#define PROJECTILE_POOL_SIZE 32
// Radius of one projectile
#define PROJECTILE_RADIUS 2


struct Transform {
    olc::vf2d position = { (float)(384 / 2), (float)(240 / 2) };
    olc::vf2d velocity;
    float radius;
    float rotation;

    // Is colliding with other
    bool operator&&(Transform& other) {
        // sqrt is usually expensive so just using squares why not
        float distance2 = (other.position - position).mag2();
        return distance2 < (radius + other.radius)* (radius + other.radius);
    }
};

struct Ship {
    Transform transform;
    olc::vf2d dimensions;

    //transform.position = { (float)(384 / 2), (float)(240 / 2) };

    struct Stats {
        float rotationSpeed;
        float movementSpeed;
        float projectileSpeed;
    } stats;
};

struct Rock {
    Transform transform;

    static const Rock null;

    // *softly* The chonk chart
    enum struct Size : char {
        NONE = 0, // indicated that the asteroid is none existent
        SMALL = 1,
        AVERAGE = 2,
        BIG = 3,
    } size;

    Rock() : transform({ { 0, 0 }, { 0, 0 }, 0, 0 }), size(Rock::Size::NONE) {};
    Rock(Transform transform, Rock::Size size = Rock::Size::NONE) : transform(transform), size(size) {}

};

struct Projectile {
    Transform transform;
};

class OneLoneCoder_Asteroids {
public:
    vector<Rock> rocks;

    size_t rock_counter;

    Projectile projectiles[PROJECTILE_POOL_SIZE];
    size_t projectileStackCounter;

    float deltaTime;
    Ship* ship;

    bool gameStart;

    uint64_t score;

    OneLoneCoder_Asteroids() {
        ship = new Ship();
        rock_counter = 0;
        projectileStackCounter = 0;
        gameStart = false;
        score = 0;

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            projectiles[i].transform.radius = 0;
        }
    }

    ~OneLoneCoder_Asteroids() {}
};