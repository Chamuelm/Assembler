# Assembler


I built an assembler for a pseudo assembly language.  
The goal was to export file with the coded instructions by opcode, operands, addresses, etc.

Every machine word is 10-bit, had to create export 3 files:  
ob file – The actual instructions code  
ent file – File for entry labels in original code – for access from external programs  
ext file – Externals variables to be loaded by the linker in run-time  
The final mahcine code is coded with a spacial 32-base code.  

## Highlights:
* Design and create a full working software.
* Input integrity verification.
* Working with files.
* Working with short schedule.

## Links:
* [Project Definition](./project-definition.pdf)

## Screenshots:  
* Example input file  
	![](./Example%20photos/assembler-input.png "Example input file")  
* Example output object file  
	![](./Example%20photos/assembler-ps.png "Example output object file")  
* Example output ext file  
	![](./Example%20photos/assembler-ext.png "Example output ext file")  
* Example output entry file  
	![](./Example%20photos/Assembler-entry.png "Example output entry file")  
* Example warning (1)
	![](./Example%20photos/Assembler-Warnings1.png "Example warning")  
* Example warning (2)
	![](./Example%20photos/Assembler-Warnings2.png "Example warning")  