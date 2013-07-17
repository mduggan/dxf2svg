/*
 * Read Blocks from file and convert to vectors of entities
 *
 * Author:
 *   Matt Squires <squiresm@colorado.edu>
 *
 * Copyright (C) 2005 Matt Squires
 *
 * Released under GNU GPL and LGPL, read the file 'GPL.txt' and 'LGPL.txt' for details
 */


#include"blocks.h"
#include<iostream>

#include <stdlib.h>
#include <string.h>

block::block(const std::vector< std::vector< dxfpair > > &sections) : entities( sections ){
	// Inherit most of the functionality of the entitites section
	
	basic_entity( sections[0] );
	block_info( sections[0] );
}

const char* block::name() const{
	return block_name.c_str();
}


void block::block_info(const std::vector< dxfpair > info){
	for (size_t i = 0; i < info.size(); i++){
		switch( info[i].group_code ){
			case 2: // Block name
				block_name = info[i].value_char();
				break;
		}
	}
}


blocks::blocks(const std::vector< std::vector< dxfpair > > &sections){
	// Read the main information about the entities section and then put it in the enetites class
	std::vector< std::vector< dxfpair > > ents;
	
	int n_loop = sections.size();
	n_loop--;
	//for(int i = 0; i < (sections.size()-1); i++){ // It is odd but the last value seems to be bad so don't use it
	// I am not really sure if I need the -1.  I needed it once upon a time to make things work but I don't have time to test it well right now 
	// But sections.size() is an unsigned int so when you subtract 1 it becomes 4294967295 and tries to run the loop so work around that by making n_loop that is signed
	for(int i = 0; i < n_loop; i++){ // It is odd but the last value seems to be bad so don't use it
		ents.clear();  // First clear out the pline information
		
		// Get everything from the start of the BLOCK designation to an ENDBLK value
		if ( strcmp(sections[i][0].value_char(),"BLOCK") == 0 && (i < sections.size())){
			do{
				ents.push_back( sections[i] );
				;
			}while( strcmp(sections[++i][0].value_char(),"ENDBLK") != 0  && (i < sections.size()-1) );
			blocks_blocks.push_back( block( ents ) );
		}
	}
}

const block &blocks::ret_block(const char *block_name) const{
	for (int i = 0; i < blocks_blocks.size();i++){
		if (strcmp(blocks_blocks[i].name(), block_name) == 0 ) return blocks_blocks[i];	
	}
	return blocks_blocks[0];
}
