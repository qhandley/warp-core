/* fsm.h
 *
 * Simple implementation of a finite state machine for launch 
 * control
 *
 */

#include <stdint.h>

#define ENTRY_STATE     IDLE

/* 
 * Defines each state's id
 */
typedef enum state_id
{
    IDLE,
    IGNITION,
    FUELING,
    ERROR,

    NUM_STATES 
}state_id;

/* 
 * Idea: create a set of ret codes for each unique state
 */
typedef enum ret_id
{
    OK,
    FAIL,
    //add additional ret id's for dif transitions

    NUM_RETS
}ret_id;

/*
 * State struct with id, function, and status
 */
typedef struct state_t
{
    state_id state;
    int8_t status;
    ret_id (* state_fn)(int8_t* status);
}state_t;

ret_id idle_state(int8_t* status);
ret_id ignition_state(int8_t* status);
ret_id fueling_state(int8_t* status);
ret_id error_state(int8_t* status);

void init_sm(state_t* states);
state_id sm_engine(state_t* curr_state);

struct state_transition
{
    state_id src_state;
    ret_id ret;
    state_id dst_state;
};

/* 
 * Lookup table for state transitions indexed by src_state and then ret
 */
struct state_transition transition_lookup[NUM_STATES][NUM_RETS] = 
{
    //IDLE(OK)->IDLE 
    { {IDLE, OK, IDLE}, {IDLE, FAIL, IDLE} },
    //IGN
    { {IGNITION, OK, IGNITION} }
    //...
};

