/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"
# include <string>

using namespace std;

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);
Type checkMultiplicative(const Type& left, const Type& right, const string& op);
Type checkEquality(const Type& left, const Type& right, const string& op);
Type checkRelational(const Type& left, const Type& right, const string& op);
Type checkLogical(const Type& left, const Type& right, const string& op);
Type checkPostfix(const Type& operand, const Type& expr);
Type checkAdditive(const Type& left, const Type& right, const string& op);
Type checkPrefix(const Type& operand);
Type checkDeref(const Type& operand, bool& lvalue);
Type checkNot(const Type& operand, bool& lvalue);
Type checkNeg(const Type& operand, bool& lvalue);
Type checkAddr(const Type& operand, bool& lvalue);
Type checkSizeof(const Type& operand, bool& lvalue);
# endif /* CHECKER_H */
