#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase { red, green };

// Definition of the class „MessageQueue“ which has the public methods send and
// receive. Send takes an rvalue reference of type TrafficLightPhase whereas
// receive should return this type. Also, the class define an std::dequeue
// called _queue, which stores objects of type TrafficLightPhase.

template <class T> class MessageQueue {
public:
  T receive();
  void send(T &&msg);

private:
  std::mutex _mutex;
  std::condition_variable _cond;
  std::deque<T> _queue;
};

// Definition of the class „TrafficLight“ which is a child class of
// TrafficObject. The class shall have the public methods „void waitForGreen()“
// and „void simulate()“ as well as „TrafficLightPhase getCurrentPhase()“, where
// TrafficLightPhase is an enum that can be either „red“ or „green“. Also, add
// the private method „void cycleThroughPhases()“. Furthermore, there shall be
// the private member _currentPhase which can take „red“ or „green“ as its
// value.

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();

  // getters / setters
  TrafficLightPhase getCurrentPhase();

  // typical behaviour methods
  void waitForGreen();
  void simulate();

private:
  // typical behaviour methods
  void cycleThroughPhases();

  MessageQueue<TrafficLightPhase> _messages;

  TrafficLightPhase _currentPhase;

  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif