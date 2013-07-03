/*
 * Code for converting dxf entities to SVG elements
 * There are multiple ways for converting different items
 * If possible most DXF enetities will be converted to paths because that is the most flexable object
 *
 * Author:
 *   Matt Squires <squiresm@colorado.edu>
 *
 * Copyright (C) 2005 Matt Squires
 *
 * Released under GNU GPL and LGPL, read the file 'GPL.txt' and 'LGPL.txt' for details
 */



/* 

Matt Squires
SoC 2005

*/

#include"entities2elements.h"
#include"tables2svg_info.h"
#include<iostream>
#include<math.h>

#include <stdlib.h>
#include <string.h>

// The names indicate the DXF entitiy first and the SVG element last

// Common elements
static char *to_arc(double bulge, double r, double start_ang, double end_ang, int precision, const char* delim, const char * units, double scaling, char *out){
	// This is used for arcs, polylines, and lwpolylines
	char temp[50];
	
	// Assume that we are adding to the input and not starting over
	strcat(out," A ");
	// For arcs there is only one radius
	strcat(out,gcvt(scaling*r,precision,temp) );
	strcat(out,",");
	strcat(out,gcvt(scaling*r,precision,temp) );
	
	strcat(out," 0"); // For arc assume no x-axis rotation.  That seems to apply to elipse elements only
	// Determine if it is a large arc
	if ( (end_ang > start_ang) && ( (end_ang - start_ang) > 180) ){
		//strcat(out," 1,0 "); // Large arc flag...Always use a zero sweep flag
		strcat(out," 1, "); // Large arc flag...Always use a zero sweep flag
	}
	else{
		//strcat(out," 0,0 "); // Small arc flag...Always use a zero sweep flag
		strcat(out," 0,");
	}
	// This may be easier if I allow r to be plus and minus, but for now this works
	if (bulge > 0){
		strcat(out,"0 ");
	}
	else{
		strcat(out,"1 ");
	}

    return out;
}

// Build Coordinate
// Pairs of coords with units will be used so often build a function
static char *coord(entity *ent, int precision, const char* delim, const char * units, double scaling, char *out){
	// Pairs of coords with units will be used so often build a function build a dedicated function for returning such
	
	char temp[20];
	if (units != NULL) scaling = 1;  // If units have been defined then ignore the scaling parameter
	strcat(out, gcvt(scaling*ent->ret_x(),precision,temp) );  // There must be a better function for double to ascii conversion that is defined in most libraries
	if (units != NULL) strcat(out, units);
	strcat(out, delim);
	strcat(out, gcvt(-scaling*ent->ret_y(),precision,temp) ); // Because SVG has a the Y-axis pointed down multiply by -1
	if (units != NULL) strcat(out, units);
	strcat(out, " ");
    return out;
}

// DXF Polyline -> SVG
// General function for the conversion of a pline to a SVG element.  Very similar functions just make accomidations for parts that may not be supported
void pline2svg(polyline pline, int type, int precision, const char * units, double scaling, tables plot_info, char *out){
	// 0 is pline2path
	// 1 is pline2pline
	// 2 is pline2polygon
	
	
	double mag_bulge = 0;
	double prev_mag_bulge = 0;
	
	std::vector< vertex >::iterator vver_iter;
	std::vector< vertex > points = pline.ret_points();
	
	if (type < 1){
		// Put the first Move To at the first, everything else will be a lineto
		const char *delim = " ";
		strcat(out, "M ");
		coord( &points[0], precision, delim, units, scaling, out ); 
		prev_mag_bulge = sqrt(pow(points[0].ret_bulge(),2));  // Because the bulge value can be positive or negative calculate the magnitude
        if ( prev_mag_bulge > pow(0.1,precision) ){
            to_arc(pline.bulge(0), pline.bulge_r(0), pline.bulge_start_angle(0), pline.bulge_end_angle(0), precision, delim, units, scaling, out);
        }
		for (size_t i = 1; i < points.size(); i++){
			if ( prev_mag_bulge < pow(0.1,precision) ){
				// If the previous point was a bulge then don't use a line to
				strcat(out, "L ");
			}
			coord( &points[i], precision, delim, units, scaling, out ); 
			// If bulge > some precsion then add bulge
			mag_bulge = sqrt(pow(points[i].ret_bulge(),2));
			if ( (mag_bulge > pow(0.1,precision))  && (i < (points.size() - 1) )){
				to_arc(pline.bulge(i), pline.bulge_r(i), pline.bulge_start_angle(i), pline.bulge_end_angle(i), precision, delim, units, scaling, out);
			}
			prev_mag_bulge = mag_bulge;
		}
		if ( pline.is_closed() ){
			strcat(out,"z");
		}
		strcat(out,"\" ");
	}
	else{
		const char *delim = ",";
		for (size_t i = 0; i < points.size(); i++){
			coord( &points[i], precision, delim, NULL, scaling, out );
			// If bulge > some precsion then add bulge	
		}
		// if the element is a SVG::pline and the DXF::pline is closed then simulate by adding an extra point
		if ( (type == 1) && pline.is_closed() ){
			coord( &points[0], precision, delim, NULL, scaling, out ); 
		}
	}	
		
}

