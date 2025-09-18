#ifndef __GENERIC_COMMAND_HANDLING_H__
#define __GENERIC_COMMAND_HANDLING_H__
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NO_OF_LENGTH 25
#define MAX_BUFF_LENGTH 250
typedef struct command_lut_entry_s command_lut_entry_t;
typedef struct module_lut_entry_s module_lut_entry_t;
command_lut_entry_t *get_command_entry(char **argv,
                                       command_lut_entry_t *current_command);
module_lut_entry_t *get_module_entry(char **argv,
                                     module_lut_entry_t *current_module);
void display_submodule(char **argv, module_lut_entry_t *current_module);
void display_command(char **argv, command_lut_entry_t *current_command);

extern uint8_t response_buffer[];
extern uint32_t response_buffer_len;

typedef struct command_lut_entry_s {
  char *command_name;
  void (*handler)(int argc, char *argv[]);
} command_lut_entry_t;
// void (*custom_handler)(int argc, char *argv[]);
typedef struct module_lut_entry_s {
  char *module_name;
  module_lut_entry_t *sub_modules;
  command_lut_entry_t *commands;
  void (*custom_handler)(int argc, char *argv[]);
} module_lut_entry_t;

#endif
