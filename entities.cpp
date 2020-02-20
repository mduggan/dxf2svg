/*
 * Class for interpereting the entities found in a DXF file
 *
 * Author:
 *   Matt Squires <squiresm@colorado.edu>
 *
 * Copyright (C) 2005 Matt Squires
 *
 * Released under GNU GPL and LGPL, read the file 'GPL.txt' and 'LGPL.txt' for details
 */


#include "entities.h"
#include <iostream>
#include <math.h>

#include <stdlib.h>
#include <string.h>

static int determine_entity(const char *value) {
    //std::cerr << "determine_entity: '" << value << "'" << std::endl;
	// Common Elements as far as I am concerend
	if (strncasecmp(value, "POLYLINE", 8) == 0 ) return 0;
	if (strncasecmp(value, "ARC", 3) == 0 ) return 1;
	if (strncasecmp(value, "CIRCLE", 6) == 0 ) return 2;
	if (strncasecmp(value, "LINE", 4) == 0 ) return 3;
	if (strncasecmp(value, "SPLINE", 6) == 0 ) return 4;
	if (strncasecmp(value, "XLINE", 5) == 0 ) return 5;
	if (strncasecmp(value, "RAY", 3) == 0 ) return 6;
	if (strncasecmp(value, "DIMENSION", 9) == 0 ) return 7;
	if (strncasecmp(value, "ELLIPSE", 7) == 0 ) return 8;
	if (strncasecmp(value, "INSERT", 6) == 0 ) return 9;
	if (strncasecmp(value, "VERTEX", 6) == 0 ) return 10;
	if (strncasecmp(value, "TEXT", 4) == 0 ) return 11;
	
	// Less Common eletities as far as I am concerend
	if (strncasecmp(value, "3DSOLID",7) == 0 ) return 12;
	if (strncasecmp(value, "ACAD_PROXY_ENTITY",17) == 0 ) return 13;
	if (strncasecmp(value, "ARCALIGNEDTEXT",14) == 0 ) return 14;
	if (strncasecmp(value, "ATTDEF",6) == 0 ) return 15;
	if (strncasecmp(value, "ATTRIB",6) == 0 ) return 16;
	if (strncasecmp(value, "BODY",4) == 0 ) return 17;
	if (strncasecmp(value, "HATCH",5) == 0 ) return 18;
	if (strncasecmp(value, "IMAGE",5) == 0 ) return 19;
	if (strncasecmp(value, "LEADER",6) == 0 ) return 20;
	if (strncasecmp(value, "LWPOLYLINE",10) == 0 ) return 21;
	if (strncasecmp(value, "MLINE",5) == 0 ) return 22;
	if (strncasecmp(value, "MTEXT",5) == 0 ) return 23;
	if (strncasecmp(value, "OLEFRAME",8) == 0 ) return 24;
	if (strncasecmp(value, "POINT",5) == 0 ) return 25;
	if (strncasecmp(value, "REGION",6) == 0 ) return 26;
	if (strncasecmp(value, "RTEXT",5) == 0 ) return 27;
	if (strncasecmp(value, "SEQEND",6) == 0 ) return 28;
	if (strncasecmp(value, "SHAPE",5) == 0 ) return 29;
	if (strncasecmp(value, "SOLID",5) == 0 ) return 30;
	if (strncasecmp(value, "3DFACE",6) == 0 ) return 31;
	if (strncasecmp(value, "TOLERANCE",9) == 0 ) return 32;
	if (strncasecmp(value, "TRACE",5) == 0 ) return 33;
	if (strncasecmp(value, "VIEWPORT",8) == 0 ) return 34;
	if (strncasecmp(value, "WIPEOUT",7) == 0 ) return 35;
	else return -1;
}


void entity::basic_entity(const std::vector<dxfpair> &info) {
	// Extract all of the typical entity information (e.g. layer name, positions)
	for (const auto &i : info) {
		switch(i.group_code) {
			case 6:
				linetype = i.value_str();
				break;
			case 8:
				layer = i.value_str();
				break;
			case 10:
				x = i.value_float();
				if (x < min_x) {
					min_x = x;
				}
				if (x > max_x) {
					max_x = x;
				}				
				break;
			case 20:
				y = i.value_float();
				if ( y < min_y ) {
					min_y = y;
				}
				if ( y > max_y ) {
					max_y = y;
				}
				break;
			case 30:
				z = i.value_float();
				break;
		}
	}
	
}

void entity::entity_display() const {
	std::cout << "\tlayer = " << layer << "\n\tlinetype = " << linetype << "\n\tx = " << x << "\ty = " << y << "\tz = " << z << std::flush;
}