char* pline2path(polyline pline, const char * units, double scaling, tables plot_info, char * out){
	// Convert a dxf polyline to a SVG path.  This is the closest conversion of the DXF polyline to an SVG element
	strcat(out, "<path d=\"");
	entity *ent_ptr = &pline;
	int precision = 6;
	
	pline2svg(pline, 0, precision, units, scaling, plot_info, out);
	// Add some line information
	strcat(out,"fill=\"none\" stroke=\"black\" stroke-width=\"1\" ");
	pattern2dasharray(plot_info.ret_ltype( ent_ptr->ret_ltype_name(),ent_ptr->ret_layer_name() ), precision, scaling, out);  // Add the linetype information
	
	strcat(out,"/>\n");
	return out;	
}

char* pline2pline(polyline pline, const char * units, double scaling, tables plot_info, char * out){
	// Convert a dxf polyline to a SVG polyline.  The conversion is not 1:1 because the SVG pline doesn't support closed objects or curves
	entity *ent_ptr = &pline;
	int precision = 6;
	
	strcat(out, "<polyline fill=\"none\" stroke=\"black\" stroke-width=\"1\" ");
	ltype linfo = plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name());
	pattern2dasharray(linfo, precision, scaling, out);  // Add the linetype information
	
	strcat(out,"points=\"");
	
	//strcat(out,"<polyline fill=\"none\" stroke=\"black\" stroke-width=\"1\" points=\"");
	pline2svg(pline, 1, precision, units, scaling, plot_info, out);
	// Add some line information
	// if the DXF pline is closed then add an extra point
	
	strcat(out,"\"/>\n");
	return out;
}
//char* pline2polygon(polyline pline, const char * units, double scaling, tables plot_info){
	// Convert a dxf polyline to a SVG polygon.  The conversion is not 1:1 because the SVG polygone assumes a closed path.  If the pline is not closed it will be forced closed
	//return pline2svg(pline, 2, 6, units, double scaling,out);	
//}


// DXF LWPolyline -> SVG


