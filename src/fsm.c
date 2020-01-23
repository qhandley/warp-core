#include "fsm.h"

ret_id idle_state(int8_t* status)
{
    return OK;
}

ret_id ignition_state(int8_t* status)
{
    return OK;
}

ret_id fueling_state(int8_t* status)
{
    return OK;
}

ret_id error_state(int8_t* status)
{
    return OK;
}

/*
 * Initialize an array of states (up to NUM_STATES) with their 
 * respective state id, status (0 at start), and state function
 * passed address of the specific state's status (do this with switch)
 */
void init_sm(state_t* states)
{
}

/*
 * Check the prev state's status, if it was non-zero
 *
 */
state_id sm_engine(state_t* curr_state)
{
    //run the current state
    //if its ret id is not FAIL, then
    //use lookup table to determine and return next state
    //else
    //update error state with error status of prev failed state
    //go-to error state and handle error, reset status
     
    return OK;
}

int main()
{
    //declare an array of states
    state_t states[NUM_STATES];

    //init the states
    //prime state machine engine with entry state
    //e.g. state_t curr_state = states[ENTRY_STATE];

    for(;;)
    {
        //run the engine and update state
        //e.g. state_id next_state = sm_engine(&curr_state);
        //curr_state = states[next_state];
    }
    
    return 0;
}
