#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

#include<stdlib.h>

typedef struct {
    int position, zeroed;
} State;

typedef struct {
    int direction;
    int steps;
} Input;

const static int MAX_STEPS = 100;
const static int START_POS = 50;
const static int MAX_STEPS_DIGITS = 5;

static Input *parse_input(const char *line) {
    char dir = line[0];
    if (dir != 'L' && dir != 'R') {
        LOG_ERROR("Invalid direction character: %c", dir);
        return NULL;
    }

    char *stepsstr = malloc(MAX_STEPS_DIGITS*sizeof(char));
    for (int i=1;i<=MAX_STEPS_DIGITS;i++) { // start from 1 as 0 is direction
        if (line[i] == '\0') {
            break;
        }
        
        stepsstr[i - 1] = line[i];
    }
    int steps = atoi(stepsstr);
    free(stepsstr);

    Input *res = malloc(sizeof(Input));
    res->direction = (dir == 'L') ? -1 : 1;
    res->steps = steps;
    return res;
}

void click(State *state) {
    LOG_DEBUG("CLICK");
    state->zeroed++;
}

static int compute_rotation(const Input *input, State *state) {
    int steps = input->steps;

    int to_zero;
    switch (input->direction) {
        case 1: // Going Right
            to_zero = MAX_STEPS - state->position;
            while (steps > to_zero) {
                LOG_DEBUG("Wrapping Right: steps=%d, to_zero=%d", steps, to_zero);
                steps -= to_zero;
                to_zero = MAX_STEPS;

                state->position = 0;
                click(state);
            }

            LOG_DEBUG("Stepping Right: steps=%d, to_zero=%d", steps, to_zero);
            state->position += steps;
            if (steps > 0 && state->position == MAX_STEPS) { // only count if we moved after the while
                LOG_DEBUG("Position = 100 -> normalizing and clicking");
                state->position = 0;
                click(state);
            }

            LOG_DEBUG("Final position after Right move: %d; Zeroes clicked: %d", state->position, state->zeroed);
            return 0;
        case -1: // Going Left
            to_zero = state->position;
            if (to_zero == 0) {
                to_zero = MAX_STEPS;
            }
            while(steps > to_zero) {
                LOG_DEBUG("Wrapping Left: steps=%d, to_zero=%d", steps, to_zero);
                steps -= to_zero;
                to_zero = MAX_STEPS;

                state->position = MAX_STEPS;
                click(state);
            }

            LOG_DEBUG("Stepping Left: steps=%d, to_zero=%d", steps, to_zero);
            state->position -= steps;
            if (steps > 0 && state->position == 0) { // only count if we moved after the while
                LOG_DEBUG("Position = 0 -> clicking");
                click(state);
            }

            if (state -> position < 0) {
                LOG_DEBUG("Position = %d -> normalizing", state->position);
                state->position += MAX_STEPS;
            }

            LOG_DEBUG("Final position after Left move: %d; Zeroes clicked: %d", state->position, state->zeroed);
            return 0;
        default:
            LOG_ERROR("Invalid direction value: %d", input->direction);
            return 1;
    }
}

int input_line_handler(const char *line, void *ctx) {
    State *state = (State *)ctx;

    fprintf(stderr, "\n");

    LOG_INFO("Input: %s, current pos: %d, current zeroed: %d", line, state->position, state->zeroed);
    
    Input *input = parse_input(line);
    if (input == NULL) {
        LOG_ERROR("Failed to parse input line: %s", line);
        return 1;
    }
    
    LOG_DEBUG("Parsed Input: direction %d; steps: %d", input->direction, input->steps);

    if (compute_rotation(input, state) > 0) {
        LOG_ERROR("compute_rotation failed");
        free(input);
        return 1;
    };

    free(input);
    return 0;
}
