/* Read Blocks from file and convert to vectors of entities
Matt Squires
Google SOC 2005
*/

#ifndef DXF_BLOCKS_H
#define DXF_BLOCKS_H

#include "read_dxf.h"
#include "entities.h"
#include <vector>
#include <string>


class block : public entity, public entities{// : public entities, {
	public:
		block(const std::vector< std::vector< dxfpair > > &sections); // Group all of the blocks as entities
		const char* name() const;
		//void blocks_display();
		
	
	protected:
		std::string block_name;
		double rotation;
		
	private:
		void block_info(const std::vector< dxfpair > &info);
};

class blocks{
	public:
		blocks(const std::vector< std::vector< dxfpair > > &sections);
		const block &ret_block(const char *block_name) const;
		
	protected:
		std::vector< block > blocks_blocks;
};

#endif
