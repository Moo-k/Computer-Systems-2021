//
//
// Author: a1805637
// Name: Yuanxi Wang
//

#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

#pragma clang diagnostic ignored "-Wunused-function"

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and generate the equivalent Hack Virtual Machine Code.
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate visit_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
static void visit_class(ast t) ;
static void visit_class_var_decs(ast t) ;
static void visit_var_dec(ast t) ;
static void visit_subr_decs(ast t) ;
static void visit_subr(ast t) ;
static void visit_constructor(ast t) ;
static void visit_function(ast t) ;
static void visit_method(ast t) ;
static void visit_param_list(ast t) ;
static void visit_subr_body(ast t) ;
static void visit_var_decs(ast t) ;
static void visit_statements(ast t) ;
static void visit_statement(ast t) ;
static void visit_let(ast t) ;
static void visit_let_array(ast t) ;
static void visit_if(ast t) ;
static void visit_if_else(ast t) ;
static void visit_while(ast t) ;
static void visit_do(ast t) ;
static void visit_return(ast t) ;
static void visit_return_expr(ast t) ;
static void visit_expr(ast t) ;
static void visit_term(ast t) ;
static void visit_int(ast t) ;
static void visit_string(ast t) ;
static void visit_bool(ast t) ;
static void visit_null(ast t) ;
static void visit_this(ast t) ;
static void visit_unary_op(ast t) ;
static void visit_var(ast t) ;
static void visit_array_index(ast t) ;
static void visit_call_as_function(ast t) ;
static void visit_call_as_method(ast t) ;
static void visit_subr_call(ast t) ;
static void visit_expr_list(ast t) ;
static void visit_infix_op(ast t) ;

// walk an ast class node with fields:
// class_name - a string
// var_decs   - ast vector of variable declarations
// subr_decs  - ast vector of subroutine declarations
//
static int thisCount = 0 ;
static int ifCount ;
static int whileCount ;
// global variables to track total number of if and while calls, as well as this declarations
static string myclassname = "" ;
// global string variable to track class name, for printing function calls

static void visit_class(ast t)
{
    myclassname = get_class_class_name(t) ;
    ast statics = get_class_statics(t) ;
    ast fields = get_class_fields(t) ;
    ast subr_decs = get_class_subr_decs(t) ;

    visit_class_var_decs(statics) ;
    visit_class_var_decs(fields) ;
    visit_subr_decs(subr_decs) ;
}

// walk an ast class var decs node
// it is an ast vector of variable declarations
//
static void visit_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        visit_var_dec(get_class_var_decs(t,i)) ;
    }
}

// walk an ast variable declaration with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
// this is used for statics, fields, parameters and local variables
//
static void visit_var_dec(ast t)
{
    // string name = get_var_dec_name(t) ;
    // string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    // int offset = get_var_dec_offset(t) ;
    if ( segment == "this" )
    {
        thisCount++ ;
    }
}

// walk an ast class var decs node
// it is an ast vector of subroutine declarations
//
static void visit_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        visit_subr(get_subr_decs(t,i)) ;
    }
}

// walk an ast subroutine node with a single field
// subr - an ast constructor, ast function or ast method node
//
static void visit_subr(ast t)
{
    ast subr = get_subr_subr(t) ;

    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        visit_constructor(subr) ;
        break ;
    case ast_function:
        visit_function(subr) ;
        break ;
    case ast_method:
        visit_method(subr) ;
        break ;
    default:
        fatal_error(0,"Unexpected subroutine kind") ;
        break ;
    }
}

// walk an ast constructor node with fields
// vtype - a string - the name of the class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static void visit_constructor(ast t)
{
    // initialize while and if counters to -1 for each subroutine (incremented at the start of each if and while)
    whileCount = -1 ;
    ifCount = -1 ;

    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;

    int vars = size_of_var_decs(get_subr_body_decs(subr_body)) ;
    write_to_output("function " + myclassname + "." + name + " " + to_string(vars) + "\npush constant " + to_string(thisCount) + "\ncall Memory.alloc 1\npop pointer 0\n") ;

    visit_param_list(param_list) ;
    visit_subr_body(subr_body) ;
}

// walk an ast function node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static void visit_function(ast t)
{
    whileCount = -1 ;
    ifCount = -1 ;

    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;

    int vars = size_of_var_decs(get_subr_body_decs(subr_body)) ;
    write_to_output("function " + myclassname + "." + name + " " + to_string(vars) + "\n") ;

    visit_param_list(param_list) ;
    visit_subr_body(subr_body) ;
}

// walk an ast method node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static void visit_method(ast t)
{
    whileCount = -1 ;
    ifCount = -1 ;

    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;

    int vars = size_of_var_decs(get_subr_body_decs(subr_body)) ;
    write_to_output("function " + myclassname + "." + name + " " + to_string(vars) + "\npush argument 0\npop pointer 0\n");

    visit_param_list(param_list) ;
    visit_subr_body(subr_body) ;
}

// walk an ast param list node
// it is an ast vector of variable declarations
//
static void visit_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        visit_var_dec(get_param_list(t,i)) ;
    }
}

