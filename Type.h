/*
 * File:	Type.h
 *
 * Description:	This file contains the class definition for types in Simple
 *		C.  A type is either a scalar type, an array type, or a
 *		function type.  These types include a specifier and the
 *		number of levels of indirection.  Array types also have a
 *		length, and function types also have a parameter list.
 *		An error type is also supported for use in undeclared
 *		identifiers and the results of type checking.
 *
 *		By convention, a null parameter list represents an
 *		unspecified parameter list.  An empty parameter list is
 *		represented by an empty vector.
 *
 *		No subclassing is used to avoid the problem of object
 *		slicing (since we'll be treating types as value types) and
 *		the proliferation of small member functions.
 *
 *		As we've designed them, types are essentially immutable,
 *		since we haven't included any mutators.  In practice, we'll
 *		be creating new types rather than changing existing types.
 */

# ifndef TYPE_H
# define TYPE_H
# include <vector>
# include <ostream>

typedef std::vector<class Type> Parameters;

class Type {
    int _specifier = 0;
    unsigned _indirection = 0;
    unsigned _length = 0;
    Parameters *_parameters = nullptr;

    enum { ARRAY, ERROR, FUNCTION, SCALAR } _kind;

public:
    Type();
    Type(int specifier, unsigned indirection = 0);
    Type(int specifier, unsigned indirection, unsigned length);
    Type(int specifier, unsigned indirection, Parameters *parameters);

    bool operator ==(const Type &rhs) const;
    bool operator !=(const Type &rhs) const;

    bool isArray() const;
    bool isScalar() const;
    bool isFunction() const;
    bool isError() const;
	bool isInteger() const;
	bool isPointer() const;
	bool isValue() const;
    int specifier() const;
    unsigned indirection() const;
    unsigned length() const;
    Parameters *parameters() const;
	Type promote() const;
	bool isCompatibleWith(const Type& that) const;
};

std::ostream &operator <<(std::ostream &ostr, const Type &type);

# endif /* TYPE_H */
