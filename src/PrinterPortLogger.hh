// $Id$

#ifndef MSXPRINTERPORTLOGGER_HH
#define MSXPRINTERPORTLOGGER_HH

#include "PrinterPortDevice.hh"
#include "serialize_meta.hh"
#include <memory>

namespace openmsx {

class CommandController;
class File;
class FilenameSetting;

class PrinterPortLogger : public PrinterPortDevice
{
public:
	explicit PrinterPortLogger(CommandController& commandController);
	virtual ~PrinterPortLogger();

	// PrinterPortDevice
	virtual bool getStatus(const EmuTime& time);
	virtual void setStrobe(bool strobe, const EmuTime& time);
	virtual void writeData(byte data, const EmuTime& time);

	// Pluggable
	virtual const std::string& getName() const;
	virtual const std::string& getDescription() const;
	virtual void plugHelper(Connector& connector, const EmuTime& time);
	virtual void unplugHelper(const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const std::auto_ptr<FilenameSetting> logFilenameSetting;
	std::auto_ptr<File> file;
	byte toPrint;
	bool prevStrobe;
};

} // namespace openmsx

#endif
