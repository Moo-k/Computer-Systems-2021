// emulate executing Hack CPU instructions
#include "iobuffer.h"
#include "symbols.h"
#include "hack_computer.h"
#include <bitset>
#include <vector>
#include <iostream>

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Hack_Computer ;

// This program must emulate the execution of Hack CPU instructions
//
// To read from registers A, D or PC, use read_A(), read_D() or read_PC()
// To write to registers A, D or PC, use write_A(value), write_D(value) or write_PC(value)
//
// To read from ROM use read_ROM(address)
// To read from RAM use read_RAM(address)
// To write to RAM use write_RAM(address,value)
//
// All values returned by a read_*() function are 16-bit unsigned integers
// All parameters passed to a write_*() function are 16-bit unsigned integers
//

/*****************   REPLACE THE FOLLOWING CODE  ******************/

// written by a1805637

// ALU
uint16_t ALU(uint16_t x, uint16_t y, uint16_t comp)
{
    uint16_t fout, out, x1, x2, y1, y2;
    bool zx, nx, zy, ny, f, no;

    // set the values of the ALU inputs to the last 6 bits in comp
    zx = comp&32;
    nx = comp&16;
    zy = comp&8;
    ny = comp&4;
    f = comp&2;
    no = comp&1;

    if (zx==1)
    {
        x1 = 0;
    } else
    {
        x1 = x;
    }

    if (nx==1)
    {
        x2 = ~x1;
    } else
    {
        x2 = x1;
    }

    if (zy==1)
    {
        y1 = 0;
    } else
    {
        y1 = y;
    }

    if (ny==1)
    {
        y2 = ~y1;
    } else
    {
        y2 = y1;
    }

    if (f==1)
    {
        fout = x2 + y2;
    } else
    {
        fout = x2 & y2;
    }

    if (no==1)
    {
        out = ~fout;
    } else
    {
        out = fout;
    }

    return out;
}


// disassemble an instruction - convert binary to symbolic form
// A instructions should be "@" followed by the value in decimal
// C instructions should be dest=alu;jump
// omit dest= if all destination bits are 0
// omit ;jump if all jump bits are 0
string disassemble_instruction(uint16_t instruction)
{
    if (((instruction >> 15) & 1) == 0)
    {
        string instruction2 = to_string(instruction);
        return "@" + instruction2;
    }   else if (((instruction >> 15) & 1) == 1)
    {
        string outputc = "";

        // use string concatenation to generate the string output for c instructions
        outputc += destination((instruction >> 3) & 7);
        outputc += aluop((instruction >> 6) & 127);
        outputc += jump((instruction) & 7);
        return outputc;
    }   else
    {
        return "** illegal instruction **";
    }
}

// emulate a cpu instruction - the Hack Computer has been initialised
// the PC contains the address in ROM of the instruction to execute
// if you want to produce additional output, use write_to_traces()
static void emulate_instruction()
{
    uint16_t pc = read_PC();
    uint16_t rom = read_ROM(pc);

    if (((rom >> 15) & 1) == 0)
    {
        write_A(rom);
        write_PC(pc+1);
    }   else if (((rom >> 15) & 1) == 1)
    {
        uint16_t ALUout = 0;
        if (((rom >> 12) & 1) == 0)
        {
            ALUout = ALU(read_D(), read_A(), ((rom >> 6) & 63));
        }   else if (((rom >> 12) & 1) == 1)
        {
            ALUout = ALU(read_D(), read_RAM(read_A()), ((rom >> 6) & 63));
        }

        //dest
        uint16_t dest = ((rom >> 3) & 7);
        switch (dest)
        {

            case 1:
                write_RAM(read_A(), ALUout);
                break;

            case 2:
                write_D(ALUout);
                break;

            case 3:
                write_RAM(read_A(), ALUout);
                write_D(ALUout);
                break;

            case 4:
                write_A(ALUout);
                break;

            case 5:
                write_A(ALUout);
                write_RAM(read_A(), ALUout);
                break;

            case 6:
                write_A(ALUout);
                write_D(ALUout);
                break;

            case 7:
                write_A(ALUout);
                write_RAM(read_A(), ALUout);
                write_D(ALUout);
                break;

            default:
                break;
        }

        // jump
        // zr and ng outputs of ALU
        bool zr = 0;
        if (ALUout == 0)
        {
            zr = 1;
        }

        bool ng = ((ALUout >> 15) & 1) ;

        uint16_t jump = rom & 7;
        switch (jump)
        {
            case 1:
                if (zr == 0 && ng == 0)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 2:
                if (zr == 1)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 3:
                if (ng == 0)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 4:
                if (ng == 1 && zr == 0)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 5:
                if (zr == 0)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 6:
                if (zr == 1 || ng == 1)
                {
                    write_PC(read_A());
                }   else
                {
                    write_PC(pc+1);
                }
                break;

            case 7:
                write_PC(read_A());
                break;

            default:
                write_PC(pc+1);
                break;
        }
    }
}


/*****************        DOWN TO HERE         ******************/

// if the first command line argument is "D" the test output must display disassembled instructions
bool display_disassembled_instructions = false ;

// this function only calls disassemble_instruction if required
// it is called from more_cpu_tests() and is passed the instruction being emulated
string disassemble(uint16_t instruction)
{
    if ( display_disassembled_instructions ) return disassemble_instruction(instruction) ;

    // default is to display instruction in binary
    return "\"" + std::bitset<16>(instruction).to_string() + "\"" ;
}


// main program
int main(int argc,char **argv)
{
    // force all output to be immediate - the test scripts will override this
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    // is the first command line argument "D", ie does the test output include disassembly of the instruction being emulated
    display_disassembled_instructions = argc > 1 && string(argv[1]) == "D" ;

    // more_cpu_tests() will initialise the next test and return true
    // if there are no more tests to be run, it returns false
    // more_cpu_tests() also outputs the results of the previous test
    while ( more_cpu_tests(disassemble) )
    {
        emulate_instruction() ;
    }

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

