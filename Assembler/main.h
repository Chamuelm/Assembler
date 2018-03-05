/* 
 * File:    main.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains main.c functions declerations
 * 
 */

int main(int argc, char *argv[]);
void assembler(char *fileName, FILE *fp);
void lerror(char *s, int lineNum);
void lwarning(char *s, int lineNum);
void exitMemory();
void freeInstArr();
void freeOperand(operand *op);

#endif /* MAIN_H */

