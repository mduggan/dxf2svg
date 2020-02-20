/*
 * For reading and slight parsing of dxf files
 *
 * Author:
 *   Matt Squires <squiresm@colorado.edu>
 *
 * Copyright (C) 2005 Matt Squires
 *
 * Released under GNU GPL and LGPL, read the file 'GPL.txt' and 'LGPL.txt' for details
 */



#include <fstream>
#include <string>
#include "read_dxf.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>

using namespace std;


int MAX_STR_LN = 10000;

int section(const char* value) {
	if (strncasecmp(value, "HEADER", 6) == 0) return 0;
	if (strncasecmp(value, "CLASSES", 7) == 0) return 1;
	if (strncasecmp(value, "TABLES", 6) == 0) return 2;
	if (strncasecmp(value, "BLOCKS", 6) == 0) return 3;
	if (strncasecmp(value, "ENTITIES", 8) == 0) return 4;
	if (strncasecmp(value, "OBJECTS", 7) == 0) return 5;
	if (strncasecmp(value, "THUMBNAILIMAGE", 14) == 0) return 6;
    return -1;
}


dxfpair::dxfpair(int gcode, const char *val) :
group_code(gcode), value(val) {
	size_t size = value.length();
	// Strip off any control characters and spaces off the end of the string
	while ((size > 0) && int(value[size-1]) < 33) {
		size--;
	}
    value = value.substr(0, size);
}

double dxfpair::value_float() const {
    return atof(value.c_str());
}

int dxfpair::value_int() const {
    return atoi(value.c_str());
}

static bool not_endsec(const char *value) {
    return strncasecmp(value, "ENDSEC",6) != 0;
}

std::vector<std::vector<dxfpair> > dxf_get_sections(const char* filename) {
	// In the dxf format information is paired into group codes that indicate the information that follows on the next line.  The information on the next line is called the value
	
	int n = 0;
	
	char value[MAX_STR_LN];

	std::vector<std::vector<dxfpair> > out;
	
	std::vector<dxfpair> header;
	std::vector<dxfpair> classes;
	std::vector<dxfpair> tables;
	std::vector<dxfpair> blocks;
	std::vector<dxfpair> entities;
	std::vector<dxfpair> objects;
	std::vector<dxfpair> thumbnailimage;
		
	// Open dxf file for reading
	std::ifstream file(filename);
	
	if (!file.is_open()) {
		exit (1);  // Change this to an exception
	}
	
	// Find the first SECTION header
	
	while ((!file.eof())) {
		n++;
		
		// get the first group code and value
		file.getline(value, MAX_STR_LN);
		int group_code = atoi(value);
		file.getline(value, MAX_STR_LN);
        
		
		do{
			// TO DO  set all the chars to be caps for later comparison
		
			// Find the SECTION codes
			if ((group_code == 0 ) && (strncasecmp(value, "SECTION", 7) == 0 )) {
				// Directly after a section value is the type of section ( e.g. HEADER, TABLES )
				file.getline(value, MAX_STR_LN);
				group_code = atoi(value);
				file.getline(value, MAX_STR_LN);
				int section_num = section(value);
				if (group_code == 2) {
					// Make sure the the group code is 2 for the SECTION name
					// This is a big block of mostly repetitive code, it will result in larger code, but would be faster than putting the switch in another while loop.  If I still live in a time when file size mattered alot I would change it
					//std::cerr <<  "section " << value << " section_num = " << section_num << std::endl;
					switch (section_num) {
						case 0:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								header.push_back(dxfpair(group_code, value));
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							} while (((group_code != 0) || not_endsec(value)) && (!file.eof()));  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							break;
						case 1:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							if ( (group_code != 0) || not_endsec(value)) {
								// Some dxf files have blank sections.  These are not handled by the do/while loop so break about if needed
								do{
									classes.push_back( dxfpair( group_code, value ) );
									file.getline(value, MAX_STR_LN);
									group_code = atoi(value);
									file.getline(value, MAX_STR_LN);
								}while( ( (group_code != 0) || not_endsec(value)) && (!file.eof()) );  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							}
							break;
						case 2:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								tables.push_back(dxfpair(group_code, value));
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							} while ( ( (group_code != 0) || not_endsec(value)) && (!file.eof()) );
							break;
						case 3:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								blocks.push_back( dxfpair( group_code, value ) );
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							} while ( ( (group_code != 0) || not_endsec(value)) && (!file.eof()) );  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							break;
						case 4:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								entities.push_back(dxfpair(group_code, value));
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							} while(((group_code != 0) || not_endsec(value)) && (!file.eof()));  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							break;
						case 5:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								objects.push_back(dxfpair(group_code, value ));
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							}while( ( (group_code != 0) || not_endsec(value)) && (!file.eof()) );  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							break;
						case 6:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
							do{
								thumbnailimage.push_back( dxfpair( group_code, value ) );
								file.getline(value, MAX_STR_LN);
								group_code = atoi(value);
								file.getline(value, MAX_STR_LN);
							} while (((group_code != 0) || not_endsec(value)) && (!file.eof()));  // I put in the (group_code != 0) in the hope that it will be a faster bool compare than the string compare.  Test this later
							break;
						default:
							file.getline(value, MAX_STR_LN);
							group_code = atoi(value);
							file.getline(value, MAX_STR_LN);
					}
				}
			}	
			file.getline(value, MAX_STR_LN);
			group_code = atoi(value);
			file.getline(value, MAX_STR_LN);
			
			n++;
		} while ((strncasecmp(value,"EOF",3) != 0) && (!file.eof()) );
	}
	
	out.push_back(header);
	out.push_back(classes);
	out.push_back(tables);
	out.push_back(blocks);
	out.push_back(entities);
	out.push_back(objects);
	out.push_back(thumbnailimage);
	
	return out;
}


std::vector<std::vector<dxfpair> > separate_parts(std::vector<dxfpair> &section) {
	///std::cerr << "1" << std::endl;
	//std::cerr << "section.size() = " << section.size() << std::endl;
	// Find where the major sections are and break into smaller parts
	// Major section is defined as anything beween group_code 0 to 0
	std::vector<dxfpair> inner;
	std::vector<std::vector<dxfpair> > outer;
	//std::cerr << "2" << std::endl;
	for (size_t i = 0; i < section.size(); i++) {
		std::string &value = section[i].value;

		//std::cerr << "i = " << i << std::endl;
		//std::cerr << "section[i].value.size() = " << section[i].value.size() << std::endl;

		// Make sure no control codes like LF or CR are making it past this section
		if ((value.size() > 0) && int(value[value.size()-1]) < 32) {
			 value = value.substr(0, value.size() - 1);
		}
		//for(int j = 0;j < section[i].value.size();j++ ) std::cerr << section[i].value[j];
		//std::cerr << std::endl;
		
		inner.push_back(section[i]);
		
		// If the next group code is 0 then push the previously found info on outer and start looking for data again
		if (i+1 < section.size() && section[i+1].group_code == 0) {
			//std::cerr << "inner.push_back" << std::endl;
			outer.push_back(inner);
			inner.clear();
		}
	}
	// Because putting the data on outer depends on find a GC=0 the last bit of data may be left behind so it inner has data in it put it on outer
	if (inner.size() > 0) {
		outer.push_back(inner);
		inner.clear();
	}
	//std::cerr << "3" << std::endl;
	//if (section.back().group_code == 0) {
		//outer.push_back( inner ); // Put the last part on if there is information, but I don't think it needs to.
	//}
	
	return outer;
}

