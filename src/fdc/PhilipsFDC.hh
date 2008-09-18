// $Id$

#ifndef PHILIPSFDC_HH
#define PHILIPSFDC_HH

#include "WD2793BasedFDC.hh"

namespace openmsx {

class PhilipsFDC : public WD2793BasedFDC
{
public:
	PhilipsFDC(MSXMotherBoard& motherBoard, const XMLElement& config);

	virtual void reset(const EmuTime& time);
	virtual byte readMem(word address, const EmuTime& time);
	virtual byte peekMem(word address, const EmuTime& time) const;
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual const byte* getReadCacheLine(word start) const;
	virtual byte* getWriteCacheLine(word address) const;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const bool brokenFDCread;
	byte sideReg;
	byte driveReg;
};

} // namespace openmsx

#endif
