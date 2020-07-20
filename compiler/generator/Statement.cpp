#include "Statement.h"

static int gIndentation = 0;

void setGlobalIndent(int n)
{
    gIndentation = n;
}

static void indentup()
{
    gIndentation++;
}
static void indentdown()
{
    gIndentation--;
}

std::ostream& indent(std::ostream& dst)
{
    for (int i = 0; i < gIndentation; i++) dst << '\t';
    return dst;
}

std::ostream& newline(std::ostream& dst)
{
    return dst << '\n';
}

class Stmt {
   public:
    virtual ~Stmt() = default;

    virtual bool          isInstr(std::string& /*str*/) const { return false; }
    virtual bool          isIf(std::string& /*cond*/, Block& /*then*/, Block& /*else*/) const { return false; }
    virtual bool          isLoop(std::string& /*descr*/, Block& /*code*/) const { return false; }
    virtual std::ostream& print(std::ostream& dst) const { return dst << "Stmtxxx"; }
};

std::ostream& operator<<(std::ostream& dst, const Statement& s)
{
    s->print(dst);
    return dst;
}

std::ostream& operator<<(std::ostream& dst, const Block& l)
{
    for (const auto& s : l) {
        s->print(dst);
    }
    return dst;
}

/**
 * @brief Simple instruction
 *
 */
class Instr : public Stmt {
    std::string fStr;

   public:
    explicit Instr(std::string s) : fStr(std::move(s)) {}

    bool isInstr(std::string& str) const override
    {
        str = fStr;
        return true;
    }

    std::ostream& print(std::ostream& dst) const override
    {
        indent(dst);
        dst << fStr;
        newline(dst);
        return dst;
    }
};

Statement instr(std::string s)
{
    return Statement{new Instr(std::move(s))};
}
bool isInstr(const Statement& s, std::string& l)
{
    return s->isInstr(l);
}
/**
 * @brief If statement
 *
 */
class If : public Stmt {
    std::string fCond;
    Block       fThen;
    Block       fElse;

   public:
    If(std::string c, Block t, Block e) : fCond(std::move(c)), fThen(std::move(t)), fElse(std::move(e)) {}
    If(std::string c, Block t) : fCond(std::move(c)), fThen(std::move(t)) {}

    bool isIf(std::string& c, Block& t, Block& e) const override
    {
        c = fCond;
        t = fThen;
        e = fElse;
        return true;
    }

    std::ostream& print(std::ostream& dst) const override
    {
        indent(dst);
        dst << "if (" << fCond << ") { \n";
        indentup();
        dst << fThen;
        indentdown();
        if (fElse.size() > 0) {
            indent(dst);
            dst << "} else {\n";
            indentup();
            dst << fElse;
            indentdown();
        }
        indent(dst);
        dst << "}\n";
        return dst;
    }
};

Statement cond(std::string c, Block t, Block e)
{
    return Statement{new If(std::move(c), std::move(t), std::move(e))};
}

Statement cond(std::string c, Block t)
{
    return Statement{new If(std::move(c), std::move(t))};
}

bool isIf(const Statement& s, std::string& c, Block& t, Block& e)
{
    return s->isIf(c, t, e);
}

/**
 * @brief Loop statement
 *
 */
class Loop : public Stmt {
    std::string fHead;
    Block       fBody;

   public:
    Loop(std::string h, Block b) : fHead(std::move(h)), fBody(std::move(b)) {}

    bool isLoop(std::string& h, Block& b) const override
    {
        h = fHead;
        b = fBody;
        return true;
    }

    std::ostream& print(std::ostream& dst) const override
    {
        indent(dst);
        dst << "for ( " << fHead << " ) {\n";
        indentup();
        dst << fBody;
        indentdown();
        indent(dst);
        dst << "}\n";
        return dst;
    }
};

Statement loop(std::string h, Block b)
{
    return Statement{new Loop(std::move(h), std::move(b))};
}
bool isLoop(const Statement& s, std::string& h, Block& b)
{
    return s->isLoop(h, b);
}

// combining code
Block operator+(const Statement& a, const Statement& b)
{
    Block r;
    r.push_back(a);
    r.push_back(b);
    return r;
}

Block operator+(const Statement& a, const Block& b)
{
    Block r;
    r.push_back(a);
    for (const auto& s : b) r.push_back(s);
    return r;
}

Block operator+(const Block& a, const Statement& b)
{
    Block r;
    for (const auto& s : a) r.push_back(s);
    r.push_back(b);
    return r;
}

Block operator+(const Block& a, const Block& b)
{
    Block r;
    for (const auto& s : a) r.push_back(s);
    for (const auto& s : b) r.push_back(s);
    return r;
}

Block block(const Statement& s)
{
    Block r;
    r.push_back(s);
    return r;
}

Block lift(const Block& B)
{
    bool        inside = false;
    std::string C;  // common condition
    Block       T;  // accumulate then branches
    Block       E;  // accumulate else branches
    Block       R;  // resulting block
    for (const auto& s1 : B) {
        // std::cerr << "s1: " << s1 << std::endl;
        std::string c;
        Block       t, e;
        auto        s2 = lift(s1);
        // std::cerr << "s2: " << s2 << std::endl;

        if (isIf(s2, c, t, e)) {
            if (inside) {
                if (C == c) {
                    // inside same condition
                    T = T + t;
                    E = E + e;
                } else {
                    // close existing condition and start a new one
                    R.push_back(cond(C, T, E));
                    C = c;
                    T = t;
                    E = e;
                }
            } else {
                // not inside a condition, start a new one
                inside = true;
                C      = c;
                T      = t;
                E      = e;
            }
        } else {
            if (inside) {
                // we close existing condition
                inside = false;
                R.push_back(cond(C, T, E));
            }
            R.push_back(s2);
        }
    }
    if (inside) {
        // we need to finish
        R.push_back(cond(C, T, E));
    }
    return R;
}
Statement lift(const Statement& s)
{
    std::string h;
    Block       b;
    Statement   r;

    //    std::cerr << "ENTER lift " << s << std::endl;
    if (isLoop(s, h, b)) {
        std::string c;
        Block       e, t;
        Block       d = lift(b);
        // std::cerr << "b: " << b << std::endl;
        // std::cerr << "d: " << d << std::endl;
        if ((d.size() == 1) && isIf(d.front(), c, t, e)) {
            if (e.size() > 0) {
                r = cond(c, block(loop(h, t)), block(loop(h, e)));
            } else {
                r = cond(c, block(loop(h, t)));
            }

        } else {
            r = s;
        }
    } else {
        r = s;
    }
    //   std::cerr << "EXIT lift " << r << std::endl;
    return r;
}
