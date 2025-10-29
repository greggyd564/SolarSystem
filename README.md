// The goal of this simulation is to visually represent the orbit of a planet around the sun.

/** Current Formulas:
// Something to note is that in our simulation we use most of these equations twice to get a vector. This is because our
// Simulation is in 2D therefore we need a change in X & Y.
// VELOCITY: dx / dt = v "the rate of change of position with respect to time is velocity."
// GRAVITATIONAL FORCE: F = G ( ( m1 * m2 ) / r^2 ) cc: https://en.wikipedia.org/wiki/Newton's_law_of_universal_gravitation
// ACCELERATION: a = F / m "force divided by mass"
// NEWTON'S SECOND LAW: F = m * a
// DISTANCE BETWEEN TWO POINTS p = sqrt( ( (x2 - x1)^2 + (y2 - y1)^2 ) "length of the vector"**/

/** How It Works:
// Gather Initial parameters of two objects (Mass, Velocity, and Position)
// Use the Distance Between Two Points Formula to calculate the distance (r)
// Use the Masses of both objects & r to compute the Force Magnitude ( Then split into direction , ie a vector )
// Use that vector to calculate the acceleration of each Object ( inverse but equal due to N3L )
// Use that acceleration to update the velocity of both objects
// Use the velocity to update the position
// Uses Delta-t as time which essentially is multiplied by to represent the change over time.
// Then the visualization changes by basically plotting that change in position on a graph**/

/** NEXT STEPS:
// - Figure out parameters that don't slingshot planets into space
// - Create a 2D Motion Visualization ( CURRENTLY IN PROGRESS )
// - Figure out N-Body System
// - Make it 3D
// - Make it look good
// - Website / App integration
// - Allow adjusting parameters / adding planets from the site ( UX / UI )
// - Adding things other than planets ( ie multiple suns, black holes, asteroid belts )
// - Showing collisions ( Would be cool )  **/