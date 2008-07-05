// $Id$

#ifndef ROMHARRYFOX_HH
#define ROMHARRYFOX_HH

#include "RomBlocks.hh"

namespace openmsx {

class RomHarryFox : public Rom16kBBlocks
{
public:
	RomHarryFox(MSXMotherBoard& motherBoard, const XMLElement& config,
	            std::auto_ptr<Rom> rom);

	virtual void reset(const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

REGISTER_MSXDEVICE(RomHarryFox, "RomHarryFox");

} // namespace openmsx

#endif