// This could be a template with polyline and lwpolyline but right now it is not that important
void lwpline2svg(lwpolyline pline, int type, int precision, const char * units, double scaling, tables plot_info, char *out){
	// 0 is pline2path
	// 1 is pline2pline
	// 2 is pline2polygon
	
	
	double mag_bulge = 0;
	double prev_mag_bulge = 0;
	
	std::vector< vertex >::iterator vver_iter;
	std::vector< vertex > points = pline.ret_points();
	
	if (type < 1){
	    const char *delim = " ";
		// Put the first Move To at the first, everything else will be a lineto
		strcat(out, "M ");
		coord( &points[0], precision, delim, NULL, scaling, out );
		prev_mag_bulge = sqrt(pow(points[0].ret_bulge(),2));  // Because the bulge value can be positive or negative calculate the magnitude
		if ( prev_mag_bulge > pow(0.1,precision) ){
				to_arc(pline.bulge(0),pline.bulge_r(0), pline.bulge_start_angle(0), pline.bulge_end_angle(0), precision, delim, NULL, scaling, out);
		}
		
		for (size_t i = 1; i < points.size(); i++){
			if ( prev_mag_bulge < pow(0.1,precision) ){
				// If the previous point was a bulge then don't use a line to
				strcat(out, "L ");
			}
			coord( &points[i], precision, delim, NULL , scaling, out ); 
			// If bulge > some precsion then add bulge
			mag_bulge = sqrt(pow(points[i].ret_bulge(),2));
			if ( ( mag_bulge > pow(0.1,precision) ) && (i < (points.size() - 1) )){ // Make sure the final point doesn't add a bulge on accident
				to_arc(pline.bulge(i), pline.bulge_r(i), pline.bulge_start_angle(i), pline.bulge_end_angle(i), precision, delim, units, scaling, out);
			}
			prev_mag_bulge = mag_bulge;	
		}
		if ( pline.is_closed() ){
			strcat(out,"z");
		}
		strcat(out,"\" ");
	}
	else{
	    const char *delim = ",";
		for (size_t i = 0; i < points.size(); i++){
			coord( &points[i], precision, delim, units, scaling, out ); 
			// If bulge > some precsion then add bulge	
		}
		// if the element is a SVG::pline and the DXF::pline is closed then simulate by adding an extra point
		if ( (type == 1) && pline.is_closed() ){
			coord( &points[0], precision, delim, units, scaling, out ); 
		}
	}	
		
}

char* lwpline2path(lwpolyline pline, const char * units, double scaling, tables plot_info, char *out){
	// Convert a dxf polyline to a SVG path.  This is the closest conversion of the DXF polyline to an SVG element
	entity *ent_ptr = &pline;
	int precision = 6;

	strcat(out, "<path d=\"");
	
	lwpline2svg(pline, 0, precision, units, scaling, plot_info, out);
	// Add some line information
	strcat(out,"fill=\"none\" stroke=\"black\" stroke-width=\"1\" ");
	pattern2dasharray(plot_info.ret_ltype( ent_ptr->ret_ltype_name(),ent_ptr->ret_layer_name() ), precision, scaling, out);  // Add the linetype information
	
	strcat(out,"/>\n");
	return out;	
}

// DXF ARC -> SVG
char* arc2path(arc a, int precision, const char * units, double scaling, tables plot_info, char *out){
	// So far this appears to be the only way to convert arcs into something recognized by SVG
	char *out_ptr;
	char temp[20];
	entity *ent_ptr = &a;
	
	strcpy(out,"<path d=\"M");
	// Calculate the starting point from the center and the start angle.  As far as I can tell the rotation is CCW in the dxf notation and it in degrees
	strcat(out,gcvt(scaling*(ent_ptr->ret_x()+a.ret_radius()*cos( a.ret_srt_ang()*3.14159/180 )),precision,temp) );
	strcat(out," ");
	strcat(out,gcvt(-1*scaling*(ent_ptr->ret_y()+a.ret_radius()*sin( a.ret_srt_ang()*3.14159/180 )),precision,temp) );
	strcat(out," A ");
	// For arcs there is only one radius
	strcat(out,gcvt(scaling*a.ret_radius(),precision,temp) );
	strcat(out,",");
	strcat(out,gcvt(scaling*a.ret_radius(),precision,temp) );
	
	strcat(out," 0"); // For arc assume no x-axis rotation.  That seems to apply to elipse elements only
	// Determine if it is a large arc
	if ( (a.ret_end_ang() > a.ret_srt_ang()) && ( (a.ret_end_ang() - a.ret_srt_ang()) > 180) ){
		strcat(out," 1,0 "); // Large arc flag...Always use a zero sweep flag
	}
	else{
		strcat(out," 0,0 "); // Small arc flag...Always use a zero sweep flag
	}
	
	//The final point
	strcat(out,gcvt(scaling*(ent_ptr->ret_x()+a.ret_radius()*cos( a.ret_end_ang()*3.14159/180 )),precision,temp) );
	strcat(out,",");
	strcat(out,gcvt(-1*scaling*(ent_ptr->ret_y()+a.ret_radius()*sin( a.ret_end_ang()*3.14159/180 )),precision,temp) );
	strcat(out,"\" fill=\"none\" stroke=\"black\" stroke-width=\"1\" ");
	ltype linfo = plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name());
	pattern2dasharray(linfo, precision, scaling, out);  // Add the linetype information
	strcat(out, "/>\n");
	
	
	out_ptr = out;
	return out_ptr;
	
}


