#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // The method receive use std::unique_lock<std::mutex> and _condition.wait()
  // to wait for and receive new messages and pull them from the queue using
  // move semantics. The received object is then returned by the receive
  // function. The modification is performed under the lock
  std::unique_lock<std::mutex> uLock(_mutex);
  _cond.wait(uLock, [this] {
    return !_queue.empty();
  }); // pass unique lock to condition variable

  // remove last vector element from queue
  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg; // will not be copied due to return value optimization (RVO) in
              // C++
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // The method send uses the mechanisms std::lock_guard<std::mutex>
  // as well as _condition.notify_one() to add a new message to the queue and
  // afterwards send a notification. perform vector modification under the lock
  std::lock_guard<std::mutex> uLock(_mutex);

  // add vector to queue
  std::cout << "   Message " << msg << " has been sent to the queue"
            << std::endl;
  _queue.push_back(std::move(msg));
  _cond.notify_one(); // notify client after pushing new Vehicle into vector
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  // An infinite while-loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  while (true) {
    TrafficLightPhase phase = _messages.receive();
    if (phase == TrafficLightPhase::green) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // Finally, the private method „cycleThroughPhases“ is started in a thread
  // when the public method „simulate“ is called.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  // The function implement an infinite loop that measures the time between two
  // loop cycles and toggles the current phase of the traffic light between red
  // and green and sends an update method to the message queue using move
  // semantics. The cycle duration should be a random value between 4 and 6
  // seconds. Also, the while-loop should use std::this_thread::sleep_for to
  // wait 1ms between two cycles.

  // Generate a random number
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_real_distribution<double> dist(4, 6);
  float cycle_duration = dist(eng);

  // Start the clock now
  auto start = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_passed;

  while (true) {
    // Check time since last color switch
    time_passed = std::chrono::high_resolution_clock::now() - start;

    if (time_passed.count() > cycle_duration) {
      // toggle phase
      _currentPhase = (_currentPhase == TrafficLightPhase::green) ? red : green;

      // send update method to message queue using move semantics.
      TrafficLightPhase phase = _currentPhase;
      _messages.send(std::move(phase));

      // pick up a new cycle duration and reset start time
      cycle_duration = dist(eng);
      start = std::chrono::high_resolution_clock::now();

      // Wait between cycles
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}
