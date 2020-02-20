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

#include "tables.h"
#include <iostream>

#include <stdlib.h>
#include <string.h>




int determine_table(const char* value) {
	// Common Elements as far as I am concerend
	if (strncasecmp(value, "LAYER", 5) == 0) return 0;
	if (strncasecmp(value, "LTYPE", 5) == 0) return 1;
	if (strncasecmp(value, "STYLE", 5) == 0) return 2;
	if (strncasecmp(value, "UCS", 3) == 0) return 3;
	if (strncasecmp(value, "VIEW", 4) == 0) return 4;
	if (strncasecmp(value, "VPORT", 4) == 0) return 5;
	if (strncasecmp(value, "APPID", 5) == 0) return 6;
	if (strncasecmp(value, "BLOCK_RECORD", 12) == 0) return 7;
	else return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TABLE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int table::ret_maxN() const {
	return max_number;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LAYER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


layer::layer(const std::vector<dxfpair> &info) {
	// Get the vertex information
	
	//basic_entity( info );
	for (const auto &i : info) {
		switch(i.group_code ) {
			case 2:
				layer_name = i.value_str();
				break;
			case 6:
				ltype_name = i.value_str();
				break;
				
			case 62:
				color_number = i.value_int();
				//std::cout << "I found a color and its number = " << color_number << std::endl;
				break;
			case 290:
				plotting_flag = i.value_int();
				break;
		}
	}	
}

void layer::display() const {
	std::cout << "LAYER\n";
	//std::cout << "\tx = " << x << "\ty = " << y << "\tz = " << z << "\tbulge = " << bulge << std::flush;
}

const char* layer::name() const {
	return layer_name.c_str();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LTYPE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ltype::ltype(const std::vector<dxfpair> &info) {
	// Get the linetype information
	
	for (const auto &i : info) {
		switch(i.group_code) {
			case 2:
				ltype_name = i.value_str();
				break;
			case 3:
				descriptive_txt = i.value_str();
				break;
			case 73:
				num_elements = i.value_int();
				break;
			case 40:
				pattern_length = i.value_float();
				break;
			case 49:
				pattern.push_back(i.value_float());
				break;
		}
	}	
}



const char* ltype::name() const {
	return ltype_name.c_str();
}


const std::vector<double> &ltype::ret_pattern() const {
	return pattern;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// tables
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	


tables::tables(const std::vector<std::vector<dxfpair> > &sections) {
	// Read the main information about the entities section and then put it in the enetites class
	int value;
	
	for(const auto &section : sections) {
		//std::cout << "start" << std::endl;
		value = determine_table(section[0].value_char());
		//std::cout << "sections.size() = " << sections.size() << std::endl << "i = " << i << std::endl << "string = " << string << std::endl;
		switch(value) {
			case 0:
				// LAYER
				//std::cout << "tables start layer " << std::endl;
				tables_layer.push_back(layer(section));
				//std::cout << "tables end layer " << std::endl;
				break;
			
			case 1:
				// LTYPE
				//std::cout << "tables start ltype " << std::endl;
				tables_ltype.push_back(ltype(section));
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

		
ltype tables::ret_ltype(const char *ltype_name, const char *layer_name) const {
	std::string name;
	// The ltype information may be given in the entitity or in the layer information
	// Assume that if there is a name defined in the linetype that it trumps any other layer information
	if (strlen(ltype_name) > 0) name = ltype_name;
	else name = layer_name;
	for (const auto &lt : tables_ltype) {
		if (strcmp(lt.name(), name.c_str()) == 0)
            return lt;
	}
	return tables_ltype[0];
}


layer tables::ret_layer(const char *layer_name) const {
    for (const auto &l : tables_layer) {
		if (strcmp(l.name(), layer_name) == 0)
            return l;
	}
	return tables_layer[0];
}


const std::vector<layer> &tables::ret_layers() const {
	return tables_layer;
}
