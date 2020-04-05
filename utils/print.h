#include "../data_structs/grammar.h"
#include "../lang_specs/entities.h"
#include "../utils/nonterminal_name.h"
#include "../utils/terminal_name.h"
#include "../data_structs/parse_table.h"

#ifndef PRINT_H
#define PRINT_H

void print_first(gm_first *fi);

void print_follow(gm_follow *fo);

void print_grammar(grammar *gm);

void print_parse_table(parse_table *pt);

#endif