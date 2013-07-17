/* Header file for reading dxf information and basic parsing.  Interprting information is found in other files
*/

#ifndef READ_DXF_H
#define READ_DXF_H

#include<vector>
#include<string>

class dxfpair{	
	public:
	dxfpair(int gcode, const char *val);
	const char * value_char() const;
	
	// Leave this data public	
	int group_code;
	std::string value;
};




//int section(const char* value);  // Convert the section titles into integers

std::vector< std::vector< dxfpair > > dxf_get_sections(const char* filename);

std::vector< std::vector< dxfpair > > separate_parts(const std::vector< dxfpair > &section);  // Find where the major sections are and break into smaller parts

#endif
