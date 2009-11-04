# $Id$

include build/node-start.mk

SRC_HDR:= \
	Keyboard \
	KeyboardSettings \
	UnicodeKeymap \
	JoystickPort \
	JoystickDevice \
	DummyJoystick \
	Joystick \
	KeyJoystick \
	SETetrisDongle \
	MagicKey \
	Mouse \
	JoyTap \
	NinjaTap \
	ArkanoidPad \
	EventDelay \
	MSXEventDistributor \
	StateChangeDistributor \
	MSXEventRecorder \
	MSXEventReplayer \
	MSXEventRecorderReplayerCLI \
	RecordedCommand

HDR_ONLY:= \
	MSXEventListener \
	StateChangeListener \
	StateChange

include build/node-end.mk

