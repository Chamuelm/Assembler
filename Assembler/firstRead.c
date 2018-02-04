/* This file is the first read of the code to parse it to machine code
 * It save the data to data structures and check for errors
 */

/* Reset the IC and DC counters
 * IC  Instruction-Counter
 * DC  Data-Counter
 * L   Number of words in the current line
 * */
int IC = 0;
int DC = 0;
int L = 0;

/* 1. Create function that read the next line from the source code: readLine();
 *
 * 2. check if the source code is over if it's over check for errors (maybe create a function for checking error?)
 *
 * 3. if found errors stop and print the errors.
 *
 * 4. if the source code not over check if in the line you have a symbol if you have a symbol turn on the flag of
 *    thereIsASymbol (1 or 0 ).
 *
 * 5 .if you don't have a symbol check for instruction type (.data, .string, .struct).
 *
 * 6. f there is not  instruction type check for .extern or .entry instructions. if yes enter the symbol the the
 * 	  symbol table with mark up for external type.
 *
 * 7. go back to 2.
 *
 * 8. if there's not .extern or .entry instructions check if the thereIsASymbol is on and enter the symbol to the
 *    symbol table with mark up for code type. is value will be the IC.
 *    (if the symbol is in the table already you should declare an error).
 *
 * 9.  search if the command name is exist in the opcode table, if not declare an error.
 *
 * 10. check the structure instruction and calculate L.
 *
 * 11. build the binary code of the first instruction.
 *
 * 12.  update IC + L = IC.
 *
 * 13. go back to 2.
 *
 * 14. update in symbol table the value for the data symbols by adding the final value of IC.
 * */
