#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

/**
 * Statements and Blocks
 * Statement ::= instr(str) | cond(str, block, block) | loop(str, block)
 * Block ::= list of Statements
 */

class Stmt;
using Statement = std::shared_ptr<Stmt>;
using Block     = std::list<Statement>;

// Instr Statement
Statement instr(std::string c);
bool      isInstr(const Statement&, std::string&);

// Cond Statement
Statement cond(std::string c, Block t);
Statement cond(std::string c, Block t, Block e);
bool      isIf(const Statement&, std::string&, Block&, Block&);

// Loop Statement
Statement loop(std::string h, Block b);
bool      isLoop(const Statement&, std::string&, Block&);

// A Block is a list of Statements
Block block(const Statement& s);

// Concatenation of blocks
Block operator+(const Statement& a, const Statement& b);
Block operator+(const Statement& a, const Block& b);
Block operator+(const Block& a, const Statement& b);
Block operator+(const Block& a, const Block& b);

// Pretty printing Statements and Blocks
void          setGlobalIndent(int n);
std::ostream& operator<<(std::ostream& dst, const Statement& s);
std::ostream& operator<<(std::ostream& dst, const Block& l);

// Lift common conditions in statements and blocks
Statement lift(const Statement& s);
Block     lift(const Block& s);

// Conversion from Statement to Block
inline Statement loop(std::string h, const Statement& b)
{
    return loop(std::move(h), block(b));
}

inline Statement cond(std::string c, const Statement& t, Block e)
{
    return cond(std::move(c), block(t), std::move(e));
}

inline Statement cond(std::string c, Block t, const Statement& e)
{
    return cond(std::move(c), std::move(t), block(e));
}

inline Statement cond(std::string c, const Statement& t, const Statement& e)
{
    return cond(std::move(c), block(t), block(e));
}

inline Statement cond(std::string c, const Statement& t)
{
    return cond(std::move(c), block(t));
}