double entity::ret_x() const {
	return x;
}

double entity::ret_y() const {
	return y;
}

double entity::ret_z() const {
	return z;
}


const char* entity::ret_layer_name() const {
	return layer.c_str();
}

const char* entity::ret_ltype_name() const {
	return linetype.c_str();
}

double entity::ret_min_x() const {
	return min_x;
}

double entity::ret_max_x() const {
	return max_x;
}

double entity::ret_min_y() const {
	return min_y;
}

double entity::ret_max_y() const {
	return max_y;
}

void entity::test_coord(double x, double y) {
	if ( x < min_x ) {
		min_x = x;
	}
	if ( x > max_x ) {
		max_x = x;
	}
	if ( y < min_y ) {
		min_y = y;
	}
	if ( y > max_y ) {
		max_y = y;
	}
}


void entity::reset_extents() {
	min_x = -1e20;
	max_x = 1e20;
	min_y = -1e20;
	max_y = 1e20;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VERTEX
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


vertex::vertex(const std::vector<dxfpair> &info)
: bulge(0)
{
	// Get the vertex information
	
	basic_entity(info);
	for (const auto &i : info) {
		switch(i.group_code) {
			case 42:
				bulge = i.value_float();
				break;
		}
	}	
}

double vertex::ret_bulge() const {
	return bulge;
}

void vertex::display() const {
	std::cout << "VERTEX\n";
	std::cout << "\tx = " << x << "\ty = " << y << "\tz = " << z << "\tbulge = " << bulge << std::flush;
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// POLYLINE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The polyline is handled a little differently compared to the other entities because a POLYLINE is built from a bunch of VERTEX entities
polyline::polyline(const std::vector<std::vector<dxfpair> > &sections) :
pline_flag(0), curves_flag(0)
{
	reset_extents();
	// get the polyline information
	basic_entity(sections[0]);
	for (const auto &dpair : sections[0]) {
		switch(dpair.group_code) {
			case 70:
				pline_flag = dpair.value_int();
				break;
			case 40:
				start_width = dpair.value_int();
				break;
			case 41:
                end_width = dpair.value_int();
				break;
			case 75:
				curves_flag = dpair.value_int();
				break;
		}
	}
	// Now add the VERTEX entities to the POLYLINE
	for (size_t i = 1; i < sections.size(); i++) {
		points.push_back(vertex(sections[i]));
	}
}


const std::vector<vertex> &polyline::ret_points() const {
	return points;
}

double polyline::bulge(size_t point) const {
	return points[point].ret_bulge();
}

double polyline::bulge_r(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	return r;
}

double polyline::bulge_start_angle(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	//double xmid = dx/2 + points[point].ret_x();
	//double ymid = dy/2 + points[point].ret_y();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	//double a = fabs(bulge*l/2);
	//double sb = bulge/fabs(bulge); //sign of bulge
	//double theta_p = 4*atan(bulge);	
	double theta_c;
	dx != 0 ? theta_c = atan(dy/dx) : theta_c = 1.57079632679489661923;  // Check to make sure that dx is not zero and will give a negative number
	//if (dx > 0)	sb *= -1; // Correct for different point ordering and bulge direction

	//double cx = xmid + sb*(r-a)*sin(theta_c);
	//double cy = ymid - sb*(r-a)*cos(theta_c);
	
	// Now calculate the angle 
	double theta = asin(points[point].ret_x()/r);
	if (dy < 0) theta = 6.2831853 - theta;  // The angle is greater than pi so fix this because max(asin) = pi
	
	return theta;	
}

double polyline::bulge_end_angle(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	//double xmid = dx/2 + points[point].ret_x();
	//double ymid = dy/2 + points[point].ret_y();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	//double a = fabs(bulge*l/2);
	//double sb = bulge/fabs(bulge); //sign of bulge
	//double theta_p = 4*atan(bulge);	
	double theta_c;
	dx != 0 ? theta_c = atan(dy/dx) : theta_c = 1.57079632679489661923;  // Check to make sure that dx is not zero and will give a negative number
	//if (dx > 0)	sb *= -1; // Correct for different point ordering and bulge direction

	//double cx = xmid + sb*(r-a)*sin(theta_c);
	//double cy = ymid - sb*(r-a)*cos(theta_c);
	
	// Now calculate the angle 
	double theta = asin(points[point+1].ret_x()/r);
	if (dy < 0) theta = 6.2831853 - theta;  // The angle is greater than pi so fix this because max(asin) = pi
	
	return theta;	
}


bool polyline::is_closed() const {
	// pline-flag holds info about closed pline in the 1 bit.  The info is bit wise encoded so use bit wise operators
	return bool(pline_flag&1);
}

void polyline::display() const {
	std::cout << "POLYLINE\n";
	entity_display();
	std::cout << std::endl;
	for (const auto &point : points) {
		point.display();
		std::cout << std::endl;
	}
	std::cout << std::endl;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LWPOLYLINE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The lwpolyline is different than the polyline because there are no vertex entities.  Use the same basic process as the polyline but parse it out differently
lwpolyline::lwpolyline(const std::vector<dxfpair> &section) :
pline_flag(0), curves_flag(0)
{
	
	// First break up the data into the same format that is used by the polyline entity
	std::vector<std::vector<dxfpair> > sections;
	std::vector<dxfpair> first;
	std::vector<dxfpair> others;

	int already_found = 0;
	
	for (const auto &sect : section) {
		const int gc = sect.group_code;
		// Encode bitwise information to keep track of what has been found
		int vertex_part = 0;
        
		if (gc == 10) vertex_part += 1;
		if (gc == 20) vertex_part += 2;
		if (gc == 30) vertex_part += 4;
		if (gc == 40) vertex_part += 8;
		if (gc == 41) vertex_part += 16;
		if (gc == 42) vertex_part += 32;
		//std::cerr << "\n\nvertex_part = " << vertex_part << std::endl << "already_found = " << already_found << std::endl << "(vertex_part&already_found) = " << (vertex_part&already_found) << std::endl;
		if (vertex_part == 0) {
			// If header stuff has been found save it under first.
			// I.E. in a polyline the first set of information is linetype and layer, all of what should be in here
			first.push_back(sect);
		}
		else if ((vertex_part & already_found) == 0) {
			// Now work on what would be the vertex information
			// New information is still being found so keep saving it
			others.push_back(sect);
			//std::cerr << "add to others" << std::endl;
			already_found += vertex_part;  // Keep track of what has been found			
		} else {
			sections.push_back(others);
			//std::cerr << "sections.size() = " << sections.size() << std::endl;
			// Now clear the information out and start over
			others.clear();
			others.push_back(sect);
			already_found = vertex_part;			
		}
	}
	
	// Now put on the last data that was found 
	if (others.size() > 0) {
		sections.push_back(others);
	}
	
	reset_extents();
	basic_entity(first);

    for (const auto &i : first) {
		switch(i.group_code) {
			case 70:
				pline_flag = i.value_int();
				break;
			case 40:
				start_width = i.value_int();
				break;
			case 41:
				end_width = i.value_int();
				break;
			case 75:
				curves_flag = i.value_int();
				break;
		}
	}
	// Now add the VERTEX entities to the POLYLINE
	for (const auto &s : sections) {
		points.push_back(vertex(s));
	}
    
}

const std::vector<vertex> &lwpolyline::ret_points() const {
	return points;
}

double lwpolyline::bulge(size_t point) const {
	return points[point].ret_bulge();
}

double lwpolyline::bulge_r(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	return r;
}

double lwpolyline::bulge_start_angle(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	//double xmid = dx/2 + points[point].ret_x();
	//double ymid = dy/2 + points[point].ret_y();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	//double a = fabs(bulge*l/2);
	//double sb = bulge/fabs(bulge); //sign of bulge
	//double theta_p = 4*atan(bulge);	
	double theta_c;
	dx != 0 ? theta_c = atan(dy/dx) : theta_c = 1.57079632679489661923;  // Check to make sure that dx is not zero and will give a negative number
	//if (dx > 0)	sb *= -1; // Correct for different point ordering and bulge direction

	//double cx = xmid + sb*(r-a)*sin(theta_c);
	//double cy = ymid - sb*(r-a)*cos(theta_c);
	
	// Now calculate the angle 
	double theta = asin(points[point].ret_x()/r);
	if (dy < 0) theta = 6.2831853 - theta;  // The angle is greater than pi so fix this because max(asin) = pi
	
	return theta;	
}

double lwpolyline::bulge_end_angle(size_t point) const {
	// Make sure we are not exceeding the bounds of the points vector
	if (point >= (points.size()-1)) return 0;
	
	double dx = points[point+1].ret_x() - points[point].ret_x();
	double dy = points[point+1].ret_y() - points[point].ret_y();
	double bulge = points[point].ret_bulge();
	//double xmid = dx/2 + points[point].ret_x();
	//double ymid = dy/2 + points[point].ret_y();
	double l = sqrt(dx*dx + dy*dy);
	double r = fabs(l*(bulge*bulge+1)/bulge/4);
	
	//double a = fabs(bulge*l/2);
	//double sb = bulge/fabs(bulge); //sign of bulge
	//double theta_p = 4*atan(bulge);	
	double theta_c;
	dx != 0 ? theta_c = atan(dy/dx) : theta_c = 1.57079632679489661923;  // Check to make sure that dx is not zero and will give a negative number
	//if (dx > 0)	sb *= -1; // Correct for different point ordering and bulge direction

	//double cx = xmid + sb*(r-a)*sin(theta_c);
	//double cy = ymid - sb*(r-a)*cos(theta_c);
	
	// Now calculate the angle 
	double theta = asin(points[point+1].ret_x()/r);
	if (dy < 0) theta = 6.2831853 - theta;  // The angle is greater than pi so fix this because max(asin) = pi
	
	return theta;	
}

bool lwpolyline::is_closed() const {
	// pline-flag holds info about closed pline in the 1 bit.  The info is bit wise encoded so use bit wise operators
	return bool(pline_flag&1);
}

void lwpolyline::display() const {
	std::cout << "lwpolyline\n";
	entity_display();
	std::cout << std::endl;
	for (const auto &point : points) {
		point.display();
		std::cout << std::endl;
	}
	std::cout << std::endl;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ARC
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


arc::arc(const std::vector<dxfpair> &info) {
	
	reset_extents();
	basic_entity( info );
	for (const auto &i : info) {
		switch(i.group_code) {
			case 40:
				radius = i.value_float();
				test_coord(x+radius, y+radius);
				test_coord(x-radius, y-radius);
				break;
			case 50:
				start_angle = i.value_float();
				break;
			case 51:
				end_angle = i.value_float();
				break;
			default:
				break;
		}
	}
}

double arc::ret_radius() const {
	return radius;
}


double arc::ret_srt_ang() const {
	return start_angle;
}


double arc::ret_end_ang() const {
	return end_angle;
}

double arc::ret_srt_ang_rads() const {
    return start_angle*3.14159/180;
}

double arc::ret_end_ang_rads() const {
    return end_angle*3.14159/180;
}

void arc::display() const {
	std::cout << "ARC\n";
	entity_display();
	std::cout << "\n\tradius = " << radius << "\tstart_angle = " << start_angle << "end_angle = " << end_angle << std::flush;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CIRCLE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


circle::circle(const std::vector<dxfpair> &info) {
	
	reset_extents();
	basic_entity(info);
	for (const auto &i : info) {
		switch(i.group_code) {
			case 40:
				radius = i.value_float();
				test_coord(x+radius, y+radius);
				test_coord(x-radius, y-radius);
				break;
		}
	}
}

void circle::display() const {
	std::cout << "CIRCLE\n";
	entity_display();
	std::cout << "\n\tradius = " << radius << std::flush;
}


double circle::ret_radius() const {
	return radius;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LINE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


line::line( const std::vector<dxfpair> &info) {
	
	reset_extents();
	basic_entity(info);
	for (const auto &i : info) {
		switch( i.group_code ) {
			case 11:
				xf = i.value_float();
				break;
			case 21:
				yf = i.value_float();
				break;
			case 31:
				zf = i.value_float();
				break;
		}
	}
	test_coord(xf,yf);
		
}

void line::display() const {
	std::cout << "LINE\n";
	entity_display();
	std::cout << "\n\txf = " << xf << "\tyf = " << yf << "\tzf = " << zf << std::flush;
}


double line::ret_xf() const {
	return xf;
}

double line::ret_yf() const {
	return yf;
}

double line::ret_zf() const {
	return zf;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

text::text( const std::vector<dxfpair> &info) {
	
	reset_extents();
	basic_entity(info);
	for (const auto &i : info) {
		switch(i.group_code) {
			case 1:
				dxf_text = i.value_char(); // directly copy the text into a string
				break;
			case 40:
				text_height = i.value_float();
				break;
			case 50:
				text_rotation = i.value_float();
				break;
		}
	}
}

const char * text::ret_text() const {
	return dxf_text.c_str();
}

double text::ret_txt_ht() const {
	return text_height;
}

double text::ret_txt_rot() const {
	return text_rotation;
}

void text::display() const {
	std::cout << "TEXT\n";
	entity_display();
	std::cout << "\ndxf_text = " << ret_text() << std::flush;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INSERT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

insert::insert(const std::vector<dxfpair> &info) :
x_scale_factor(1), y_scale_factor(1), z_scale_factor(1), rotation(0)
{
	basic_entity(info);
	for (const auto &i : info) {
		switch(i.group_code) {
			case 2:
				block_name = i.value_str(); // directly copy the text into a string
				break;
			case 41:
				x_scale_factor = i.value_float();
				break;
			case 42:
				y_scale_factor = i.value_float();
				break;
			case 43:
				z_scale_factor = i.value_float();
				break;
			case 50:
				rotation = i.value_float();
				break;
		}
	}
}


const char * insert::name() const {
	return block_name.c_str();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// entities
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	


entities::entities(const std::vector<std::vector<dxfpair> > &sections) {
	// Read the main information about the entities section and then put it in the enetites class
	
	for(size_t i = 0; i < sections.size(); i++) {
		const int value = determine_entity(sections[i][0].value_char());
		switch (value) {
			case 0:
				// Get everything from the start of the polyline designation to an SEQEND value
            {
                std::vector<std::vector<dxfpair> > pline;
				do {
					pline.push_back(sections[i]);
				}
                while (strncasecmp(sections[++i][0].value_char(), "SEQEND", 6) != 0);
				ents_polyline.push_back(polyline(pline));
				break;
            }
			
			case 1:
				// ARC
				ents_arc.push_back(arc(sections[i]));
				break;
			case 2:
				// CIRCLE
				ents_circle.push_back(circle(sections[i]));
				break;
			case 3:
				// LINE
				ents_line.push_back(line(sections[i]));
				break;
			case 11:
				// TEXT
				ents_text.push_back(text(sections[i]));
				break;
			case 21:
				// LWPOLYLINE
				ents_lwpolyline.push_back(lwpolyline(sections[i]));
			case 9:
				// INSERT
				ents_insert.push_back(insert(sections[i]));
			
			//default:
				// Nothing here
		}
		
	}
	
}


// Maybe all of this could be turned into fewer function by using templates, but no time right now.  MBS

const std::vector<polyline> &entities::ret_plines() const {
	return ents_polyline;
}

const std::vector<lwpolyline> &entities::ret_lwplines() const {
	return ents_lwpolyline;
}


const std::vector<arc> &entities::ret_arcs() const {
	return ents_arc;
}

const std::vector<circle> &entities::ret_circles() const {
    return ents_circle;
}


const std::vector<line> &entities::ret_lines() const {
	return ents_line;
}


const std::vector<text> &entities::ret_texts() const {
	return ents_text;
}


const std::vector<insert> &entities::ret_inserts() const {
	return ents_insert;
}



// Overload the return function to depend on the layer
std::vector<polyline> entities::ret_plines(const char * layer) const {
	std::vector<polyline> pls;
	
    for(const auto &ent : ents_polyline) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			pls.push_back(ent);
		}
	}
	return pls;
}


std::vector<lwpolyline > entities::ret_lwplines(const char * layer) const {
	std::vector<lwpolyline > lwpls;
	
	for(const auto &ent : ents_lwpolyline) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			lwpls.push_back(ent);
		}
	}
	return lwpls;
}


std::vector<circle> entities::ret_circles(const char * layer) const {
	std::vector<circle> circs;
	
	for(const auto &ent : ents_circle) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			circs.push_back(ent);
		}
	}
	return circs;
}


std::vector<line> entities::ret_lines(const char * layer) const {
	std::vector<line> lns;
	
	for(const auto &ent : ents_line) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			lns.push_back(ent);
		}
	}
	return lns;
}


std::vector<text> entities::ret_texts(const char * layer) const {
	std::vector<text> txts;
	
	for(const auto &ent : ents_text) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			txts.push_back(ent);
		}
	}
	return txts;
}


/*std::vector<ellipse> entities::ret_ellipses(char * layer) const {
	std::vector<polyline> pls;
	
	for(const auto &ent : ents_polyline) {
		if (strcmp(layer, ent.ret_layer_name()) == 0 ) {
			pls.push_back(ent);
		}
	}
	return pls;
}*/


std::vector<arc> entities::ret_arcs(const char * layer) const {
	std::vector<arc> a;
	
	for(const auto &ent : ents_arc) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			a.push_back(ent);
		}
	}
	return a;
}


std::vector<insert> entities::ret_inserts(const char * layer) const {
	std::vector<insert> ins;
	
	for(const auto &ent : ents_insert) {
		if (strcmp(layer, ent.ret_layer_name()) == 0) {
			ins.push_back(ent);
		}
	}
	return ins;
}


void entities::display_all() const {
	for (const auto &ent : ents_polyline) {
		ent.display();
	}
	std::cout << std::endl;
	for (const auto &ent : ents_lwpolyline) {
		ent.display();
	}
	std::cout << std::endl;
	for (const auto &ent : ents_circle) {
		ent.display();
	}
	std::cout << std::endl;
	for (const auto &ent : ents_text) {
		ent.display();
	}
}


