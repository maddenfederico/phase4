/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		If a symbol is redeclared, the redeclaration is discarded
 *		and the original declaration is retained.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include <string>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static Scope *outermost, *toplevel;
static const Type error;

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string void_object = "'%s' has type void";

static string bad_unary = "invalid operand to unary %s";
static string bad_binary = "invalid operands to binary %s";
static string bad_lvalue = "lvalue required in expression";


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;
    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);
    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();

    } else
	delete type.parameters();

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	if (type.specifier() == VOID && type.indirection() == 0)
	    report(void_object, name);

	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkMultiplicative(const Type& left, const Type& right, const string& op)
{
	if(left.promote().isInteger() && right.promote().isInteger())
		return left;
	else{
		report(bad_binary, op);
		return error;
	}
}

Type checkEquality(const Type& left, const Type& right, const string& op)
{
	if(left.isCompatibleWith(right))
		return Type(INT);

	report(bad_binary, op);
	return error;
}

Type checkRelational(const Type& left, const Type& right, const string& op)
{
	Type l = left.promote();
	Type r = right.promote();

	if(l.isValue() && r.isValue()){
		
		if(l.specifier() == r.specifier())
			return Type(INT);
	}
	report(bad_binary, op);
	return error;

}

Type checkLogical(const Type& left, const Type& right, const string& op)
{
	Type l = left.promote();
	Type r = right.promote();

	if(l.isValue() && r.isValue())
		return Type(INT);
	
	report(bad_binary, op);
	return error;
}

Type checkPostfix(const Type& operand, const Type& expr)
{
	Type o = operand.promote();
	Type e = expr.promote();

	if(o.isPointer() && (o.specifier() != VOID)){
		
		if(e.specifier() == INT && e.indirection() == 0)
			return o;
	}
	report(bad_binary, "[]");
	return error;
}

Type checkAdditive(const Type& left, const Type& right, const string& op)
{
	Type l = left.promote();
	Type r = right.promote();

	if(l.isPointer() && l.specifier() != VOID){
		if(r.isInteger())
			return Type(l.specifier(), l.indirection());

		else if(op == "-" && r.isPointer() && r.specifier() == l.specifier())
			return Type(INT);

	}else if(l.isInteger()){
		if(r.isInteger())
			return Type(INT);

		else if(op == "+" && r.isPointer() && r.specifier() != VOID)
			return Type(r.specifier(), r.indirection());
	}
	report(bad_binary, op);
	return error;
}	

Type checkDeref(const Type& operand, bool& lvalue)
{
	Type o = operand.promote();
	if(o.isPointer() && o.specifier() != VOID){
		lvalue = true;
		return Type(o.specifier(), o.indirection() - 1);
	}
	
	report(bad_unary, "*");
	return error;
}

Type checkAddr(const Type& operand, bool& lvalue){

	if(lvalue){
		lvalue = false;
		return Type(operand.specifier(), operand.indirection() + 1);
	}
	report(bad_unary, "&");
	return error;
}

Type checkNot(const Type& operand, bool& lvalue){
	lvalue = false;
	if(operand.isValue())
		return Type(INT);
	
	report(bad_unary, "!");
	return error;
}

Type checkNeg(const Type& operand, bool& lvalue)
{
	lvalue = false;
	if(operand.promote().isInteger())
		return Type(INT);

	report(bad_unary, "-");
	return error;
}


Type checkSizeof(const Type& operand, bool& lvalue)
{
	lvalue = false;
	if(operand.isValue())
		return Type(INT);

	report(bad_unary, "sizeof");
	return error;
}


