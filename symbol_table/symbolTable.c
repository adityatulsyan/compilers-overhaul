#include "./symbolTableDef.h"

#define DEFAULT_ST_SIZE 71
#define DEFAULT_SCOPE_SIZE 41

id_type terminal_to_type (terminal t) {
	switch (t) {
		case INTEGER : return integer;
		case REAL : return real;
		case BOOLEAN : return boolean;
		case ARRAY : return array;
		case NUM : return integer;
		case RNUM: return real;
		case TRUE : return boolean;
		case FALSE : return boolean;
		default : return -1;
	}
}

id_type get_type_from_node (tree_node *node) {
	return terminal_to_type(
			((ast_node *) get_data(
				node))->label.gms.t);
}

int* get_range_from_node (tree_node *node) {
	int *indices = (int *) malloc(2*sizeof(int));
	indices[0] = ((ast_leaf *) get_data(
				get_child(node, 0)))->ltk->nv.int_val;
	indices[1] = ((ast_leaf *) get_data(
				get_child(node, 1)))->ltk->nv.int_val;
	return indices;
}

scope_node *create_new_scope (scope_node *parent) {
	scope_node *new_scope = (scope_node *) malloc(sizeof(scope_node));
	new_scope->var_id_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->arr_st = create_hash_map(DEFAULT_ST_SIZE);
	new_scope->parent_scope = parent;
	new_scope->child_scopes = create_hash_map(DEFAULT_SCOPE_SIZE);
	return new_scope;
}

var_id_entry *create_var_entry (char *lexeme, id_type type, int width, int offset) {
	var_id_entry *entry = (var_id_entry *) malloc(sizeof(var_id_entry));
	strcpy(entry->lexeme, lexeme);
	entry->type = type;
	entry->width = width;
	entry->offset = offset;
	return entry;
}

arr_id_entry *create_arr_entry (char *lexeme, id_type type, int rstart, int rend, int width, int offset) {
	arr_id_entry *entry = (arr_id_entry *) malloc(sizeof(arr_id_entry));
	strcpy(entry->lexeme, lexeme);
	entry->type = type;
	entry->range_start = rstart;
	entry->range_end = rend;
	entry->width = width;
	entry->offset = offset;
	return entry;
}

func_entry *create_func_entry (char *name, bool only_declared, bool is_called, int offset, int width) {
	func_entry *entry = (func_entry *) malloc(sizeof(func_entry));
	strcpy(entry->name, name);
	entry->input_param_list = create_linked_list();
	entry->output_param_list = create_linked_list();
	entry->local_scope = create_new_scope(NULL);
	entry->only_declared = only_declared;
	entry->is_called = is_called;
	entry->offset = offset;
	entry->width = width;
	return entry;
}

common_id_entry *find_id (char *lexeme, scope_node *curr_scope, bool is_recursive) {
	common_id_entry *centry = (common_id_entry *) malloc(sizeof(common_id_entry));

	var_id_entry *ventry = (var_id_entry *) fetch_from_hash_map(curr_scope->var_id_st, lexeme);
	if (ventry != NULL) {
		centry->is_array = false;
		centry->entry.var_entry = ventry;
		return centry;
	}

	arr_id_entry *aentry = (arr_id_entry *) fetch_from_hash_map(curr_scope->arr_st, lexeme);
	if (aentry != NULL) {
		centry->is_array = true;
		centry->entry.arr_entry = aentry;
		return centry;
	}

	// function local params ???????????????/
	if (is_recursive) {
		if (curr_scope->parent_scope != NULL)
			return find_id_rec(lexeme, curr_scope->parent_scope);
		else {
			// => in the function local scope
			// search in input/output params???????????????
		}
	}
	return centry;
}

common_id_entry *find_id_rec (char *lexeme, scope_node *curr_scope) {
	return find_id (lexeme, curr_scope, true);
}

common_id_entry *find_id_in_scope (char *lexeme, scope_node *curr_scope) {
	return find_id (lexeme, curr_scope, false);
}

bool match_array_type (arr_id_entry *arr1, arr_id_entry *arr2) {
	return (arr1->type == arr2->type &&
		arr1->range_start == arr2->range_start &&
		arr1->range_end == arr2->range_end);
}

