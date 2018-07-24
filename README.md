Traffic Light Controller
Design a traffic light controller for the intersection of two equally busy one-way
streets. The goal is to maximize traffic flow, minimize waiting time at a red light, and
avoid accidents.
The intersection has two one-ways roads with the same amount of traffic: North and
East, as shown in Figure. In this system, the light pattern defines which road has right
of way over the other. Since an output pattern to the lights is necessary to remain in
a state, we will solve this system with a Moore FSM. It will have three inputs (car
sensors on North and East roads and Emergency signal) and seven outputs (one for
each light in the traffic signal.) The seven traffic lights are interfaced to Port B bits
6–0, and the three sensors are connected to Port E bits 2–0