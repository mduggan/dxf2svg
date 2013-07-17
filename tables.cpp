/*
 * Code for the conversion of DXF information in the TABLES section
 *
 * Author:
 *   Matt Squires <squiresm@colorado.edu>
 *
 * Copyright (C) 2005 Matt Squires
 *
 * Released under GNU GPL and LGPL, read the file 'GPL.txt' and 'LGPL.txt' for details
 */

#include"tables.h"
#include<iostream>

#include <stdlib.h>
#include <string.h>




int determine_table(const char* value){
	// Common Elements as far as I am concerend
	if ( strncmp(value,"LAYER",5) == 0 ) return 0;	
	if ( strncmp(value,"LTYPE",5) == 0 ) return 1;	
	if ( strncmp(value,"STYLE",5) == 0 ) return 2;
	if ( strncmp(value,"UCS",3) == 0 ) return 3;
	if ( strncmp(value,"VIEW",4) == 0 ) return 4;
	if ( strncmp(value,"VPORT",4) == 0 ) return 5;
	if ( strncmp(value,"APPID",5) == 0 ) return 6;
	if ( strncmp(value,"BLOCK_RECORD",12) == 0 ) return 7;
	else return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TABLE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int table::ret_maxN() const{
	return max_number;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LAYER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


layer::layer(const std::vector< dxfpair > &info){
	// Get the vertex information
	
	//basic_entity( info );
	for (size_t i = 0; i < info.size(); i++){
		switch( info[i].group_code ){
			case 2:
				layer_name = info[i].value_char();
				break;
			case 6:
				ltype_name = info[i].value_char();
				break;
				
			case 62:
				color_number = atoi(info[i].value_char());
				//std::cout << "I found a color and its number = " << color_number << std::endl;
				break;
			case 290:
				plotting_flag = atoi(info[i].value_char());				
				break;
		}
	}	
}

void layer::display() const{
	std::cout << "LAYER\n";
	//std::cout << "\tx = " << x << "\ty = " << y << "\tz = " << z << "\tbulge = " << bulge << std::flush;
}

const char* layer::name() const{
	return layer_name.c_str();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LTYPE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ltype::ltype(const std::vector< dxfpair > &info){
	// Get the linetype information
	
	for (int i = 0; i < info.size(); i++){
		switch( info[i].group_code ){
			case 2:
				ltype_name = info[i].value_char();
				break;
			case 3:
				descriptive_txt = info[i].value_char();
				break;
			case 73:
				num_elements = atoi(info[i].value_char());
				break;
			case 40:
				pattern_length = atof(info[i].value_char());				
				break;
			case 49:
				pattern.push_back(atof(info[i].value_char()));
				break;
		}
	}	
}



const char* ltype::name() const{
	return ltype_name.c_str();
}


const std::vector< double > &ltype::ret_pattern() const{
	return pattern;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// tables
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	


tables::tables(const std::vector< std::vector< dxfpair > > &sections){
	// Read the main information about the entities section and then put it in the enetites class
	int value;
	
	for(int i = 0; i < sections.size(); i++){
		//std::cout << "start" << std::endl;
		value = determine_table(sections[i][0].value_char());
		//std::cout << "sections.size() = " << sections.size() << std::endl << "i = " << i << std::endl << "string = " << string << std::endl;
		switch( value ){
			case 0:
				// LAYER
				//std::cout << "tables start layer " << std::endl;
				tables_layer.push_back( layer( sections[i] ) );
				//std::cout << "tables end layer " << std::endl;
				break;
			
			case 1:
				// LTYPE
				//std::cout << "tables start ltype " << std::endl;
				tables_ltype.push_back( ltype( sections[i] ) );
				//std::cout << "tables end ltype " << std::endl;
				break;
				
			//case 3:
			//	break;
			
			default:
				break;
				// Nothing here
		}
		
	}
	
}



		
ltype tables::ret_ltype(const char *ltype_name, const char *layer_name) const{
	std::string name;
	// The ltype information may be given in the entitity or in the layer information
	// Assume that if there is a name defined in the linetype that it trumps any other layer information
	if ( strlen(ltype_name) > 0 ) name = ltype_name; 
	else name = layer_name;
	for (int i = 0; i < tables_ltype.size();i++){
		if (strcmp(tables_ltype[i].name(),name.c_str()) == 0 ) return tables_ltype[i];	
	}
	return tables_ltype[0];
}


layer tables::ret_layer(const char *layer_name) const{
	for (int i = 0; i < tables_layer.size();i++){
		if (strcmp(tables_layer[i].name(),layer_name) == 0 ) return tables_layer[i];	
	}
	return tables_layer[0];
}


const std::vector< layer > &tables::ret_layers() const{
	return tables_layer;
}