// DXF Circle -> SVG
char* circle2circle(circle circ, int precision, const char * units, double scaling, tables plot_info, char *out){
	// Direct conversion of DXF circle to SVG circle
	char temp[1000]="\" cy=\"";
	entity *ent_ptr = &circ;
	strcpy(out,"<circle cx=\"");
	coord(ent_ptr, precision, temp, units, scaling, out);
	strcat(out,"\" r=\"");
	strcat(out,gcvt(circ.ret_radius(), precision, temp) );
	strcat(out,units);
	strcat(out,"\" fill=\"none\" stroke=\"black\" stroke-width=\"1\" ");
	ltype linfo = plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name());
	//plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name());
	pattern2dasharray(linfo, precision, scaling, out);  // Add the linetype information
	//pattern2dasharray(plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name()), precision, scaling, out);  // Add the linetype information
	strcat(out, "/>\n");
	return out;
}

char* circle2path(circle circ, int precision, const char * units, double scaling, tables plot_info, char *out){
	// Conversion of DXF circle to SVG circle assuming the path will represent the circle
	
	char temp[20];
	entity *ent_ptr = &circ;	
	
	strcpy(out,"<path d=\"M");
	// The starting point is x-r,y so subtract off the radius from the x coord
	strcat(out,gcvt(ent_ptr->ret_x()-circ.ret_radius(),precision,temp) );
	strcat(out," ");
	strcat(out,gcvt(ent_ptr->ret_y(),precision,temp) );
	
	strcat(out," a");
	strcat(out,gcvt(circ.ret_radius(),precision,temp) );
	strcat(out,",");
	strcat(out,gcvt(circ.ret_radius(),precision,temp) );
	strcat(out,"0 0,0 0,0\"  fill=\"none\" stroke=\"black\" stroke-width=\"1\" />\n");
		
	return out;
}


// DXF Line -> SVG
char* line2line(line ln, int precision, const char * units, double scaling, tables plot_info, char *out){
	// Directly convert DXF to SVG because it works
	char temp[20];
	entity *ent_ptr = &ln;	
	
	strcpy(out,"<line x1=\"");
	strcat(out,gcvt(ent_ptr->ret_x(),precision,temp) );
	strcat(out,units);
	strcat(out,"\" y1=\"");
	strcat(out,gcvt(-1*ent_ptr->ret_y(),precision,temp) ); // Put in an extra minus because of the way SVG has defined the axis
	strcat(out,units);
	
	strcat(out,"\" x2=\"");
	strcat(out,gcvt(ln.ret_xf(),precision,temp) );
	strcat(out,units);
	strcat(out,"\" y2=\"");  
	strcat(out,gcvt(-1*ln.ret_yf(),precision,temp) ); // Put in an extra minus because of the way SVG has defined the axis
	strcat(out,units);
	strcat(out,"\" stroke-width=\"1\" stroke=\"black\" ");
	ltype linfo = plot_info.ret_ltype(ent_ptr->ret_ltype_name(), ent_ptr->ret_layer_name());
	pattern2dasharray(linfo, precision, scaling, out);  // Add the linetype information
	strcat(out, " />");
		
	return out;
}


char* line2path(line ln, int precision, const char * units, double scaling, tables plot_info, char *out){
	// Convert DXF line to SVG path
	
	char temp[20];
	entity *ent_ptr = &ln;	
	
	strcpy(out,"<path d=\"M");
	strcat(out,gcvt(scaling*ent_ptr->ret_x(),precision,temp) );
	strcat(out," ");
	strcat(out,gcvt(scaling*ent_ptr->ret_y(),precision,temp) );
	
	strcat(out," L");
	strcat(out,gcvt(scaling*ln.ret_xf(),precision,temp) );
	strcat(out," ");
	strcat(out,gcvt(scaling*ln.ret_yf(),precision,temp) );
	strcat(out,"\" fill=\"none\" stroke=\"black\" stroke-width=\"1\" /");
	strcat(out, " />");
		
	return out;
}

