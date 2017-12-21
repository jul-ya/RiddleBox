#include <LinkedList.h>
#include "State.h"

#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

class StateMachine
{
  public:
    // Methods
    
    StateMachine();
    ~StateMachine();
    void init();
    void run();

    // When a stated is added we pass the function that represents 
    // that state logic
    State* addState(void (*functionPointer)());
	State* transitionTo(State* s);
	
    // Attributes
    LinkedList<State*> *stateList;
	bool executeOnce = true; 	//Indicates that a transition to a different state has occurred
    int currentState = -1;	//Indicates the current state number
};

StateMachine::StateMachine(){
  stateList = new LinkedList<State*>();
};

StateMachine::~StateMachine(){};

/*
 * Main execution of the machine occurs here in run
 * The current state is executed and it's transitions are evaluated
 * to determine the next state. 
 * 
 * By design, only one state is executed in one loop() cycle.
 */
void StateMachine::run(){
  // Early exit, no states are defined
  if(stateList->size() == 0) return;

  // Initial condition
  if(currentState == -1){
    currentState = 0;
  }
  
  // Execute state logic and return transitioned
  // to state number. Returns -1 if no transitioned
  // occured.
  int next = stateList->get(currentState)->execute();
  
  if(next >= 0){
	executeOnce = (currentState == next)?false:true;
    currentState = next;
  }
}

/*
 * Adds a state to the machine
 * It adds the state in sequential order.
 */
State* StateMachine::addState(void(*functionPointer)()){
  State* s = new State();
  s->stateLogic = functionPointer;
  stateList->add(s);
  s->index = stateList->size()-1;
  return s;
}

/*
 * Jump to a give state
 * given by a pointer to that state.
 */
State* StateMachine::transitionTo(State* s){
  this->currentState = s->index;
  return s;
}

#endif
