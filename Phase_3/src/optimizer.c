#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "GAS_utility.h"


char s[32768];
typedef struct IR_list
{
    char *ss[10];
    struct IR_list *next, *prev;
} IR_list;

char ss[10][32768];
int lss;

IR_list *new_IR_list(IR_list *prev)
{
    IR_list *p = (IR_list *)malloc(sizeof(IR_list));
    for (int i = 0; i < 10; i++)
    {
        size_t ll = strlen(ss[i]);
        if (ll == 0 || i>=lss)
        {
            p->ss[i] = NULL;
        }
        else
        {
            p->ss[i] = (char *)malloc(sizeof(char) * (ll + 1));
            strcpy(p->ss[i], ss[i]);
        }
    }
    p->next = NULL;
    p->prev = prev;
    if (prev != NULL)
    {
        prev->next = p;
    }
    return p;
}

inline static char is_ws(const char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

void split_str(const char *s)
{
    int l1 = 0, l2 = 0, ls = 0;
    while (s[ls])
    {
        while (is_ws(s[ls]))
            ++ls;

        while (s[ls] && !is_ws(s[ls]))
        {
            ss[l1][l2] = s[ls];
            ++l2;
            ++ls;
        }
        ss[l1][l2] = 0;
        l2 = 0;
        ++l1;
    }
    lss=l1;
}

void del_list(IR_list *p)
{
    if (p->prev != NULL)
    {
        p->prev->next = p->next;
    }
    if (p->next != NULL)
    {
        p->next->prev = p->prev;
    }
    for(int i=0;i<10;++i)
        if(p->ss[i] != NULL)
            free(p->ss[i]);
    free(p);
}

size_t get_list_len(IR_list *list)
{
    for (int i = 0; ; i++)
        if (list->ss[i] == NULL)
            return i;
}


bool opt_if(IR_list *u)
{
    bool flg = false;
    while (u != NULL)
    {
        if (strcmp(u->ss[0], "IF") == 0)
        {
            // if (u->next != NULL && strcmp(u->next->ss[0], "GOTO") == 0 &&
            //     u->next->next != NULL && strcmp(u->next->next->ss[0], "LABEL") == 0)
            // {
            //     if (strcmp(u->ss[5], u->next->next->ss[1]) == 0)
            //     {
            //         free(u->ss[5]);
            //         int ll = strlen(u->next->ss[1]);
            //         u->ss[5] = (char *)malloc(sizeof(char) * (ll + 1));
            //         strcpy(u->ss[5], u->next->ss[1]);
            //         u->ss[5][ll] = 0;

            //         del_list(u->next);

            //         if (u->ss[2][0] == '=')
            //         {
            //             u->ss[2][0] = '!';
            //         }
            //         else if (u->ss[2][0] == '!')
            //         {
            //             u->ss[2][0] = '=';
            //         }
            //         else if (u->ss[2][0] == '<')
            //         {
            //             free(u->ss[2]);
            //             if(u->ss[2][1] == '='){
            //                 u->ss[2] = (char*)malloc(sizeof(char)*2);
            //                 u->ss[2][0]='>';
            //                 u->ss[2][1]=0;
            //             } else {
            //                 u->ss[2] = (char*)malloc(sizeof(char)*3);
            //                 u->ss[2][0]='>';
            //                 u->ss[2][1]='=';
            //                 u->ss[2][2]=0;
            //             }
            //         }
            //         else if (u->ss[2][0] == '>')
            //         {
            //             free(u->ss[2]);
            //             if(u->ss[2][1] == '='){
            //                 u->ss[2] = (char*)malloc(sizeof(char)*2);
            //                 u->ss[2][0]='<';
            //                 u->ss[2][1]=0;
            //             } else {
            //                 u->ss[2] = (char*)malloc(sizeof(char)*3);
            //                 u->ss[2][0]='<';
            //                 u->ss[2][1]='=';
            //                 u->ss[2][2]=0;
            //             }
            //         }
            //         flg = 1;
            //     }
            // }
            if (u->next != NULL && strcmp(u->next->ss[0], "LABEL") == 0){
                if(strcmp(u->ss[5], u->next->ss[1]) == 0){
                    /* IF xxx GOTO Lu
                       LABEL: Lu*/
                    u=u->next;
                    del_list(u->prev);
                    flg = true;
                }
            }
        }
        if(strcmp(u->ss[0], "GOTO") == 0){
            if (u->next != NULL && strcmp(u->next->ss[0], "LABEL") == 0){
                if(strcmp(u->ss[1], u->next->ss[1]) == 0){
                    /* GOTO Lu
                       LABEL: Lu*/
                    u=u->next;
                    del_list(u->prev);
                    flg = true;
                }
            }
        }
        u = u->next;
    }
    return flg;
}


typedef struct _Var
{
    char name[7]; // "?65535\0"
    Var *parent[2];
    IR_list *recent_ir, parent_ir[2];
    bool constant;
    size_t val;
} Var;

Var v_var[USHRT_MAX], t_var[USHRT_MAX];
bool useful_v[USHRT_MAX], useful_t[USHRT_MAX];


Var *get_var(const char *name)
{
    switch (name[0])
    {
        case 'v':
            return &v_var[atoi(name + 1)];
        case 't':
            return &t_var[atoi(name + 1)];
        case '#':
            return new_contant_var(name + 1);
        case '&':
        case '*':
            return get_var(name + 1);
    }
}

bool equal_var(Var *a, Var *b)
{
    // TODO
    return false;
}

char *val_to_const(size_t val)
{
    size_t len = mlg10(val);
    char *ss = (char *)malloc(sizeof(char) * (len + 2));
    for (size_t i = 0; i < len; i++)
    {
        ss[i] = val % 10;
        val /= 10;
    }
    ss[len] = '#';
    reverse_str(ss, len + 1);
    return ss;
}

size_t calc(size_t a, size_t b, char op)
{
    switch (op)
    {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
    }
}

void simplify_IR(IR_list *ir)
{
    Var *x = get_var(ir->ss[0]);
    x->recent_ir = ir;
    switch (get_list_len(ir->ss))
    {
        case 3:
            if (ir->ss[2][0] == '&' || ir->ss[2][0] == '*')
                break;
            Var *y = get_var(ir->ss[3]);
            if (y->constant = true)
            {
                ir->ss[3] = val_to_const(y->val);
            }
            if (ir->ss[0][0] != '*')
                *x = *y;
            break;
        case 5:
            Var *y = get_var(ir->ss[2]), *z = get_var(ir->ss[4]);
            char op = ir->ss[3];
            if (y->constant && z->constant)
            {
                x->constant = true;
                x->val = (y->val, z->val, op);
                for (size_t i = 2; i <= 4; i++)
                {
                    free(ir->ss[i]);
                    ir->ss[i] = NULL;
                }
                ir->ss[2] = val_to_const(x->val);
            }
            else if ((op == '*' && ((y->constant && y->val == 0) || (z->constant && z->val == 0))) ||
                     (op == '-' && equal_var(y, z)) ||
                     (op == '/' && y->constant && y->val == 0))
            {
                x->constant = true;
                x->val = 0;
                for (size_t i = 2; i <= 4; i++)
                {
                    free(ir->ss[i]);
                    ir->ss[i] = NULL;
                }
                ir->ss[2] = val_to_const(0);
            }
            break;
    }
}

char opt_exp(IR_list *u)
{
    memset(v_var, 0, sizeof(v_var));
    memset(t_var, 0, sizeof(t_var));
    IR_list *ir, *tail;

    // pos: simplify const ops
    while (ir != NULL)
    {
        if (strcmp(ir->ss[1], ":=") == 0)
        {
            if (strcmp(ir->ss[2], "CALL"));
            else
                simplify_IR(ir);
        }

        if (ir->next == NULL)
            tail = ir;
        ir = ir->next;
    }

    // neg: remove useless vars
    ir = tail;
    while (ir != NULL)
    {
        if (strcmp(ir->ss[1], ":=") == 0);
        else if (strcmp(ir->ss[0], "LABEL") == 0);
        else if (strcmp(ir->ss[0], "FUNCTION") == 0);
        else if (strcmp(ir->ss[0], "GOTO") == 0);
        else if (strcmp(ir->ss[0], "IF") == 0);
        else if (strcmp(ir->ss[0], "RETURN") == 0);
        else if (strcmp(ir->ss[0], "DEC") == 0);
        else if (strcmp(ir->ss[0], "PARAM") == 0);
        else if (strcmp(ir->ss[0], "ARG") == 0);
        else if (strcmp(ir->ss[0], "READ") == 0);
        else if (strcmp(ir->ss[0], "WRITE") == 0)
        ir = ir->prev;
    }
    for (int i = 0; i < useful_cnt; i++)
        inhert_useful(useful_var[i]);
    
    return 0;
}

void output_list(const IR_list *u, FILE *fout)
{
    while (u != NULL)
    {
        for (int i = 0; u->ss[i] != NULL; ++i)
        {
            fprintf(fout, "%s ", u->ss[i]);
        }
        fprintf(fout, "\n");
        u = u->next;
    }
}

void optimize(FILE *fin, FILE *fout)
{
    IR_list *rootw = NULL, *nowp;
    while (fscanf(fin, "%[^\n]", s) != EOF)
    {
        fscanf(fin, "%*c");
        split_str(s); // similar to ss = s.split(whitespace), ss : list
        if (rootw == NULL)
        {
            rootw = new_IR_list(NULL);
            nowp = rootw;
        }
        else
        {
            nowp = new_IR_list(nowp);
        }
    }

    while (1)
    {
        if (opt_if(rootw))
        {
            continue;
        }
        else if (opt_exp(rootw))
        {
            continue;
        }
        break;
    }

    output_list(rootw, fout);

    // if (strncmp(s, "LABEL", 5) == 0)
    // {
    //     // LABEL
    // }
    // else if (strncmp(s, "GOTO", 4) == 0)
    // {
    //     // GOTO
    // }
    // else if (strncmp(s, "IF", 2) == 0)
    // {
    //     // IF
    // }
    // else if (strncmp(s, "RETURN", 6) == 0)
    // {
    //     // RETURN
    // }
    // else if (strncmp(s, "DEC", 3) == 0)
    // {
    //     // DEC
    // }
    // else if (strncmp(s, "PARAM", 5) == 0)
    // {
    //     // PARAM
    // }
    // else if (strncmp(s, "ARG", 3) == 0)
    // {
    //     // ARG
    // }
    // else if (strncmp(s, "READ", 4) == 0)
    // {
    //     // READ
    // }
    // else if (strncmp(s, "WRITE", 5) == 0)
    // {
    //     // WRITE
    // }
    // else
    // {
    //     // :=
    // }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "useage: %s <in_file> <out_file>\n", argv[0]);
        exit(1);
    }
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    if (fin == NULL || fout == NULL)
    {
        fprintf(stderr, "Open file error\n");
        exit(0);
    }
    optimize(fin, fout);
    return 0;
}