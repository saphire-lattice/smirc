#include "telnet.h"
#include "stdlib.h"
#include "memory.h"
#include "../utils.h"
#include <stdio.h>

const char* colors = "01,04,03,07,02,06,11,15,99,99,";

const char telnet_protocols[] = {
    COMPRESS2,
    ATCP,
    GMCP,
    TTYPE,
    MSDP,
    MSSP,
    MSP,
    0
};

char* get_protocols() {
    return (char *) telnet_protocols;
}

char* ttoa(unsigned int code) {
    char* result = calloc(6, sizeof(char));
    switch(code) { // There needs to be a better option
        case IAC:
            memcpy(result, "IAC", 4);
            break;
        case WONT:
            memcpy(result, "WONT", 5);
            break;
        case WILL:
            memcpy(result, "WILL", 5);
            break;
        case DO:
            memcpy(result, "DO", 3);
            break;
        case DONT:
            memcpy(result, "DONT", 5);
            break;
        case SE:
            memcpy(result, "SE", 3);
            break;
        case SB:
            memcpy(result, "SB", 3);
            break;
        case GMCP:
            memcpy(result, "GMCP", 5);
            break;
        case COMPRESS2:
            memcpy(result, "MCCP2", 6);
        case NOP:
            memcpy(result, "NOP", 4);
            break;
        default: {
            int i = 0;
            result[i++] = '0'; // "0"
            result[i++] = 'x'; // "0x"
            if (code < 0x10) {
                result[i++] = '0'; // "0x0"
            }
            itoa(code, result + i, 16); // Because printf is for weak!
        }
    }
    return result;
}

char* ansi_to_irc_color(char* str) {
    int sl = (int) strlen(str);
    char* ret = calloc(strlen(str) + 1, sizeof(char));
    int sp, rp;
    sp = 0;
    rp = 0;

    struct state* state;
    state = malloc(sizeof(struct state));
    state->fg = -1;
    state->bg = -1;

    int parsing = 0;
    char* code = calloc(3, sizeof(char));
    int cp = 0;
    for (sp = 0; sp < sl; sp++ ) {
        if (parsing == 1) {
            if (str[sp] == ';' || str[sp] == 'm') {
                int c = atoi(code);
                cp = 0;
                memset(code, 0, 3);

                if (c >= 30 && c < 37)
                    state->fg = c - 30;
                if (c == 37)
                    state->fg = -1;
                if (c == 39)
                    state->fg = -1;

                if (c == 40)
                    state->bg = -1;
                if (c >= 41 && c < 48)
                    state->bg = c - 40;
                if (c == 49)
                    state->bg = -1;

                if (c == 0) {
                    state->fg = -1;
                    state->bg = -1;
                    ret[rp++] = '\x0f';
                }
            } else {
                code[cp++] = str[sp];
            }
            if (str[sp] == 'm') {
                if (state->fg != -1 || state->bg != -1) {
                    ret[rp++] = '\x03';
                    if (state->fg >= 0) {
                        memcpy(ret + rp, colors + (state->fg * 3), 2);
                        rp += 2;
                    }
                    if (state->bg >= 0) {
                        ret[rp++] = ',';
                        memcpy(ret + rp, colors + (state->bg * 3), 3);
                        rp += 2;
                    }
                }
                parsing = 0;
            }
        } else {
            if (str[sp] == '\x1b') {
                sp++;
                if (str[sp] == '[')
                    parsing = 1;
                else {
                    sp--;
                    ret[rp++] = str[sp];
                }
            } else
                ret[rp++] = str[sp];
        }
    }
    free(state);
    free(code);
    return ret;
}
