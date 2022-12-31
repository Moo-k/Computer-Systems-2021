// tokeniser implementation for the workshop example language

#include "iobuffer.h"
#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Assignment_Tokeniser ;

// we are extending the Assignment_Tokeniser namespace

namespace Assignment_Tokeniser
{

    // identifier ::= ('a'-'z'|'A'-'Z'|'$') letter*
    static void parse_identifier()
    {
        next_char_mustbe(cg_identifier) ;
        while(next_char_isa(cg_letter))
        {
            read_next_char();
        }
        
    }
    
    // integer ::= '0' | (digit19 digit*)
    static void parse_integer()
    {
        if ( next_char_isa('0') )
        {
            read_next_char() ;
        }
        
        else
        {
            next_char_mustbe(cg_digit19) ;
            while ( next_char_isa(cg_digit) )
            {
                read_next_char()  ;
            }
            
        }
        
    }
    
    // - exponent ::= eee sign? integer?
    static void parse_exponent()
    {
        next_char_mustbe(cg_eee);
        if(next_char_isa(cg_integer))
        {
            read_next_char();
        }
        if(next_char_isa(cg_sign))
        {
            read_next_char();
        }
        
    }
    
    // - fraction ::= '.' digit*
    static void parse_fraction()
    {
        next_char_mustbe('.');
        while(next_char_isa(cg_digit) )
        {
            read_next_char();
        }
        if( next_char_isa(cg_exponent) )
        {
            parse_exponent();
        }
        
    }
    
    // number ::= integer | scientific
    static void parse_number()
    {
        parse_integer();
        if( next_char_isa('.'))
        {
            parse_fraction();
        }
        
    }
    
    // string ::= '"' instring* '"'
    static void parse_string()
    {
        next_char_mustbe('"');
        while(next_char_isa(cg_instring) )
        {
            read_next_char();
        }
        
        next_char_mustbe('"');
    }
    
    // * eol_comment    ::= '/' eol_suffix
    //   - eol_suffix   ::= '/' comment_char* '\n'
    static void parse_eol_comment()
    {
        next_char_mustbe('/');
        while(next_char_isa(cg_comment_char) )
        {
            read_next_char();
        }
        
        next_char_mustbe('\n');
    }
    
    // hash_comment   ::= '#' comment_char* '\n'
    static void parse_hash_comment()
    {
        next_char_mustbe('#');
        while (next_char_isa(cg_comment_char))
        {
            read_next_char();
        }
        
        next_char_mustbe('\n');
    }
    
    // symbol ::= '@'|'~'|'='|'-'|'+'|'/'|'*='|'~='|'=='|'<<<'|'<<'|'>>>'|'>>'|'{'|'}'|'('|')'|'['|']'|'.'
    static void parse_symbol()
    {
        if (next_char_isa('/'))
        {
            read_next_char();
            if (next_char_isa('/'))
            {
                parse_eol_comment();
            }
            
            return;
        }
        
        if (next_char_isa('*'))
        {
            read_next_char();
            next_char_mustbe('=');
            return;
        }
        
        if (next_char_isa('='))
        {
            read_next_char();
            if (next_char_isa('='))
            {
                read_next_char();
            }
            
            return;
        }
        
        if (next_char_isa('~'))
        {
            read_next_char();
            if (next_char_isa('='))
            {
                read_next_char();
                return;
            }
            
            return;
        }
        
        if (next_char_isa('<'))
        {
            read_next_char();
            next_char_mustbe('<');
            if (next_char_isa('<'))
            {
                read_next_char();
            }
            
            return;
        }
        
        if (next_char_isa('>'))
        {
            read_next_char();
            next_char_mustbe('>');
            if (next_char_isa('>'))
            {
                read_next_char();
            }
            
            return;
        }
        
        if (next_char_isa(cg_symbol))
        {
            read_next_char();
            return;
        }
        
    }

    // token ::= ...
    static void parse_token()
    {
        if ( next_char_isa(cg_wspace) ) read_next_char() ; else
        if ( next_char_isa(cg_identifier) ) parse_identifier() ; else
        if ( next_char_isa('"') ) parse_string() ; else
        if ( next_char_isa(cg_integer) ) parse_number() ; else
        if ( next_char_isa(cg_symbol) ) parse_symbol() ; else
        if ( next_char_isa('#') ) parse_hash_comment() ; else
        if ( next_char_isa(EOF) ) ; else
        did_not_find_start_of_token() ;
    }

    // parse the next token in the input and return a new
    // Token object that describes its kind and spelling
    // Note: you must not call new_token() anywhere else in your program
    // Note: you should not modify this function
    //
    Token read_next_token()
    {
        parse_token() ;

        return new_token() ;
    }
}

