// $Id$

#ifndef __LEDS_HH__
#define __LEDS_HH__

namespace openmsx {

class CliCommOutput;

class Leds 
{
public:
	enum LEDCommand {
		POWER_ON, POWER_OFF,
		CAPS_ON,  CAPS_OFF,
		KANA_ON,  KANA_OFF,	// same as CODE LED
		PAUSE_ON, PAUSE_OFF,
		TURBO_ON, TURBO_OFF,
		FDD_ON,   FDD_OFF
	};
	
	static Leds& instance();
	void setLed(LEDCommand led);
	
private:
	Leds();
	~Leds();

	bool pwrLed, capsLed, kanaLed, turboLed;
	int  pauseLedCounter; // only used to combine mutliple PAUSE leds
	int  fddLedCounter; // only used to combine mutliple FDD leds

	CliCommOutput& output;
};

} // namespace openmsx
#endif
