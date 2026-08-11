/*
 * debug viewer shared memory segment
 */

#define MEM_SIZE 1024
#define SHM_NAME "/forth_debug"

typedef struct {

	uint32_t generation;      /* increment every update; viewer can detect "torn read" */
	int      ip;
	int      dstack[32];
	int      dsp;
	token_t  prog[64];
	int      ntok;
	int      begin_ip;

} shared_state_t;
