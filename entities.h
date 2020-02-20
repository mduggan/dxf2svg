/*  Class for interpereting the entities found in a DXF file
Matt Squires
Google SOC
2 July 05
*/

#ifndef DXF_ENTITIES_H
#define DXF_ENTITIES_H

#include "read_dxf.h"
#include <vector>



class entity {
	public:
    entity() : x(0), y(0), z(0), min_x(0), max_x(0), min_y(0), max_y(0) {};
		void basic_entity(const std::vector<dxfpair> &info); // Extract all of the typical entity information (e.g. layer name, positions)
		void entity_display() const;
		double ret_x() const;
		double ret_y() const;
		double ret_z() const;
		const char* ret_layer_name() const;
		const char* ret_ltype_name() const;
		double ret_min_x() const;
		double ret_max_x() const;
		double ret_min_y() const;
		double ret_max_y() const;	
		
	
	protected:
		std::string layer;
		std::string linetype;
		double x;
		double y;
		double z;
		double min_x;
		double max_x;
		double min_y;
		double max_y;
		void reset_extents();
		void test_coord(double x, double y);
};



class vertex : public entity {
	public:
		vertex(const std::vector<dxfpair> &info);
		void display() const;
		double ret_bulge() const;
	
	private:
		double bulge;
};



class polyline : public entity {
	public:
		polyline(const std::vector<std::vector<dxfpair> > &sections);
		const std::vector<vertex> &ret_points() const;
		double bulge(size_t point) const;
		double bulge_r(size_t point) const;
		double bulge_start_angle(size_t point) const;
		double bulge_end_angle(size_t point) const;
		bool is_closed() const;
		void display() const;
	
	private:
		int pline_flag; // 70
		double start_width; // 40
		double end_width; // 41
		int curves_flag;
		std::vector<vertex > points;
};

class lwpolyline : public entity {
	public:
		lwpolyline(const std::vector<dxfpair> &section);
		const std::vector<vertex> &ret_points() const;
		double bulge(size_t point) const;
		double bulge_r(size_t point) const;
		double bulge_start_angle(size_t point) const;
		double bulge_end_angle(size_t point) const;
		bool is_closed() const;
		void display() const;
	
	private:
		int pline_flag; // 70
		double start_width; // 40
		double end_width; // 41
		int curves_flag;
		std::vector<vertex> points;
};

class arc : public entity {
	public:
		arc(const std::vector<dxfpair> &info);
		double ret_radius() const;
		double ret_srt_ang() const;
		double ret_end_ang() const;
        double ret_srt_ang_rads() const;
        double ret_end_ang_rads() const;
		void display() const;
	
	private:
		double radius;
		double start_angle;
		double end_angle;
};




class circle : public entity {
	public:
		circle(const std::vector<dxfpair> &info);
		void display() const;
		double ret_radius() const;
	
	private:
		double radius;
};


class line : public entity {
	public:
		line(const std::vector<dxfpair> &info);
		void display() const;
		double ret_xf() const;
		double ret_yf() const;
		double ret_zf() const;
	
	private:
		double xf;
		double yf;
		double zf;
};

class ellipse : public entity {
	public:
		ellipse(const std::vector<dxfpair> &info);
		void display() const;
		double ret_x_ma() const;
		double ret_y_ma() const;
		double ret_z_ma() const;
		double ret_ratio() const;
		double ret_start_p() const;
		double ret_end_p() const;
		
	
	private:
		double x_major_axis;
		double y_major_axis;
		double z_major_axis;
		double ratio;
		double start_param;
		double end_param;
};



class text : public entity {
	public:
		text(const std::vector<dxfpair> &info);
		void display() const;
		const char * ret_text() const;
		double ret_txt_ht() const;
		double ret_txt_rot() const;
	
	private:
		std::string dxf_text;
		double text_height; // dxf 40
		double text_rotation; //dxf 50
};


class insert : public entity {
	public:
		insert(const std::vector<dxfpair> &info);
		void display() const;
		const char* name() const;
		double ret_x_sf() const;
		double ret_y_sf() const;
		double ret_z_sf() const;
		double ret_rotation() const;
	
	private:
		std::string block_name;
		double x_scale_factor;
		double y_scale_factor;
		double z_scale_factor;
		double rotation;
};




class entities{
	// Well I said that I would only use STL containers internally, but I would have to use a dynamically linked list, and I haven't done for a long time soo STL is my crutch.
	// I also think that there are others in my same boat that prefer stl containers because they are much easier to use
	public:
		entities(const std::vector<std::vector<dxfpair> > &sections); // Put the various entities into their respective vectors
		void display_all() const;
		const std::vector<polyline> &ret_plines() const;
		const std::vector<circle> &ret_circles() const;
		const std::vector<line> &ret_lines() const;
		const std::vector<text> &ret_texts() const;
		const std::vector<ellipse> &ret_ellipses() const;
		const std::vector<arc> &ret_arcs() const;
		const std::vector<lwpolyline> &ret_lwplines() const;
		const std::vector<insert> &ret_inserts() const;
    
		// Overload the return function to depend on the layer
		std::vector<polyline> ret_plines(const char * layer) const;
		std::vector<circle> ret_circles(const char * layer) const;
		std::vector<line> ret_lines(const char * layer) const;
		std::vector<text> ret_texts(const char * layer) const;
		std::vector<ellipse> ret_ellipses(const char * layer) const;
		std::vector<arc> ret_arcs(const char * layer) const;
		std::vector<lwpolyline> ret_lwplines(const char * layer) const;
		std::vector<insert> ret_inserts(const char * layer) const;
		
		int plines_size() const;
		int circles_size() const;
		int lines_size() const;
		int texts_size() const;
		
	private:
		std::vector<polyline> ents_polyline;
		std::vector<arc> ents_arc;
		std::vector<circle> ents_circle;
		std::vector<line> ents_line;
		std::vector<ellipse> ents_ellipse;
		std::vector<text> ents_text;
		std::vector<lwpolyline> ents_lwpolyline;
		std::vector<insert> ents_insert;
		
};



#endif