// walk an ast subr body node with fields
// decs - an ast vector of variable declarations
// body - an ast vector of statement nodes
//
static void visit_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    visit_var_decs(decs) ;
    visit_statements(body) ;
}

// walk an ast param list node
// it is an ast vector of variable declarations
//
static void visit_var_decs(ast t)
{
    int ndecs = size_of_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        visit_var_dec(get_var_decs(t,i)) ;
    }
}

// walk an ast statements node
// it is an ast vector of statement nodes
//
static void visit_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        visit_statement(get_statements(t,i)) ;
    }
}

// walk an ast statement node with a single field
// statement - one of the following ast nodes, let, let array, if, if else, while, do, return, return expr or statements
//
static void visit_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    switch(ast_node_kind(statement))
    {
    case ast_let:
        visit_let(statement) ;
        break ;
    case ast_let_array:
        visit_let_array(statement) ;
        break ;
    case ast_if:
        visit_if(statement) ;
        break ;
    case ast_if_else:
        visit_if_else(statement) ;
        break ;
    case ast_while:
        visit_while(statement) ;
        break ;
    case ast_do:
        visit_do(statement) ;
        break ;
    case ast_return:
        visit_return(statement) ;
        break ;
    case ast_return_expr:
        visit_return_expr(statement) ;
        break ;
    case ast_statements:
        visit_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        break ;
    }
}

// walk an ast let node with fields
// var  - an ast variable
// expr - an ast expr node
//
static void visit_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;

    visit_expr(expr) ;
    // pop instead of push to assign value to var
    write_to_output("pop " + get_var_segment(var) + " " + to_string(get_var_offset(var))+ "\n") ;
}

// walk an ast let array node with fields
// var    - an ast variable
// index  - an ast expr node
// expr   - an ast expr node
//
static void visit_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    visit_var(var) ;
    visit_expr(index) ;
    visit_expr(expr) ;
}

// walk an ast if node with fields
// condition - an ast expr node
// if true   - an ast statements node
//
static void visit_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;
    ifCount++ ;
    int temp = ifCount ;

    visit_expr(condition) ;
    write_to_output("if-goto IF_TRUE" + to_string(temp) + "\ngoto IF_FALSE" + to_string(temp) + "\nlabel IF_TRUE" + to_string(temp) + "\n") ;
    visit_statements(if_true) ;
    write_to_output("label IF_FALSE" + to_string(temp) + "\n") ;
}

// walk an ast if else node with fields
// condition - an ast expr node
// if true   - an ast statements node
// if else   - an ast statements node
//
static void visit_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;
    ifCount++ ;
    int temp = ifCount ;

    visit_expr(condition) ;
    write_to_output("if-goto IF_TRUE" + to_string(temp) + "\ngoto IF_FALSE" + to_string(temp) + "\nlabel IF_TRUE" + to_string(temp) + "\n") ;
    visit_statements(if_true) ;
    write_to_output("goto IF_END" + to_string(temp) + "\nlabel IF_FALSE" + to_string(temp) + "\n") ;
    visit_statements(if_false) ;
    write_to_output("label IF_END" + to_string(temp) + "\n") ;
}

// walk an ast while node with fields
// condition - an ast expr node
// body      - an ast statements node
//
static void visit_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;
    whileCount++ ;
    int temp = whileCount ;

    write_to_output("label WHILE_EXP" + to_string(temp) + "\n");
    visit_expr(condition) ;
    write_to_output("not\nif-goto WHILE_END" + to_string(temp) + "\n" ) ;
    visit_statements(body) ;
    write_to_output("goto WHILE_EXP" + to_string(temp) + "\nlabel WHILE_END" + to_string(temp) + "\n" ) ;
}

// walk an ast do node with a single field
// call - an ast call as function node or an ast call as method node
//
static void visit_do(ast t)
{
    ast call = get_do_call(t) ;

    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        visit_call_as_function(call) ;
        break ;
    case ast_call_as_method:
        visit_call_as_method(call) ;
        break ;
    default:
        fatal_error(0,"Unexpected call kind") ;
        break ;
    }
    write_to_output("pop temp 0\n") ;
}

// walk an ast return node, it has not fields
//
static void visit_return(ast t)
{
    write_to_output("push constant 0\nreturn\n") ;
}

// walk an ast return expr node with a single field
// expr - an ast expr node
//
static void visit_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    visit_expr(expr) ;
    write_to_output("return\n") ;
}

// walk an ast param list node
// it is an ast vector of ast term and infix op nodes
// it must be of odd length > 0, ie 1, 3, 5, 7, ...
// all elements at even indices are an ast term node
// all elements at odd indices are an ast infix op
//
static void visit_expr(ast t)
{
    int term_ops = size_of_expr(t) ;
    int count = 0;
    ast temp;
    for ( int i = 0 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;

        if ( i % 2 == 0 )
        {
            count++;
            visit_term(term_op) ;
        }
        else
        {
            temp = term_op;
        }

        if ( count == 2 )
        {
            count = 1 ;
            visit_infix_op(temp);
        }
    }
}

