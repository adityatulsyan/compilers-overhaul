run : new_driver.o lexer.o symbol_table.o semantics.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o
	gcc new_driver.o lexer.o symbol_table.o semantics.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o -o compiler -lm

debug_exe : driver.o lexer.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o symbol_table.o semantics.o code_generator.o output_code.o label_generator.o
	gcc driver.o lexer.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o symbol_table.o semantics.o code_generator.o output_code.o label_generator.o -o debug_exe -lm

label_generator.o : ./code_generator/label_generator.h ./code_generator/label_generator.c
	gcc -c ./code_generator/label_generator.c

code_generator.o : ./code_generator/code_generator.h ./code_generator/code_generator.c
	gcc -c ./code_generator/code_generator.c

output_code.o : ./utils/output_code.h ./utils/output_code.c
	gcc -c ./utils/output_code.c

kunal : driver.o lexer.o symbol_table.o semantics.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o
	gcc driver.o lexer.o symbol_table.o semantics.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o terminal_name.o nonterminal_name.o terminal_hash_map.o nonterminal_hash_map.o print.o parse_grammar.o grammar.o hash_map.o parse_table.o generate_parse_table.o tree.o parser.o keyword_hash_map.o generate_ast.o linked_list.o -o debug_exe

symbol_table.o : ./symbol_table/symbol_table_def.h ./symbol_table/symbol_table.h ./symbol_table/symbol_table.c
	gcc -c ./symbol_table/symbol_table.c

semantics.o : ./semantic_analyzer/semantics.h ./semantic_analyzer/semantics.c
	gcc -c ./semantic_analyzer/semantics.c

exe : driver.o lexer.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o
	gcc driver.o lexer.o file_handler.o parse_dfa.o dfa.o stack.o gen_utils.o -o exe

linked_list.o : ./data_structs/linked_list.h ./data_structs/linked_list.c
	gcc -c ./data_structs/linked_list.c

generate_ast.o : ./ast/generate_ast.h ./ast/generate_ast.c
	gcc -c ./ast/generate_ast.c

keyword_hash_map.o : ./utils/keyword_hash_map.h ./utils/keyword_hash_map.c
	gcc -c ./utils/keyword_hash_map.c

tree.o : ./data_structs/tree.h ./data_structs/tree.c
	gcc -c ./data_structs/tree.c

parser.o : ./parser/parser.h ./parser/parser.c
	gcc -c ./parser/parser.c

terminal_name.o : ./utils/terminal_name.h ./utils/terminal_name.c
	gcc -c ./utils/terminal_name.c

nonterminal_name.o : ./utils/nonterminal_name.h ./utils/nonterminal_name.c
	gcc -c ./utils/nonterminal_name.c

terminal_hash_map.o : ./utils/terminal_hash_map.h ./utils/terminal_hash_map.c
	gcc -c ./utils/terminal_hash_map.c

hash_map.o : ./data_structs/hash_map.h ./data_structs/hash_map.c
	gcc -c ./data_structs/hash_map.c

nonterminal_hash_map.o : ./utils/nonterminal_hash_map.h ./utils/nonterminal_hash_map.c
	gcc -c ./utils/nonterminal_hash_map.c

print.o : ./utils/print.h ./utils/print.c
	gcc -c ./utils/print.c

parse_grammar.o : ./utils/parse_grammar.h ./utils/parse_grammar.c
	gcc -c ./utils/parse_grammar.c

grammar.o : ./data_structs/grammar.h ./data_structs/grammar.c
	gcc -c ./data_structs/grammar.c

parse_table.o : ./data_structs/parse_table.h ./data_structs/parse_table.c
	gcc -c ./data_structs/parse_table.c

generate_parse_table.o : ./utils/generate_parse_table.h ./utils/generate_parse_table.c
	gcc -c ./utils/generate_parse_table.c

driver.o : driver.c
	gcc -c driver.c

new_driver.o : driver.h new_driver.c
	gcc -c new_driver.c

lexer.o : ./lexer/lexer.h ./lexer/lexer.c
	gcc -c ./lexer/lexer.c

parse_dfa.o : ./utils/parse_dfa.h ./utils/parse_dfa.c
	gcc -c ./utils/parse_dfa.c

dfa.o : ./data_structs/dfa.h ./data_structs/dfa.c
	gcc -c ./data_structs/dfa.c

stack.o : ./data_structs/stack.h ./data_structs/stack.c
	gcc -c ./data_structs/stack.c

file_handler.o : ./file_handler/file_handler.h ./file_handler/file_handler.c
	gcc -c ./file_handler/file_handler.c

gen_utils.o : ./utils/gen_utils.h ./utils/gen_utils.c
	gcc -c ./utils/gen_utils.c


clean :
	rm *.o
