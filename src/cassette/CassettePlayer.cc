// $Id$

#include "CassettePlayer.hh"
#include "CommandController.hh"
#include "MSXConfig.hh"
#include "libxmlx/xmlx.hh"
#include "File.hh"
#include "CassetteImage.hh"
#include "WavImage.hh"
#include "CasImage.hh"
#include "DummyCassetteImage.hh"
#include "Mixer.hh"
#include "RealTime.hh"
#include <cstdlib>


namespace openmsx {

MSXCassettePlayerCLI msxCassettePlayerCLI;

MSXCassettePlayerCLI::MSXCassettePlayerCLI()
{
	CommandLineParser::instance()->registerOption("-cassetteplayer", this);
	CommandLineParser::instance()->registerFileClass("rawtapeimages", this);
}

bool MSXCassettePlayerCLI::parseOption(const string &option,
                                       list<string> &cmdLine)
{
	parseFileType(getArgument(option, cmdLine));
	return true;
}
const string& MSXCassettePlayerCLI::optionHelp() const
{
	static const string text(
	  "Put raw tape image specified in argument in virtual cassetteplayer");
	return text;
}

void MSXCassettePlayerCLI::parseFileType(const string &filename_)
{
	string filename(filename_); XML::Escape(filename);
	ostringstream s;
	s << "<?xml version=\"1.0\"?>";
	s << "<msxconfig>";
	s << " <config id=\"cassetteplayer\">";
	s << "  <parameter name=\"filename\">" << filename << "</parameter>";
	s << " </config>";
	s << "</msxconfig>";

	MSXConfig *config = MSXConfig::instance();
	UserFileContext context;
	config->loadStream(context, s);
}
const string& MSXCassettePlayerCLI::fileTypeHelp() const
{
	static const string text("Raw tape image, as recorded from real tape");
	return text;
}


CassettePlayer::CassettePlayer()
	: cassette(NULL), motor(false), forcePlay(false)
{
	removeTape();

	MSXConfig *conf = MSXConfig::instance();
	if (conf->hasConfigWithId("cassetteplayer")) {
		Config *config = conf->getConfigById("cassetteplayer");
		const string &filename = config->getParameter("filename");
		try {
			insertTape(config->getContext(), filename);
		} catch (MSXException& e) {
			throw FatalError("Couldn't load tape image: " + filename);
		}
	} else {
		// no cassette image specified
	}
	CommandController::instance()->registerCommand(this, "cassetteplayer");

	int bufSize = Mixer::instance()->registerSound(this,
	                                               5000, Mixer::MONO);
	buffer = new int[bufSize];
}

CassettePlayer::~CassettePlayer()
{
	Mixer::instance()->unregisterSound(this);
	delete[] buffer;

	CommandController::instance()->unregisterCommand(this, "cassetteplayer");
	delete cassette;
}

void CassettePlayer::insertTape(FileContext &context,
                                const string &filename)
{
	CassetteImage *tmp;
	try {
		// first try WAV
		tmp = new WavImage(context, filename);
	} catch (MSXException &e) {
		// if that fails use CAS
		tmp = new CasImage(context, filename);
	}
	delete cassette;
	cassette = tmp;

	rewind();
}

void CassettePlayer::removeTape()
{
	delete cassette;
	cassette = new DummyCassetteImage();
}

void CassettePlayer::rewind()
{
	tapeTime = EmuTime::zero;
	playTapeTime = EmuTime::zero;
}

void CassettePlayer::updatePosition(const EmuTime &time)
{
	if (motor || forcePlay) {
		tapeTime += (time - prevTime);
		playTapeTime = tapeTime;
	}
	prevTime = time;
}

void CassettePlayer::setMotor(bool status, const EmuTime &time)
{
	updatePosition(time);
	motor = status;
}

short CassettePlayer::getSample(const EmuTime &time)
{
	if (motor || forcePlay) {
		return cassette->getSampleAt(time);
	} else {
		return 0;
	}
}

short CassettePlayer::readSample(const EmuTime &time)
{
	updatePosition(time);
	return getSample(tapeTime);
}

void CassettePlayer::writeWave(short *buf, int length)
{
	// recording not implemented yet
}

int CassettePlayer::getWriteSampleRate()
{
	// recording not implemented yet
	return 0;	// 0 -> not interested in writeWave() data
}


const string& CassettePlayer::getName() const
{
	static const string name("cassetteplayer");
	return name;
}

const string& CassettePlayer::getDescription() const
{
	static const string desc("Cassetteplayer, use to read .cas or .wav files.\n");
	return desc;
}

void CassettePlayer::plug(Connector* connector, const EmuTime& time)
	throw()
{
}

void CassettePlayer::unplug(const EmuTime& time)
{
}


string CassettePlayer::execute(const vector<string> &tokens)
	throw (CommandException)
{
	string result;
	if (tokens.size() != 2) {
		throw CommandException("Syntax error");
	}
	if (tokens[1] == "eject") {
		result += "Tape ejected\n";
		removeTape();
	} else if (tokens[1] == "rewind") {
		result += "Tape rewinded\n";
		rewind();
	} else if (tokens[1] == "force_play") {
		forcePlay = true;
	} else if (tokens[1] == "no_force_play") {
		forcePlay = false;
	} else {
		try {
			result += "Changing tape\n";
			UserFileContext context;
			insertTape(context, tokens[1]);
		} catch (MSXException &e) {
			throw CommandException(e.getMessage());
		}
	}
	return result;
}

string CassettePlayer::help(const vector<string> &tokens) const throw()
{
	return "cassetteplayer eject      : remove tape from virtual player\n"
	       "cassetteplayer <filename> : change the tape file\n";
}

void CassettePlayer::tabCompletion(vector<string> &tokens) const throw()
{
	if (tokens.size() == 2) {
		CommandController::completeFileName(tokens);
	}
}


void CassettePlayer::setInternalVolume(short newVolume)
{
	volume = newVolume;
}

void CassettePlayer::setSampleRate(int sampleRate)
{
	delta = EmuDuration(1.0 / sampleRate);
}

int *CassettePlayer::updateBuffer(int length) throw()
{
	if (!motor && !forcePlay) {
		return NULL;
	}
	int *buf = buffer;
	while (length--) {
		*(buf++) = (((int)getSample(playTapeTime)) * volume) >> 15;
		playTapeTime += delta;
	}
	return buffer;
}

} // namespace openmsx
