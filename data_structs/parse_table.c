#include "parse_table.h"

parse_table *create_parse_table(int num_rows, int num_cols)
{
    parse_table *pt = calloc(1, sizeof(parse_table));

    pt->num_rows = num_rows;
    pt->num_cols = num_cols;

    pt->matrix = (gm_rule ***)calloc(num_rows, sizeof(gm_rule **));

    for (int i = 0; i < num_rows; i++)
    {
        pt->matrix[i] = (gm_rule **)calloc(num_cols, sizeof(gm_rule *));
        for (int j = 0; j < num_cols; j++)
        {
            pt->matrix[i][j] = NULL;
        }
    }

    return pt;
}

void add_to_parse_table(parse_table *pt, gm_rule *rule, int r, int c)
{
    assert(pt->matrix[r][c] == NULL || pt->matrix[r][c] == rule, "parse table rule collision didn't occur");

    pt->matrix[r][c] = rule;
}

gm_rule *get_from_parse_table(parse_table *pt, int r, int c)
{
    return pt->matrix[r][c];
}

void destory_parse_table(parse_table *pt)
{
    for (int i = 0; i < pt->num_rows; i++)
    {
        for (int j = 0; j < pt->num_cols; j++)
        {
            pt->matrix[i][j] = NULL;
        }
        free(pt->matrix[i]);
    }
    free(pt->matrix);
    free(pt);
}