// DXF Text -> SVG
char* text2text(text txt, int precision, const char * units, double scaling, tables plot_info, char *out){
	// Directly convert DXF to SVG because it works
	char temp[100];	
	entity *ent_ptr = &txt;
	
	// If the text is rotated use the transform matrix

	if ( txt.ret_txt_rot() > precision ){
		strcpy(out, "<g transform=\"matrix(");
		strcat(out,gcvt(cos(0.017453*txt.ret_txt_rot()),precision,temp) );
		strcat(out," ");
		strcat(out,gcvt(sin(0.017453*txt.ret_txt_rot()),precision,temp) );
		strcat(out," ");
		strcat(out,gcvt(-sin(0.017453*txt.ret_txt_rot()),precision,temp) );
		strcat(out," ");
		strcat(out,gcvt(cos(0.017453*txt.ret_txt_rot()),precision,temp) );
		strcat(out," 0 0)\" >\n");
		strcat(out,"<text x=\"");
	}
	else{
		strcpy(out,"<text x=\"");
	}
	strcat(out,gcvt(ent_ptr->ret_x(),precision,temp) );
	strcat(out,units);
	strcat(out,"\" y=\"-"); // Put in an extra minus because of the way SVG has defined the axis
	strcat(out,gcvt(ent_ptr->ret_y(),precision,temp) );
	strcat(out,units);
	strcat(out,"\" font-family=\"Verdana\" font-size=\"");
	strcat(out,gcvt(scaling*txt.ret_txt_ht(),precision,temp) );
	strcat(out,"\" Fill=\"black\"");
	
	strcat(out," >");
	//  Now put in the text
	strcat(out,txt.ret_text());
	
	// Now close the text element
	strcat(out,"</text>");	
	// If the text was rotated finish off the tranform group
	if ( txt.ret_txt_rot() > precision ){
		strcat(out,"</g>");
	}
	
	return out;
}



// DXF Insert -> SVG
char* insert2group(insert in, int precision, const char * units, double scaling, tables plot_info, blocks blks, char *out){
	char tmp_char[100000];
	
	//  get the block using the name from the insert information
	block blk = blks.ret_block(in.name());
	
	entity *ent_ptr = &in;
	entities *ents_ptr = &blk;
	// For now just translations  MBS 22 Aug 05
	strcpy(out, "<g transform=\"matrix(1,0,0,1,");
	strcat(out,gcvt(scaling*ent_ptr->ret_x(),precision,tmp_char) );
	strcat(out,",");
	strcat(out,gcvt(-scaling*ent_ptr->ret_y(),precision,tmp_char) );
	strcat(out,")\" >\n");
	
	
	// Now convert the entities in the block
	std::vector< polyline > plines = ents_ptr->ret_plines();
	std::vector< lwpolyline > lwplines = ents_ptr->ret_lwplines();
	std::vector< arc > arcs = ents_ptr->ret_arcs();
	std::vector< circle > circs = ents_ptr->ret_circles();
	std::vector< line > lns = ents_ptr->ret_lines();
	std::vector< text > txts = ents_ptr->ret_texts();
	
		
	
	for(size_t i = 0; i < plines.size();i++){
		strcat( out,pline2pline(plines[i], units, scaling, plot_info, tmp_char ) );
		strcat( out, "\n" );
	}
	for(size_t i = 0; i < lwplines.size();i++){
		strcat( out,lwpline2path(lwplines[i], units, scaling, plot_info, tmp_char ) );
		strcat( out, "\n" );
	}
	for(size_t i = 0; i < arcs.size();i++){
		strcat( out, arc2path(arcs[i], 6,units, scaling, plot_info, tmp_char ) );
		strcat( out, "\n" );
	}
	for(size_t i = 0; i < circs.size();i++){
		strcat( out, circle2circle(circs[i], 6, units, scaling, plot_info, tmp_char) );
		strcat( out, "\n" );
	}
	for(size_t i = 0; i < lns.size();i++){
		strcat( out, line2line(lns[i], 6, units, scaling, plot_info, tmp_char) );
		strcat( out, "\n" );
	}
	for(size_t i = 0; i < txts.size();i++){
		strcat( out, text2text(txts[i], 6, units, scaling, plot_info, tmp_char) );
		strcat( out, "\n" );
	}
	// End the group
	strcat(out,"</g>");
	
	return out;	
}



