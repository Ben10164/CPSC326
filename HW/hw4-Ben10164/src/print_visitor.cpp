//----------------------------------------------------------------------
// FILE: print_visitor.cpp
// DATE: CPSC 326, Spring 2023
// AUTH:
// DESC:
//----------------------------------------------------------------------

#include "print_visitor.h"

using namespace std;

PrintVisitor::PrintVisitor(ostream &output)
    : out(output)
{
}

void PrintVisitor::inc_indent()
{
    indent += INDENT_AMT;
}

void PrintVisitor::dec_indent()
{
    indent -= INDENT_AMT;
}

void PrintVisitor::print_indent()
{
    out << string(indent, ' ');
}

void PrintVisitor::visit(Program &p) // just redo this for everything
{
    for (auto struct_def : p.struct_defs)
        struct_def.accept(*this);
    for (auto fun_def : p.fun_defs)
        fun_def.accept(*this);
}

// TODO: Finish the visitor functions

void PrintVisitor::visit(FunDef &f)
{
    out << "\n";
    out << f.return_type.type_name << " ";
    out << f.fun_name.lexeme() << "(";
    if (f.params.size() != 0)
    {
        for (int i = 0; i < f.params.size() - 1; i++)
        {
            auto vardef = f.params[i];
            out << vardef.data_type.type_name << " ";
            out << vardef.var_name.lexeme() << ", ";
        }
        auto vardef = f.params[f.params.size() - 1];
        out << vardef.data_type.type_name << " ";
        out << vardef.var_name.lexeme() << ") {\n";
    }
    else
    {
        out << ") {\n";
    }
    inc_indent();

    for (auto stmt : f.stmts)
    {
        print_indent();
        stmt->accept(*this);
        out << "\n";
    }
    dec_indent();
    print_indent();
    out << "}\n";
}
void PrintVisitor::visit(StructDef &s)
{
    out << "\nstruct ";
    out << s.struct_name.lexeme() << " {\n";
    inc_indent();
    for (int i = 0; i < s.fields.size() - 1; i++)
    {
        print_indent();
        auto fields = s.fields[i];
        out << fields.data_type.type_name << " ";
        out << fields.var_name.lexeme() << ",\n";
    }
    print_indent();
    auto fields = s.fields[s.fields.size() - 1];
    out << fields.data_type.type_name << " ";
    out << fields.var_name.lexeme() << "\n";
    dec_indent();
    print_indent();
    out << "}\n";
}
void PrintVisitor::visit(ReturnStmt &s)
{
    // print_indent();
    out << "return ";
    s.expr.accept(*this);
}
void PrintVisitor::visit(WhileStmt &s)
{
    out << "while (";
    s.condition.accept(*this);
    out << ") {\n";
    inc_indent();
    for (int i = 0; i < s.stmts.size(); i++)
    {
        print_indent();
        s.stmts[i]->accept(*this);
        out << "\n";
    }
    dec_indent();
    print_indent();
    out << "}";
}
void PrintVisitor::visit(ForStmt &s)
{
    // print_indent();
    out << "for (";
    s.var_decl.accept(*this);
    out << "; ";
    s.condition.accept(*this);
    out << "; ";
    s.assign_stmt.accept(*this);
    out << ") {\n";
    inc_indent();
    for (int i = 0; i < s.stmts.size(); i++)
    {
        print_indent();
        s.stmts[i]->accept(*this);
        out << "\n";
    }
    dec_indent();
    print_indent();
    out << "}";
}
void PrintVisitor::visit(IfStmt &s)
{
    // print_indent();
    out << "if (";
    s.if_part.condition.accept(*this);
    out << ") {\n";
    inc_indent();
    for (int i = 0; i < s.if_part.stmts.size(); i++)
    {
        print_indent();
        s.if_part.stmts[i]->accept(*this);
        out << "\n";
    }
    dec_indent();
    print_indent();
    out << "}";
    for (int i = 0; i < s.else_ifs.size(); i++)
    {
        out << "\n";
        print_indent();
        out << "elseif (";
        s.else_ifs[i].condition.accept(*this);
        out << ") {\n";
        inc_indent();
        for (int j = 0; j < s.else_ifs[i].stmts.size(); j++)
        {
            print_indent();
            s.else_ifs[i].stmts[j]->accept(*this);
            out << "\n";
        }
        dec_indent();
        print_indent();
        out << "}";
    }
    if (s.else_stmts.size() != 0)
    {
        out << "\n";
        print_indent();
        out << "else {\n";
        inc_indent();
        for (int i = 0; i < s.else_stmts.size(); i++)
        {
            print_indent();
            s.else_stmts[i]->accept(*this);
            out << "\n";
        }
        dec_indent();
        print_indent();
        out << "}";
    }
}
void PrintVisitor::visit(VarDeclStmt &s)
{
    out << s.var_def.data_type.type_name << " ";
    out << s.var_def.var_name.lexeme() << " = ";
    s.expr.accept(*this);
}
void PrintVisitor::visit(AssignStmt &s)
{
    // print_indent();
    for (int i = 0; i < s.lvalue.size() - 1; i++)
    {
        out << s.lvalue[i].var_name.lexeme();
        if (s.lvalue[i].array_expr.has_value())
        {
            out << "[";
            s.lvalue[i].array_expr.value().accept(*this);
            out << "]";
        }
        out << ".";
    }
    out << s.lvalue[s.lvalue.size() - 1].var_name.lexeme();
    if (s.lvalue[s.lvalue.size() - 1].array_expr.has_value())
    {
        out << "[";
        s.lvalue[s.lvalue.size() - 1].array_expr.value().accept(*this);
        out << "]";
    }
    out << " = ";
    s.expr.accept(*this);
}
void PrintVisitor::visit(CallExpr &e)
{
    // print_indent();
    out << e.fun_name.lexeme() << "(";
    for (int i = 0; i < e.args.size() - 1; i++)
    {
        auto expr = e.args[i];
        expr.accept(*this);
        out << ", ";
    }
    auto expr = e.args[e.args.size() - 1];
    expr.accept(*this);
    out << ")";
}
void PrintVisitor::visit(Expr &e)
{
    if (e.negated)
    {
        out << "not (";
    }
    e.first->accept(*this);
    if (e.op.has_value())
    {
        out << " " << e.op.value().lexeme() << " ";
        e.rest->accept(*this);
    }
    if (e.negated)
    {
        out << ")";
    }
}
void PrintVisitor::visit(SimpleTerm &t)
{
    t.rvalue->accept(*this);
}
void PrintVisitor::visit(ComplexTerm &t)
{
    out << "(";
    t.expr.accept(*this);
    out << ")";
}
void PrintVisitor::visit(SimpleRValue &v)
{
    if (v.value.type() == TokenType::CHAR_VAL)
    {
        out << "\'";
        out << v.value.lexeme();
        out << "\'";
    }
    else if (v.value.type() == TokenType::STRING_VAL)
    {
        out << "\"";
        out << v.value.lexeme();
        out << "\"";
    }
    else
    {
        out << v.value.lexeme();
    }
}
void PrintVisitor::visit(NewRValue &v)
{
    out << "new ";
    out << v.type.lexeme();
    if (v.array_expr.has_value())
    {
        v.array_expr.value().accept(*this);
    }
}
void PrintVisitor::visit(VarRValue &v)
{
    for (int i = 0; i < v.path.size() - 1; i++)
    {
        auto varref = v.path[i];
        out << varref.var_name.lexeme();
        if (varref.array_expr.has_value())
        {
            varref.array_expr.value().accept(*this);
        }
        out << ".";
    }

    auto varref = v.path[v.path.size() - 1];
    out << varref.var_name.lexeme();
    if (varref.array_expr.has_value())
    {
        varref.array_expr.value().accept(*this);
    }
}