void symbol_table_fill (hash_map *main_st, tree_node *astn, scope_node *curr_scope) {
	ast_node* ast_data = (ast_node *)get_data(astn);
	gm_unit data_label = ast_data->label;
	
	assert(!data_label.is_terminal, "ast node for non terminal");
	int num_children = get_num_children(astn);

	nonterminal ast_nt = data_label.gms.nt;

    if (ast_nt == program) {
		symbol_table_fill(main_st, get_child(astn, 0), curr_scope);
		symbol_table_fill(main_st, get_child(astn, 1), curr_scope);
		symbol_table_fill(main_st, get_child(astn, 2), curr_scope);
		symbol_table_fill(main_st, get_child(astn, 3), curr_scope);
    }

    if (ast_nt == moduleDeclarations) {
		linked_list *module_decs = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < module_decs->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(module_decs, i), curr_scope);
    }

	if (ast_nt == moduleDeclaration) {
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "moduleDeclaration : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, true, false, -1, -1);
			add_to_hash_map(main_st, func_name, f_st_entry);
		}
		else {
			// TODO: module redeclaration error
		}
		//
		//
	}

	if (ast_nt == otherModules) {
		linked_list *other_mods = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < other_mods->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(other_mods, i), curr_scope);
	}

	if (ast_nt == driverModule) {
		func_entry *st_entry = (func_entry *) fetch_from_hash_map(main_st, "program");
		assert(st_entry != NULL, "default entry for driver module not found");
		st_entry->only_declared = false;
		//
		//
	}

	if (ast_nt == module) {
		// MODULE ID
		tree_node* module_id_node = get_child(astn, 0);
		ast_leaf* module_id_data = (ast_leaf *) get_data(module_id_node);

		assert(module_id_data->label.is_terminal && module_id_data->label.gms.t == ID, "module : need ID here");

		char *func_name = module_id_data->ltk->lexeme;
		func_entry *f_st_entry = (func_entry *) fetch_from_hash_map(main_st, func_name);
		if (f_st_entry == NULL) {
			f_st_entry = create_func_entry(func_name, false, false, -1, -1);
			add_to_hash_map(main_st, func_name, f_st_entry);
		}
		else if (f_st_entry->only_declared && f_st_entry->is_called) {
			f_st_entry->only_declared = false;
		}
		else if (f_st_entry->only_declared && !f_st_entry->is_called) {
			// TODO: redundant declaration, only definition required
		}
		else {
			// TODO: module redefinition error
		}

		// MODULE INPUT PARAM LIST
		tree_node* module_iplist = get_child(astn, 1);
		ast_node* module_iplist_data = (ast_node *) get_data(module_iplist);

		assert(!module_iplist_data->label.is_terminal && module_iplist_data->label.gms.nt == input_plist, "module : need input_plist here");

		linked_list *ip_ll = module_iplist_data->ll;
		linked_list *fip_ll = create_linked_list();
		for (int i = 0; i < ip_ll->num_nodes; i++) {
			param_node *fparam = (param_node *) malloc(sizeof(param_node));
			tree_node *param = (tree_node *) ll_get(ip_ll, i);
			ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
			tree_node *param_type = get_child(param, 1);
			id_type type_param = get_type_from_node(param_type);

			if (type_param == array) {
				fparam->is_array = true;
				
				tree_node *range_arrays = get_child(param_type, 0);
				int *range_indices = get_range_from_node(range_arrays);

				fparam->param.arr_entry = create_arr_entry(
						param_id->ltk->lexeme,
						get_type_from_node(get_child(param_type, 1)),
						range_indices[0],
						range_indices[1],
						-1, -1);
			}
			else {
				fparam->is_array = false;
				fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);
			}
			ll_append(fip_ll, fparam);
		}
		f_st_entry->input_param_list = fip_ll;

		// MODULE OUTPUT PARAM LIST
		tree_node* module_oplist = get_child(astn, 2);
		ast_node* module_oplist_data = (ast_node *) get_data(module_oplist);

		assert(!module_oplist_data->label.is_terminal && module_oplist_data->label.gms.nt == output_plist, "module : need output_plist here");

		linked_list *op_ll = module_oplist_data->ll;
		linked_list *fop_ll = create_linked_list();
		for (int i = 0; i < op_ll->num_nodes; i++) {
			param_node *fparam = (param_node *) malloc(sizeof(param_node));
			tree_node *param = (tree_node *) ll_get(op_ll, i);
			ast_leaf *param_id = (ast_leaf *) get_data(get_child(param, 0));
			tree_node *param_type = get_child(param, 1);
			id_type type_param = get_type_from_node(param_type);

			fparam->is_array = false;
			fparam->param.var_entry = create_var_entry(param_id->ltk->lexeme, type_param, -1, -1);

			ll_append(fop_ll, fparam);
		}
		f_st_entry->output_param_list = fop_ll;

		// MODULEDEF
		symbol_table_fill(main_st, get_child(astn, 3), f_st_entry->local_scope);
	}

	if (ast_nt == moduleDef) {
		linked_list *statements = ((ast_node *) get_data(astn))->ll;

		for (int i = 0; i < statements->num_nodes; i++)
			symbol_table_fill(main_st, ll_get(statements, i), curr_scope);
	}

	/*ioStmt start*/
	if (ast_nt == input_stmt) {
		char *var_name = ((ast_leaf *) get_data(get_child(astn, 0)))->ltk->lexeme;
		common_id_entry *entry = dfind_id_rec(var_name, curr_scope);
		if (entry == NULL) {
			// TODO: variable not declared
		}
		else {
			// TODO:
			// generate assembly code to take input
			// and store at corresponding offset
			// taken from ST
		}
	}

	if (ast_nt == output_stmt) {
		tree_node *var_node = get_child(astn, 0);
		int child_cnt = get_num_children(var_node);
		
		if (child_cnt == 1) {
			// VAR -> NUM, RNUM, TRUE, FALSE
			tree_node *const_var_node = get_child(var_node, 0);
			// TODO: generate ast code to display value of
			// this constant
		}
		else if (child_cnt == 2) {
			ast_leaf *id_data = (ast_leaf *) get_data(get_child(var_node, 0));
			tree_node *whichid_node = get_child(var_node, 1);
			
			char *var_name = id_data->ltk->lexeme;
			common_id_entry *entry = find_id_rec(var_name, curr_scope);
			if (get_data(whichid_node) == NULL) {
				// VAR -> ID
				if (entry->is_array) {
					// TODO: assembly code to display all
					// elements of the array
				}
				else {
					// TODO: generate ast code to get value of var
					// and display it
				}
			}
			else {
				// VAR -> ID WHICHID
				if (!entry->is_array) {
					// TODO: type err
				}
				else {
					// TODO: generate ast code to index arr
					// and display corresponding value
				}
			}
		}
	}
	/*ioStmt end*/

	if (ast_nt == assignmentStmt) {
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 0));
		tree_node *whichstmt_node = get_child(astn, 1);
		ast_node *whichstmt_data = (ast_node *) get_data(whichstmt_node);
		
		char *var_name = id_data->ltk->lexeme;
		common_id_entry *entry = find_id_rec(var_name, curr_scope);
		if (entry == NULL) {
			// TODO: undeclared id
		}
		else {
			if (whichstmt_data->label.gms.nt == lvalueIDStmt) {
				if (entry->is_array) {
					// TODO: type err - should not be array
				}
				else {
					tree_node *expr_node = get_child(whichstmt_node, 0);
					// TODO: generate assembly code to compute expr value
					// and assign it to the id here
				}
			}
			else if (whichstmt_data->label.gms.nt == lvalueARRStmt) {
				if (!entry->is_array) {
					// TODO: type err - should be array
				}
				else {
					tree_node *index_node = get_child(whichstmt_node, 0);
					tree_node *expr_node = get_child(whichstmt_node, 1);

					ast_leaf *index_data = (ast_leaf *) get_data(index_node);
					if (index_data->label.gms.t == NUM) {
						// TODO:
					}
					else if (index_data->label.gms.t == ID) {
						// TODO:
					}
					// TODO: generate asm code to compute expr
					// and assign it to array element of given index
				}
			}
		}
	}

	if (ast_nt == moduleReuseStmt) {
		tree_node *opt_node = get_child(astn, 0);
		ast_leaf *id_data = (ast_leaf *) get_data(get_child(astn, 1));

		char *func_name = id_data->ltk->lexeme;
		func_entry *fentry = fetch_from_hash_map(main_st, func_name);

		if (fentry == NULL) {
			// TODO: undeclared func err
		}

		// OUTPUT PARAMS - return values
		if (get_data(opt_node) != NULL) {
			linked_list *ret_var_ll = ((ast_node *) get_data(opt_node))->ll;
			linked_list *op_ll = fentry->output_param_list;

			if (ret_var_ll->num_nodes != op_ll->num_nodes) {
				// TODO: params count mismatch err
			}
			else {
				int pcnt = op_ll->num_nodes;
				for (int i = 0; i < pcnt; i++) {
					tree_node *ret_var_lnode = (tree_node *) ll_get(ret_var_ll, i);
					char *ret_var_name = ((ast_leaf *) get_data(ret_var_lnode))->ltk->lexeme;
					common_id_entry *ret_var_entry = find_id_rec(ret_var_name, curr_scope);
					if (ret_var_entry == NULL) {
						// TODO: undeclared var
					}
					else {
						param_node *op_lnode = (param_node *) ll_get(op_ll, i);
						if (
								(ret_var_entry->is_array != op_lnode->is_array) ||
								(ret_var_entry->is_array && 
								 !match_array_type(ret_var_entry->entry.arr_entry, op_lnode->param.arr_entry)
								 ) ||
								(!ret_var_entry->is_array &&
								 ret_var_entry->entry.var_entry->type != op_lnode->param.var_entry->type
								 )
						   ) {
							// TODO: type mismatch between function call and assign
						}
					}
				}
			}
		}

		// INPUT PARAMS - args
		linked_list *arg_var_ll = ((ast_node *) get_data(get_child(astn, 2)))->ll;
		linked_list *ip_ll = fentry->input_param_list;

		if (arg_var_ll->num_nodes != ip_ll->num_nodes) {
			// TODO: params count mismatch err
		}
		else {
			int pcnt = ip_ll->num_nodes;
			for (int i = 0; i < pcnt; i++) {
				tree_node *arg_var_lnode = (tree_node *) ll_get(arg_var_ll, i);
				char *arg_var_name = ((ast_leaf *) get_data(arg_var_lnode))->ltk->lexeme;
				common_id_entry *arg_var_entry = find_id_rec(arg_var_name, curr_scope);
				if (arg_var_entry == NULL) {
					// TODO: undeclared var
				}
				else {
					param_node *ip_lnode = (param_node *) ll_get(ip_ll, i);
					if (
							(arg_var_entry->is_array != ip_lnode->is_array) ||
							(arg_var_entry->is_array && 
							 !match_array_type(arg_var_entry->entry.arr_entry, ip_lnode->param.arr_entry)
							 ) ||
							(!arg_var_entry->is_array &&
							 arg_var_entry->entry.var_entry->type != ip_lnode->param.var_entry->type
							 )
					   ) {
						// TODO: type mismatch between function call and assign
					}
				}
			}
		}
	}
    
    if (ast_nt == expression)
    {
    }

    if (ast_nt == unary_nt)
    {
    }

    if (ast_nt == new_NT)
    {
    }

    if (ast_nt == unary_op)
    {
    }

    if (ast_nt == arithmeticOrBooleanExpr)
    {
    }

    if (ast_nt == arithmeticOrBooleanExpr2)
    {
    }

    if (ast_nt == AnyTerm)
    {
    }

    if (ast_nt == AnyTerm2)
    {
    }

    if (ast_nt == arithmeticExpr)
    {
    }

    if (ast_nt == arithmeticExpr2)
    {
    }

    if (ast_nt == term)
    {
    }

    if (ast_nt == term2)
    {
    }

    if (ast_nt == factor)
    {
    }

    if (ast_nt == op1)
    {
    }

    if (ast_nt == op2)
    {
    }

    if (ast_nt == logicalOp)
    {
    }

    if (ast_nt == relationalOp)
    {
    }

	if (ast_nt == declareStmt) {
		linked_list *id_ll = ((ast_node *) get_data(get_child(astn, 0)))->ll;
		tree_node *dtype_node = get_child(astn, 1);
		id_type dtype = get_type_from_node(dtype_node);

		int cnt = id_ll->num_nodes;
		for (int i = 0; i < cnt; i++) {
			tree_node *id_lnode = (tree_node *) ll_get(id_ll, i);
			char *id_var_name = ((ast_leaf *) get_data(id_lnode))->ltk->lexeme;
			common_id_entry *id_var_entry = find_id_rec(id_var_name, curr_scope);
			if (id_var_entry != NULL) {
				// TODO: redeclaration error 
			}
			else {
				if (dtype == array) {
					tree_node *range_arrays = get_child(dtype_node, 0);
					int *range_indices = get_range_from_node(range_arrays);

					
					arr_id_entry *entry = create_arr_entry(
							id_var_name,
							get_type_from_node(get_child(dtype_node, 1)),
							range_indices[0],
							range_indices[1],
							-1, -1);
					add_to_hash_map(curr_scope->arr_st, id_var_name, entry);
				}
				else {
					var_id_entry *entry = create_var_entry(id_var_name, dtype, -1, -1);
					add_to_hash_map(curr_scope->var_id_st, id_var_name, entry);
				}
			}
		}
	}

	if (ast_nt == condionalStmt) {
		tree_node *id_node = get_child(astn, 0);
		tree_node *casestmt_node = get_child(astn, 1);
		tree_node *default_node = get_child(astn, 2);
		linked_list *casestmts_ll = ((ast_node *) get_data(casestmt_node))->ll;

		// using line_num to generate key for hash map
		int line_num = ((ast_leaf *) get_data(id_node))->ltk->line_num;
		char str_line_num[25];
		sprintf(str_line_num, "%d", line_num);
		scope_node *new_scope = create_new_scope(curr_scope);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		id_type var_type = get_type_from_node(id_node);
		if (var_type == real || var_type == array) {
			// TODO: type err switch var cant be these
		}
		else if (var_type == integer) {
			for (int i = 0; i < casestmts_ll->num_nodes; i++) {
				tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
				tree_node *val_node = get_child(cstmt_node, 0);
				tree_node *stmts_node = get_child(cstmt_node, 1);

				id_type val_type = get_type_from_node(val_node); // can only boolean or integer
																// because of grammar
				if (val_type != integer) {
					// TODO: type error, should be integer
				}

				linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
				for (int j = 0; j < stmts_ll->num_nodes; j++)
					symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
			}

			if (get_data(default_node) == NULL) {
				// TODO: err required default stmt
			}
			else {

			}
		}
		else if (var_type == boolean) {
			if (casestmts_ll->num_nodes != 2) {
				// TODO: err need exactly 2 case stmts (true/false)
			}

			bool got_true = false, got_false = false;
			for (int i = 0; i < 2 && i < casestmts_ll->num_nodes; i++) {
				tree_node *cstmt_node = (tree_node *) ll_get(casestmts_ll, i);
				tree_node *val_node = get_child(cstmt_node, 0);
				tree_node *stmts_node = get_child(cstmt_node, 1);

				id_type val_type = get_type_from_node(val_node); // can only boolean or integer
																// because of grammar
				if (val_type != boolean) {
					// TODO: type error, should be integer
				}

				terminal val_term = ((ast_leaf *) get_data(val_node))->label.gms.t;
				got_true |= (val_term == TRUE);
				got_false |= (val_term == FALSE);

				linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
				for (int j = 0; j < stmts_ll->num_nodes; j++)
					symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
			}

			if (!got_true || !got_false) {
				// TODO: true false both cases required
			}

			if (get_data(default_node) != NULL) {
				// TODO: err should not have default stmt
			}
		}
		else {
			// TODO: unknown err
		}

		//
		// code gen
		//
	}

	if (ast_nt == for_loop) {
		tree_node *id_node = get_child(astn, 0);
		ast_leaf *id_data = get_data(id_node, 0);
		tree_node *range_node = get_child(astn, 1);
		tree_node *stmts_node = get_child(astn, 2);

		// using line_num to generate key for hash map
		int line_num = id_data->ltk->line_num;
		char str_line_num[25];
		sprintf(str_line_num, "%d", line_num);
		scope_node *new_scope = create_new_scope(curr_scope);
		add_to_hash_map(curr_scope->child_scopes, str_line_num, new_scope);

		// insert loop var in scope
		var_id_entry *entry = create_var_entry(id_data->ltk->lexeme, integer, -1, -1);
		add_to_hash_map(new_scope->var_id_st, id_data->ltk->lexeme, entry);

		int range_indices[2] = get_range_from_node(get_child(range_node));
		// TODO: what to do with range here for code generation
		// and statements (below)
		//
		// ??
		linked_list *stmts_ll = ((ast_node *) get_data(stmts_node))->ll;
		for (int j = 0; j < stmts_ll->num_nodes; j++)
			symbol_table_fill(main_st, ll_get(stmts_ll, j), new_scope);
	}

	if (ast_nt == while_loop) {
		// TODO: need line_num here to create new scope
		tree_node *aobexpr_node = get_child(astn, 0);
		tree_node *stmts_node = get_child(astn, 1);
	}
}

void create_symbol_table (tree_node *astn) {
	hash_map *main_st = create_hash_map(DEFAULT_ST_SIZE);

	// default entry for driver module
	func_entry *st_entry = create_func_entry("program", true, false, -1, -1);
	add_to_hash_map(main_st, st_entry->name, st_entry);

	symbol_table_fill (main_st, astn, NULL);
}