// walk an ast term node with a single field
// term - one of the following ast nodes:
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
static void visit_term(ast t)
{
    ast term = get_term_term(t) ;

    switch(ast_node_kind(term))
    {
    case ast_int:
        visit_int(term) ;
        break ;
    case ast_string:
        visit_string(term) ;
        break ;
    case ast_bool:
        visit_bool(term) ;
        break ;
    case ast_null:
        visit_null(term) ;
        break ;
    case ast_this:
        visit_this(term) ;
        break ;
    case ast_expr:
        visit_expr(term) ;
        break ;
    case ast_unary_op:
        visit_unary_op(term) ;
        break ;
    case ast_var:
        visit_var(term) ;
        break ;
    case ast_array_index:
        visit_array_index(term) ;
        break ;
    case ast_call_as_function:
        visit_call_as_function(term) ;
        break ;
    case ast_call_as_method:
        visit_call_as_method(term) ;
        break ;
    default:
        fatal_error(0,"Unexpected term kind") ;
        break ;
    }
}

// walk an ast int node with a single field
// constant - an integer in the range -32,768 to 32,767
//
static void visit_int(ast t)
{
    int _constant = get_int_constant(t) ;
    write_to_output("push constant " + to_string(_constant) + "\n") ;
}

// walk an ast string node with a single field
// constant - a string
//
static void visit_string(ast t)
{
    string _constant = get_string_constant(t) ;
    write_to_output("push constant " + to_string(_constant.length()) + "\ncall String.new 1\n") ;

    for ( int i = 0; i < _constant.length(); i++ )
    {
        write_to_output("push constant " + to_string((int)_constant[i]) + "\ncall String.appendChar 2\n") ;
    }
}

// walk an ast bool node with a single field
// constant - either true or false
//
static void visit_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    if ( _constant )
    {
        write_to_output("push constant 0\nnot\n") ;
    }
    else
    {
        write_to_output("push constant 0\n") ;
    }
}

// walk an ast null node, it has not fields
//
static void visit_null(ast t)
{
    write_to_output("push constant 0\n") ;
}

// walk an ast this node, it has not fields
//
static void visit_this(ast t)
{
    write_to_output("push pointer 0\n") ;
}

// walk an ast unary op node with fields
// op   - a string, either "-" or "~"
// term - an ast term node
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
static void visit_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;

    visit_term(term) ;

    if ( uop == "-")
    {
        write_to_output("neg\n") ;
    }
    else
    {
        write_to_output("not\n") ;
    }
}

// walk an ast variable node with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
//
static void visit_var(ast t)
{
    // string name = get_var_name(t) ;
    // string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;

    write_to_output("push " + segment + " " + to_string(offset) + "\n") ;
}

// walk an ast array index node with fields
// var   - an ast variable node
// index - an ast expr node
//
static void visit_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    visit_var(var) ;
    visit_expr(index) ;

    write_to_output("add\npop pointer 1\npush that 0\n");
}

// walk an ast subr call as method with fields
// class name - a string
// call       - an ast subr call node
//
static void visit_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;

    visit_subr_call(subr_call) ;
    write_to_output("call " + class_name + "." + subr_name + " " + to_string(size_of_expr_list(expr_list)) + "\n") ;
}

// walk an ast subr call as method with fields
// class name - a string
// var        - an ast variable or ast this node, hidden first parameter of the call
// call       - an ast subr call node
//
static void visit_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;
    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;

    switch(ast_node_kind(var))
    {
    case ast_this:
        visit_this(var) ;
        break ;
    case ast_var:
        visit_var(var) ;
        break ;
    default:
        fatal_error(0,"Expected var or this") ;
        break ;
    }
    visit_subr_call(subr_call) ;

    write_to_output("call " + class_name + "." + subr_name + " " + to_string(size_of_expr_list(expr_list) + 1) + "\n") ;
}

// walk an ast subr call node with fields
// name      - a string
// expr list - a vector of ast expr nodes
//
static void visit_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;

    visit_expr_list(expr_list) ;
}

// walk an ast expr list node
// it is an ast vector of ast expr nodes
//
static void visit_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        visit_expr(get_expr_list(t,i)) ;
    }
}

// walk an ast infix op node with a single field
// op - a string - one of "+", "-", "*", "/", "&", "|", "<", ">", "="
//
static void visit_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;

    if ( op == "+" )
    {
        write_to_output("add\n") ;
    }
    else if ( op == "-" )
    {
        write_to_output("sub\n") ;
    }
    else if ( op == "*" )
    {
        write_to_output("call Math.multiply 2\n") ;
    }
    else if ( op == "/" )
    {
        write_to_output("call Math.divide 2\n") ;
    }
    else if ( op == "&" )
    {
        write_to_output("and\n") ;
    }
    else if ( op == "|" )
    {
        write_to_output("or\n") ;
    }
    else if ( op == "<" )
    {
        write_to_output("lt\n") ;
    }
    else if ( op == ">" )
    {
        write_to_output("gt\n") ;
    }
    else if ( op == "=" )
    {
        write_to_output("eq\n") ;
    }

}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and print VM code
    visit_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}