void write_by_layer(entities &ents, tables &tbls, blocks &blks, double scaling, const char * units, const char * layer){
	// output_type = 0 is to std:out
	// output_type = 1 is to the input filename but with .dxf on the end
	
	// For now everything will go to stdout later may directed to other places
	
	// Get the various file informations as dependent on the layer type
	std::vector< polyline > plines = ents.ret_plines(layer);
	std::vector< lwpolyline > lwplines = ents.ret_lwplines(layer);
	std::vector< arc > arcs = ents.ret_arcs(layer);
	std::vector< circle > circs = ents.ret_circles(layer);
	std::vector< line > lns = ents.ret_lines(layer);
	std::vector< text > txts = ents.ret_texts(layer);
	std::vector< insert > ins = ents.ret_inserts(layer);
	
	// It would be better to redirect stdout to different places.  That would make the code cleaner but I don't think it will work better
	char tmp_char[100000];
	for(size_t i = 0; i < plines.size();i++){
		std::cout << "\t" << pline2path(plines[i], NULL, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < lwplines.size();i++){
		std::cout << "\t" << lwpline2path(lwplines[i], units, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < arcs.size();i++){
		std::cout << "\t" << arc2path(arcs[i], 6,units, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < circs.size();i++){
		std::cout << "\t" << circle2circle(circs[i], 6, units, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < lns.size();i++){
		std::cout << "\t" << line2line(lns[i], 6, units, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < txts.size();i++){
		std::cout << "\t" << text2text(txts[i], 6, units, scaling, tbls, tmp_char) << std::endl;
	}
	for(size_t i = 0; i < ins.size();i++){
		std::cout << "\t" << insert2group(ins[i], 6, units, scaling, tbls, blks, tmp_char) << std::endl;
	}
}



void write_all(entities &ents, tables &tbls, blocks &blks, double scaling, const char * units){
	// output_type = 0 is to std:out
	// output_type = 1 is to the input filename but with .dxf on the end
	
	// For now everything will go to stdout later may directed to other places
	
	// Get the various file informations as dependent on the layer type
	std::vector< polyline > plines = ents.ret_plines();
	std::vector< lwpolyline > lwplines = ents.ret_lwplines();
	std::vector< arc > arcs = ents.ret_arcs();
	std::vector< circle > circs = ents.ret_circles();
	std::vector< line > lns = ents.ret_lines();
	std::vector< text > txts = ents.ret_texts();
	std::vector< insert > ins = ents.ret_inserts();
	
	// It would be better to redirect stdout to different places.  That would make the code cleaner but I don't think it will work better
	char tmp_char[100000];
    for(size_t i = 0; i < plines.size();i++){
        std::cout << "\t" << pline2path(plines[i], NULL, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < lwplines.size();i++){
        std::cout << "\t" << lwpline2path(lwplines[i], units, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < arcs.size();i++){
        std::cout << "\t" << arc2path(arcs[i], 6,units, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < circs.size();i++){
        std::cout << "\t" << circle2circle(circs[i], 6, units, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < lns.size();i++){
        std::cout << "\t" << line2line(lns[i], 6, units, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < txts.size();i++){
        std::cout << "\t" << text2text(txts[i], 6, units, scaling, tbls, tmp_char) << std::endl;
    }
    for(size_t i = 0; i < ins.size();i++){
        std::cout << "\t" << insert2group(ins[i], 6, units, scaling, tbls, blks, tmp_char) << std::endl;
    }
}



